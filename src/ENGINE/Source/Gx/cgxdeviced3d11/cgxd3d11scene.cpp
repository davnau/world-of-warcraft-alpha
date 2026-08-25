#include "ENGINE/Source/Gx/cgxdeviced3d11/cgxdeviced3d11.h"

#include <algorithm>
#include <cstring>

void CGxDeviceD3d11::SceneClear(unsigned mask)
{
    if (m_context == nullptr) return;
    const float color[4] = {
        m_clearColor.r / 255.0F,
        m_clearColor.g / 255.0F,
        m_clearColor.b / 255.0F,
        m_clearColor.a / 255.0F,
    };
    ID3D11RenderTargetView* renderTarget = m_backBufferRtv.Get();
    if (m_renderTargetTexture != nullptr) {
        const auto found = m_textureResources.find(m_renderTargetTexture);
        if (found != m_textureResources.end() && found->second.rtv != nullptr)
            renderTarget = found->second.rtv.Get();
    }
    if ((mask & GxBufferBit_Color) != 0 && renderTarget != nullptr)
        m_context->ClearRenderTargetView(renderTarget, color);
    if ((mask & GxBufferBit_Depth) != 0 && m_depthView != nullptr) {
        UINT flags = D3D11_CLEAR_DEPTH;
        if ((mask & 4U) != 0) flags |= D3D11_CLEAR_STENCIL;
        m_context->ClearDepthStencilView(m_depthView.Get(), flags, 1.0F, 0);
    }
}

void CGxDeviceD3d11::ScenePresent(unsigned mask)
{
    if (m_scrShotClick != 0) DeviceScreenShot();
    if (m_swapChain != nullptr) {
        const HRESULT result = m_swapChain->Present(m_format.vsync != 0 ? 1U : 0U, 0);
        if (FAILED(result)) {
            char message[128]{};
            sprintf_s(message, "D3D11: Present failed (HRESULT 0x%08lX)\n",
                static_cast<unsigned long>(result));
            OutputDebugStringA(message);
        }
    }
    CGxDevice::ScenePresent(mask);
    if ((m_appState.m_masterEnables & (1U << GxMasterEnable_ClearOnPresent)) != 0)
        SceneClear(mask);
}

void CGxDeviceD3d11::DeviceReadPixels(
    const NTempest::CiRect& inputRect,
    std::vector<NTempest::CImVector>& pixels)
{
    pixels.clear();
    if (m_device == nullptr || m_context == nullptr || m_swapChain == nullptr) return;

    Microsoft::WRL::ComPtr<ID3D11Texture2D> source;
    if (FAILED(m_swapChain->GetBuffer(0, IID_PPV_ARGS(&source)))) return;
    D3D11_TEXTURE2D_DESC sourceDesc{};
    source->GetDesc(&sourceDesc);

    NTempest::CiRect rect = inputRect;
    rect.minX = std::clamp(rect.minX, 0, static_cast<int>(sourceDesc.Width));
    rect.maxX = std::clamp(rect.maxX, rect.minX, static_cast<int>(sourceDesc.Width));
    rect.minY = std::clamp(rect.minY, 0, static_cast<int>(sourceDesc.Height));
    rect.maxY = std::clamp(rect.maxY, rect.minY, static_cast<int>(sourceDesc.Height));
    const unsigned width = static_cast<unsigned>(rect.Width());
    const unsigned height = static_cast<unsigned>(rect.Height());
    if (width == 0 || height == 0) return;

    D3D11_TEXTURE2D_DESC stagingDesc = sourceDesc;
    stagingDesc.Width = width;
    stagingDesc.Height = height;
    stagingDesc.MipLevels = 1;
    stagingDesc.ArraySize = 1;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags = 0;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> staging;
    if (FAILED(m_device->CreateTexture2D(&stagingDesc, nullptr, &staging))) return;

    const D3D11_BOX box{
        static_cast<UINT>(rect.minX), static_cast<UINT>(rect.minY), 0,
        static_cast<UINT>(rect.maxX), static_cast<UINT>(rect.maxY), 1};
    m_context->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0, source.Get(), 0, &box);
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped))) return;
    pixels.resize(static_cast<std::size_t>(width) * height);
    for (unsigned y = 0; y < height; ++y) {
        std::memcpy(pixels.data() + static_cast<std::size_t>(y) * width,
            static_cast<const std::uint8_t*>(mapped.pData)
                + static_cast<std::size_t>(y) * mapped.RowPitch,
            static_cast<std::size_t>(width) * sizeof(NTempest::CImVector));
    }
    m_context->Unmap(staging.Get(), 0);
}

void CGxDeviceD3d11::DeviceReadDepths(
    const NTempest::CiRect& inputRect,
    std::vector<float>& depths)
{
    depths.clear();
    if (m_device == nullptr || m_context == nullptr || m_depthTexture == nullptr) return;
    D3D11_TEXTURE2D_DESC sourceDesc{};
    m_depthTexture->GetDesc(&sourceDesc);

    NTempest::CiRect rect = inputRect;
    rect.minX = std::clamp(rect.minX, 0, static_cast<int>(sourceDesc.Width));
    rect.maxX = std::clamp(rect.maxX, rect.minX, static_cast<int>(sourceDesc.Width));
    rect.minY = std::clamp(rect.minY, 0, static_cast<int>(sourceDesc.Height));
    rect.maxY = std::clamp(rect.maxY, rect.minY, static_cast<int>(sourceDesc.Height));
    const unsigned width = static_cast<unsigned>(rect.Width());
    const unsigned height = static_cast<unsigned>(rect.Height());
    if (width == 0 || height == 0) return;

    D3D11_TEXTURE2D_DESC stagingDesc = sourceDesc;
    stagingDesc.Width = width;
    stagingDesc.Height = height;
    stagingDesc.Usage = D3D11_USAGE_STAGING;
    stagingDesc.BindFlags = 0;
    stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    stagingDesc.MiscFlags = 0;
    Microsoft::WRL::ComPtr<ID3D11Texture2D> staging;
    if (FAILED(m_device->CreateTexture2D(&stagingDesc, nullptr, &staging))) {
        CGxDevice::DeviceReadDepths(inputRect, depths);
        return;
    }
    const D3D11_BOX box{
        static_cast<UINT>(rect.minX), static_cast<UINT>(rect.minY), 0,
        static_cast<UINT>(rect.maxX), static_cast<UINT>(rect.maxY), 1};
    m_context->CopySubresourceRegion(staging.Get(), 0, 0, 0, 0,
        m_depthTexture.Get(), 0, &box);
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_context->Map(staging.Get(), 0, D3D11_MAP_READ, 0, &mapped))) {
        CGxDevice::DeviceReadDepths(inputRect, depths);
        return;
    }
    depths.resize(static_cast<std::size_t>(width) * height);
    constexpr float inverseDepth = 1.0F / 16777215.0F;
    for (unsigned y = 0; y < height; ++y) {
        const auto* row = reinterpret_cast<const std::uint32_t*>(
            static_cast<const std::uint8_t*>(mapped.pData)
            + static_cast<std::size_t>(y) * mapped.RowPitch);
        for (unsigned x = 0; x < width; ++x)
            depths[static_cast<std::size_t>(y) * width + x]
                = static_cast<float>(row[x] & 0x00FFFFFFU) * inverseDepth;
    }
    m_context->Unmap(staging.Get(), 0);
}

void CGxDeviceD3d11::DeviceSetRenderTarget(
    EGxBuffer buffer, CGxTex* texture, unsigned plane)
{
    (void)plane;
    if (m_context == nullptr) return;
    if (buffer == GxBuffers_Color) {
        m_renderTargetTexture = texture;
        ID3D11RenderTargetView* renderTarget = m_backBufferRtv.Get();
        ID3D11DepthStencilView* depth = m_depthView.Get();
        if (texture != nullptr && EnsureTexture(texture)) {
            auto& resource = m_textureResources[texture];
            if (resource.rtv != nullptr) renderTarget = resource.rtv.Get();
            if (resource.width != static_cast<unsigned>(m_defWindow.r)
                || resource.height != static_cast<unsigned>(m_defWindow.b)) depth = nullptr;
        }
        m_context->OMSetRenderTargets(1, &renderTarget, depth);
    }
    CGxDevice::DeviceSetRenderTarget(buffer, texture, plane);
    UpdateViewport();
}
