#include "ENGINE/Source/Tempest/c2vector.h"

namespace NTempest {

C2Vector& C2Vector::operator-=(const C2Vector& value) {
    x -= value.x;
    y -= value.y;
    return *this;
}

float C2Vector::SquaredMag() const {
    return x * x + y * y;
}

C2Vector __fastcall operator-(const C2Vector& left, const C2Vector& right) {
    return C2Vector(left.x - right.x, left.y - right.y);
}

C2Vector __fastcall operator*(float scalar, const C2Vector& value) {
    return C2Vector(scalar * value.x, scalar * value.y);
}

} // namespace NTempest
