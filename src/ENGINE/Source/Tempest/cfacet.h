#pragma once

#include "ENGINE/Source/Tempest/c4plane.h"

namespace NTempest {

class CFacet {
public:
    C4Plane plane;
    C3Vector vertices[3];

    void Set(float value);
    void Set(const C3Vector& a, const C3Vector& b, const C3Vector& c);
};

static_assert(sizeof(CFacet) == sizeof(float) * 13, "CFacet layout must remain binary-compatible.");

} // namespace NTempest
