#include "WoW/Source/Console/consoledetect.h"

#include "WoW/Source/Console/consoleclient.h"
#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cstdio>
#include <limits>

Hardware g_hardware{};
int g_hardwareDatabaseLoaded = 0;

namespace {
CGxFormat g_formats[7];
}

void __fastcall SetDefaultsFormat(CGxFormat* format) {
    if (!format) return;
    format->window = 1;
    format->size = NTempest::C2iVector{1280, 720};
    format->colorFormat = CGxFormat::Fmt_ArgbX888;
    format->depthFormat = CGxFormat::Fmt_Ds24X;
    format->refreshRate = 60;
    format->vsync = 0;
    format->hwTnL = 1;
    format->fixLag = 1;
}

void __fastcall SetDefaults(Hardware* hardware) {
    if (!hardware) return;
    *hardware = {};
    hardware->videoMemory = 64;
    hardware->videoHwTransform = 1;
    hardware->videoPixelShader = 1;
    hardware->videoVertexShader = 1;
    hardware->format = &_E98_2()[0];
}

void __fastcall PrintUnknownHardware(std::uint16_t* hardwareId) {
    if (!hardwareId) return;
    ConsolePrintf("Unknown video hardware: %04x:%04x", hardwareId[0], hardwareId[1]);
}

void __fastcall SetVideoIdx(std::uint16_t* hardwareId) {
    if (!hardwareId) return;
    g_hardware.videoVendor = hardwareId[0];
    g_hardware.videoDevice = hardwareId[1];
}

void __fastcall SaveHardware(const Hardware* hardware, std::uint8_t* changed) {
    if (hardware) g_hardware = *hardware;
    if (changed) *changed = 0;
}

void __fastcall DetectHardware(std::uint8_t* changed, int) {
    Hardware detected{};
    SetDefaults(&detected);
    SetDefaultsFormat(const_cast<CGxFormat*>(detected.format));
    detected.format = &_E98_2()[0];
    SaveHardware(&detected, changed);
    g_hardwareDatabaseLoaded = 1;
}

int _E19_50() { g_hardwareDatabaseLoaded = 0; return 0; }
void _E20_67() { g_hardwareDatabaseLoaded = 0; }
int _E21_22() { return 0; }
int _E22_58() { _E19_50(); return _E21_22(); }
int _E1_297() { INFINITY_294 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2_297() { return _E1_297(); }
CGxFormat* _E24_28() {
    static const NTempest::C2iVector sizes[] = {{640, 480}, {800, 600}, {1024, 768}, {1280, 720}, {1280, 1024}, {1600, 900}, {1920, 1080}};
    for (int i = 0; i < 7; ++i) {
        g_formats[i] = CGxFormat{};
        g_formats[i].window = 1;
        g_formats[i].size = sizes[i];
        g_formats[i].colorFormat = i < 2 ? CGxFormat::Fmt_Rgb565 : CGxFormat::Fmt_ArgbX888;
        g_formats[i].depthFormat = i < 2 ? CGxFormat::Fmt_Ds160 : CGxFormat::Fmt_Ds24X;
        g_formats[i].refreshRate = 60;
        g_formats[i].vsync = 1;
        g_formats[i].hwTnL = 1;
        g_formats[i].fixLag = 1;
    }
    return g_formats;
}
int _E26_68() { return 0; }
void _E25_59() {}
void _E4_294() { TWO_PI_289 = 6.28318530717958647692F; }
int _E5_294() { _E4_294(); return 0; }
void _E7_294() { OO_TWO_PI_290 = 1.0F / TWO_PI_289; }
int _E8_294() { _E7_294(); return 0; }
