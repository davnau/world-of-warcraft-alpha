#include "ENGINE/Source/Gx/cgxdeviceopengl/w32/glwndsupport.h"

namespace {

void LogPixelFormatFailure(const char* message)
{
    OutputDebugStringA("Gx: ");
    OutputDebugStringA(message);
    OutputDebugStringA("\n");
}

struct PixelFormatBits {
    BYTE color = 16;
    BYTE red = 5;
    BYTE green = 6;
    BYTE blue = 5;
    BYTE alpha = 0;
};

PixelFormatBits ColorBits(CGxFormat::Format format)
{
    switch (format) {
    case CGxFormat::Fmt_ArgbX888:
        return {24, 8, 8, 8, 0};
    case CGxFormat::Fmt_Argb8888:
        return {24, 8, 8, 8, 8};
    case CGxFormat::Fmt_Argb2101010:
        return {30, 10, 10, 10, 2};
    case CGxFormat::Fmt_Rgb565:
    default:
        return {16, 5, 6, 5, 0};
    }
}

void DepthBits(CGxFormat::Format format, BYTE& depth, BYTE& stencil)
{
    depth = 16;
    stencil = 0;
    switch (format) {
    case CGxFormat::Fmt_Ds24X:
        depth = 24;
        break;
    case CGxFormat::Fmt_Ds248:
        depth = 24;
        stencil = 8;
        break;
    case CGxFormat::Fmt_Ds320:
        depth = 32;
        break;
    case CGxFormat::Fmt_Ds160:
    default:
        break;
    }
}

} // namespace

HGLRC AttachGlContext(HWND window, const CGxFormat* format)
{
    HDC dc = GetDC(window);
    if (dc == nullptr || SetupPixelFormat(dc, format) == 0) {
        return nullptr;
    }
    return wglCreateContext(dc);
}

int SetupPixelFormat(HDC dc, const CGxFormat* format)
{
    if (dc == nullptr || format == nullptr) {
        return 0;
    }

    const PixelFormatBits color = ColorBits(format->colorFormat);
    BYTE depthBits = 0;
    BYTE stencilBits = 0;
    DepthBits(format->depthFormat, depthBits, stencilBits);

    PIXELFORMATDESCRIPTOR pfd{};
    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = color.color;
    pfd.cRedBits = color.red;
    pfd.cGreenBits = color.green;
    pfd.cBlueBits = color.blue;
    pfd.cAlphaBits = color.alpha;
    pfd.cDepthBits = depthBits;
    pfd.cStencilBits = stencilBits;

    const int pixelFormat = ChoosePixelFormat(dc, &pfd);
    if (pixelFormat == 0) {
        LogPixelFormatFailure("ChoosePixelFormat failed");
        return 0;
    }

    if (DescribePixelFormat(dc, pixelFormat, sizeof(pfd), &pfd) == 0) {
        LogPixelFormatFailure("DescribePixelFormat failed");
        return 0;
    }

    bool matches = true;
    if (format->window == 0) {
        if (format->colorFormat != CGxFormat::Fmt_Argb8888) {
            matches = pfd.cAlphaBits == color.alpha;
        }
        matches = matches && pfd.cRedBits == color.red
            && pfd.cGreenBits == color.green
            && pfd.cBlueBits == color.blue;
    }
    if (format->depthFormat != CGxFormat::Fmt_Ds24X) {
        matches = matches && pfd.cStencilBits == stencilBits;
    }
    matches = matches && pfd.cDepthBits == depthBits;
    if (!matches) {
        LogPixelFormatFailure("Selected pixel format does not match requested format");
        return 0;
    }

    if (!SetPixelFormat(dc, pixelFormat, &pfd)) {
        LogPixelFormatFailure("SetPixelFormat failed");
        return 0;
    }
    return 1;
}

void RemoveGlContext(HGLRC context)
{
    wglMakeCurrent(nullptr, nullptr);
    if (context != nullptr) {
        wglDeleteContext(context);
    }
}
