#include "ENGINE/Source/Tempest/c3vector.h"

#include <cmath>
#include <algorithm>

namespace NTempest {

void C3Vector::Set(float xValue, float yValue, float zValue) {
    x = xValue;
    y = yValue;
    z = zValue;
}

C3Vector& C3Vector::operator+=(const C3Vector& value) {
    x += value.x;
    y += value.y;
    z += value.z;
    return *this;
}

C3Vector& C3Vector::operator-=(const C3Vector& value) {
    x -= value.x;
    y -= value.y;
    z -= value.z;
    return *this;
}

C3Vector& C3Vector::operator*=(float value) {
    x *= value;
    y *= value;
    z *= value;
    return *this;
}

C3Vector& C3Vector::operator/=(float value) {
    const float reciprocal = 1.0F / value;
    x *= reciprocal;
    y *= reciprocal;
    z *= reciprocal;
    return *this;
}

float C3Vector::SquaredMag() const {
    return x * x + y * y + z * z;
}

void C3Vector::Normalize() {
    const float inverseMagnitude = 1.0F / std::sqrt(SquaredMag());
    *this *= inverseMagnitude;
}

float __fastcall C3Vector::Dot(const C3Vector& left, const C3Vector& right) {
    return left.x * right.x + left.y * right.y + left.z * right.z;
}

C3Vector __fastcall C3Vector::Cross(const C3Vector& left, const C3Vector& right) {
    return C3Vector(
        left.y * right.z - left.z * right.y,
        left.z * right.x - left.x * right.z,
        left.x * right.y - left.y * right.x);
}

C3Vector __fastcall C3Vector::Min(const C3Vector& left, const C3Vector& right) {
    return C3Vector(std::min(left.x, right.x), std::min(left.y, right.y), std::min(left.z, right.z));
}

C3Vector __fastcall C3Vector::Max(const C3Vector& left, const C3Vector& right) {
    return C3Vector(std::max(left.x, right.x), std::max(left.y, right.y), std::max(left.z, right.z));
}

void C3Vector::Maximize(const C3Vector& value) {
    x = std::max(x, value.x);
    y = std::max(y, value.y);
    z = std::max(z, value.z);
}

// PDB source: D:\build\buildWoW\ENGINE\Source\Tempest\c3vector.cpp
// RVA: 0x003C8320
// Evidence: WoWClient_recovered\_external\d\build\buildwow\engine\source\tempest\c3vector.cpp/.asm
C3Vector::EAxis C3Vector::MajorAxis() const {
    const float fx = std::fabs(x);
    const float fy = std::fabs(y);
    const float fz = std::fabs(z);

    if (fx > fy) {
        return fx > fz ? C3AXIS_X : C3AXIS_Z;
    }

    return fy > fz ? C3AXIS_Y : C3AXIS_Z;
}

// PDB source: D:\build\buildWoW\ENGINE\Source\Tempest\c3vector.cpp
// RVA: 0x003C83A0
// Evidence: WoWClient_recovered\_external\d\build\buildwow\engine\source\tempest\c3vector.cpp/.asm
C3Vector::EAxis C3Vector::MinorAxis() const {
    const float fx = std::fabs(x);
    const float fy = std::fabs(y);
    const float fz = std::fabs(z);

    if (fx >= fy) {
        return fz >= fy ? C3AXIS_Y : C3AXIS_Z;
    }

    return fx >= fz ? C3AXIS_Z : C3AXIS_X;
}

C3Vector __fastcall operator+(const C3Vector& left, const C3Vector& right) {
    return C3Vector(left.x + right.x, left.y + right.y, left.z + right.z);
}

C3Vector __fastcall operator-(const C3Vector& left, const C3Vector& right) {
    return C3Vector(left.x - right.x, left.y - right.y, left.z - right.z);
}

C3Vector __fastcall operator*(float scalar, const C3Vector& value) {
    return C3Vector(scalar * value.x, scalar * value.y, scalar * value.z);
}

C3Vector __fastcall operator*(const C3Vector& value, float scalar) {
    return scalar * value;
}

C3Vector __fastcall operator/(const C3Vector& value, float scalar) {
    C3Vector result = value;
    result /= scalar;
    return result;
}

} // namespace NTempest
