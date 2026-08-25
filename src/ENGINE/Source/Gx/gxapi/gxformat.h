#pragma once

#include "ENGINE/Source/Tempest/c2ivector.h"

class CGxFormat {
public:
    enum Format : int {
        Fmt_Rgb565 = 0,
        Fmt_ArgbX888 = 1,
        Fmt_Argb8888 = 2,
        Fmt_Argb2101010 = 3,
        Fmt_Ds160 = 4,
        Fmt_Ds24X = 5,
        Fmt_Ds248 = 6,
        Fmt_Ds320 = 7,
        Formats_Last = 8,
    };

    CGxFormat() noexcept;
    CGxFormat(
        unsigned char window,
        const NTempest::C2iVector& size,
        Format colorFormat,
        Format depthFormat,
        unsigned refreshRate,
        unsigned char vsync,
        unsigned char hwTnL,
        unsigned char fixLag) noexcept;

    unsigned apiSpecificModeID = 0;
    unsigned char hwTnL = 0;
    unsigned char fixLag = 0;
    unsigned char window = 0;
    Format depthFormat = Fmt_Ds160;
    NTempest::C2iVector size{};
    Format colorFormat = Fmt_Rgb565;
    unsigned refreshRate = 0;
    unsigned char vsync = 0;
    NTempest::C2iVector pos{};
};

static_assert(sizeof(CGxFormat) == 40);
