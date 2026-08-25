#pragma once

#include "ENGINE/Source/Gx/gxapi/gxformat.h"

#include <cstdint>

struct Hardware {
    int videoVendor = 0;
    int videoDevice = 0;
    int videoMemory = 0;
    int videoHwTransform = 0;
    int videoPixelShader = 0;
    int videoVertexShader = 0;
    int videoFormat = 0;
    int videoIndex = 0;
    const CGxFormat* format = nullptr;
};

void __fastcall DetectHardware(std::uint8_t* changed, int flags = 0);
void __fastcall PrintUnknownHardware(std::uint16_t* hardwareId);
void __fastcall SetVideoIdx(std::uint16_t* hardwareId);
void __fastcall SaveHardware(const Hardware* hardware, std::uint8_t* changed);
void __fastcall SetDefaults(Hardware* hardware);
void __fastcall SetDefaultsFormat(CGxFormat* format);

extern Hardware g_hardware;
extern int g_hardwareDatabaseLoaded;

int _E22_58(); int _E19_50(); int _E21_22(); void _E20_67();
int _E2_297(); int _E1_297(); int _E27_66(); CGxFormat* _E24_28(); int _E26_68(); void _E25_59();
int _E5_294(); void _E4_294(); int _E8_294(); void _E7_294();

