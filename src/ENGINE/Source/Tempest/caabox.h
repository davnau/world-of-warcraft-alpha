#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

namespace NTempest {

class CAaBox {
public:
    C3Vector b;
    C3Vector t;

    CAaBox() = default;
    constexpr CAaBox(const C3Vector& bottom, const C3Vector& top) : b(bottom), t(top) {}

    CAaBox& Bounding(const C3Vector* vectors, unsigned int count);
};

static_assert(sizeof(CAaBox) == sizeof(float) * 6, "CAaBox layout must remain binary-compatible.");

} // namespace NTempest
