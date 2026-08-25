#include "ENGINE/Source/Gx/cgxdeviceopengl/w32/cgxogldevicew32.h"

#include "ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.h"
#include "ENGINE/Source/Os/W32/input.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <algorithm>

namespace {

constexpr char kWindowClassName[] = "GxWindowClassOpenGl";
constexpr char kWindowTitle[] = "World of Warcraft";
CGxDeviceOpenGl::WindowProc s_clientWindowProc = nullptr;

} // namespace

void CGxDeviceOpenGl::DeviceCreatePbuffer()
{
    if (!wglARBPbuffer || m_hdc == nullptr || wglCreatePbufferARB == nullptr
        || wglGetPbufferDCARB == nullptr) {
        return;
    }
    const int pixelFormat = GetPixelFormat(m_hdc);
    if (pixelFormat == 0) {
        return;
    }
    m_hPbuffer = wglCreatePbufferARB(
        m_hdc,
        pixelFormat,
        static_cast<int>(m_maxTextureSize),
        static_cast<int>(m_maxTextureSize),
        nullptr);
    if (m_hPbuffer == nullptr) {
        return;
    }
    m_hPbufferDC = wglGetPbufferDCARB(m_hPbuffer);
    m_hPbufferRC = wglGetCurrentContext();
}

void CGxDeviceOpenGl::DeviceQueryPbuffer()
{
    if (m_hPbuffer == nullptr || wglQueryPbufferARB == nullptr) {
        return;
    }
    int lost = 0;
    if (wglQueryPbufferARB(m_hPbuffer, 0x8246, &lost) && lost != 0) {
        DeviceDestroyPbuffer();
        DeviceCreatePbuffer();
    }
}

void CGxDeviceOpenGl::DeviceDestroyPbuffer()
{
    if (m_hPbuffer != nullptr) {
        if (wglReleasePbufferDCARB != nullptr && m_hPbufferDC != nullptr) {
            wglReleasePbufferDCARB(m_hPbuffer, m_hPbufferDC);
        }
        if (wglDestroyPbufferARB != nullptr) {
            wglDestroyPbufferARB(m_hPbuffer);
        }
    }
    m_hPbuffer = nullptr;
    m_hPbufferDC = nullptr;
    m_hPbufferRC = nullptr;
}

void CGxDeviceOpenGl::IDevSetFocus(int focus, const CGxFormat& format)
{
    if (m_hwnd == nullptr) {
        return;
    }
    if (format.window != 0) {
        ShowWindow(m_hwnd, focus != 0 ? SW_SHOW : SW_HIDE);
        return;
    }
    if (focus != 0) {
        DEVMODEA mode{};
        mode.dmSize = sizeof(mode);
        DISPLAY_DEVICEA display{};
        display.cb = sizeof(display);
        if (EnumDisplayDevicesA(nullptr, 0, &display, 0)
            && EnumDisplaySettingsA(display.DeviceName, format.apiSpecificModeID, &mode)) {
            ChangeDisplaySettingsExA(display.DeviceName, &mode, nullptr, CDS_FULLSCREEN, nullptr);
        }
        SetWindowPos(
            m_hwnd,
            HWND_TOP,
            format.pos.x,
            format.pos.y,
            format.size.x,
            format.size.y,
            SWP_SHOWWINDOW | SWP_NOACTIVATE);
        ShowWindow(m_hwnd, SW_SHOWMAXIMIZED);
    } else {
        ChangeDisplaySettingsExA(nullptr, nullptr, nullptr, 0, nullptr);
        ShowWindow(m_hwnd, SW_MINIMIZE);
    }
}

bool CGxDeviceOpenGl::SetFormatMode(CGxFormat& format)
{
    if (format.window != 0) {
        format.apiSpecificModeID = 0;
        return true;
    }

    DISPLAY_DEVICEA display{};
    display.cb = sizeof(display);
    if (!EnumDisplayDevicesA(nullptr, 0, &display, 0)) {
        return false;
    }
    for (DWORD modeIndex = 0;; ++modeIndex) {
        DEVMODEA mode{};
        mode.dmSize = sizeof(mode);
        if (!EnumDisplaySettingsA(display.DeviceName, modeIndex, &mode)) {
            break;
        }
        const DWORD bpp = format.colorFormat == CGxFormat::Fmt_Rgb565 ? 16 : 32;
        if (mode.dmBitsPerPel == bpp
            && mode.dmDisplayFrequency == format.refreshRate
            && mode.dmPelsWidth == static_cast<DWORD>(format.size.x)
            && mode.dmPelsHeight == static_cast<DWORD>(format.size.y)) {
            format.apiSpecificModeID = modeIndex;
            return true;
        }
    }
    return false;
}

bool CGxDeviceOpenGl::IDevAttachGlContext(const CGxFormat& format)
{
    if (m_hwnd == nullptr || m_hdc != nullptr || m_hglrc != nullptr) {
        return false;
    }
    m_hdc = GetDC(m_hwnd);
    m_hglrc = AttachGlContext(m_hwnd, &format);
    if (m_hdc == nullptr || m_hglrc == nullptr) {
        IDevRemoveGlContext();
        return false;
    }
    if (!wglMakeCurrent(m_hdc, m_hglrc)) {
        IDevRemoveGlContext();
        return false;
    }
    BindGlExtensions();
    m_numTmus = static_cast<unsigned>(std::clamp(glExtMultiTextureCount, 1, 4));
    if (wglEXTSwapControl && wglSwapIntervalEXT != nullptr) {
        wglSwapIntervalEXT(format.vsync != 0 ? 1 : 0);
    }
    IStateSetContextDefaults();
    DeviceCreatePbuffer();
    m_context = true;
    ISceneBegin(3);
    return true;
}

void CGxDeviceOpenGl::IDevRemoveGlContext()
{
    DeviceDestroyPbuffer();
    if (m_hglrc != nullptr) {
        IShaderForceRecreation();
        UnbindGlExtensions();
        RemoveGlContext(m_hglrc);
        m_hglrc = nullptr;
    }
    if (m_hdc != nullptr) {
        if (m_hwnd != nullptr) {
            ReleaseDC(m_hwnd, m_hdc);
        }
        m_hdc = nullptr;
    }
    m_context = false;
}

LRESULT CALLBACK CGxDeviceOpenGl::WindowProcGl(
    HWND window,
    UINT message,
    WPARAM wParam,
    LPARAM lParam)
{
    CGxDeviceOpenGl* device = reinterpret_cast<CGxDeviceOpenGl*>(
        GetWindowLongPtrA(window, GWLP_USERDATA));
    if (message == WM_NCCREATE) {
        const CREATESTRUCTA* create = reinterpret_cast<const CREATESTRUCTA*>(lParam);
        device = static_cast<CGxDeviceOpenGl*>(create->lpCreateParams);
        SetWindowLongPtrA(window, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(device));
    }
    if (device != nullptr) {
        switch (message) {
        case WM_SIZE: {
            NTempest::CRect rect(0.0F, 0.0F,
                static_cast<float>(LOWORD(lParam)),
                static_cast<float>(HIWORD(lParam)));
            device->DeviceWM(GxWM_Size, &rect, 0);
            break;
        }
        case WM_SETFOCUS:
            device->DeviceWM(GxWM_SetFocus, nullptr, 0);
            break;
        case WM_KILLFOCUS:
            device->DeviceWM(GxWM_KillFocus, nullptr, 0);
            break;
        case WM_DESTROY:
            device->DeviceWM(GxWM_Destroy, nullptr, 0);
            break;
        case WM_PAINT: {
            PAINTSTRUCT paint{};
            BeginPaint(window, &paint);
            EndPaint(window, &paint);
            return 0;
        }
        default:
            break;
        }
    }
    if (message == WM_ERASEBKGND)
        return 0;
    if (s_clientWindowProc != nullptr) {
        return s_clientWindowProc(window, message, wParam, lParam);
    }
    return DefWindowProcA(window, message, wParam, lParam);
}

void CGxDeviceOpenGl::DeviceWM(
    WindowMessage message,
    const NTempest::CRect* rect,
    int param)
{
    (void)param;
    switch (message) {
    case GxWM_Size:
        if (rect != nullptr) {
            DeviceSetDefWindow(rect);
        }
        DeviceQueryPbuffer();
        break;
    case GxWM_Destroy:
        IDevRemoveGlContext();
        break;
    case GxWM_SetFocus:
        if (!m_format.window && m_hglrc == nullptr) {
            IDevSetFocus(1, m_format);
            IDevAttachGlContext(m_format);
        }
        break;
    case GxWM_KillFocus:
        if (!m_format.window) {
            IDevRemoveGlContext();
            IDevSetFocus(0, m_format);
        }
        break;
    default:
        break;
    }
}

bool CGxDeviceOpenGl::DeviceCreate(WindowProc windowProc, const CGxFormat& format)
{
    s_clientWindowProc = windowProc;
    m_ownhwnd = true;
    m_format = format;
    m_hwndClass = WindowClassCreate(WindowProcGl);
    if (m_hwndClass == 0 || !SetFormatMode(m_format)) {
        DeviceDestroy();
        return false;
    }
    m_hwnd = WindowCreate(this, m_format);
    m_currentWindow = NTempest::CRect(
        0.0F, 0.0F,
        static_cast<float>(m_format.size.x),
        static_cast<float>(m_format.size.y));
    if (m_hwnd == nullptr || !IDevAttachGlContext(m_format)) {
        DeviceDestroy();
        return false;
    }
    IDevSetFocus(1, m_format);
    return true;
}

bool CGxDeviceOpenGl::DeviceCreate(HWND clientWindow, const CGxFormat& format)
{
    m_ownhwnd = false;
    m_hwnd = clientWindow;
    m_format = format;
    m_currentWindow = NTempest::CRect(
        0.0F, 0.0F,
        static_cast<float>(format.size.x),
        static_cast<float>(format.size.y));
    return IDevAttachGlContext(m_format);
}

void CGxDeviceOpenGl::DeviceDestroy()
{
    IDevRemoveGlContext();
    if (m_ownhwnd && m_hwnd != nullptr) {
        WindowDestroy(m_hwnd);
    }
    if (m_hwndClass != 0) {
        WindowClassDestroy(m_hwndClass);
    }
    m_hwnd = nullptr;
    m_ownhwnd = false;
    s_clientWindowProc = nullptr;
}

bool CGxDeviceOpenGl::DeviceSetFormat(CGxFormat& format)
{
    if (!m_ownhwnd) {
        return false;
    }
    IDevRemoveGlContext();
    if (m_hwnd != nullptr) {
        WindowDestroy(m_hwnd);
    }
    m_format = format;
    if (!SetFormatMode(m_format)) {
        return false;
    }
    m_hwnd = WindowCreate(this, m_format);
    if (m_hwnd == nullptr || !IDevAttachGlContext(m_format)) {
        return false;
    }
    IDevSetFocus(1, m_format);
    return true;
}

void CGxDeviceOpenGl::DeviceSetBaseMipLevel(unsigned baseMipLevel)
{
    (void)baseMipLevel;
    IShaderForceRecreation();
}

void CGxDeviceOpenGl::DeviceSetGamma(float gamma)
{
    (void)gamma;
}

void CGxDeviceOpenGl::DeviceSetRenderTarget(unsigned buffer, CGxTex* texture, unsigned plane)
{
    (void)buffer;
    (void)plane;
    if (texture != nullptr && m_hPbufferDC != nullptr && m_hPbufferRC != nullptr) {
        wglMakeCurrent(m_hPbufferDC, m_hPbufferRC);
    } else if (m_hdc != nullptr && m_hglrc != nullptr) {
        wglMakeCurrent(m_hdc, m_hglrc);
    }
}

void CGxDeviceOpenGl::DeviceSetTextureQuality(int force32)
{
    (void)force32;
    IShaderForceRecreation();
}

HWND CGxDeviceOpenGl::DeviceWindow() const
{
    return m_hwnd;
}

bool WindowClassCreate(CGxDeviceOpenGl::WindowProc windowProc)
{
    WNDCLASSEXA windowClass{};
    windowClass.cbSize = sizeof(windowClass);
    windowClass.style = CS_OWNDC;
    windowClass.lpfnWndProc = windowProc;
    windowClass.hInstance = GetModuleHandleA(nullptr);
    windowClass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
    windowClass.lpszClassName = kWindowClassName;
    return RegisterClassExA(&windowClass) != 0;
}

void WindowClassDestroy(ATOM& atom)
{
    if (atom != 0) {
        UnregisterClassA(kWindowClassName, GetModuleHandleA(nullptr));
        atom = 0;
    }
}

void WindowDestroy(HWND& window)
{
    if (window != nullptr) {
        DestroyWindow(window);
        window = nullptr;
    }
}

HWND WindowCreate(CGxDeviceOpenGl* device, const CGxFormat& format)
{
    const DWORD style = (format.window != 0
        ? WS_OVERLAPPEDWINDOW
        : WS_POPUP | WS_VISIBLE) | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;
    return CreateWindowExA(
        WS_EX_APPWINDOW,
        kWindowClassName,
        kWindowTitle,
        style,
        format.pos.x,
        format.pos.y,
        format.size.x,
        format.size.y,
        nullptr,
        nullptr,
        GetModuleHandleA(nullptr),
        device);
}

bool IsGlDisplayModeGood(const DEVMODEA& mode)
{
    return (mode.dmBitsPerPel == 16 || mode.dmBitsPerPel == 32)
        && mode.dmPelsWidth >= 640
        && mode.dmPelsHeight >= 480;
}

bool CGxDevice::OpenGlEnumFormats(std::vector<CGxFormat>& formats)
{
    formats.clear();
    DISPLAY_DEVICEA display{};
    display.cb = sizeof(display);
    if (!EnumDisplayDevicesA(nullptr, 0, &display, 0)
        || (display.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0) {
        return false;
    }
    for (DWORD modeIndex = 0;; ++modeIndex) {
        DEVMODEA mode{};
        mode.dmSize = sizeof(mode);
        if (!EnumDisplaySettingsA(display.DeviceName, modeIndex, &mode)) {
            break;
        }
        if (!IsGlDisplayModeGood(mode)) {
            continue;
        }
        const CGxFormat::Format color = mode.dmBitsPerPel == 16
            ? CGxFormat::Fmt_Rgb565
            : CGxFormat::Fmt_Argb8888;
        CGxFormat format(
            0,
            NTempest::C2iVector(
                static_cast<int>(mode.dmPelsWidth),
                static_cast<int>(mode.dmPelsHeight)),
            color,
            CGxFormat::Fmt_Ds160,
            mode.dmDisplayFrequency,
            0,
            0,
            0);
        format.apiSpecificModeID = modeIndex;
        formats.push_back(format);
    }
    return !formats.empty();
}

void CGxDeviceOpenGl::CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const
{
    if (destination == nullptr) {
        return;
    }
    RECT rect{
        0,
        0,
        static_cast<LONG>(m_currentWindow.r),
        static_cast<LONG>(m_currentWindow.b)};
    MapWindowPoints(m_hwnd, nullptr, reinterpret_cast<POINT*>(&rect), 2);
    destination->l = static_cast<float>(rect.left);
    destination->t = static_cast<float>(rect.top);
    destination->r = static_cast<float>(rect.right);
    destination->b = static_cast<float>(rect.bottom);
}
