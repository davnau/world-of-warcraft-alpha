#pragma once

#include "ENGINE/Source/Tempest/c2ivector.h"

class CGxMonitorMode {
public:
    CGxMonitorMode() noexcept = default;
    NTempest::C2iVector size{};
    unsigned bpp = 0;
    unsigned refreshRate = 0;
};
