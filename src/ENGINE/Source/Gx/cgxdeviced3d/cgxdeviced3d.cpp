#include "ENGINE/Source/Gx/cgxdeviced3d/cgxdeviced3d.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <d3d9.h>

#include <cctype>

namespace {

struct ColorFormat {
    D3DFORMAT d3d;
    CGxFormat::Format gx;
};

constexpr ColorFormat kColorFormats[] = {
    {D3DFMT_R5G6B5, CGxFormat::Fmt_Rgb565},
    {D3DFMT_X8R8G8B8, CGxFormat::Fmt_ArgbX888},
    {D3DFMT_A8R8G8B8, CGxFormat::Fmt_Argb8888},
    {D3DFMT_A2R10G10B10, CGxFormat::Fmt_Argb2101010},
};

} // namespace

unsigned short HToI(const char* text, unsigned length)
{
    if (text == nullptr) {
        return 0;
    }
    unsigned value = 0;
    for (unsigned index = 0; index < length; ++index) {
        const unsigned char character = static_cast<unsigned char>(text[index]);
        if (!std::isxdigit(character)) {
            continue;
        }
        value <<= 4;
        value += character >= '0' && character <= '9'
            ? character - '0'
            : (static_cast<unsigned char>(std::toupper(character)) - 'A' + 10);
    }
    return static_cast<unsigned short>(value);
}

CGxDeviceD3d* CGxDevice::NewD3d9()
{
    return new CGxDeviceD3d();
}

bool CGxDevice::D3dEnumFormats(std::vector<CGxFormat>& formats)
{
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == nullptr) {
        return false;
    }

    formats.clear();
    for (const ColorFormat& color : kColorFormats) {
        const UINT modeCount = d3d->GetAdapterModeCount(0, color.d3d);
        for (UINT mode = 0; mode < modeCount; ++mode) {
            D3DDISPLAYMODE displayMode{};
            if (FAILED(d3d->EnumAdapterModes(0, color.d3d, mode, &displayMode))) {
                continue;
            }
            if (displayMode.Width < 640 || displayMode.Height < 480) {
                continue;
            }
            CGxFormat format(
                0,
                NTempest::C2iVector(
                    static_cast<int>(displayMode.Width),
                    static_cast<int>(displayMode.Height)),
                color.gx,
                CGxFormat::Fmt_Ds160,
                displayMode.RefreshRate,
                0,
                0,
                0);
            format.apiSpecificModeID = mode;
            formats.push_back(format);
        }
    }
    d3d->Release();
    return !formats.empty();
}

int CGxDevice::AdapterID(
    unsigned short* vendorID,
    unsigned short* deviceID,
    unsigned* driverVersionHi,
    unsigned* driverVersionLow)
{
    if (vendorID == nullptr || deviceID == nullptr
        || driverVersionHi == nullptr || driverVersionLow == nullptr) {
        return 0;
    }
    *vendorID = 0xffff;
    *deviceID = 0xffff;
    *driverVersionHi = 0;
    *driverVersionLow = 0;

    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == nullptr) {
        return 0;
    }
    D3DADAPTER_IDENTIFIER9 identifier{};
    const HRESULT result = d3d->GetAdapterIdentifier(0, 0, &identifier);
    if (SUCCEEDED(result)) {
        *vendorID = static_cast<unsigned short>(identifier.VendorId);
        *deviceID = static_cast<unsigned short>(identifier.DeviceId);
        *driverVersionHi = static_cast<unsigned>(identifier.DriverVersion.HighPart);
        *driverVersionLow = static_cast<unsigned>(identifier.DriverVersion.LowPart);
    }
    d3d->Release();
    return SUCCEEDED(result) ? 1 : 0;
}

int CGxDevice::AdapterInfer(unsigned short* deviceClass)
{
    if (deviceClass == nullptr) {
        return 0;
    }
    *deviceClass = 0;
    IDirect3D9* d3d = Direct3DCreate9(D3D_SDK_VERSION);
    if (d3d == nullptr) {
        return 0;
    }
    D3DCAPS9 caps{};
    const HRESULT result = d3d->GetDeviceCaps(0, D3DDEVTYPE_HAL, &caps);
    if (SUCCEEDED(result)) {
        const bool hardwareTransform = (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT) != 0;
        if (hardwareTransform && caps.MaxSimultaneousTextures > 2
            && D3DSHADER_VERSION_MAJOR(caps.PixelShaderVersion) >= 1
            && D3DSHADER_VERSION_MINOR(caps.PixelShaderVersion) >= 1) {
            *deviceClass = 2;
        } else if (hardwareTransform && caps.MaxSimultaneousTextures >= 2) {
            *deviceClass = 1;
        }
    }
    d3d->Release();
    return SUCCEEDED(result) ? 1 : 0;
}

bool CGxDevice::AdapterMonitorModes(std::vector<CGxMonitorMode>& modes)
{
    modes.clear();
    DISPLAY_DEVICEA displayDevice{};
    displayDevice.cb = sizeof(displayDevice);
    if (!EnumDisplayDevicesA(nullptr, 0, &displayDevice, 0)
        || (displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0) {
        return false;
    }

    for (DWORD mode = 0;; ++mode) {
        DEVMODEA displayMode{};
        displayMode.dmSize = sizeof(displayMode);
        if (!EnumDisplaySettingsA(displayDevice.DeviceName, mode, &displayMode)) {
            break;
        }
        if (displayMode.dmPelsWidth < 640 || displayMode.dmPelsHeight < 480
            || displayMode.dmBitsPerPel < 16 || displayMode.dmDisplayFrequency < 60) {
            continue;
        }
        CGxMonitorMode value;
        value.size = NTempest::C2iVector(
            static_cast<int>(displayMode.dmPelsWidth),
            static_cast<int>(displayMode.dmPelsHeight));
        value.bpp = displayMode.dmBitsPerPel;
        value.refreshRate = displayMode.dmDisplayFrequency;
        modes.push_back(value);
    }
    return !modes.empty();
}

int CGxDevice::AdapterDesktopMode(unsigned mode[4])
{
    if (mode == nullptr) {
        return 0;
    }
    DISPLAY_DEVICEA displayDevice{};
    displayDevice.cb = sizeof(displayDevice);
    if (!EnumDisplayDevicesA(nullptr, 0, &displayDevice, 0)
        || (displayDevice.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) == 0) {
        return 0;
    }

    DEVMODEA displayMode{};
    displayMode.dmSize = sizeof(displayMode);
    if (!EnumDisplaySettingsA(displayDevice.DeviceName, ENUM_CURRENT_SETTINGS, &displayMode)) {
        return 0;
    }
    mode[0] = displayMode.dmPelsWidth;
    mode[1] = displayMode.dmPelsHeight;
    mode[2] = displayMode.dmBitsPerPel;
    mode[3] = displayMode.dmDisplayFrequency;
    return 1;
}
