#include "ENGINE/Source/Gx/gxapi/gxapi.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

CGxDevice* g_theGxDevicePtr = nullptr;

namespace {

std::vector<CGxFormat> s_formats;
std::vector<std::uint8_t> s_vertexMemory;
std::vector<std::uint8_t> s_indexMemory;
std::vector<std::uint8_t> s_pixelMemory;

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

void UntrackTexture(CGxTex* texture)
{
    if (g_theGxDevicePtr == nullptr) return;
    const auto it = std::find(g_theGxDevicePtr->m_textures.begin(), g_theGxDevicePtr->m_textures.end(), texture);
    if (it != g_theGxDevicePtr->m_textures.end()) g_theGxDevicePtr->m_textures.erase(it);
}

} // namespace

int GxAdapterID(unsigned short* vendorID, unsigned short* deviceID, unsigned* driverVersionHi, unsigned* driverVersionLow)
{
    return CGxDevice::AdapterID(vendorID, deviceID, driverVersionHi, driverVersionLow);
}

int GxAdapterInfer(unsigned short* deviceClass) { return CGxDevice::AdapterInfer(deviceClass); }
bool GxAdapterMonitorModes(std::vector<CGxMonitorMode>& modes) { return CGxDevice::AdapterMonitorModes(modes); }
int GxAdapterDesktopMode(unsigned mode[4]) { return CGxDevice::AdapterDesktopMode(mode); }

const std::vector<CGxFormat>* GxEnumFormats(EGxApi api)
{
    s_formats.clear();
    if (api == GxApi_Direct3d) {
        CGxDevice::D3dEnumFormats(s_formats);
    } else {
        CGxDevice::OpenGlEnumFormats(s_formats);
    }
    return s_formats.empty() ? nullptr : &s_formats;
}

CGxDevice* GxDevCreate(EGxApi api, void* windowProc, const CGxFormat& format)
{
    GxDevDestroy(g_theGxDevicePtr);
    if (api == GxApi_Direct3d) {
        g_theGxDevicePtr = CGxDevice::NewD3d();
        if (g_theGxDevicePtr == nullptr || !g_theGxDevicePtr->DeviceCreate(windowProc, format)) {
            GxDevDestroy(g_theGxDevicePtr);
            return nullptr;
        }
        g_theGxDevicePtr->m_api = GxApi_Direct3d;
    } else {
        auto* device = CGxDevice::NewOpenGl();
        g_theGxDevicePtr = device;
        if (device == nullptr) return nullptr;
        auto* proc = reinterpret_cast<CGxDeviceOpenGl::WindowProc>(windowProc);
        if (!device->DeviceCreate(proc, format)) {
            GxDevDestroy(g_theGxDevicePtr);
            return nullptr;
        }
        g_theGxDevicePtr->m_api = GxApi_OpenGl;
    }
    return g_theGxDevicePtr;
}

CGxDevice* GxDevCreate(EGxApi api, HWND window, const CGxFormat& format)
{
    GxDevDestroy(g_theGxDevicePtr);
    if (api == GxApi_Direct3d) {
        g_theGxDevicePtr = CGxDevice::NewD3d();
        if (g_theGxDevicePtr == nullptr || !g_theGxDevicePtr->DeviceCreate(window, format)) {
            GxDevDestroy(g_theGxDevicePtr);
            return nullptr;
        }
        g_theGxDevicePtr->m_api = GxApi_Direct3d;
    } else {
        auto* device = CGxDevice::NewOpenGl();
        g_theGxDevicePtr = device;
        if (device == nullptr || !device->DeviceCreate(window, format)) {
            GxDevDestroy(g_theGxDevicePtr);
            return nullptr;
        }
        g_theGxDevicePtr->m_api = GxApi_OpenGl;
    }
    return g_theGxDevicePtr;
}

void GxDevWM(EGxWM message, const NTempest::CRect& rect, int param)
{
    if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceWM(message, &rect, param);
}

void GxDevDestroy(CGxDevice* device)
{
    if (device == nullptr) return;
    if (device == g_theGxDevicePtr) {
        device->DeviceDestroy();
        delete device;
        g_theGxDevicePtr = nullptr;
    }
}

bool GxDevSetFormat(const CGxFormat& format)
{
    if (g_theGxDevicePtr == nullptr) return false;
    auto copy = format;
    return g_theGxDevicePtr->DeviceSetFormat(copy);
}

void GxDevSetBaseMipLevel(unsigned baseMipLevel) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceSetBaseMipLevel(baseMipLevel); }
void GxDevSetGamma(float gamma) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceSetGamma(gamma); }
void GxDevSetGamma(const CGxGammaRamp& ramp) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceSetGamma(ramp); }
void GxDevSetTextureQuality(int force32) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceSetTextureQuality(force32); }
const CGxFormat* GxDevFormat() { return g_theGxDevicePtr == nullptr ? nullptr : g_theGxDevicePtr->DeviceFormat(); }
unsigned GxDevBaseMipLevel() { return g_theGxDevicePtr == nullptr ? 0 : g_theGxDevicePtr->DeviceBaseMipLevel(); }
void GxDevGammaRamp(CGxGammaRamp* ramp) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceGamma(ramp); }
void GxDevSystemGammaRamp(CGxGammaRamp* ramp) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceSystemGamma(ramp); }
int GxDevTextureQuality() { return g_theGxDevicePtr == nullptr ? 0 : g_theGxDevicePtr->DeviceTextureQuality(); }
HWND GxDevWindow() { return g_theGxDevicePtr == nullptr ? nullptr : g_theGxDevicePtr->DeviceWindow(); }
EGxApi GxDevApi() { return g_theGxDevicePtr == nullptr ? GxApi_OpenGl : g_theGxDevicePtr->DeviceApi(); }
void GxDevTakeScreenShot() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceTakeScreenShot(); }
void GxDevReadScreenShot(unsigned* width, unsigned* height, const NTempest::CImVector** pixels) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceReadScreenShot(width, height, pixels); }
void GxDevClearScreenShot() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceClearScreenShot(); }

void GxDevReadPixels(const NTempest::CiRect& rect, std::vector<NTempest::CImVector>& pixels)
{
    if (g_theGxDevicePtr != nullptr) {
        g_theGxDevicePtr->DeviceReadPixels(rect, pixels);
    } else {
        pixels.assign(static_cast<std::size_t>(std::max(0, rect.Width())) * std::max(0, rect.Height()), NTempest::CImVector{});
    }
}

void GxDevReadDepth(const NTempest::CiRect& rect, std::vector<float>& depths)
{
    if (g_theGxDevicePtr != nullptr) {
        g_theGxDevicePtr->DeviceReadDepths(rect, depths);
    } else {
        depths.assign(static_cast<std::size_t>(std::max(0, rect.Width())) * std::max(0, rect.Height()), 1.0F);
    }
}

void GxDevSetRenderTarget(EGxBuffer buffer, CGxTex* texture, unsigned plane)
{
    if (auto* device = dynamic_cast<CGxDeviceOpenGl*>(g_theGxDevicePtr); device != nullptr) {
        device->DeviceSetRenderTarget(static_cast<unsigned>(buffer), texture, plane);
    } else if (g_theGxDevicePtr != nullptr) {
        g_theGxDevicePtr->DeviceSetRenderTarget(buffer, texture, plane);
    }
}

void GxDevOverride(EGxOverride value, unsigned flags) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->DeviceOverride(value, flags); }

void GxLightSet(unsigned which, const CGxLight* light, const NTempest::C3Vector& cameraPosition) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->LightSet(which, light, cameraPosition); }
void GxLight(unsigned which, CGxLight* light) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->Light(which, light); }
void GxLightEnable(unsigned which, bool enable) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->LightEnable(which, enable); }

void GxRsSet(EGxRenderState state, NTempest::CImVector value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsSet(state, value); }
void GxRsSet(EGxRenderState state, float value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsSet(state, value); }
void GxRsSet(EGxRenderState state, int value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsSet(state, value); }
void GxRsSet(EGxRenderState state, EGxBlend value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsSet(state, value); }
void GxRsSet(EGxRenderState state, CGxTex* texture) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsSet(state, texture); }
void GxRsGet(EGxRenderState state, NTempest::CImVector* value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsGet(state, value); }
void GxRsGet(EGxRenderState state, float* value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsGet(state, value); }
void GxRsGet(EGxRenderState state, int* value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsGet(state, value); }
void GxRsGet(EGxRenderState state, void** value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsGet(state, value); }
void GxRsPush() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsPush(); }
void GxRsPop() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsPop(); }
void GxRsInit() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->RsInit(); }
unsigned GxRsStackOffset() { return g_theGxDevicePtr == nullptr ? 0 : g_theGxDevicePtr->RsStackOffset(); }
void GxVertexShaderSelect(EGxVertexShader shader) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->VertexShaderSelect(shader); }

CGxBuf* GxBufCreate(EGxBufWriteFreq writeFreq, EGxVertexBufferFormat format, unsigned numVertices, unsigned numIndices, CGxBufUserCallback userCallback, void* userArg)
{
    return g_theGxDevicePtr == nullptr ? nullptr : g_theGxDevicePtr->BufCreate(writeFreq, format, numVertices, numIndices, userCallback, userArg);
}

void GxBufLock(CGxBuf* buffer) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->BufLock(buffer); }
void GxBufUnlock() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->BufUnlock(); }
void GxBufRender(const CGxBatch* batches, unsigned count) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->BufRender(batches, count); }
void GxBufRender(const CGxBatch* batch) { GxBufRender(batch, 1); }
void GxBufDestroy(CGxBuf** buffer) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->BufDestroy(buffer); }
void GxBufReserve(EGxBufWriteFreq writeFreq, EGxVertexBufferFormat format, unsigned numVertices, unsigned numIndices) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->BufReserve(writeFreq, format, numVertices, numIndices); }
CGxBuf* GxBufGetDynamic(EGxVertexBufferFormat format) { return g_theGxDevicePtr == nullptr ? nullptr : g_theGxDevicePtr->BufGetDynamic(format); }
unsigned GxPerfCounter(EGxPerfCounter counter) { return g_theGxDevicePtr == nullptr ? 0 : g_theGxDevicePtr->PerfCounter(counter); }
void GxMasterEnableSet(EGxMasterEnables enable, bool value) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->MasterEnableSet(enable, value); }
bool GxMasterEnable(EGxMasterEnables enable) { return g_theGxDevicePtr != nullptr && g_theGxDevicePtr->MasterEnable(enable); }
const CGxCaps* GxCaps() { return g_theGxDevicePtr == nullptr ? nullptr : g_theGxDevicePtr->Caps(); }

void GxCapsWindowSize(NTempest::CRect* rect)
{
    if (rect == nullptr || g_theGxDevicePtr == nullptr) return;
    g_theGxDevicePtr->CapsWindowSize(rect);
}

void GxCapsScreenSize(NTempest::CRect* rect)
{
    if (rect == nullptr || g_theGxDevicePtr == nullptr) return;
    g_theGxDevicePtr->CapsWindowSizeInScreenCoords(rect);
}

int GxCapsIsWindowVisible()
{
    return g_theGxDevicePtr != nullptr ? g_theGxDevicePtr->CapsIsWindowVisible() : 0;
}

void GxPrimLockVertexPtrs(
    unsigned vertexCount, const NTempest::C3Vector* positions, unsigned positionStride,
    const NTempest::C3Vector* normals, unsigned normalStride, const NTempest::CImVector* colors,
    unsigned colorStride, const std::uint8_t* bones, unsigned boneStride, const NTempest::C2Vector* tex0,
    unsigned tex0Stride, const NTempest::C2Vector* tex1, unsigned tex1Stride)
{
    if (g_theGxDevicePtr != nullptr && vertexCount != 0 && positions != nullptr && positionStride != 0)
        g_theGxDevicePtr->PrimLockAndProcessVertexPtrs(vertexCount, positions, positionStride,
            normals, normalStride, colors, colorStride, bones, boneStride,
            tex0, tex0Stride, tex1, tex1Stride);
}

void GxPrimLockIndexPtr(EGxPrim primitive, unsigned count, const std::uint16_t* indices) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimLockIndexPtr(primitive, count, indices); }
void GxPrimDrawElements() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimDrawElements(); }
void GxPrimDrawElements(EGxPrim primitive, unsigned count, const std::uint16_t* indices) { GxPrimLockIndexPtr(primitive, count, indices); GxPrimDrawElements(); GxPrimUnlockIndexPtr(); }
void GxPrimUnlockIndexPtr() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimUnlockIndexPtr(); }
void GxPrimUnlockVertexPtrs() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimUnlockVertexPtrs(); }
void GxPrimBegin(EGxPrim primitive) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimBegin(primitive); }
void GxPrimEnd() { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimEnd(); }
void GxPrimVertex(const NTempest::C3Vector& vertex) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimVertex(vertex); }
void GxPrimTexCoord(unsigned unit, const NTempest::C2Vector& texCoord) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimTexCoord(unit, texCoord); }
void GxPrimNormal(const NTempest::C3Vector& normal) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimNormal(normal); }
void GxPrimColor(const NTempest::CImVector& color) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimColor(color); }
void GxPrimPointSize(float size) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimPointSize(size); }
void GxPrimLineWidth(float width) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->PrimLineWidth(width); }

void GxSceneSetClearColor(NTempest::CImVector color) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->SceneSetClearColor(color); }
NTempest::CImVector* GxSceneClearColor(NTempest::CImVector* color) { return g_theGxDevicePtr == nullptr ? color : g_theGxDevicePtr->SceneClearColor(color); }
void GxScenePresent(unsigned mask) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->ScenePresent(mask); }
void GxSceneClear(unsigned mask) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->SceneClear(mask); }

int GxTexCreate(unsigned width, unsigned height, EGxTexFormat format, CGxTexFlags flags, void* userArg, CGxTexUserCallback userFunc, CGxTex** texture)
{
    if (g_theGxDevicePtr == nullptr) return 0;
    if (texture == nullptr) return 0;
    *texture = nullptr;
    const bool created = g_theGxDevicePtr->TexCreate(width, height, format, flags, userArg, userFunc, texture);
    return created ? 1 : 0;
}

int GxTexCreate(EGxTexTarget target, unsigned width, unsigned height, unsigned depth, EGxTexFormat format, EGxTexFormat dataFormat, CGxTexFlags flags, void* userArg, CGxTexUserCallback userFunc, CGxTex** texture)
{
    if (g_theGxDevicePtr == nullptr || texture == nullptr) return 0;
    *texture = nullptr;
    return g_theGxDevicePtr->TexCreate(target, width, height, depth, format, dataFormat, flags, userArg, userFunc, texture) ? 1 : 0;
}

void GxTexUpdate(CGxTex* texture, const NTempest::CiRect* rect, bool immediate)
{
    if (g_theGxDevicePtr != nullptr && texture != nullptr) g_theGxDevicePtr->TexMarkForUpdate(texture, rect == nullptr ? NTempest::CiRect{} : *rect, immediate);
}

bool GxTexNeedsUpdate(const CGxTex* texture) { return g_theGxDevicePtr != nullptr && g_theGxDevicePtr->TexNeedsUpdate(texture); }
void GxTexSetUserData(CGxTex* texture, CGxTexUserCallback userFunc, void* userArg) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->TexSetUserData(texture, userFunc, userArg); }
void GxTexSetFlags(CGxTex* texture, CGxTexFlags flags) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->TexSetFlags(texture, flags); }

void GxTexDestroy(CGxTex* texture)
{
    if (texture == nullptr || g_theGxDevicePtr == nullptr) return;
    if (auto* gl = dynamic_cast<CGxDeviceOpenGl*>(g_theGxDevicePtr); gl != nullptr && texture->glTexture != 0) gl->TexDestroy(texture);
    else if (auto* d3d = dynamic_cast<CGxDeviceD3d*>(g_theGxDevicePtr); d3d != nullptr && texture->d3dTexture != nullptr) d3d->TexDestroy(texture);
    else {
        CGxTex* value = texture;
        g_theGxDevicePtr->TexDestroy(&value);
    }
    UntrackTexture(texture);
}

void GxTexParameters(const CGxTex* texture, CGxTexParms* parameters) { if (g_theGxDevicePtr != nullptr && parameters != nullptr) *parameters = g_theGxDevicePtr->TexParameters(texture); }
void GxTexParametersEx(const CGxTex* texture, CGxTexParmsEx* parameters) { if (g_theGxDevicePtr != nullptr && parameters != nullptr) *parameters = g_theGxDevicePtr->TexParametersEx(texture); }
void GxTexSetDataFormat(CGxTex* texture, EGxTexFormat format) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->TexSetDataFormat(texture, format); }
void GxTexFlags(const CGxTex* texture, CGxTexFlags* flags) { if (g_theGxDevicePtr != nullptr && flags != nullptr) *flags = g_theGxDevicePtr->TexFlags(texture); }
void GxTexGetDimensions(const CGxTex* texture, unsigned* width, unsigned* height) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->TexGetDimensions(texture, width, height); }

void GxXformSetViewport(float minX, float maxX, float minY, float maxY, float minZ, float maxZ) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ); }
void GxXformSetProjection(const NTempest::C44Matrix& matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformSetProjection(matrix); }
void GxXformSetView(const NTempest::C44Matrix& matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformSetView(matrix); }
void GxXformSetBones(unsigned numBones, const NTempest::C34Matrix* matrices) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformSetBones(numBones, matrices); }
void GxXformViewport(float* minX, float* maxX, float* minY, float* maxY, float* minZ, float* maxZ) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformViewport(minX, maxX, minY, maxY, minZ, maxZ); }
void GxXformProjection(NTempest::C44Matrix* matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformProjection(matrix); }
void GxXformView(NTempest::C44Matrix* matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformView(matrix); }
void GxXformBone(unsigned index, NTempest::C34Matrix* matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformBone(index, matrix); }

void GxXformViewProj(NTempest::C44Matrix* matrix)
{
    if (matrix == nullptr || g_theGxDevicePtr == nullptr) return;
    NTempest::C44Matrix view;
    NTempest::C44Matrix projection;
    g_theGxDevicePtr->XformView(&view);
    g_theGxDevicePtr->XformProjection(&projection);
    *matrix = view * projection;
}

void GxXformPush(EGxXform transform) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformPush(transform); }
void GxXformPush(EGxXform transform, const NTempest::C44Matrix& matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformPush(transform, matrix); }
void GxXformPop(EGxXform transform) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformPop(transform); }
void GxXformIdentity(EGxXform transform) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformIdentity(transform); }
void GxXformSet(EGxXform transform, const NTempest::C44Matrix& matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformSet(transform, matrix); }
void GxXformTranslate(EGxXform transform, const NTempest::C3Vector& translation) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformTranslate(transform, translation); }
void GxXformScale(EGxXform transform, const NTempest::C3Vector& scale) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformScale(transform, scale); }
void GxXformMult(EGxXform transform, const NTempest::C44Matrix& matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->XformMult(transform, matrix); }
void GxXform(EGxXform transform, NTempest::C44Matrix* matrix) { if (g_theGxDevicePtr != nullptr) g_theGxDevicePtr->Xform(transform, matrix); }

void GxPixelShaderCreate(CGxPixelShader** shader, const char* filename)
{
    if (g_theGxDevicePtr == nullptr || shader == nullptr) return;
    if (auto* d3d = dynamic_cast<CGxDeviceD3d*>(g_theGxDevicePtr); d3d != nullptr) d3d->PixelShaderCreate(shader, filename);
    else if (auto* gl = dynamic_cast<CGxDeviceOpenGl*>(g_theGxDevicePtr); gl != nullptr) gl->PixelShaderCreate(shader, filename);
    else if (*shader == nullptr) {
        *shader = new CGxPixelShader();
        g_theGxDevicePtr->m_pixelShaders.push_back(*shader);
    }
}

void GxPixelShaderDestroy(CGxPixelShader** shader)
{
    if (g_theGxDevicePtr == nullptr || shader == nullptr) return;
    if (auto* d3d = dynamic_cast<CGxDeviceD3d*>(g_theGxDevicePtr); d3d != nullptr) d3d->PixelShaderDestroy(shader);
    else if (auto* gl = dynamic_cast<CGxDeviceOpenGl*>(g_theGxDevicePtr); gl != nullptr) gl->PixelShaderDestroy(shader);
    else {
        delete *shader;
        *shader = nullptr;
    }
}

std::uint8_t* GxAllocVertexMem(unsigned bytes) { s_vertexMemory.resize(bytes); return s_vertexMemory.empty() ? nullptr : s_vertexMemory.data(); }
void GxFreeVertexMem() { s_vertexMemory.clear(); s_vertexMemory.shrink_to_fit(); }
std::uint8_t* GxAllocIndexMem(unsigned bytes) { s_indexMemory.resize(bytes); return s_indexMemory.empty() ? nullptr : s_indexMemory.data(); }
void GxFreeIndexMem() { s_indexMemory.clear(); s_indexMemory.shrink_to_fit(); }
std::uint8_t* GxAllocPixelMem(unsigned bytes) { s_pixelMemory.resize(bytes); return s_pixelMemory.empty() ? nullptr : s_pixelMemory.data(); }
void GxFreePixelMem() { s_pixelMemory.clear(); s_pixelMemory.shrink_to_fit(); }

EGxTexFormat GxGetGxTexFormat(int blitFormat)
{
    static constexpr EGxTexFormat formats[] = {GxTex_Argb8888, GxTex_Rgb565, GxTex_Dxt1, GxTex_Dxt3, GxTex_Dxt5, GxTex_Argb8888, GxTex_Argb8888, GxTex_Argb8888};
    return blitFormat >= 0 && blitFormat < 8 ? formats[blitFormat] : GxTex_Argb8888;
}

BlitFormat GxGetBlitFormat(EGxTexFormat format)
{
    switch (format) {
    case GxTex_Rgb565: return BlitFormat_Rgb565;
    case GxTex_Dxt1: return BlitFormat_Dxt1;
    case GxTex_Dxt3: return BlitFormat_Dxt3;
    case GxTex_Dxt5: return BlitFormat_Dxt5;
    case GxTex_Argb8888:
    default: return BlitFormat_Argb8888;
    }
}

void GxLogOpen(const char* filename) { CGxDevice::LogOpen(filename); }
void GxLogClose() { CGxDevice::LogClose(); }

void GxLog(const char* format, ...)
{
    if (format == nullptr) return;
    char buffer[2048]{};
    std::va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    CGxDevice::Log("%s", buffer);
}
