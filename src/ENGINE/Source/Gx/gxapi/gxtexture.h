#pragma once

#include "ENGINE/Source/Gx/gxapi/gxbase.h"

#include <cstdint>

#define WIN32_LEAN_AND_MEAN
#include <d3d9.h>

class CGxTex {
public:
    enum Format : unsigned {
        Unknown = 0,
        Argb8888 = 1,
        Rgb565 = 2,
        Dxt1 = 3,
        Dxt3 = 4,
        Dxt5 = 5,
    };

    unsigned glTexture = 0;
    IDirect3DBaseTexture9* d3dTexture = nullptr;
    EGxTexTarget target = GxTex_2d;
    unsigned width = 0;
    unsigned height = 0;
    unsigned depth = 1;
    std::uint32_t flags = 0;
    Format format = Argb8888;
    Format dataFormat = Argb8888;
    void* userArg = nullptr;
    CGxTexUserCallback userFunc = nullptr;
    NTempest::CiRect updateRect{};
    unsigned frameTag = 0;
    bool needsCreation = true;
    bool needsUpdate = false;
    bool needsFlagUpdate = true;

    void SetFlags(CGxTexFlags value) noexcept { flags = value.bits; needsFlagUpdate = true; }
    CGxTexFlags GetFlags() const noexcept { return CGxTexFlags(flags); }
};
