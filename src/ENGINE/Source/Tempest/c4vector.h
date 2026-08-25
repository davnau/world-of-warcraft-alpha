#pragma once

namespace NTempest {

class C4Vector {
public:
    float x, y, z, w;
    constexpr C4Vector() : x(0), y(0), z(0), w(0) {}
    constexpr explicit C4Vector(float v) : x(v), y(v), z(v), w(v) {}
    constexpr C4Vector(float x_, float y_, float z_, float w_) : x(x_), y(y_), z(z_), w(w_) {}
    float SquaredMag() const;
};
static_assert(sizeof(C4Vector) == 16, "C4Vector layout mismatch");

} // namespace NTempest
