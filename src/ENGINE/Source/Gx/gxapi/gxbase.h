#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/cirect.h"
#include "ENGINE/Source/Tempest/crect.h"

#include <array>
#include <cstdint>
#include <cstring>
#include <vector>

class CGxBuf;
class CGxTex;
class CGxShader;

enum EGxApi : unsigned {
    GxApi_OpenGl = 0,
    GxApi_Direct3d = 1,
    GxApis_Last = 2,
};

enum EGxBuffer : unsigned {
    GxBuffers_Color = 0,
    GxBuffers_Depth = 1,
    GxBuffers_Last = 2,
};

enum EGxBufferBit : unsigned {
    GxBufferBit_Color = 0x1,
    GxBufferBit_Depth = 0x2,
};

enum EGxBlend : unsigned {
    GxBlend_Opaque = 0,
    GxBlend_AlphaKey = 1,
    GxBlend_Alpha = 2,
    GxBlend_Add = 3,
    GxBlend_Mod = 4,
    GxBlend_Mod2x = 5,
    GxBlend_ModAdd = 6,
    GxBlend_InvSrcAlphaAdd = 7,
    GxBlends_Last = 8,
};

enum EGxOverride : unsigned {
    GxOverride_PixelShader = 0,
    GxOverrides_Last = 1,
};

enum EGxBufOp : unsigned {
    GxBufOp_Nop = 0,
    GxBufOp_Fill = 1,
    GxBufOp_Assign = 2,
    GxBufOps_Last = 3,
};

enum EGxCmp : unsigned {
    GxCmp_LEqual = 0,
    GxCmp_Equal = 1,
    GxCmp_GEqual = 2,
    GxCmps_Last = 3,
};

enum EGxColorFormat : unsigned {
    GxCF_argb = 0,
    GxCF_rgba = 1,
    GxColorFormats_Last = 2,
};

enum EGxEnable : unsigned {
    Gx_Disable = 0,
    Gx_Enable = 1,
};

enum EGxFogStyle : unsigned {
    GxFog_Linear = 0,
    GxFog_Exp = 1,
    GxFog_Exp2 = 2,
    GxFogStyles_Last = 3,
};

enum EGxMasterEnables : unsigned {
    GxMasterEnable_Lighting = 0,
    GxMasterEnable_Fog = 1,
    GxMasterEnable_DepthTest = 2,
    GxMasterEnable_DepthWrite = 3,
    GxMasterEnable_Culling = 4,
    GxMasterEnable_ClearOnPresent = 5,
    GxMasterEnable_DoubleBuffering = 6,
    GxMasterEnable_NormalProjection = 7,
    GxMasterEnable_PolygonFill = 8,
    GxMasterEnables_Last = 9,
};

enum EGxPerfCounter : unsigned {
    GxPerf_FrameRate = 0,
    GxPerf_FrameNum = 1,
    GxPerf_Vertices = 2,
    GxPerf_Primitives = 3,
    GxPerf_Batches = 4,
    GxPerf_Textures = 5,
    GxPerf_TextureBytes = 6,
    GxPerf_TexUploads = 7,
    GxPerf_TexUploadBytes = 8,
    GxPerf_TexBinds = 9,
    GxPerf_TexBindBytes = 10,
    GxPerf_VertexBytes = 11,
    GxPerf_IndexBytes = 12,
    GxPerfCounters_Last = 13,
};

enum EGxBufWriteFreq : unsigned {
    GxBWF_Static = 0,
    GxBWF_Low = 1,
    GxBWF_Medium = 2,
    GxBWF_Dynamic = 3,
    GxBufWriteFreqs_Last = 4,
};

enum EGxVertexBufferFormat : unsigned {
    GxVBF_PN = 0,
    GxVBF_PNC = 1,
    GxVBF_PNT0 = 2,
    GxVBF_PNCT0 = 3,
    GxVBF_PNT0T1 = 4,
    GxVBF_PNCT0T1 = 5,
    GxVBF_PCT0 = 6,
    GxVBF_PC = 7,
    GxVBF_PT0T1 = 8,
    GxVertexBufferFormats_Last = 9,
};

enum EGxVertexMember : unsigned {
    GxVM_Indices = 0,
    GxVM_Vertex = 0,
    GxVM_Position = 0,
    GxVM_Normal = 1,
    GxVM_Color = 2,
    GxVM_Texture0 = 3,
    GxVM_Texture1 = 4,
    GxVM_Texture2 = 5,
    GxVM_Texture3 = 6,
    GxVertexMembers_Last = 7,
};

enum EGxVertexShader : unsigned {
    GxVS_PassThru = 0,
    GxVS_Skin = 1,
    GxVertexShaders_Last = 2,
};

enum EGxPrim : unsigned {
    GxPrim_Points = 0,
    GxPrim_Lines = 1,
    GxPrim_LineStrip = 2,
    GxPrim_Triangles = 3,
    GxPrim_TriangleStrip = 4,
    GxPrim_TriangleFan = 5,
    GxPrims_Last = 6,
};

enum EGxXform : unsigned {
    GxXform_Tex0 = 0,
    GxXform_Tex1 = 1,
    GxXform_Tex2 = 2,
    GxXform_Tex3 = 3,
    GxXform_World = 4,
    GxXform_Projection = 5,
    GxXform_View = 6,
    GxXforms_Last = 7,
};

enum EGxTexTarget : unsigned {
    GxTex_2d = 0,
    GxTex_CubeMap = 1,
    GxTexTargets_Last = 2,
};

enum EGxTexFilter : unsigned {
    GxTex_Nearest = 0,
    GxTex_Linear = 1,
    GxTex_LinearMipNearest = 2,
    GxTex_LinearMipLinear = 3,
    GxTex_Anisotropic = 4,
    GxTexFilters_Last = 5,
};

enum EGxTexFormat : unsigned {
    GxTex_Unknown = 0,
    GxTex_Argb8888 = 1,
    GxTex_Argb4444 = 2,
    GxTex_Argb1555 = 3,
    GxTex_Rgb565 = 4,
    GxTex_Dxt1 = 5,
    GxTex_Dxt3 = 6,
    GxTex_Dxt5 = 7,
    GxTexFormats_Last = 8,
};

enum EGxTexCommand : unsigned {
    GxTex_Lock = 0,
    GxTex_Latch = 1,
    GxTex_Unlock = 2,
    GxTexCommands_Last = 3,
};

enum EGxTexCubeFace : unsigned {
    GxTex_PositiveX = 0,
    GxTex_NegativeX = 1,
    GxTex_PositiveY = 2,
    GxTex_NegativeY = 3,
    GxTex_PositiveZ = 4,
    GxTex_NegativeZ = 5,
    GxTexCubeFaces_Last = 6,
};

enum EGxTexBlend : unsigned {
    GxTexBlend_Opaque = 0,
    GxTexBlend_Mod = 1,
    GxTexBlend_Decal = 2,
    GxTexBlend_Add = 3,
    GxTexBlend_Mod2x = 4,
    GxTexBlends_Last = 5,
};

enum EGxTexGen : unsigned {
    GxTexGen_Disable = 0,
    GxTexGen_Object = 1,
    GxTexGen_World = 2,
    GxTexGen_View = 3,
    GxTexGen_ViewReflection = 4,
    GxTexGen_ViewNormal = 5,
    GxTexGen_SphereMap = 6,
    GxTexGens_Last = 7,
};

enum EGxTextureShader : unsigned {
    GxTS_PassThru = 0,
    GxTS_Affine = 1,
    GxTS_Proj = 2,
    GxTextureShaders_Last = 3,
};

enum BlitFormat : unsigned {
    BlitFormat_Unknown = 0,
    BlitFormat_Rgb565 = 1,
    BlitFormat_Argb8888 = 2,
    BlitFormat_Dxt1 = 3,
    BlitFormat_Dxt3 = 4,
    BlitFormat_Dxt5 = 5,
};

enum EGxWM : unsigned {
    GxWM_Size = 0,
    GxWM_DisplayChange = 1,
    GxWM_Destroy = 2,
    GxWM_SetFocus = 3,
    GxWM_KillFocus = 4,
};

enum EGxRenderState : unsigned {
    // Exact CGxDevice::RsInit/CGxDeviceOpenGl::IRsSendToHw ABI recovered
    // from 0x593900 and 0x59F2E0.  Several reconstructed users retain the
    // original numeric render-state values, so this ordering is contractual.
    GxRs_PolygonOffset = 0,
    GxRs_MatDiffuse = 1,
    GxRs_MatEmissive = 2,
    GxRs_MatSpecular = 3,
    GxRs_MatSpecularExp = 4,
    GxRs_NormalizeNormals = 5,
    GxRs_SceneAmbient = 6,
    GxRs_Blend = 7,
    GxRs_AlphaRef = 8,
    GxRs_FogStyle = 9,
    GxRs_FogStart = 10,
    GxRs_FogEnd = 11,
    GxRs_FogDensity = 12,
    GxRs_FogColor = 13,
    GxRs_Lighting = 14,
    GxRs_Fog = 15,
    GxRs_DepthTest = 16,
    GxRs_DepthFunc = 17,
    GxRs_DepthWrite = 18,
    GxRs_Culling = 19,
    GxRs_Texture0 = 20,
    GxRs_Texture1 = 21,
    GxRs_Texture2 = 22,
    GxRs_Texture3 = 23,
    GxRs_TexBlend0 = 24,
    GxRs_TexBlend1 = 25,
    GxRs_TexBlend2 = 26,
    GxRs_TexBlend3 = 27,
    GxRs_TexLodBias0 = 28,
    GxRs_TexLodBias1 = 29,
    GxRs_TexLodBias2 = 30,
    GxRs_TexLodBias3 = 31,
    GxRs_TexGen0 = 32,
    GxRs_TexGen1 = 33,
    GxRs_TexGen2 = 34,
    GxRs_TexGen3 = 35,
    GxRs_TextureShader0 = 36,
    GxRs_TextureShader1 = 37,
    GxRs_TextureShader2 = 38,
    GxRs_TextureShader3 = 39,
    GxRs_PixelShader = 40,
    GxRs_VertexShader = 41,
    GxRenderStates_Last = 42,
};

unsigned GxVertexSize(EGxVertexBufferFormat format) noexcept;
unsigned GxVertexMemberOffset(EGxVertexBufferFormat format, unsigned member) noexcept;

struct CGxTexFlags {
    enum Bit : unsigned {
        FilterMask = 0x00000007,
        WrapUMask = 0x00000008,
        WrapVMask = 0x00000010,
        ForceMask = 0x00000020,
        GenerateMipMapsMask = 0x00000040,
        RenderTargetMask = 0x00000080,
        AnisotropyMask = 0x00001F00,
    };

    constexpr CGxTexFlags() noexcept = default;
    constexpr explicit CGxTexFlags(unsigned value) noexcept : bits(value) {}
    constexpr CGxTexFlags(
        EGxTexFilter filter,
        bool wrapU,
        bool wrapV,
        bool force,
        bool generateMipMaps,
        bool renderTarget,
        unsigned maxAnisotropy) noexcept
        : bits(static_cast<unsigned>(filter)
            | (wrapU ? WrapUMask : 0U)
            | (wrapV ? WrapVMask : 0U)
            | (force ? ForceMask : 0U)
            | (generateMipMaps ? GenerateMipMapsMask : 0U)
            | (renderTarget ? RenderTargetMask : 0U)
            | ((maxAnisotropy & 0x1FU) << 8)) {}

    constexpr operator unsigned() const noexcept { return bits; }
    constexpr EGxTexFilter Filter() const noexcept { return static_cast<EGxTexFilter>(bits & FilterMask); }
    constexpr bool WrapU() const noexcept { return (bits & WrapUMask) != 0; }
    constexpr bool WrapV() const noexcept { return (bits & WrapVMask) != 0; }
    constexpr bool Force() const noexcept { return (bits & ForceMask) != 0; }
    constexpr bool GenerateMipMaps() const noexcept { return (bits & GenerateMipMapsMask) != 0; }
    constexpr bool RenderTarget() const noexcept { return (bits & RenderTargetMask) != 0; }
    constexpr unsigned MaxAnisotropy() const noexcept { return (bits >> 8) & 0x1FU; }

    unsigned bits = 0;
};

struct CGxCaps {
    unsigned m_numTmus = 1;
    int m_pixelCenterOnEdge = 0;
    int m_texelCenterOnEdge = 0;
    unsigned m_maxTextureSize = 2048;
    int m_texOpAdd = 1;
    int m_texOpMod2X = 1;
    EGxColorFormat m_colorFormat = GxCF_argb;
    int m_texFmtDxt = 1;
    unsigned m_maxIndex = 0xFFFF;
    int m_generateMipMaps = 1;
    std::array<int, 8> m_rttFormat{};
    int m_rttOriginUpperLeft = 0;
    int m_pixelShaderTarget = 0;
    int m_vertexShaderTarget = 0;
    int m_texFilterTrilinear = 1;
    int m_texFilterAnisotropic = 0;
    unsigned m_maxTexAnisotropy = 1;
    int m_depthBias = 0;
    int m_mipMapLodBias = 0;
};

struct CGxGammaRamp {
    std::array<std::uint16_t, 256> red{};
    std::array<std::uint16_t, 256> green{};
    std::array<std::uint16_t, 256> blue{};

    void Set(float gamma);
};

struct CGxLight {
    // Recovered CGxLight is a 60-byte value type.  Bit 0 enables the light and
    // bit 1 selects a positional light; m_dir stores either the position or
    // the direction according to bit 1.
    std::uint32_t m_flags = 0;
    NTempest::C3Vector m_dir{0.0F, 0.0F, 1.0F};
    NTempest::CImVector m_ambColor{0xFF000000U};
    NTempest::CImVector m_dirColor{0xFFFFFFFFU};
    NTempest::CImVector m_specColor{};
    float m_ambIntensity = 1.0F;
    float m_dirIntensity = 1.0F;
    float m_specIntensity = 0.0F;
    float m_constantAttenuation = 0.0F;
    float m_linearAttenuation = 0.7F;
    float m_quadraticAttenuation = 0.03F;
    float m_attenStart = 0.0F;
    float m_attenEnd = 0.0F;
};

static_assert(sizeof(CGxLight) == 60, "recovered CGxLight ABI must remain 60 bytes");

struct CGxStateRegister {
    std::array<CGxLight, 8> m_lights{};
    std::array<std::uint32_t, 8> m_lightsDirty{};
    float m_lightLinearFalloff = 0.0F;
    float m_lightQuadraticFalloff = 0.0F;
    unsigned m_masterEnables = 0;
    std::array<std::uint32_t, GxRenderStates_Last> m_values{};
    std::array<NTempest::CImVector, 8> m_colors{};
    std::array<CGxTex*, 4> m_textures{};
};

class CGxMatrixStack {
public:
    CGxMatrixStack();
    ~CGxMatrixStack();

    void Identity();
    void Push();
    void Pop();
    NTempest::C44Matrix* Top() noexcept {
        // Exact CGxMatrixStack::Top side effects: requesting a mutable matrix
        // invalidates the cached hardware transform and the identity flag.
        m_dirty = true;
        m_flags[m_level] &= ~1U;
        return &m_mtx[m_level];
    }
    const NTempest::C44Matrix* Top() const noexcept { return &m_mtx[m_level]; }
    unsigned Level() const noexcept { return m_level; }

    std::array<NTempest::C44Matrix, 4> m_mtx{};
    std::array<unsigned, 4> m_flags{};
    unsigned m_level = 0;
    bool m_dirty = false;
};

struct CGxBatch {
    EGxPrim m_primType = GxPrim_Triangles;
    unsigned m_count = 0;
    unsigned m_start = 0;
    int m_minIndex = 0;
    int m_maxIndex = 0;
};

using CGxBufUserCallback = void (*)(void*, CGxBuf*);
using CGxTexUserCallback = void (*)(EGxTexCommand, unsigned, unsigned, unsigned, unsigned, void*, unsigned*, const void**);

struct CGxShaderParamValue {
    unsigned index = 0;
    unsigned vectorCount = 1;
    std::array<float, 16> values{};
    bool dirty = true;
};

struct CGxTexParms {
    CGxTexFlags flags{};
    EGxTexFormat format = GxTex_Argb8888;
    unsigned width = 0;
    unsigned height = 0;
};

struct CGxTexParmsEx : CGxTexParms {
    EGxTexFormat dataFormat = GxTex_Argb8888;
    unsigned depth = 1;
};
