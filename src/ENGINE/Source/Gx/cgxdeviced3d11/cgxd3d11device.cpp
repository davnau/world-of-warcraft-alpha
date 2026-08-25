#include "ENGINE/Source/Gx/cgxdeviced3d11/cgxdeviced3d11.h"
#include "ENGINE/Source/Base/coordinate.h"

#include <d3dcompiler.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdio>
#include <filesystem>
#include <string>

namespace {

constexpr wchar_t kWindowClassName[] = L"WoWClient2D3D11";

void ReportD3dError(const char* operation, HRESULT result)
{
    char message[256]{};
    sprintf_s(message, "D3D11: %s failed (HRESULT 0x%08lX)\n",
        operation, static_cast<unsigned long>(result));
    OutputDebugStringA(message);
    std::fputs(message, stderr);
}

std::filesystem::path FindShaderFile()
{
    std::array<wchar_t, MAX_PATH> modulePath{};
    GetModuleFileNameW(nullptr, modulePath.data(), static_cast<DWORD>(modulePath.size()));
    const auto executableDirectory = std::filesystem::path(modulePath.data()).parent_path();
    const std::array<std::filesystem::path, 4> candidates{{
        executableDirectory / L"shaders" / L"gx_legacy.hlsl",
        std::filesystem::current_path() / L"shaders" / L"gx_legacy.hlsl",
        std::filesystem::current_path() / L".." / L"shaders" / L"gx_legacy.hlsl",
        L"E:\\World of Warcraft\\shaders\\gx_legacy.hlsl",
    }};
    for (const auto& candidate : candidates) {
        std::error_code error;
        if (std::filesystem::is_regular_file(candidate, error)) return candidate;
    }
    return candidates.front();
}

unsigned ClientWidth(HWND window, unsigned fallback)
{
    RECT rect{};
    return window != nullptr && GetClientRect(window, &rect) && rect.right > rect.left
        ? static_cast<unsigned>(rect.right - rect.left) : std::max(1U, fallback);
}

unsigned ClientHeight(HWND window, unsigned fallback)
{
    RECT rect{};
    return window != nullptr && GetClientRect(window, &rect) && rect.bottom > rect.top
        ? static_cast<unsigned>(rect.bottom - rect.top) : std::max(1U, fallback);
}

} // namespace

CGxDeviceD3d11::CGxDeviceD3d11()
{
    m_api = GxApi_Direct3d;
}

CGxDeviceD3d11::~CGxDeviceD3d11()
{
    DeviceDestroy();
}

CGxDevice* CGxDevice::NewD3d()
{
    return new CGxDeviceD3d11();
}

bool CGxDeviceD3d11::CreateOwnedWindow(const CGxFormat& format)
{
    WNDCLASSEXW windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = WindowProcD3d11;
    windowClass.hInstance = GetModuleHandleW(nullptr);
    windowClass.hCursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(32512));
    windowClass.lpszClassName = kWindowClassName;
    m_windowClass = RegisterClassExW(&windowClass);
    if (m_windowClass == 0 && GetLastError() != ERROR_CLASS_ALREADY_EXISTS) return false;

    const DWORD style = format.window != 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP;
    RECT windowRect{0, 0, std::max(1, format.size.x), std::max(1, format.size.y)};
    if (format.window != 0) AdjustWindowRect(&windowRect, style, FALSE);
    const int width = windowRect.right - windowRect.left;
    const int height = windowRect.bottom - windowRect.top;
    const int x = format.pos.x != 0 ? format.pos.x : CW_USEDEFAULT;
    const int y = format.pos.y != 0 ? format.pos.y : CW_USEDEFAULT;

    m_hwnd = CreateWindowExW(0, kWindowClassName, L"World of Warcraft", style,
        x, y, width, height, nullptr, nullptr, GetModuleHandleW(nullptr), this);
    if (m_hwnd == nullptr) return false;
    ShowWindow(m_hwnd, SW_SHOW);
    UpdateWindow(m_hwnd);
    m_windowVisible = true;
    return true;
}

bool CGxDeviceD3d11::CreateDeviceResources(const CGxFormat& format)
{
    const unsigned width = ClientWidth(m_hwnd, static_cast<unsigned>(std::max(1, format.size.x)));
    const unsigned height = ClientHeight(m_hwnd, static_cast<unsigned>(std::max(1, format.size.y)));

    DXGI_SWAP_CHAIN_DESC swapDesc{};
    swapDesc.BufferDesc.Width = width;
    swapDesc.BufferDesc.Height = height;
    swapDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    swapDesc.SampleDesc.Count = 1;
    swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapDesc.BufferCount = 2;
    swapDesc.OutputWindow = m_hwnd;
    swapDesc.Windowed = TRUE;
    swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    constexpr D3D_FEATURE_LEVEL featureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
    };
    D3D_FEATURE_LEVEL featureLevel{};
    const HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr,
        D3D11_CREATE_DEVICE_BGRA_SUPPORT,
        featureLevels, static_cast<UINT>(std::size(featureLevels)),
        D3D11_SDK_VERSION, &swapDesc, &m_swapChain, &m_device,
        &featureLevel, &m_context);
    if (FAILED(result)) {
        ReportD3dError("D3D11CreateDeviceAndSwapChain", result);
        return false;
    }

    m_caps.m_numTmus = 4;
    m_caps.m_maxTextureSize = featureLevel >= D3D_FEATURE_LEVEL_11_0 ? 16384U : 8192U;
    m_caps.m_maxIndex = 0xFFFFU;
    m_caps.m_texFmtDxt = 1;
    m_caps.m_generateMipMaps = 1;
    m_caps.m_texFilterTrilinear = 1;
    m_caps.m_texFilterAnisotropic = 1;
    m_caps.m_maxTexAnisotropy = 16;
    m_caps.m_depthBias = 1;
    m_caps.m_mipMapLodBias = 1;
    m_caps.m_rttOriginUpperLeft = 1;
    m_caps.m_pixelCenterOnEdge = 0;
    m_caps.m_texelCenterOnEdge = 0;

    if (!CreateSizeResources(width, height) || !CreateShaders()) return false;
    RsInit();
    IRsForceUpdate();
    m_pipelineDirty = true;
    UpdateViewport();
    return true;
}

bool CGxDeviceD3d11::CreateSizeResources(unsigned width, unsigned height)
{
    if (m_device == nullptr || m_swapChain == nullptr || width == 0 || height == 0) return false;
    CoordinateSetDisplaySize(static_cast<float>(width), static_cast<float>(height));
    ReleaseSizeResources();

    Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
    HRESULT result = m_swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
    if (FAILED(result)) {
        ReportD3dError("IDXGISwapChain::GetBuffer", result);
        return false;
    }
    result = m_device->CreateRenderTargetView(backBuffer.Get(), nullptr, &m_backBufferRtv);
    if (FAILED(result)) {
        ReportD3dError("CreateRenderTargetView", result);
        return false;
    }

    D3D11_TEXTURE2D_DESC depthDesc{};
    depthDesc.Width = width;
    depthDesc.Height = height;
    depthDesc.MipLevels = 1;
    depthDesc.ArraySize = 1;
    depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthDesc.SampleDesc.Count = 1;
    depthDesc.Usage = D3D11_USAGE_DEFAULT;
    depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    result = m_device->CreateTexture2D(&depthDesc, nullptr, &m_depthTexture);
    if (FAILED(result)) {
        ReportD3dError("CreateTexture2D(depth)", result);
        return false;
    }
    result = m_device->CreateDepthStencilView(m_depthTexture.Get(), nullptr, &m_depthView);
    if (FAILED(result)) {
        ReportD3dError("CreateDepthStencilView", result);
        return false;
    }

    ID3D11RenderTargetView* rtv = m_backBufferRtv.Get();
    m_context->OMSetRenderTargets(1, &rtv, m_depthView.Get());
    m_renderTargetTexture = nullptr;
    m_currentWindow = NTempest::CRect(0.0F, 0.0F,
        static_cast<float>(width), static_cast<float>(height));
    m_defWindow = m_currentWindow;
    UpdateViewport();
    return true;
}

bool CGxDeviceD3d11::CreateShaders()
{
    const auto shaderPath = FindShaderFile();
    Microsoft::WRL::ComPtr<ID3DBlob> vertexCode;
    Microsoft::WRL::ComPtr<ID3DBlob> pixelCode;
    Microsoft::WRL::ComPtr<ID3DBlob> errors;
    constexpr UINT flags = D3DCOMPILE_ENABLE_STRICTNESS | D3DCOMPILE_OPTIMIZATION_LEVEL3;

    HRESULT result = D3DCompileFromFile(shaderPath.c_str(), nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "GxVertexMain", "vs_4_0", flags, 0,
        &vertexCode, &errors);
    if (FAILED(result)) {
        if (errors != nullptr) OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
        ReportD3dError("compile gx_legacy vertex shader", result);
        return false;
    }
    errors.Reset();
    result = D3DCompileFromFile(shaderPath.c_str(), nullptr,
        D3D_COMPILE_STANDARD_FILE_INCLUDE, "GxPixelMain", "ps_4_0", flags, 0,
        &pixelCode, &errors);
    if (FAILED(result)) {
        if (errors != nullptr) OutputDebugStringA(static_cast<const char*>(errors->GetBufferPointer()));
        ReportD3dError("compile gx_legacy pixel shader", result);
        return false;
    }

    result = m_device->CreateVertexShader(vertexCode->GetBufferPointer(),
        vertexCode->GetBufferSize(), nullptr, &m_vertexShader11);
    if (FAILED(result)) return false;
    result = m_device->CreatePixelShader(pixelCode->GetBufferPointer(),
        pixelCode->GetBufferSize(), nullptr, &m_pixelShader11);
    if (FAILED(result)) return false;

    const D3D11_INPUT_ELEMENT_DESC inputElements[] = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            static_cast<UINT>(offsetof(Vertex, position)), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
            static_cast<UINT>(offsetof(Vertex, normal)), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,
            static_cast<UINT>(offsetof(Vertex, color)), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0,
            static_cast<UINT>(offsetof(Vertex, tex0)), D3D11_INPUT_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 1, DXGI_FORMAT_R32G32_FLOAT, 0,
            static_cast<UINT>(offsetof(Vertex, tex1)), D3D11_INPUT_PER_VERTEX_DATA, 0},
    };
    result = m_device->CreateInputLayout(inputElements,
        static_cast<UINT>(std::size(inputElements)), vertexCode->GetBufferPointer(),
        vertexCode->GetBufferSize(), &m_inputLayout);
    if (FAILED(result)) return false;

    D3D11_BUFFER_DESC constantDesc{};
    constantDesc.ByteWidth = sizeof(ShaderConstants);
    constantDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    result = m_device->CreateBuffer(&constantDesc, nullptr, &m_constantBuffer);
    if (FAILED(result)) return false;

    ID3D11Buffer* constants = m_constantBuffer.Get();
    m_context->VSSetConstantBuffers(0, 1, &constants);
    m_context->PSSetConstantBuffers(0, 1, &constants);
    return true;
}

void CGxDeviceD3d11::ReleaseSizeResources()
{
    if (m_context != nullptr) m_context->OMSetRenderTargets(0, nullptr, nullptr);
    m_depthView.Reset();
    m_depthTexture.Reset();
    m_backBufferRtv.Reset();
}

bool CGxDeviceD3d11::DeviceCreate(void* windowProc, const CGxFormat& format)
{
    DeviceDestroy();
    m_clientWindowProc = reinterpret_cast<WindowProc>(windowProc);
    m_ownWindow = true;
    m_format = format;
    m_windowed = format.window != 0;
    if (!CreateOwnedWindow(format) || !CreateDeviceResources(format)) {
        DeviceDestroy();
        return false;
    }
    return true;
}

bool CGxDeviceD3d11::DeviceCreate(HWND clientWindow, const CGxFormat& format)
{
    DeviceDestroy();
    if (clientWindow == nullptr) return false;
    m_hwnd = clientWindow;
    m_ownWindow = false;
    m_format = format;
    m_windowed = format.window != 0;
    m_windowVisible = IsWindowVisible(m_hwnd) != FALSE;
    return CreateDeviceResources(format);
}

void CGxDeviceD3d11::DeviceDestroy()
{
    if (m_context != nullptr) {
        m_context->ClearState();
        m_context->Flush();
    }
    for (CGxTex* texture : m_textures) {
        if (texture != nullptr) {
            texture->needsCreation = true;
            texture->needsUpdate = true;
        }
    }
    m_textureResources.clear();
    m_samplerStates = {};
    m_rasterState.Reset();
    m_depthState.Reset();
    m_blendState.Reset();
    m_dynamicIndexBuffer.Reset();
    m_dynamicVertexBuffer.Reset();
    m_constantBuffer.Reset();
    m_inputLayout.Reset();
    m_pixelShader11.Reset();
    m_vertexShader11.Reset();
    ReleaseSizeResources();
    m_swapChain.Reset();
    m_context.Reset();
    m_device.Reset();
    m_vertices.clear();
    m_indices.clear();
    m_vertexCapacity = 0;
    m_indexCapacity = 0;

    const HWND oldWindow = m_hwnd;
    const bool destroyWindow = m_ownWindow && oldWindow != nullptr;
    m_hwnd = nullptr;
    m_windowVisible = false;
    m_renderTargetTexture = nullptr;
    if (destroyWindow && IsWindow(oldWindow)) DestroyWindow(oldWindow);
    if (m_windowClass != 0) {
        UnregisterClassW(kWindowClassName, GetModuleHandleW(nullptr));
        m_windowClass = 0;
    }
    m_ownWindow = false;
    m_clientWindowProc = nullptr;
    CGxDevice::DeviceDestroy();
}

bool CGxDeviceD3d11::DeviceSetFormat(CGxFormat& format)
{
    if (m_hwnd == nullptr || m_swapChain == nullptr) {
        m_format = format;
        return CGxDevice::DeviceSetFormat(format);
    }

    m_format = format;
    m_windowed = format.window != 0;
    if (m_ownWindow) {
        const DWORD style = format.window != 0 ? WS_OVERLAPPEDWINDOW : WS_POPUP;
        SetWindowLongPtrW(m_hwnd, GWL_STYLE, style);
        RECT rect{0, 0, std::max(1, format.size.x), std::max(1, format.size.y)};
        if (format.window != 0) AdjustWindowRect(&rect, style, FALSE);
        SetWindowPos(m_hwnd, nullptr, format.pos.x, format.pos.y,
            rect.right - rect.left, rect.bottom - rect.top,
            SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }

    const unsigned width = ClientWidth(m_hwnd, static_cast<unsigned>(std::max(1, format.size.x)));
    const unsigned height = ClientHeight(m_hwnd, static_cast<unsigned>(std::max(1, format.size.y)));
    ReleaseSizeResources();
    const HRESULT result = m_swapChain->ResizeBuffers(0, width, height,
        DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);
    if (FAILED(result)) {
        ReportD3dError("ResizeBuffers(format)", result);
        return false;
    }
    return CreateSizeResources(width, height);
}

void CGxDeviceD3d11::DeviceWM(EGxWM message, const NTempest::CRect* rect, int)
{
    switch (message) {
    case GxWM_Size:
        if (rect != nullptr) DeviceSetDefWindow(rect);
        if (!m_resizing && m_swapChain != nullptr && rect != nullptr) {
            const unsigned width = static_cast<unsigned>(std::max(0.0F, rect->r - rect->l));
            const unsigned height = static_cast<unsigned>(std::max(0.0F, rect->b - rect->t));
            if (width != 0 && height != 0) {
                m_resizing = true;
                ReleaseSizeResources();
                if (SUCCEEDED(m_swapChain->ResizeBuffers(0, width, height,
                    DXGI_FORMAT_UNKNOWN, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH))) {
                    CreateSizeResources(width, height);
                }
                m_resizing = false;
            }
        }
        break;
    case GxWM_SetFocus:
        m_windowVisible = true;
        break;
    case GxWM_KillFocus:
        m_windowVisible = IsWindowVisible(m_hwnd) != FALSE;
        break;
    case GxWM_Destroy:
        m_windowVisible = false;
        break;
    default:
        break;
    }
}

HWND CGxDeviceD3d11::DeviceWindow() const
{
    return m_hwnd;
}

void CGxDeviceD3d11::CapsWindowSize(NTempest::CRect* destination) const
{
    if (destination != nullptr) *destination = *DeviceCurWindow();
}

void CGxDeviceD3d11::CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const
{
    if (destination == nullptr) return;
    RECT rect{};
    if (m_hwnd == nullptr || !GetClientRect(m_hwnd, &rect)) {
        *destination = *DeviceCurWindow();
        return;
    }
    MapWindowPoints(m_hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
    *destination = NTempest::CRect(static_cast<float>(rect.left),
        static_cast<float>(rect.top), static_cast<float>(rect.right),
        static_cast<float>(rect.bottom));
}

int CGxDeviceD3d11::CapsIsWindowVisible() const
{
    return m_hwnd != nullptr && m_windowVisible && IsWindowVisible(m_hwnd) ? 1 : 0;
}

void CGxDeviceD3d11::DeviceOverride(EGxOverride value, unsigned flags)
{
    if (value == GxOverride_PixelShader) m_shaderOverride = flags != 0;
    CGxDevice::DeviceOverride(value, flags);
}

LRESULT CALLBACK CGxDeviceD3d11::WindowProcD3d11(
    HWND window, UINT message, WPARAM wParam, LPARAM lParam)
{
    auto* device = reinterpret_cast<CGxDeviceD3d11*>(
        GetWindowLongPtrW(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const auto* create = reinterpret_cast<const CREATESTRUCTW*>(lParam);
        device = static_cast<CGxDeviceD3d11*>(create->lpCreateParams);
        SetWindowLongPtrW(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(device));
    }
    if (device != nullptr) {
        switch (message) {
        case WM_SIZE: {
            const NTempest::CRect rect(0.0F, 0.0F,
                static_cast<float>(LOWORD(lParam)), static_cast<float>(HIWORD(lParam)));
            device->DeviceWM(GxWM_Size, &rect, 0);
            break;
        }
        case WM_SETFOCUS: device->DeviceWM(GxWM_SetFocus, nullptr, 0); break;
        case WM_KILLFOCUS: device->DeviceWM(GxWM_KillFocus, nullptr, 0); break;
        case WM_DESTROY: device->DeviceWM(GxWM_Destroy, nullptr, 0); break;
        case WM_ERASEBKGND: return 1;
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            BeginPaint(window, &paint);
            EndPaint(window, &paint);
            return 0;
        }
        default: break;
        }
        if (device->m_clientWindowProc != nullptr)
            return device->m_clientWindowProc(window, message, wParam, lParam);
    }
    return DefWindowProcW(window, message, wParam, lParam);
}
