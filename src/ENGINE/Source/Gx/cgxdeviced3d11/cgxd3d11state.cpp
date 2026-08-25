#include "ENGINE/Source/Gx/cgxdeviced3d11/cgxdeviced3d11.h"

#include <algorithm>
#include <cfloat>
#include <cmath>
#include <cstring>

namespace {

float StateFloat(const std::array<std::uint32_t, GxRenderStates_Last>& values,
    EGxRenderState state)
{
    float value = 0.0F;
    const auto bits = values[state];
    std::memcpy(&value, &bits, sizeof(value));
    return value;
}

void SetColor(float destination[4], std::uint32_t bits)
{
    const NTempest::CImVector color(bits);
    destination[0] = color.r / 255.0F;
    destination[1] = color.g / 255.0F;
    destination[2] = color.b / 255.0F;
    destination[3] = color.a / 255.0F;
}

void SetScaledColor(float destination[4], const NTempest::CImVector& color,
    float intensity)
{
    destination[0] = color.r * intensity / 255.0F;
    destination[1] = color.g * intensity / 255.0F;
    destination[2] = color.b * intensity / 255.0F;
    destination[3] = color.a * intensity / 255.0F;
}

} // namespace

void CGxDeviceD3d11::IRsSendToHw(unsigned)
{
    m_pipelineDirty = true;
}

void CGxDeviceD3d11::ApplyPipeline()
{
    if (m_device == nullptr || m_context == nullptr) return;
    IRsSync(false);
    for (CGxTex* texture : m_appState.m_textures) {
        if (texture != nullptr && texture->needsFlagUpdate) m_pipelineDirty = true;
    }

    if (m_pipelineDirty) {
        const auto& values = m_appState.m_values;
        const unsigned master = m_appState.m_masterEnables;

        D3D11_BLEND_DESC blendDesc{};
        auto& target = blendDesc.RenderTarget[0];
        target.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
        const unsigned blend = values[GxRs_Blend];
        target.BlendOp = D3D11_BLEND_OP_ADD;
        target.BlendOpAlpha = D3D11_BLEND_OP_ADD;
        target.SrcBlendAlpha = D3D11_BLEND_ONE;
        target.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
        if (blend != GxBlend_Opaque && blend != GxBlend_AlphaKey) {
            target.BlendEnable = TRUE;
            static constexpr D3D11_BLEND sourceBlend[GxBlends_Last] = {
                D3D11_BLEND_ONE, D3D11_BLEND_ONE,
                D3D11_BLEND_SRC_ALPHA, D3D11_BLEND_SRC_ALPHA,
                D3D11_BLEND_DEST_COLOR, D3D11_BLEND_DEST_COLOR,
                D3D11_BLEND_DEST_COLOR, D3D11_BLEND_INV_SRC_ALPHA,
            };
            static constexpr D3D11_BLEND destinationBlend[GxBlends_Last] = {
                D3D11_BLEND_ZERO, D3D11_BLEND_ZERO,
                D3D11_BLEND_INV_SRC_ALPHA, D3D11_BLEND_ONE,
                D3D11_BLEND_ZERO, D3D11_BLEND_SRC_COLOR,
                D3D11_BLEND_ONE, D3D11_BLEND_ONE,
            };
            const unsigned validBlend = std::min(blend, GxBlends_Last - 1U);
            target.SrcBlend = sourceBlend[validBlend];
            target.DestBlend = destinationBlend[validBlend];
        }
        m_blendState.Reset();
        m_device->CreateBlendState(&blendDesc, &m_blendState);

        D3D11_DEPTH_STENCIL_DESC depthDesc{};
        depthDesc.DepthEnable = ((master & (1U << GxMasterEnable_DepthTest)) != 0
            && values[GxRs_DepthTest] != 0) ? TRUE : FALSE;
        depthDesc.DepthWriteMask = ((master & (1U << GxMasterEnable_DepthWrite)) != 0
            && values[GxRs_DepthWrite] != 0)
            ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
        static constexpr D3D11_COMPARISON_FUNC comparisons[GxCmps_Last] = {
            D3D11_COMPARISON_LESS_EQUAL,
            D3D11_COMPARISON_EQUAL,
            D3D11_COMPARISON_GREATER_EQUAL,
        };
        depthDesc.DepthFunc = comparisons[std::min(values[GxRs_DepthFunc], GxCmps_Last - 1U)];
        m_depthState.Reset();
        m_device->CreateDepthStencilState(&depthDesc, &m_depthState);

        D3D11_RASTERIZER_DESC rasterDesc{};
        rasterDesc.FillMode = (master & (1U << GxMasterEnable_PolygonFill)) != 0
            ? D3D11_FILL_SOLID : D3D11_FILL_WIREFRAME;
        rasterDesc.CullMode = ((master & (1U << GxMasterEnable_Culling)) != 0
            && values[GxRs_Culling] != 0) ? D3D11_CULL_BACK : D3D11_CULL_NONE;
        rasterDesc.FrontCounterClockwise = values[GxRs_Culling] == 2U ? FALSE : TRUE;
        rasterDesc.DepthClipEnable = TRUE;
        rasterDesc.ScissorEnable = TRUE;
        const float polygonOffset = StateFloat(values, GxRs_PolygonOffset) * -16.0F;
        rasterDesc.SlopeScaledDepthBias = polygonOffset;
        rasterDesc.DepthBiasClamp = 0.0F;
        m_rasterState.Reset();
        m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);

        for (unsigned unit = 0; unit < m_samplerStates.size(); ++unit) {
            D3D11_SAMPLER_DESC samplerDesc{};
            const CGxTex* texture = m_appState.m_textures[unit];
            const CGxTexFlags flags(texture != nullptr ? texture->flags : 0U);
            switch (flags.Filter()) {
            case GxTex_Nearest:
                samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
                break;
            case GxTex_Linear:
                samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                break;
            case GxTex_LinearMipNearest:
                samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
                break;
            case GxTex_Anisotropic:
                samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
                break;
            case GxTex_LinearMipLinear:
            default:
                samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
                break;
            }
            samplerDesc.AddressU = flags.WrapU()
                ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressV = flags.WrapV()
                ? D3D11_TEXTURE_ADDRESS_WRAP : D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
            samplerDesc.MipLODBias = StateFloat(values,
                static_cast<EGxRenderState>(GxRs_TexLodBias0 + unit));
            samplerDesc.MaxAnisotropy = std::clamp(flags.MaxAnisotropy(), 1U, 16U);
            samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
            samplerDesc.MinLOD = 0.0F;
            samplerDesc.MaxLOD = FLT_MAX;
            m_samplerStates[unit].Reset();
            m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[unit]);
        }
        m_pipelineDirty = false;
    }

    const float blendFactor[4]{};
    m_context->OMSetBlendState(m_blendState.Get(), blendFactor, 0xFFFFFFFFU);
    m_context->OMSetDepthStencilState(m_depthState.Get(), 0);
    m_context->RSSetState(m_rasterState.Get());
    std::array<ID3D11SamplerState*, 4> samplers{};
    for (unsigned i = 0; i < samplers.size(); ++i) samplers[i] = m_samplerStates[i].Get();
    m_context->PSSetSamplers(0, static_cast<UINT>(samplers.size()), samplers.data());
    m_context->IASetInputLayout(m_inputLayout.Get());
    m_context->VSSetShader(m_vertexShader11.Get(), nullptr, 0);
    m_context->PSSetShader(m_pixelShader11.Get(), nullptr, 0);
    UpdateShaderConstants();
    BindTextures();
}

void CGxDeviceD3d11::UpdateShaderConstants()
{
    if (m_context == nullptr || m_constantBuffer == nullptr) return;
    ShaderConstants constants{};
    const auto& values = m_appState.m_values;

    const auto& worldStack = static_cast<const CGxMatrixStack&>(m_xforms[GxXform_World]);
    const auto& viewStack = static_cast<const CGxMatrixStack&>(m_xforms[GxXform_View]);
    std::memcpy(constants.world, worldStack.Top()->Data(), sizeof(constants.world));
    std::memcpy(constants.view, viewStack.Top()->Data(), sizeof(constants.view));
    const NTempest::C44Matrix projection = D3dProjection(m_projection);
    std::memcpy(constants.projection, projection.Data(), sizeof(constants.projection));
    for (unsigned unit = 0; unit < 4; ++unit) {
        const auto& textureStack = static_cast<const CGxMatrixStack&>(
            m_xforms[GxXform_Tex0 + unit]);
        std::memcpy(constants.textureMatrix[unit], textureStack.Top()->Data(),
            sizeof(constants.textureMatrix[unit]));
    }

    SetColor(constants.materialDiffuse, values[GxRs_MatDiffuse]);
    SetColor(constants.materialEmissive, values[GxRs_MatEmissive]);
    SetColor(constants.sceneAmbient, values[GxRs_SceneAmbient]);
    SetColor(constants.fogColor, values[GxRs_FogColor]);
    constants.fogParams[0] = StateFloat(values, GxRs_FogStart);
    constants.fogParams[1] = StateFloat(values, GxRs_FogEnd);
    constants.fogParams[2] = StateFloat(values, GxRs_FogDensity);
    constants.fogParams[3] = static_cast<float>(values[GxRs_FogStyle]);

    for (unsigned unit = 0; unit < 4; ++unit) {
        constants.stageBlend[unit] = static_cast<float>(values[GxRs_TexBlend0 + unit]);
        constants.stageTexGen[unit] = static_cast<float>(values[GxRs_TexGen0 + unit]);
        constants.stageTexShader[unit] = static_cast<float>(values[GxRs_TextureShader0 + unit]);
        constants.textureEnabled[unit] = m_appState.m_textures[unit] != nullptr ? 1.0F : 0.0F;
    }

    constants.renderFlags[0] = ((m_appState.m_masterEnables
        & (1U << GxMasterEnable_Lighting)) != 0 && values[GxRs_Lighting] != 0) ? 1.0F : 0.0F;
    constants.renderFlags[1] = std::min(values[GxRs_AlphaRef], 255U) / 255.0F;
    constants.renderFlags[2] = ((m_appState.m_masterEnables
        & (1U << GxMasterEnable_Fog)) != 0 && values[GxRs_Fog] != 0) ? 1.0F : 0.0F;
    constants.renderFlags[3] = values[GxRs_NormalizeNormals] != 0 ? 1.0F : 0.0F;

    for (unsigned index = 0; index < m_appState.m_lights.size(); ++index) {
        const CGxLight& light = m_appState.m_lights[index];
        auto& destination = constants.lights[index];
        destination.vector[0] = (light.m_flags & 2U) != 0 ? light.m_dir.x : -light.m_dir.x;
        destination.vector[1] = (light.m_flags & 2U) != 0 ? light.m_dir.y : -light.m_dir.y;
        destination.vector[2] = (light.m_flags & 2U) != 0 ? light.m_dir.z : -light.m_dir.z;
        destination.vector[3] = (light.m_flags & 2U) != 0 ? 1.0F : 0.0F;
        SetScaledColor(destination.diffuse, light.m_dirColor, light.m_dirIntensity);
        SetScaledColor(destination.ambient, light.m_ambColor, light.m_ambIntensity);
        destination.attenuation[0] = light.m_constantAttenuation;
        destination.attenuation[1] = light.m_linearAttenuation;
        destination.attenuation[2] = light.m_quadraticAttenuation;
        destination.attenuation[3] = (light.m_flags & 1U) != 0 ? 1.0F : 0.0F;
    }

    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (SUCCEEDED(m_context->Map(m_constantBuffer.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mapped))) {
        std::memcpy(mapped.pData, &constants, sizeof(constants));
        m_context->Unmap(m_constantBuffer.Get(), 0);
    }
    ID3D11Buffer* buffer = m_constantBuffer.Get();
    m_context->VSSetConstantBuffers(0, 1, &buffer);
    m_context->PSSetConstantBuffers(0, 1, &buffer);
}

void CGxDeviceD3d11::XformSetViewport(
    float minX, float maxX, float minY, float maxY, float minZ, float maxZ)
{
    CGxDevice::XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    UpdateViewport();
}

void CGxDeviceD3d11::UpdateViewport()
{
    if (m_context == nullptr) return;
    const float width = std::max(1.0F, m_currentWindow.r - m_currentWindow.l);
    const float height = std::max(1.0F, m_currentWindow.b - m_currentWindow.t);
    D3D11_VIEWPORT viewport{};
    viewport.TopLeftX = m_viewportMin.x * width;
    viewport.TopLeftY = (1.0F - m_viewportMax.y) * height;
    viewport.Width = std::max(1.0F, (m_viewportMax.x - m_viewportMin.x) * width);
    viewport.Height = std::max(1.0F, (m_viewportMax.y - m_viewportMin.y) * height);
    viewport.MinDepth = m_viewportMin.z;
    viewport.MaxDepth = m_viewportMax.z;
    m_context->RSSetViewports(1, &viewport);

    D3D11_RECT scissor{
        static_cast<LONG>(std::floor(viewport.TopLeftX)),
        static_cast<LONG>(std::floor(viewport.TopLeftY)),
        static_cast<LONG>(std::ceil(viewport.TopLeftX + viewport.Width)),
        static_cast<LONG>(std::ceil(viewport.TopLeftY + viewport.Height)),
    };
    m_context->RSSetScissorRects(1, &scissor);
}

NTempest::C44Matrix CGxDeviceD3d11::D3dProjection(
    const NTempest::C44Matrix& source) const
{
    NTempest::C44Matrix projection = source;
    // C44Matrix is copied directly to D3DMATRIX.  Consequently c3 is D3D
    // _34 and d2 is D3D _43; keep these accesses aligned with the recovered
    // CGxDeviceD3d::XformSetProjection implementation.
    if (std::fabs(projection.c3 - 1.0F) >= 0.00000023841858F
        && std::fabs(projection.c3) >= 0.00000023841858F) {
        projection *= 1.0F / projection.c3;
    }
    if (projection.d3 == 0.0F) {
        const float nearPlane = -(projection.d2 / (projection.c2 + 1.0F));
        const float farPlane = -(projection.d2 / (projection.c2 - 1.0F));
        projection.c2 = farPlane / (farPlane - nearPlane);
        projection.d2 = farPlane * nearPlane / (nearPlane - farPlane);
    } else {
        const float inverse = 1.0F / projection.c2;
        const float nearPlane = (-1.0F - projection.d2) * inverse;
        const float farPlane = (1.0F - projection.d2) * inverse;
        projection.c2 = 1.0F / (farPlane - nearPlane);
        projection.d2 = nearPlane / (nearPlane - farPlane);
    }
    if ((m_appState.m_masterEnables & (1U << GxMasterEnable_NormalProjection)) == 0
        && projection.d3 != 1.0F) {
        // CGxDeviceD3d::XformSetProjection multiplies matProj * shrink,
        // where shrink is diag(0.2, 0.2, 0.2, 1).  This scales the first
        // three D3D matrix columns, including _41/_42/_43, not the first
        // three rows.
        projection.a0 *= 0.2F; projection.b0 *= 0.2F;
        projection.c0 *= 0.2F; projection.d0 *= 0.2F;
        projection.a1 *= 0.2F; projection.b1 *= 0.2F;
        projection.c1 *= 0.2F; projection.d1 *= 0.2F;
        projection.a2 *= 0.2F; projection.b2 *= 0.2F;
        projection.c2 *= 0.2F; projection.d2 *= 0.2F;
    }
    return projection;
}

void CGxDeviceD3d11::XformSetProjection(const NTempest::C44Matrix& matrix)
{
    CGxDevice::XformSetProjection(matrix);
}

void CGxDeviceD3d11::XformSetView(const NTempest::C44Matrix& matrix)
{
    CGxDevice::XformSetView(matrix);
}

void CGxDeviceD3d11::XformSetBones(
    unsigned count, const NTempest::C34Matrix* matrices)
{
    CGxDevice::XformSetBones(count, matrices);
}
