#include "ENGINE/Source/Tempest/cfacet.h"

namespace NTempest {

void CFacet::Set(float value) {
    vertices[0] = C3Vector(value);
    vertices[1] = C3Vector(value);
    vertices[2] = C3Vector(value);
    plane.n = C3Vector(0.0F, 0.0F, 1.0F);
    plane.d = -value;
}

void CFacet::Set(const C3Vector& a, const C3Vector& b, const C3Vector& c) {
    vertices[0] = a;
    vertices[1] = b;
    vertices[2] = c;
    plane.n = C3Vector::Cross(b - a, c - a);
    if (plane.n.SquaredMag() > 0.0F) plane.n.Normalize();
    plane.d = -C3Vector::Dot(plane.n, a);
}

} // namespace NTempest
