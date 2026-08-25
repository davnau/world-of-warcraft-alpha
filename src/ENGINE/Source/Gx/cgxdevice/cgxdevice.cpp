#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <limits>
#include <mutex>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {

std::mutex s_logMutex;
FILE* s_logFile = nullptr;

unsigned UpdateFrameRate()
{
    static std::array<std::uint64_t, 8> times{};
    static unsigned tail = 0;
    static std::uint64_t start = CGxDevice::CpuTicks();

    const auto stop = CGxDevice::CpuTicks();
    times[tail] = stop - start;
    std::uint64_t total = 0;
    for (const auto time : times) total += time;

    const auto frameRate = total <= 1024
        ? 99U
        : static_cast<unsigned>(CGxDevice::CpuFrequency() * 8.0 / static_cast<double>(total));
    tail = (tail + 1U) & 7U;
    start = stop;
    return frameRate;
}

CGxTex::Format ToBackendFormat(EGxTexFormat format)
{
    switch (format) {
    case GxTex_Rgb565: return CGxTex::Rgb565;
    case GxTex_Dxt1: return CGxTex::Dxt1;
    case GxTex_Dxt3: return CGxTex::Dxt3;
    case GxTex_Dxt5: return CGxTex::Dxt5;
    case GxTex_Argb8888:
    default: return CGxTex::Argb8888;
    }
}

unsigned FormatBits(EGxTexFormat format)
{
    switch (format) {
    case GxTex_Rgb565: return 16;
    case GxTex_Dxt1: return 4;
    case GxTex_Dxt3:
    case GxTex_Dxt5: return 8;
    case GxTex_Argb8888:
    default: return 32;
    }
}

unsigned StateIndex(EGxRenderState state)
{
    return std::min<unsigned>(static_cast<unsigned>(state), GxRenderStates_Last - 1);
}

EGxTexFormat ToApiFormat(CGxTex::Format format)
{
    switch (format) {
    case CGxTex::Rgb565: return GxTex_Rgb565;
    case CGxTex::Dxt1: return GxTex_Dxt1;
    case CGxTex::Dxt3: return GxTex_Dxt3;
    case CGxTex::Dxt5: return GxTex_Dxt5;
    case CGxTex::Argb8888:
    default: return GxTex_Argb8888;
    }
}

} // namespace

unsigned GxVertexSize(EGxVertexBufferFormat format) noexcept
{
    // Exact vtxBufSize table at 0x857664 in the preserved client.
    constexpr unsigned sizes[] = {24, 28, 32, 36, 40, 44, 24, 16, 28};
    return format < GxVertexBufferFormats_Last ? sizes[format] : 0;
}

unsigned GxVertexMemberOffset(EGxVertexBufferFormat format, unsigned member) noexcept
{
    // Exact s_vtxBufOffset table at 0x80AF48.  Missing members are ~0U,
    // which callers use to distinguish an absent stream from offset zero.
    constexpr unsigned absent = ~0U;
    constexpr unsigned offsets[GxVertexBufferFormats_Last][GxVertexMembers_Last] = {
        {0, 12, absent, absent, absent, absent, absent},
        {0, 12, 24, absent, absent, absent, absent},
        {0, 12, absent, 24, absent, absent, absent},
        {0, 12, 24, 28, absent, absent, absent},
        {0, 12, absent, 24, 32, absent, absent},
        {0, 12, 24, 28, 36, absent, absent},
        {0, absent, 12, 16, absent, absent, absent},
        {0, absent, 12, absent, absent, absent, absent},
        {0, absent, absent, 12, 20, absent, absent},
    };
    if (format >= GxVertexBufferFormats_Last || member >= GxVertexMembers_Last)
        return absent;
    return offsets[format][member];
}

void CGxGammaRamp::Set(float gamma)
{
    if (!(gamma > 0.0F)) gamma = 1.0F;
    const float exponent = 1.0F / gamma;
    for (unsigned i = 0; i < red.size(); ++i) {
        const float normalized = static_cast<float>(i) / 255.0F;
        const auto value = static_cast<std::uint16_t>(std::clamp(
            std::pow(normalized, exponent) * 65535.0F + 0.5F, 0.0F, 65535.0F));
        red[i] = value;
        green[i] = value;
        blue[i] = value;
    }
}

CGxMatrixStack::CGxMatrixStack()
{
    for (auto& matrix : m_mtx) matrix = NTempest::C44Matrix{};
    m_flags.fill(0);
    m_flags[0] = 1;
}

CGxMatrixStack::~CGxMatrixStack() = default;

void CGxMatrixStack::Identity()
{
    if (m_flags[m_level] == 0) {
        m_mtx[m_level] = NTempest::C44Matrix{};
        m_flags[m_level] = 1;
        m_dirty = true;
    }
}

void CGxMatrixStack::Push()
{
    m_level = std::min(m_level + 1, 3U);
    if (m_level != 0) {
        m_mtx[m_level] = m_mtx[m_level - 1];
        m_flags[m_level] = m_flags[m_level - 1];
    }
    m_dirty = true;
}

void CGxMatrixStack::Pop()
{
    if (m_level != 0) {
        --m_level;
        m_dirty = true;
    }
}

CGxDevice::CGxDevice()
{
    m_gammaRamp.Set(1.0F);
    m_systemGammaRamp.Set(1.0F);
    m_projection = NTempest::C44Matrix{};
    m_clearColor = NTempest::CImVector(0x00000000U);
    m_currentWindow = NTempest::CRect(0.0F, 0.0F, 1.0F, 1.0F);
    m_defWindow = m_currentWindow;
    RsInit();
}

CGxDevice::~CGxDevice()
{
    for (CGxTex* texture : m_textures) delete texture;
    for (CGxBuf* buffer : m_ownedBuffers) delete buffer;
    for (CGxPixelShader* shader : m_pixelShaders) delete shader;
    for (CGxVertexShader* shader : m_vertexShaders) delete shader;
    m_textures.clear();
    m_ownedBuffers.clear();
    m_dynamicBuffers.clear();
    m_pixelShaders.clear();
    m_vertexShaders.clear();
}

void CGxDevice::ScenePresent(unsigned)
{
    ++m_perfCountersAcc[GxPerf_FrameNum];
    m_perfCountersAcc[GxPerf_FrameRate] = UpdateFrameRate();
    m_scrShotClick = 0;
    PerfCountersLatch();
}

void CGxDevice::ClampRectToWindow(NTempest::CiRect* rect) const
{
    if (rect == nullptr) return;
    rect->minX = std::max(rect->minX, static_cast<int>(m_currentWindow.l));
    rect->minY = std::max(rect->minY, static_cast<int>(m_currentWindow.t));
    rect->maxX = std::min(rect->maxX, static_cast<int>(m_currentWindow.r));
    rect->maxY = std::min(rect->maxY, static_cast<int>(m_currentWindow.b));
}

bool CGxDevice::DeviceCreate(void*, const CGxFormat& format)
{
    m_api = GxApi_OpenGl;
    return DeviceSetFormat(const_cast<CGxFormat&>(format));
}

bool CGxDevice::DeviceCreate(HWND, const CGxFormat& format)
{
    m_api = GxApi_Direct3d;
    return DeviceSetFormat(const_cast<CGxFormat&>(format));
}

void CGxDevice::DeviceDestroy()
{
    m_bufLocked = nullptr;
    DeviceClearScreenShot();
}

bool CGxDevice::DeviceSetFormat(CGxFormat& format)
{
    m_format = format;
    m_windowed = format.window != 0;
    m_defWindow = NTempest::CRect(
        static_cast<float>(format.pos.x), static_cast<float>(format.pos.y),
        static_cast<float>(format.pos.x + format.size.x), static_cast<float>(format.pos.y + format.size.y));
    if (m_defWindow.IsEmpty()) {
        m_defWindow = NTempest::CRect(0.0F, 0.0F, static_cast<float>(format.size.x), static_cast<float>(format.size.y));
    }
    m_currentWindow = m_defWindow;
    return format.size.x > 0 && format.size.y > 0;
}

void CGxDevice::DeviceSetBaseMipLevel(unsigned baseMipLevel) { m_baseMipLevel = std::min(baseMipLevel, 15U); }
void CGxDevice::DeviceSetGamma(float gamma) { m_gammaRamp.Set(gamma); }
void CGxDevice::DeviceSetGamma(const CGxGammaRamp& ramp) { m_gammaRamp = ramp; }
void CGxDevice::DeviceSetTextureQuality(int force32) { m_force32BitTextures = force32 != 0; }

void CGxDevice::DeviceGamma(CGxGammaRamp* ramp) const { if (ramp != nullptr) *ramp = m_gammaRamp; }
void CGxDevice::DeviceSystemGamma(CGxGammaRamp* ramp) const { if (ramp != nullptr) *ramp = m_systemGammaRamp; }

void CGxDevice::DeviceTakeScreenShot()
{
    m_scrShotClick = 1;
}

void CGxDevice::DeviceScreenShot()
{
    const NTempest::CRect* window = DeviceCurWindow();
    m_scrShotWidth = static_cast<unsigned>(window->r);
    m_scrShotHeight = static_cast<unsigned>(window->b);
    const NTempest::CiRect pixelRect(0, 0,
        static_cast<int>(m_scrShotWidth), static_cast<int>(m_scrShotHeight));
    DeviceReadPixels(pixelRect, m_scrShotPixels);
}

void CGxDevice::DeviceReadPixels(
    const NTempest::CiRect&,
    std::vector<NTempest::CImVector>& pixels)
{
    pixels.clear();
}

void CGxDevice::DeviceReadDepths(
    const NTempest::CiRect& rect,
    std::vector<float>& depths)
{
    depths.assign(static_cast<std::size_t>(std::max(0, rect.Width()))
        * static_cast<std::size_t>(std::max(0, rect.Height())), 1.0F);
}

void CGxDevice::DeviceReadScreenShot(unsigned* width, unsigned* height, const NTempest::CImVector** pixels) const
{
    if (width != nullptr) *width = m_scrShotWidth;
    if (height != nullptr) *height = m_scrShotHeight;
    if (pixels != nullptr) *pixels = m_scrShotPixels.empty() ? nullptr : m_scrShotPixels.data();
}

void CGxDevice::DeviceClearScreenShot()
{
    m_scrShotPixels.clear();
    m_scrShotWidth = 0;
    m_scrShotHeight = 0;
}

void CGxDevice::DeviceSetRenderTarget(EGxBuffer, CGxTex* texture, unsigned)
{
    if (texture == nullptr) {
        DeviceSetCurWindow(&m_defWindow);
    } else {
        m_currentWindow = NTempest::CRect(0.0F, 0.0F, static_cast<float>(texture->width), static_cast<float>(texture->height));
    }
}

void CGxDevice::DeviceSetDefWindow(const NTempest::CRect* rect)
{
    if (rect != nullptr) {
        m_defWindow = *rect;
        m_currentWindow = *rect;
    }
}

void CGxDevice::DeviceSetCurWindow(const NTempest::CRect* rect) { if (rect != nullptr) m_currentWindow = *rect; }

void CGxDevice::DeviceOverride(EGxOverride value, unsigned flags)
{
    Log("DeviceOverride(): %d set to %d", static_cast<int>(value), flags);
}

void CGxDevice::CapsWindowSize(NTempest::CRect* destination) const
{
    if (destination != nullptr) *destination = *DeviceCurWindow();
}

void CGxDevice::CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const
{
    if (destination != nullptr) *destination = *DeviceCurWindow();
}

int CGxDevice::CapsIsWindowVisible() const
{
    const HWND window = DeviceWindow();
    return window != nullptr && IsWindowVisible(window) ? 1 : 0;
}

NTempest::CImVector* CGxDevice::SceneClearColor(NTempest::CImVector* destination) const
{
    if (destination != nullptr) *destination = m_clearColor;
    return destination;
}

void CGxDevice::XformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
    m_viewportMin = NTempest::C3Vector(minX, minY, minZ);
    m_viewportMax = NTempest::C3Vector(maxX, maxY, maxZ);
}

void CGxDevice::XformSetProjection(const NTempest::C44Matrix& matrix)
{
    m_projection = matrix;
    *m_xforms[GxXform_Projection].Top() = matrix;
}

void CGxDevice::XformSetView(const NTempest::C44Matrix& matrix) { *m_xforms[GxXform_View].Top() = matrix; }
void CGxDevice::XformSetBones(unsigned count, const NTempest::C34Matrix* matrices) { m_boneCount = count; m_bones = matrices; }

void CGxDevice::XformProjection(NTempest::C44Matrix* matrix) const { if (matrix != nullptr) *matrix = m_projection; }
void CGxDevice::XformView(NTempest::C44Matrix* matrix) const { if (matrix != nullptr) *matrix = *m_xforms[GxXform_View].Top(); }
void CGxDevice::XformBone(unsigned index, NTempest::C34Matrix* matrix) const { if (matrix != nullptr && m_bones != nullptr && index < m_boneCount) *matrix = m_bones[index]; }

void CGxDevice::XformPush(EGxXform transform) { if (transform < GxXforms_Last) m_xforms[transform].Push(); }

void CGxDevice::XformPush(EGxXform transform, const NTempest::C44Matrix& matrix)
{
    if (transform < GxXforms_Last) {
        m_xforms[transform].Push();
        *m_xforms[transform].Top() = matrix;
    }
}

void CGxDevice::XformPop(EGxXform transform) { if (transform < GxXforms_Last) m_xforms[transform].Pop(); }
void CGxDevice::XformIdentity(EGxXform transform) { if (transform < GxXforms_Last) m_xforms[transform].Identity(); }

void CGxDevice::XformSet(EGxXform transform, const NTempest::C44Matrix& matrix)
{
    if (transform < GxXforms_Last) {
        *m_xforms[transform].Top() = matrix;
        if (transform == GxXform_Projection) m_projection = matrix;
    }
}

void CGxDevice::XformTranslate(EGxXform transform, const NTempest::C3Vector& translation)
{
    if (transform < GxXforms_Last) {
        m_xforms[transform].Top()->Translate(translation);
        m_xforms[transform].m_flags[m_xforms[transform].Level()] = 0;
    }
}

void CGxDevice::XformScale(EGxXform transform, const NTempest::C3Vector& scale)
{
    if (transform < GxXforms_Last) {
        m_xforms[transform].Top()->Scale(scale);
        m_xforms[transform].m_flags[m_xforms[transform].Level()] = 0;
    }
}

void CGxDevice::XformMult(EGxXform transform, const NTempest::C44Matrix& matrix)
{
    if (transform < GxXforms_Last) {
        *m_xforms[transform].Top() *= matrix;
        m_xforms[transform].m_flags[m_xforms[transform].Level()] = 0;
    }
}

void CGxDevice::Xform(EGxXform transform, NTempest::C44Matrix* matrix) const
{
    if (matrix != nullptr && transform < GxXforms_Last) *matrix = *m_xforms[transform].Top();
}

bool CGxDevice::IVbHasColor(EGxVertexBufferFormat format) const
{
    return format == GxVBF_PC || format == GxVBF_PNC || format == GxVBF_PCT0
        || format == GxVBF_PNCT0 || format == GxVBF_PNCT0T1;
}

EGxVertexBufferFormat CGxDevice::IGiveVbColor(EGxVertexBufferFormat format) const
{
    switch (format) {
    case GxVBF_PN: return GxVBF_PNC;
    case GxVBF_PNT0: return GxVBF_PNCT0;
    case GxVBF_PNT0T1: return GxVBF_PNCT0T1;
    default: return format;
    }
}

unsigned CGxDevice::PrimCalcCount(EGxPrim primitive, unsigned indexCount) const
{
    switch (primitive) {
    case GxPrim_Lines: return indexCount / 2;
    case GxPrim_LineStrip: return indexCount > 1 ? indexCount - 1 : 0;
    case GxPrim_Triangles: return indexCount / 3;
    case GxPrim_TriangleStrip:
    case GxPrim_TriangleFan: return indexCount > 2 ? indexCount - 2 : 0;
    case GxPrim_Points:
    default: return indexCount;
    }
}

void CGxDevice::PrimBegin(EGxPrim primitive)
{
    m_primType = primitive;
    m_inBeginEnd = true;
    m_immediateIndices.clear();
    m_immediateVertices.clear();
    for (auto& coordinates : m_immediateTexCoords) coordinates.clear();
    m_immediateNormals.clear();
    m_immediateColors.clear();
}

void CGxDevice::PrimVertex(const NTempest::C3Vector& vertex)
{
    m_primVertex = vertex;
    m_primMask |= 1U;
    if (!m_inBeginEnd) return;

    m_immediateIndices.push_back(static_cast<std::uint16_t>(m_immediateVertices.size()));
    m_immediateVertices.push_back(m_primVertex);
    for (unsigned unit = 0; unit < m_immediateTexCoords.size(); ++unit)
        m_immediateTexCoords[unit].push_back(m_primTexCoord[unit]);
    m_immediateNormals.push_back(m_primNormal);
    m_immediateColors.push_back(m_primColor);

    if (m_immediateVertices.size() == 0x4000U) {
        PrimEnd();
        PrimBegin(m_primType);
    }
}

void CGxDevice::PrimNormal(const NTempest::C3Vector& normal)
{
    m_primNormal = normal;
    m_primMask |= 0x20U;
}

void CGxDevice::PrimColor(const NTempest::CImVector& color)
{
    m_primColor = color;
    m_primMask |= 0x40U;
}

void CGxDevice::PrimTexCoord(unsigned unit, const NTempest::C2Vector& texCoord)
{
    if (unit >= 4) return;
    m_primTexCoord[unit] = texCoord;
    m_primMask |= 2U << unit;
}

void CGxDevice::PrimEnd()
{
    m_inBeginEnd = false;
    if (m_immediateVertices.empty()) return;

    const NTempest::C3Vector* positions = (m_primMask & 1U) ? m_immediateVertices.data() : nullptr;
    const NTempest::C3Vector* normals = (m_primMask & 0x20U) ? m_immediateNormals.data() : nullptr;
    const NTempest::CImVector* colors = (m_primMask & 0x40U) ? m_immediateColors.data() : nullptr;
    const NTempest::C2Vector* tex0 = (m_primMask & 2U) ? m_immediateTexCoords[0].data() : nullptr;
    const NTempest::C2Vector* tex1 = (m_primMask & 4U) ? m_immediateTexCoords[1].data() : nullptr;

    PrimLockAndProcessVertexPtrs(static_cast<unsigned>(m_immediateVertices.size()),
        positions, sizeof(NTempest::C3Vector), normals, sizeof(NTempest::C3Vector),
        colors, sizeof(NTempest::CImVector), nullptr, 0,
        tex0, sizeof(NTempest::C2Vector), tex1, sizeof(NTempest::C2Vector));
    PrimLockIndexPtr(m_primType, static_cast<unsigned>(m_immediateIndices.size()),
        m_immediateIndices.data());
    PrimDrawElements();
    PrimUnlockIndexPtr();
    PrimUnlockVertexPtrs();
}

void CGxDevice::PrimLockAndProcessVertexPtrs(
    unsigned vertexCount, const NTempest::C3Vector* positions, unsigned positionStride,
    const NTempest::C3Vector* normals, unsigned, const NTempest::CImVector* colors, unsigned,
    const std::uint8_t*, unsigned, const NTempest::C2Vector* tex0, unsigned,
    const NTempest::C2Vector* tex1, unsigned)
{
    m_vertexLocked = true;
    m_perfCountersAcc[GxPerf_Vertices] += vertexCount;

    if (positions == nullptr) return;
    if (normals == nullptr) {
        if (colors != nullptr && tex0 != nullptr && tex1 == nullptr) m_vertexBufferFormat = GxVBF_PCT0;
        else if (colors != nullptr && tex0 == nullptr && tex1 == nullptr) m_vertexBufferFormat = GxVBF_PC;
        else if (colors == nullptr && tex0 != nullptr && tex1 != nullptr) m_vertexBufferFormat = GxVBF_PT0T1;
        else return;
    } else if (colors != nullptr) {
        if (tex0 == nullptr && tex1 == nullptr) m_vertexBufferFormat = GxVBF_PNC;
        else if (tex0 != nullptr && tex1 == nullptr) m_vertexBufferFormat = GxVBF_PNCT0;
        else if (tex0 != nullptr && tex1 != nullptr) m_vertexBufferFormat = GxVBF_PNCT0T1;
        else return;
    } else {
        if (tex0 == nullptr && tex1 == nullptr) m_vertexBufferFormat = GxVBF_PN;
        else if (tex0 != nullptr && tex1 == nullptr) m_vertexBufferFormat = GxVBF_PNT0;
        else if (tex0 != nullptr && tex1 != nullptr) m_vertexBufferFormat = GxVBF_PNT0T1;
        else return;
    }
    m_perfCountersAcc[GxPerf_VertexBytes] += vertexCount * GxVertexSize(m_vertexBufferFormat);
}

void CGxDevice::PrimLockIndexPtr(EGxPrim primitive, unsigned count, const std::uint16_t* indices)
{
    m_primType = primitive;
    m_primIndexCount = count;
    m_indexLocked = true;
    m_perfCountersAcc[GxPerf_IndexBytes] += 2U * count;
}

void CGxDevice::PrimDrawElements()
{
    m_perfCountersAcc[GxPerf_Primitives] += PrimCalcCount(m_primType, m_primIndexCount);
    ++m_perfCountersAcc[GxPerf_Batches];
}

void CGxDevice::PrimUnlockIndexPtr() { m_indexLocked = false; }
void CGxDevice::PrimUnlockVertexPtrs() { m_vertexLocked = false; }

void CGxDevice::LightSet(unsigned which, const CGxLight* light,
                         const NTempest::C3Vector& cameraPosition)
{
    if (which >= m_appState.m_lights.size()) return;
    const CGxLight defaultLight{};
    const CGxLight& source = light ? *light : defaultLight;
    auto& destination = m_appState.m_lights[which];

    // The original copies only the two defined flag bits, then the remaining
    // 56 bytes field-for-field.
    destination.m_flags = (destination.m_flags & ~3U) | (source.m_flags & 3U);
    destination.m_dir = source.m_dir;
    destination.m_ambColor = source.m_ambColor;
    destination.m_dirColor = source.m_dirColor;
    destination.m_specColor = source.m_specColor;
    destination.m_ambIntensity = source.m_ambIntensity;
    destination.m_dirIntensity = source.m_dirIntensity;
    destination.m_specIntensity = source.m_specIntensity;
    destination.m_constantAttenuation = source.m_constantAttenuation;
    destination.m_linearAttenuation = source.m_linearAttenuation;
    destination.m_quadraticAttenuation = source.m_quadraticAttenuation;
    destination.m_attenStart = source.m_attenStart;
    destination.m_attenEnd = source.m_attenEnd;
    if ((destination.m_flags & 2U) != 0U)
        destination.m_dir -= cameraPosition;

    m_hwState.m_lightsDirty[which] =
        std::memcmp(&m_hwState.m_lights[which], &source, sizeof(CGxLight)) != 0;
}

void CGxDevice::Light(unsigned which, CGxLight* light) const { if (light != nullptr && which < 8) *light = m_appState.m_lights[which]; }
void CGxDevice::LightEnable(unsigned which, bool enable)
{
    if (which >= m_appState.m_lights.size()) return;
    auto& flags = m_appState.m_lights[which].m_flags;
    flags = enable ? flags | 1U : flags & ~1U;
    if (((m_hwState.m_lights[which].m_flags & 1U) != 0U) != enable)
        m_hwState.m_lightsDirty[which] = 1U;
}

bool CGxDevice::EnableState(unsigned application, unsigned applicationDisables, unsigned flagPosition) const
{
    const unsigned bit = 1U << std::min(flagPosition, 31U);
    return (application & bit) != 0 && (applicationDisables & bit) == 0;
}

bool CGxDevice::NeedsUpdate(unsigned application, unsigned applicationDisables, unsigned hardware, unsigned hardwareDisables, unsigned flagPosition) const
{
    return EnableState(application, applicationDisables, flagPosition) != EnableState(hardware, hardwareDisables, flagPosition);
}

void CGxDevice::MasterEnableSet(EGxMasterEnables state, bool enable)
{
    if (state < GxMasterEnables_Last) {
        const unsigned bit = 1U << state;
        m_appState.m_masterEnables = enable ? m_appState.m_masterEnables | bit : m_appState.m_masterEnables & ~bit;
        switch (state) {
        case GxMasterEnable_Lighting: IRsForceUpdate(GxRs_Lighting); break;
        case GxMasterEnable_Fog: IRsForceUpdate(GxRs_Fog); break;
        case GxMasterEnable_DepthTest: IRsForceUpdate(GxRs_DepthTest); break;
        case GxMasterEnable_DepthWrite: IRsForceUpdate(GxRs_DepthWrite); break;
        case GxMasterEnable_Culling: IRsForceUpdate(GxRs_Culling); break;
        default: break;
        }
    }
}

bool CGxDevice::MasterEnable(EGxMasterEnables state) const
{
    return state < GxMasterEnables_Last && (m_appState.m_masterEnables & (1U << state)) != 0;
}

void CGxDevice::RsSet(EGxRenderState state, NTempest::CImVector value)
{
    if (state >= GxRenderStates_Last) return;
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(value));
    if (m_appState.m_values[StateIndex(state)] == bits) return;
    m_appState.m_values[StateIndex(state)] = bits;
    m_appState.m_colors[StateIndex(state) % m_appState.m_colors.size()] = value;
    m_rsDirty[StateIndex(state)] = true;
}

void CGxDevice::RsSet(EGxRenderState state, float value)
{
    if (state >= GxRenderStates_Last) return;
    std::uint32_t bits = 0;
    std::memcpy(&bits, &value, sizeof(value));
    if (m_appState.m_values[StateIndex(state)] == bits) return;
    m_appState.m_values[StateIndex(state)] = bits;
    m_rsDirty[StateIndex(state)] = true;
}

void CGxDevice::RsSet(EGxRenderState state, int value)
{
    if (state >= GxRenderStates_Last) return;
    const auto bits = static_cast<unsigned>(value);
    if (m_appState.m_values[StateIndex(state)] == bits) return;
    m_appState.m_values[StateIndex(state)] = bits;
    m_rsDirty[StateIndex(state)] = true;
}
void CGxDevice::RsSet(EGxRenderState state, EGxBlend value)
{
    RsSet(state, static_cast<int>(value));
    if (state == GxRs_Blend && value < GxBlends_Last) {
        // Exact eight-entry table at s_alphaRef in the recovered client.
        static constexpr std::array<unsigned, GxBlends_Last> alphaRef{{0U, 224U, 1U, 1U, 1U, 1U, 1U, 0U}};
        RsSet(GxRs_AlphaRef, static_cast<int>(alphaRef[value]));
    }
}

void CGxDevice::RsSet(EGxRenderState state, CGxTex* texture)
{
    if (state < GxRs_Texture0 || state > GxRs_Texture3) return;
    const unsigned unit = static_cast<unsigned>(state - GxRs_Texture0);
    if (m_appState.m_textures[unit] == texture) return;
    m_appState.m_textures[unit] = texture;
    m_appState.m_values[StateIndex(state)] = static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(texture));
    m_rsDirty[StateIndex(state)] = true;
}

void CGxDevice::RsGet(EGxRenderState state, NTempest::CImVector* value) const
{
    if (value != nullptr && state < GxRenderStates_Last) std::memcpy(value, &m_appState.m_values[StateIndex(state)], sizeof(*value));
}

void CGxDevice::RsGet(EGxRenderState state, float* value) const
{
    if (value != nullptr && state < GxRenderStates_Last) std::memcpy(value, &m_appState.m_values[StateIndex(state)], sizeof(*value));
}

void CGxDevice::RsGet(EGxRenderState state, int* value) const { if (value != nullptr && state < GxRenderStates_Last) *value = static_cast<int>(m_appState.m_values[StateIndex(state)]); }

void CGxDevice::RsGet(EGxRenderState state, void** value) const
{
    if (value == nullptr) return;
    *value = state >= GxRs_Texture0 && state <= GxRs_Texture3 ? m_appState.m_textures[state - GxRs_Texture0] : nullptr;
}

void CGxDevice::RsPush()
{
    m_rsStack.push_back(m_appState.m_values);
    m_rsTextureStack.push_back(m_appState.m_textures);
}

void CGxDevice::RsPop()
{
    if (m_rsStack.empty()) return;
    const auto values = m_rsStack.back();
    const auto textures = m_rsTextureStack.back();
    m_rsStack.pop_back();
    m_rsTextureStack.pop_back();
    for (unsigned state = 0; state < GxRenderStates_Last; ++state) {
        if (m_appState.m_values[state] != values[state]) m_rsDirty[state] = true;
    }
    for (unsigned unit = 0; unit < 4; ++unit) {
        if (m_appState.m_textures[unit] != textures[unit])
            m_rsDirty[GxRs_Texture0 + unit] = true;
    }
    m_appState.m_values = values;
    m_appState.m_textures = textures;
}

void CGxDevice::RsInit()
{
    m_appState.m_values.fill(0);
    m_hwState.m_values.fill(0);
    m_appState.m_textures.fill(nullptr);
    m_hwState.m_textures.fill(nullptr);
    m_rsDirty.fill(false);
    // The recovered constructor initializes all nine GX master enables.  In
    // particular, bit 6 controls presentation of the OpenGL back buffer.
    m_appState.m_masterEnables = 0x1ffU;
    m_hwState.m_masterEnables = 0x1ffU;
    m_rsStack.clear();
    m_rsTextureStack.clear();

    // Exact state initialization sequence at CGxDevice::RsInit (0x593900).
    RsSet(GxRs_PolygonOffset, 0.0F);
    RsSet(GxRs_MatDiffuse, NTempest::CImVector{0xFFFFFFFFU});
    RsSet(GxRs_MatEmissive, 0.0F);
    RsSet(GxRs_MatSpecular, 0.0F);
    RsSet(GxRs_NormalizeNormals, 0);
    RsSet(GxRs_SceneAmbient, NTempest::CImVector{0x00000000U});
    RsSet(GxRs_Blend, GxBlend_Opaque);
    RsSet(GxRs_FogStyle, 0);
    RsSet(GxRs_FogStart, 0.0F);
    RsSet(GxRs_FogEnd, 1.0F);
    RsSet(GxRs_FogDensity, 0.0F);
    RsSet(GxRs_FogColor, NTempest::CImVector{0xFFFF0000U});
    RsSet(GxRs_Lighting, 1);
    RsSet(GxRs_Fog, 1);
    RsSet(GxRs_DepthTest, 1);
    RsSet(GxRs_DepthFunc, 0);
    RsSet(GxRs_Culling, 1);
    for (unsigned unit = 0; unit < 4; ++unit) {
        RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + unit), static_cast<CGxTex*>(nullptr));
        RsSet(static_cast<EGxRenderState>(GxRs_TexBlend0 + unit), 1);
        RsSet(static_cast<EGxRenderState>(GxRs_TexLodBias0 + unit), 0.0F);
        RsSet(static_cast<EGxRenderState>(GxRs_TexGen0 + unit), 0);
    }
    RsSet(GxRs_PixelShader, 0);
    RsSet(GxRs_VertexShader, 0);
}

void CGxDevice::IRsForceUpdate(unsigned state)
{
    if (state < GxRenderStates_Last) m_rsDirty[state] = true;
}

void CGxDevice::IRsForceUpdate()
{
    m_rsDirty.fill(true);
}

void CGxDevice::IRsSync(bool force)
{
    if (force) IRsForceUpdate();
    for (unsigned state = 0; state < GxRenderStates_Last; ++state) {
        if (!m_rsDirty[state]) continue;
        IRsSendToHw(state);
        m_hwState.m_values[state] = m_appState.m_values[state];
        if (state >= GxRs_Texture0 && state <= GxRs_Texture3)
            m_hwState.m_textures[state - GxRs_Texture0] = m_appState.m_textures[state - GxRs_Texture0];
        m_rsDirty[state] = false;
    }

}

CGxBuf* CGxDevice::BufCreate(EGxBufWriteFreq writeFreq, EGxVertexBufferFormat format, unsigned numVertices, unsigned numIndices, CGxBufUserCallback callback, void* userArg)
{
    if (numVertices > 0xFFFFU || numIndices > 0xFFFFU) return nullptr;
    auto* buffer = new CGxBuf();
    buffer->writeFreq = writeFreq;
    buffer->vertexFormat = format;
    buffer->vertexCount = numVertices;
    buffer->indexCount = numIndices;
    buffer->vertexStride = GxVertexSize(format);
    buffer->vertexData.resize(static_cast<std::size_t>(buffer->vertexStride) * numVertices);
    buffer->indexData.resize(static_cast<std::size_t>(numIndices) * sizeof(std::uint16_t));
    buffer->userCallback = callback;
    buffer->userArg = userArg;
    m_ownedBuffers.push_back(buffer);
    if (writeFreq != GxBWF_Static) m_dynamicBuffers.push_back(buffer);
    return buffer;
}

void CGxDevice::BufDestroy(CGxBuf** buffer)
{
    if (buffer == nullptr || *buffer == nullptr) return;
    auto* target = *buffer;
    std::erase(m_dynamicBuffers, target);
    std::erase(m_ownedBuffers, target);
    if (m_bufLocked == target) m_bufLocked = nullptr;
    delete target;
    *buffer = nullptr;
}

void CGxDevice::BufLock(CGxBuf* buffer)
{
    if (buffer == nullptr || m_bufLocked != nullptr) return;
    m_bufLocked = buffer;
    m_vertexBufferFormat = buffer->vertexFormat;
    buffer->vertexLocked = true;
    m_perfCountersAcc[GxPerf_Vertices] += buffer->vertexCount;
}

void CGxDevice::BufUnlock() { if (m_bufLocked != nullptr) { m_bufLocked->vertexLocked = false; m_bufLocked = nullptr; } }

void CGxDevice::BufRender(const CGxBatch* batches, unsigned count)
{
    if (m_bufLocked == nullptr || batches == nullptr) return;
    for (unsigned i = 0; i < count; ++i) m_perfCountersAcc[GxPerf_Primitives] += PrimCalcCount(batches[i].m_primType, batches[i].m_count);
    m_perfCountersAcc[GxPerf_Batches] += count;
}

void CGxDevice::BufReserve(EGxBufWriteFreq frequency, EGxVertexBufferFormat format, unsigned vertexCount, unsigned indexCount)
{
    if (frequency <= GxBWF_Static || frequency >= GxBufWriteFreqs_Last || format >= GxVertexBufferFormats_Last) return;
    const unsigned index = static_cast<unsigned>(frequency) * GxVertexBufferFormats_Last + format;
    m_VBReserve[index] = vertexCount;
    m_IBReserve[index] = indexCount;
}

CGxBuf* CGxDevice::BufGetDynamic(EGxVertexBufferFormat format) const
{
    for (CGxBuf* buffer : m_dynamicBuffers) if (buffer != nullptr && buffer->vertexFormat == format) return buffer;
    return nullptr;
}

bool CGxDevice::TexCreate(unsigned width, unsigned height, EGxTexFormat format, CGxTexFlags flags, void* userArg, CGxTexUserCallback callback, CGxTex** texture)
{
    return TexCreate(GxTex_2d, width, height, 1, format, format, flags, userArg, callback, texture);
}

bool CGxDevice::TexCreate(EGxTexTarget target, unsigned width, unsigned height, unsigned depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, CGxTexUserCallback callback, CGxTex** texture)
{
    if (texture == nullptr || width == 0 || height == 0 || width > m_caps.m_maxTextureSize || height > m_caps.m_maxTextureSize) return false;
    auto* result = new CGxTex();
    result->target = target;
    result->width = width;
    result->height = height;
    result->depth = depth;
    result->format = ToBackendFormat(format);
    result->dataFormat = ToBackendFormat(dataFormat);
    result->flags = flags.bits;
    result->userArg = userArg;
    result->userFunc = callback;
    result->updateRect = NTempest::CiRect(0, 0, static_cast<int>(width), static_cast<int>(height));
    m_textures.push_back(result);
    *texture = result;
    ++m_perfCountersAcc[GxPerf_Textures];
    m_perfCountersAcc[GxPerf_TextureBytes] += ITexComputeByteSize(result, width, height);
    return true;
}

void CGxDevice::TexMarkForUpdate(CGxTex* texture, const NTempest::CiRect& updateRect, bool immediate)
{
    if (texture == nullptr) return;
    texture->needsUpdate = true;
    const NTempest::CiRect full(0, 0, static_cast<int>(texture->width), static_cast<int>(texture->height));
    if (updateRect.minX >= updateRect.maxX || updateRect.minY >= updateRect.maxY) {
        texture->updateRect = full;
    } else {
        auto clipped = updateRect;
        clipped.minX = std::clamp(clipped.minX, full.minX, full.maxX);
        clipped.maxX = std::clamp(clipped.maxX, full.minX, full.maxX);
        clipped.minY = std::clamp(clipped.minY, full.minY, full.maxY);
        clipped.maxY = std::clamp(clipped.maxY, full.minY, full.maxY);
        texture->updateRect.minX = std::min(texture->updateRect.minX, clipped.minX);
        texture->updateRect.minY = std::min(texture->updateRect.minY, clipped.minY);
        texture->updateRect.maxX = std::max(texture->updateRect.maxX, clipped.maxX);
        texture->updateRect.maxY = std::max(texture->updateRect.maxY, clipped.maxY);
    }
    if (immediate) ITexMarkAsUpdated(texture);
}

bool CGxDevice::TexNeedsUpdate(const CGxTex* texture) const { return texture != nullptr && texture->needsUpdate; }

void CGxDevice::ITexMarkAsUpdated(CGxTex* texture)
{
    if (texture == nullptr || !texture->needsUpdate) return;
    ++m_perfCountersAcc[GxPerf_TexUploads];
    const unsigned width = static_cast<unsigned>(std::max(0, texture->updateRect.maxX - texture->updateRect.minX));
    const unsigned height = static_cast<unsigned>(std::max(0, texture->updateRect.maxY - texture->updateRect.minY));
    m_perfCountersAcc[GxPerf_TexUploadBytes] += ITexComputeByteSize(texture, width, height);
    texture->needsUpdate = false;
    texture->updateRect = NTempest::CiRect(static_cast<int>(texture->width), static_cast<int>(texture->height), 0, 0);
}

unsigned CGxDevice::ITexComputeByteSize(const CGxTex* texture, unsigned width, unsigned height) const
{
    if (texture == nullptr) return 0;
    if (width == std::numeric_limits<unsigned>::max()) width = texture->width;
    if (height == std::numeric_limits<unsigned>::max()) height = texture->height;
    width = std::max(1U, width >> m_baseMipLevel);
    height = std::max(1U, height >> m_baseMipLevel);
    const unsigned bits = texture->dataFormat == CGxTex::Rgb565 ? 16 : texture->dataFormat == CGxTex::Dxt1 ? 4 : texture->dataFormat == CGxTex::Dxt3 || texture->dataFormat == CGxTex::Dxt5 ? 8 : 32;
    return static_cast<unsigned>(std::min<std::uint64_t>(static_cast<std::uint64_t>(width) * height * bits / 8, std::numeric_limits<unsigned>::max()));
}

void CGxDevice::ITexBind(CGxTex* texture)
{
    if (texture != nullptr && texture->frameTag != m_perfCountersAcc[GxPerf_FrameNum]) {
        texture->frameTag = m_perfCountersAcc[GxPerf_FrameNum];
        ++m_perfCountersAcc[GxPerf_TexBinds];
        m_perfCountersAcc[GxPerf_TexBindBytes] += ITexComputeByteSize(texture, std::numeric_limits<unsigned>::max(), std::numeric_limits<unsigned>::max());
    }
}

void CGxDevice::TexSetUserData(CGxTex* texture, CGxTexUserCallback callback, void* userArg)
{
    if (texture != nullptr) {
        texture->userFunc = callback;
        texture->userArg = userArg;
        TexMarkForUpdate(texture, NTempest::CiRect{}, false);
    }
}

void CGxDevice::TexSetFlags(CGxTex* texture, CGxTexFlags flags) { if (texture != nullptr && texture->flags != flags.bits) texture->SetFlags(flags); }
void* CGxDevice::TexUserArg(const CGxTex* texture) const { return texture == nullptr ? nullptr : texture->userArg; }

void CGxDevice::TexGetDimensions(const CGxTex* texture, unsigned* width, unsigned* height) const
{
    if (width != nullptr) *width = texture == nullptr ? 0 : texture->width;
    if (height != nullptr) *height = texture == nullptr ? 0 : texture->height;
}

void CGxDevice::TexDestroy(CGxTex** texture)
{
    if (texture == nullptr || *texture == nullptr) return;
    std::erase(m_textures, *texture);
    delete *texture;
    *texture = nullptr;
}

void CGxDevice::TexSetDataFormat(CGxTex* texture, EGxTexFormat format) { if (texture != nullptr) texture->dataFormat = ToBackendFormat(format); }

CGxTexParms CGxDevice::TexParameters(const CGxTex* texture) const
{
    CGxTexParms result{};
    if (texture != nullptr) {
        result.flags = CGxTexFlags(texture->flags);
        result.format = ToApiFormat(texture->format);
        result.width = texture->width;
        result.height = texture->height;
    }
    return result;
}

CGxTexParmsEx CGxDevice::TexParametersEx(const CGxTex* texture) const
{
    CGxTexParmsEx result{};
    if (texture != nullptr) {
        static_cast<CGxTexParms&>(result) = TexParameters(texture);
        result.dataFormat = ToApiFormat(texture->dataFormat);
        result.depth = texture->depth;
    }
    return result;
}

CGxTexFlags CGxDevice::TexFlags(const CGxTex* texture) const { return texture == nullptr ? CGxTexFlags{} : CGxTexFlags(texture->flags); }

void CGxDevice::ISetShaderParameters(CGxShader* shader, bool force)
{
    if (shader == nullptr) return;
    for (auto& parameter : shader->params) if (force || parameter.dirty) parameter.dirty = false;
    shader->paramsDirty = false;
}

unsigned CGxDevice::PerfCounter(EGxPerfCounter counter) const { return counter < GxPerfCounters_Last ? m_perfCountersLatched[counter] : 0; }
void CGxDevice::PerfCountersLatch()
{
    for (unsigned counter = 0; counter < GxPerfCounters_Last; ++counter) {
        m_perfCountersLatched[counter] = m_perfCountersAcc[counter];
        if (counter != GxPerf_Textures && counter != GxPerf_TextureBytes)
            m_perfCountersAcc[counter] = 0;
    }
    m_perfCountersAcc[GxPerf_FrameNum] = m_perfCountersLatched[GxPerf_FrameNum];
}
double CGxDevice::CpuFrequency() { return 1000000000.0; }
std::uint64_t CGxDevice::CpuTicks() { return static_cast<std::uint64_t>(std::chrono::steady_clock::now().time_since_epoch().count()); }

void CGxDevice::DbgPrintf(const char* format, ...)
{
    if (format == nullptr) return;
    std::va_list args;
    va_start(args, format);
    std::vfprintf(stderr, format, args);
    va_end(args);
}

void CGxDevice::LogOpen(const char* filename)
{
    std::lock_guard lock(s_logMutex);
    if (s_logFile != nullptr) std::fclose(s_logFile);
    s_logFile = nullptr;
    if (filename != nullptr) {
        FILE* file = nullptr;
        if (fopen_s(&file, filename, "a") == 0) s_logFile = file;
    }
}

void CGxDevice::LogClose()
{
    std::lock_guard lock(s_logMutex);
    if (s_logFile != nullptr) std::fclose(s_logFile);
    s_logFile = nullptr;
}

void CGxDevice::Log(const char* format, ...)
{
    if (format == nullptr) return;
    std::lock_guard lock(s_logMutex);
    if (s_logFile == nullptr) return;
    std::va_list args;
    va_start(args, format);
    std::vfprintf(s_logFile, format, args);
    std::fputc('\n', s_logFile);
    std::fflush(s_logFile);
    va_end(args);
}

void CGxShaderParam::Set(unsigned newIndex, unsigned newVectorCount, const float* newValues)
{
    index = newIndex;
    vectorCount = std::min(newVectorCount, 4U);
    if (newValues != nullptr) std::copy_n(newValues, vectorCount * 4U, values);
    dirty = true;
}

void CGxShaderParam::Set(const NTempest::C4Vector& value)
{
    vectorCount = 1;
    values[0] = value.x;
    values[1] = value.y;
    values[2] = value.z;
    values[3] = value.w;
    dirty = true;
}

void CGxShaderParam::Set(const NTempest::C34Matrix& matrix)
{
    vectorCount = 3;
    std::copy_n(matrix.Data(), 12, values);
    dirty = true;
}

void CGxShaderParam::Set(const NTempest::C44Matrix& matrix)
{
    vectorCount = 4;
    std::copy_n(matrix.Data(), 16, values);
    dirty = true;
}

void CGxShader::SetParam(unsigned index, unsigned vectorCount, const float* values)
{
    auto* parameter = GetParam(index);
    if (parameter == nullptr) {
        params.emplace_back();
        parameter = &params.back();
    }
    parameter->Set(index, vectorCount, values);
    paramsDirty = true;
}

void CGxShader::SetParam(CGxShaderParam* parameter, const NTempest::C4Vector& value)
{
    if (parameter != nullptr) {
        parameter->Set(value);
        paramsDirty = true;
    }
}

void CGxShader::SetParam(CGxShaderParam* parameter, const NTempest::C34Matrix& matrix)
{
    if (parameter != nullptr) {
        parameter->Set(matrix);
        paramsDirty = true;
    }
}

void CGxShader::SetParam(CGxShaderParam* parameter, const NTempest::C44Matrix& matrix)
{
    if (parameter != nullptr) {
        parameter->Set(matrix);
        paramsDirty = true;
    }
}

CGxShaderParam* CGxShader::GetParam(unsigned index)
{
    for (auto& parameter : params) if (parameter.index == index) return &parameter;
    return nullptr;
}

const CGxShaderParam* CGxShader::GetParam(unsigned index) const
{
    for (const auto& parameter : params) if (parameter.index == index) return &parameter;
    return nullptr;
}

CGxShaderParam* CGxShader::GetParam(const char* parameterName)
{
    if (parameterName == nullptr) return nullptr;
    for (auto& parameter : params) if (parameter.name == parameterName) return &parameter;
    return nullptr;
}

CGxShaderParam* CGxShader::GetFirstParam() { return params.empty() ? nullptr : &params.front(); }

CGxShaderParam* CGxShader::GetNextParam(CGxShaderParam* parameter)
{
    if (parameter == nullptr || params.empty()) return nullptr;
    const std::size_t index = static_cast<std::size_t>(parameter - params.data()) + 1;
    return index < params.size() ? &params[index] : nullptr;
}
