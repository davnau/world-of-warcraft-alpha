#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

namespace NTempest {

class C4Plane {
public:
    C3Vector n;
    float d;

    constexpr C4Plane() : n(), d(0.0F) {}
    constexpr C4Plane(const C3Vector& normal, float distance) : n(normal), d(distance) {}
};

static_assert(sizeof(C4Plane) == sizeof(float) * 4, "C4Plane layout must remain binary-compatible.");

} // namespace NTempest
