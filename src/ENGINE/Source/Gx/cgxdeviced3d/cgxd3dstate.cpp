#include "ENGINE/Source/Gx/cgxdeviced3d/cgxd3dstate.h"

#include <cstring>

namespace {

constexpr unsigned kSrcBlend = 0;
constexpr unsigned kDstBlend = 1;
constexpr unsigned kMagFilter0 = 2;
constexpr unsigned kMinFilter0 = 6;
constexpr unsigned kMipFilter0 = 10;
constexpr unsigned kWrapU0 = 14;
constexpr unsigned kWrapV0 = 18;
constexpr unsigned kTextureTransformFlags0 = 22;
constexpr unsigned kMaxAnisotropy0 = 26;
constexpr unsigned kDiffuseMaterialSource = 30;
constexpr unsigned kAmbientMaterialSource = 31;
constexpr unsigned kAlphaBlendEnable = 32;
constexpr unsigned kAlphaTestEnable = 33;

} // namespace

void SetD3dColor(
    D3DCOLORVALUE& destination,
    const NTempest::CImVector& source,
    float scale)
{
    destination.r = static_cast<float>(source.r) * scale;
    destination.g = static_cast<float>(source.g) * scale;
    destination.b = static_cast<float>(source.b) * scale;
    destination.a = static_cast<float>(source.a) * scale;
}

void CGxDeviceD3d::IStateSync()
{
    IStateSyncLights();
    IStateSyncEnables();
    IStateSyncMaterial();
    IStateSyncTransforms();
}

void CGxDeviceD3d::IStateSyncLights()
{
    IForceLights();
}

void CGxDeviceD3d::IStateSyncEnables()
{
    if (m_d3dDevice == nullptr) {
        return;
    }
    m_d3dDevice->SetRenderState(
        D3DRS_FILLMODE,
        (m_masterEnables & 8U) != 0 ? D3DFILL_WIREFRAME : D3DFILL_SOLID);
}

void CGxDeviceD3d::IStateSyncMaterial()
{
    if (m_d3dDevice != nullptr) {
        m_d3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
        m_d3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, D3DMCS_COLOR1);
    }
}

void CGxDeviceD3d::IStateSyncTransforms()
{
    IXformSetWorld();
    for (unsigned textureUnit = 0; textureUnit < m_numTmus && textureUnit < 4; ++textureUnit) {
        if (m_textureGenerationEnabled[textureUnit]) {
            IXformSetTex(textureUnit, 1);
        }
    }
}

void CGxDeviceD3d::IStateSetD3DDefaults()
{
    if (m_d3dDevice == nullptr) {
        return;
    }
    m_d3dDevice->SetRenderState(D3DRS_ZENABLE, TRUE);
    m_d3dDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
    m_d3dDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    m_d3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);
    m_d3dDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
    for (StateD3dLight& state : m_d3dStatesLight) {
        state = {};
    }
}

void CGxDeviceD3d::ISetLight(unsigned which, const D3DLIGHT9& value, bool enabled)
{
    if (m_d3dDevice == nullptr || which >= 8) {
        return;
    }
    StateD3dLight& cache = m_d3dStatesLight[which];
    if (cache.which != static_cast<int>(which)
        || std::memcmp(&cache.value, &value, sizeof(value)) != 0) {
        m_d3dDevice->SetLight(which, &value);
        cache.value = value;
        cache.which = static_cast<int>(which);
    }
    if (cache.enabled != enabled) {
        m_d3dDevice->LightEnable(which, enabled ? TRUE : FALSE);
        cache.enabled = enabled;
    }
}

void CGxDeviceD3d::IForceLights()
{
    if (m_d3dDevice == nullptr) {
        return;
    }
    for (unsigned index = 0; index < 8; ++index) {
        const StateD3dLight& state = m_d3dStatesLight[index];
        if (state.which >= 0) {
            m_d3dDevice->SetLight(index, &state.value);
            m_d3dDevice->LightEnable(index, state.enabled ? TRUE : FALSE);
        }
    }
}

void CGxDeviceD3d::DsSet(unsigned state, unsigned value)
{
    if (m_d3dDevice == nullptr || state >= 43 || m_deviceState[state] == value) {
        return;
    }
    switch (state) {
    case kSrcBlend:
        m_d3dDevice->SetRenderState(D3DRS_SRCBLEND, value);
        break;
    case kDstBlend:
        m_d3dDevice->SetRenderState(D3DRS_DESTBLEND, value);
        break;
    case kMagFilter0:
    case kMagFilter0 + 1:
    case kMagFilter0 + 2:
    case kMagFilter0 + 3:
        m_d3dDevice->SetSamplerState(state - kMagFilter0, D3DSAMP_MAGFILTER, value);
        break;
    case kMinFilter0:
    case kMinFilter0 + 1:
    case kMinFilter0 + 2:
    case kMinFilter0 + 3:
        m_d3dDevice->SetSamplerState(state - kMinFilter0, D3DSAMP_MINFILTER, value);
        break;
    case kMipFilter0:
    case kMipFilter0 + 1:
    case kMipFilter0 + 2:
    case kMipFilter0 + 3:
        m_d3dDevice->SetSamplerState(state - kMipFilter0, D3DSAMP_MIPFILTER, value);
        break;
    case kWrapU0:
    case kWrapU0 + 1:
    case kWrapU0 + 2:
    case kWrapU0 + 3:
        m_d3dDevice->SetSamplerState(state - kWrapU0, D3DSAMP_ADDRESSU, value);
        break;
    case kWrapV0:
    case kWrapV0 + 1:
    case kWrapV0 + 2:
    case kWrapV0 + 3:
        m_d3dDevice->SetSamplerState(state - kWrapV0, D3DSAMP_ADDRESSV, value);
        break;
    case kTextureTransformFlags0:
    case kTextureTransformFlags0 + 1:
    case kTextureTransformFlags0 + 2:
    case kTextureTransformFlags0 + 3:
        m_d3dDevice->SetTextureStageState(
            state - kTextureTransformFlags0,
            D3DTSS_TEXTURETRANSFORMFLAGS,
            value);
        break;
    case kMaxAnisotropy0:
    case kMaxAnisotropy0 + 1:
    case kMaxAnisotropy0 + 2:
    case kMaxAnisotropy0 + 3:
        m_d3dDevice->SetSamplerState(state - kMaxAnisotropy0, D3DSAMP_MAXANISOTROPY, value);
        break;
    case kDiffuseMaterialSource:
        m_d3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, value);
        break;
    case kAmbientMaterialSource:
        m_d3dDevice->SetRenderState(D3DRS_AMBIENTMATERIALSOURCE, value);
        break;
    case kAlphaBlendEnable:
        m_d3dDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, value);
        break;
    case kAlphaTestEnable:
        m_d3dDevice->SetRenderState(D3DRS_ALPHATESTENABLE, value);
        break;
    default:
        break;
    }
    m_deviceState[state] = value;
}

void CGxDeviceD3d::ISetTexture(unsigned textureUnit, CGxTex* texture)
{
    if (m_d3dDevice == nullptr || textureUnit >= 4) {
        return;
    }
    m_d3dDevice->SetTexture(textureUnit, texture != nullptr ? texture->d3dTexture : nullptr);
    if (texture == nullptr && m_texEnable[textureUnit]) {
        m_d3dDevice->SetTextureStageState(textureUnit, D3DTSS_COLOROP, D3DTOP_DISABLE);
        m_d3dDevice->SetTextureStageState(textureUnit, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    }
    m_texEnable[textureUnit] = texture != nullptr;
}

void CGxDeviceD3d::ISetTexGen(unsigned textureUnit, unsigned texGen)
{
    if (m_d3dDevice == nullptr || textureUnit >= 4) {
        return;
    }
    DWORD index = textureUnit;
    if (texGen == 1 || texGen == 2 || texGen == 3) {
        index |= D3DTTFF_COUNT3;
    } else if (texGen >= 4) {
        index |= D3DTTFF_COUNT3 | D3DTTFF_PROJECTED;
    }
    m_d3dDevice->SetTextureStageState(textureUnit, D3DTSS_TEXCOORDINDEX, index);
    m_textureGenerationEnabled[textureUnit] = texGen != 0;
}

void CGxDeviceD3d::ISetTexLodBias(unsigned textureUnit, float bias)
{
    if (m_d3dDevice != nullptr && textureUnit < 4) {
        m_d3dDevice->SetSamplerState(
            textureUnit,
            D3DSAMP_MIPMAPLODBIAS,
            *reinterpret_cast<const DWORD*>(&bias));
    }
}

void CGxDeviceD3d::ISetTexBlend(unsigned textureUnit, unsigned blend)
{
    if (m_d3dDevice == nullptr || textureUnit >= 4 || !m_texEnable[textureUnit]) {
        return;
    }
    const D3DTEXTUREOP operation = blend == 0 ? D3DTOP_SELECTARG1 : D3DTOP_MODULATE;
    m_d3dDevice->SetTextureStageState(textureUnit, D3DTSS_COLOROP, operation);
    m_d3dDevice->SetTextureStageState(textureUnit, D3DTSS_ALPHAOP, operation);
}

void CGxDeviceD3d::IRsSendToHw(unsigned renderState)
{
    (void)renderState;
}
