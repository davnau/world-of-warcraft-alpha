#pragma once

#include <cstdint>

namespace NTempest {

// Reconstructed from the type record in types_manifest.csv and the PDB-backed
// implementation records for c3vector.cpp. Keep this class name, member order,
// and method names aligned with the preserved binary.
class C3Vector {
public:
    enum EAxis : std::int32_t {
        C3AXIS_X = 0,
        C3AXIS_Y = 1,
        C3AXIS_Z = 2,
    };

    float x;
    float y;
    float z;

    constexpr C3Vector() : x(0.0F), y(0.0F), z(0.0F) {}
    constexpr explicit C3Vector(float value) : x(value), y(value), z(value) {}
    constexpr C3Vector(float xValue, float yValue, float zValue) : x(xValue), y(yValue), z(zValue) {}

    constexpr float& operator[](std::uint32_t component) { return (&x)[component]; }
    constexpr const float& operator[](std::uint32_t component) const { return (&x)[component]; }

    void Set(float xValue, float yValue, float zValue);
    C3Vector& operator+=(const C3Vector& value);
    C3Vector& operator-=(const C3Vector& value);
    C3Vector& operator*=(float value);
    C3Vector& operator/=(float value);

    void Normalize();
    EAxis MajorAxis() const;
    EAxis MinorAxis() const;
    float SquaredMag() const;

    static float __fastcall Dot(const C3Vector& left, const C3Vector& right);
    static C3Vector __fastcall Cross(const C3Vector& left, const C3Vector& right);
    static C3Vector __fastcall Min(const C3Vector& left, const C3Vector& right);
    static C3Vector __fastcall Max(const C3Vector& left, const C3Vector& right);
    void Maximize(const C3Vector& value);
};

static_assert(sizeof(C3Vector) == sizeof(float) * 3, "C3Vector layout must remain binary-compatible.");

C3Vector __fastcall operator+(const C3Vector& left, const C3Vector& right);
C3Vector __fastcall operator-(const C3Vector& left, const C3Vector& right);
C3Vector __fastcall operator*(float scalar, const C3Vector& value);
C3Vector __fastcall operator*(const C3Vector& value, float scalar);
C3Vector __fastcall operator/(const C3Vector& value, float scalar);

} // namespace NTempest
