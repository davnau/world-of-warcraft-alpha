#include "ENGINE/Source/Gx/cgxdeviced3d11/cgxdeviced3d11.h"

#include <algorithm>

namespace {

DXGI_FORMAT TextureFormat(CGxTex::Format format)
{
    switch (format) {
    case CGxTex::Rgb565: return DXGI_FORMAT_B5G6R5_UNORM;
    case CGxTex::Dxt1: return DXGI_FORMAT_BC1_UNORM;
    case CGxTex::Dxt3: return DXGI_FORMAT_BC2_UNORM;
    case CGxTex::Dxt5: return DXGI_FORMAT_BC3_UNORM;
    case CGxTex::Argb8888:
    case CGxTex::Unknown:
    default: return DXGI_FORMAT_B8G8R8A8_UNORM;
    }
}

unsigned TexturePitch(CGxTex::Format format, unsigned width)
{
    switch (format) {
    case CGxTex::Rgb565: return width * 2U;
    case CGxTex::Dxt1: return std::max(1U, (width + 3U) / 4U) * 8U;
    case CGxTex::Dxt3:
    case CGxTex::Dxt5: return std::max(1U, (width + 3U) / 4U) * 16U;
    case CGxTex::Argb8888:
    case CGxTex::Unknown:
    default: return width * 4U;
    }
}

bool IsCompressed(CGxTex::Format format)
{
    return format == CGxTex::Dxt1 || format == CGxTex::Dxt3
        || format == CGxTex::Dxt5;
}

} // namespace

bool CGxDeviceD3d11::EnsureTexture(CGxTex* texture)
{
    if (texture == nullptr || m_device == nullptr || m_context == nullptr) return false;
    auto& resource = m_textureResources[texture];
    const DXGI_FORMAT format = TextureFormat(texture->format);
    const bool renderTarget = (texture->flags & CGxTexFlags::RenderTargetMask) != 0
        || texture == m_renderTargetTexture;
    if (resource.texture != nullptr && (resource.width != texture->width
        || resource.height != texture->height || resource.format != format
        || resource.renderTarget != renderTarget)) {
        resource = {};
        texture->needsCreation = true;
        texture->needsUpdate = true;
    }

    const bool shouldReportUpload = texture->needsUpdate;
    if (resource.texture == nullptr || texture->needsCreation || texture->needsUpdate) {
        if (!UploadTexture(texture, resource)) return false;
        if (shouldReportUpload) CGxDevice::ITexMarkAsUpdated(texture);
        texture->needsCreation = false;
        texture->needsUpdate = false;
    }
    texture->needsFlagUpdate = false;
    return resource.srv != nullptr || resource.rtv != nullptr;
}

bool CGxDeviceD3d11::UploadTexture(CGxTex* texture, TextureResource& resource)
{
    unsigned stride = 0;
    const void* texels = nullptr;
    if (texture->userFunc != nullptr) {
        texture->userFunc(GxTex_Lock, texture->width, texture->height,
            texture->depth, 0, texture->userArg, &stride, &texels);
    }

    const DXGI_FORMAT format = TextureFormat(texture->format);
    const bool renderTarget = (texture->flags & CGxTexFlags::RenderTargetMask) != 0
        || texture == m_renderTargetTexture;
    const bool generateMips = (texture->flags & CGxTexFlags::GenerateMipMapsMask) != 0
        && !IsCompressed(texture->format) && !renderTarget;
    const unsigned rowPitch = stride != 0 ? stride : TexturePitch(texture->dataFormat, texture->width);

    HRESULT result = S_OK;
    if (resource.texture == nullptr || texture->needsCreation) {
        D3D11_TEXTURE2D_DESC desc{};
        desc.Width = std::max(1U, texture->width);
        desc.Height = std::max(1U, texture->height);
        desc.MipLevels = generateMips ? 0U : 1U;
        desc.ArraySize = 1;
        desc.Format = format;
        desc.SampleDesc.Count = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE
            | (renderTarget || generateMips ? D3D11_BIND_RENDER_TARGET : 0U);
        desc.MiscFlags = generateMips ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0U;

        D3D11_SUBRESOURCE_DATA initial{};
        initial.pSysMem = texels;
        initial.SysMemPitch = rowPitch;
        const D3D11_SUBRESOURCE_DATA* initialPtr = texels != nullptr && !generateMips
            ? &initial : nullptr;
        resource = {};
        result = m_device->CreateTexture2D(&desc, initialPtr, &resource.texture);
        if (SUCCEEDED(result)) {
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
            srvDesc.Format = format;
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
            srvDesc.Texture2D.MostDetailedMip = 0;
            srvDesc.Texture2D.MipLevels = generateMips ? UINT(-1) : 1U;
            result = m_device->CreateShaderResourceView(resource.texture.Get(),
                &srvDesc, &resource.srv);
        }
        if (SUCCEEDED(result) && renderTarget) {
            result = m_device->CreateRenderTargetView(resource.texture.Get(),
                nullptr, &resource.rtv);
        }
        resource.format = format;
        resource.width = texture->width;
        resource.height = texture->height;
        resource.renderTarget = renderTarget;
        if (SUCCEEDED(result) && texels != nullptr && generateMips) {
            m_context->UpdateSubresource(resource.texture.Get(), 0, nullptr,
                texels, rowPitch, 0);
            m_context->GenerateMips(resource.srv.Get());
        }
    } else if (texels != nullptr) {
        m_context->UpdateSubresource(resource.texture.Get(), 0, nullptr,
            texels, rowPitch, 0);
        if (generateMips) m_context->GenerateMips(resource.srv.Get());
    }

    if (texture->userFunc != nullptr) {
        texture->userFunc(GxTex_Unlock, texture->width, texture->height,
            texture->depth, 0, texture->userArg, nullptr, nullptr);
    }
    if (FAILED(result)) {
        char message[160]{};
        sprintf_s(message, "D3D11: texture creation failed (%ux%u, HRESULT 0x%08lX)\n",
            texture->width, texture->height, static_cast<unsigned long>(result));
        OutputDebugStringA(message);
        resource = {};
        return false;
    }
    return true;
}

void CGxDeviceD3d11::ITexMarkAsUpdated(CGxTex* texture)
{
    EnsureTexture(texture);
}

void CGxDeviceD3d11::ITexBind(CGxTex* texture)
{
    if (EnsureTexture(texture)) CGxDevice::ITexBind(texture);
}

void CGxDeviceD3d11::BindTextures()
{
    if (m_context == nullptr) return;
    std::array<ID3D11ShaderResourceView*, 4> views{};
    for (unsigned unit = 0; unit < views.size(); ++unit) {
        CGxTex* texture = m_appState.m_textures[unit];
        if (texture == nullptr || texture == m_renderTargetTexture) continue;
        if (EnsureTexture(texture)) {
            views[unit] = m_textureResources[texture].srv.Get();
            CGxDevice::ITexBind(texture);
        }
    }
    m_context->PSSetShaderResources(0, static_cast<UINT>(views.size()), views.data());
}

void CGxDeviceD3d11::TexDestroy(CGxTex** texture)
{
    if (texture == nullptr || *texture == nullptr) return;
    CGxTex* target = *texture;
    if (target == m_renderTargetTexture) DeviceSetRenderTarget(GxBuffers_Color, nullptr, 0);
    for (unsigned unit = 0; unit < m_appState.m_textures.size(); ++unit) {
        if (m_appState.m_textures[unit] == target)
            RsSet(static_cast<EGxRenderState>(GxRs_Texture0 + unit), static_cast<CGxTex*>(nullptr));
    }
    m_textureResources.erase(target);
    CGxDevice::TexDestroy(texture);
}
