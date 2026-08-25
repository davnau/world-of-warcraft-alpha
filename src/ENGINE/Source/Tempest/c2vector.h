#pragma once

namespace NTempest {

// Reconstructed from the PDB type record for NTempest::C2Vector.
class C2Vector {
public:
    float x;
    float y;

    constexpr C2Vector() : x(0.0F), y(0.0F) {}
    constexpr explicit C2Vector(float value) : x(value), y(value) {}
    constexpr C2Vector(float xValue, float yValue) : x(xValue), y(yValue) {}

    C2Vector& operator-=(const C2Vector& value);
    float SquaredMag() const;
};

static_assert(sizeof(C2Vector) == sizeof(float) * 2, "C2Vector layout must remain binary-compatible.");

C2Vector __fastcall operator-(const C2Vector& left, const C2Vector& right);
C2Vector __fastcall operator*(float scalar, const C2Vector& value);

} // namespace NTempest
