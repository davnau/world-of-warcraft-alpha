#include "WoW/Source/WorldCommon/WorldMath.h"

#include "WoW/common/aabsp.h"

#include <algorithm>
#include <cmath>
#include <limits>

float INFINITY_371 = 0.0F;
float TWO_PI_330 = 0.0F;
float OO_TWO_PI_331 = 0.0F;

namespace {
constexpr float kEpsilon = 0.000001F;

template<class Matrix>
void TransformBox(const Matrix& matrix, const NTempest::CAaBox& box,
                  NTempest::CAaBox& result) {
    const NTempest::C3Vector corners[8] = {
        {box.b.x, box.b.y, box.b.z}, {box.t.x, box.b.y, box.b.z},
        {box.b.x, box.t.y, box.b.z}, {box.t.x, box.t.y, box.b.z},
        {box.b.x, box.b.y, box.t.z}, {box.t.x, box.b.y, box.t.z},
        {box.b.x, box.t.y, box.t.z}, {box.t.x, box.t.y, box.t.z},
    };
    result.b = matrix * corners[0];
    result.t = result.b;
    for (unsigned i = 1; i < 8; ++i) {
        const NTempest::C3Vector value = matrix * corners[i];
        result.b = NTempest::C3Vector::Min(result.b, value);
        result.t = NTempest::C3Vector::Max(result.t, value);
    }
}

float PointSegmentSqrDistance(const NTempest::C3Vector& point,
                              const NTempest::C3Vector& a,
                              const NTempest::C3Vector& b) {
    const NTempest::C3Vector delta = b - a;
    const float lengthSquared = delta.SquaredMag();
    float t = lengthSquared > kEpsilon
        ? NTempest::C3Vector::Dot(point - a, delta) / lengthSquared : 0.0F;
    t = std::clamp(t, 0.0F, 1.0F);
    const NTempest::C3Vector closest = a + delta * t;
    return (point - closest).SquaredMag();
}
}

int __fastcall CWorldMath::EdgeIntersectEdge(const NTempest::C2Vector& a,
                                             const NTempest::C2Vector& b,
                                             const NTempest::C2Vector& c,
                                             const NTempest::C2Vector& d,
                                             NTempest::C2Vector& point) {
    const float denominator = (b.y - a.y) * (c.x - d.x) +
                              (d.y - c.y) * (a.x - b.x);
    if (std::fabs(denominator) < kEpsilon)
        return 0;
    const float s = ((a.y - d.y) * (c.x - d.x) +
                     (d.y - c.y) * (a.x - d.x)) / denominator;
    const float t = -((c.y - a.y) * (a.x - b.x) +
                      (b.y - a.y) * (a.x - c.x)) / denominator;
    const NTempest::C2Vector delta = b - a;
    point.x = a.x + delta.x * s;
    point.y = a.y + delta.y * s;
    return s >= 0.0F && s <= 1.0F && t >= 0.0F && t <= 1.0F;
}

int __fastcall CWorldMath::RayIntersectTri(const NTempest::C3Vector& rayOrigin,
                                            const NTempest::C3Vector& rayDirection,
                                            const NTempest::C3Vector& v0,
                                            const NTempest::C3Vector& v1,
                                            const NTempest::C3Vector& v2,
                                            float& dist) {
    const NTempest::C3Vector edge1 = v1 - v0;
    const NTempest::C3Vector edge2 = v2 - v0;
    const NTempest::C3Vector pvec = NTempest::C3Vector::Cross(rayDirection, edge2);
    const float det = NTempest::C3Vector::Dot(edge1, pvec);
    const NTempest::C3Vector tvec = rayOrigin - v0;
    const NTempest::C3Vector qvec = NTempest::C3Vector::Cross(tvec, edge1);
    const float u = NTempest::C3Vector::Dot(tvec, pvec);
    const float v = NTempest::C3Vector::Dot(qvec, rayDirection);
    if (det > kEpsilon) {
        if (u < 0.0F || u > det || v < 0.0F || u + v > det) return 0;
    } else if (det < -kEpsilon) {
        if (u > 0.0F || u < det || v > 0.0F || u + v < det) return 0;
    } else {
        return 0;
    }
    dist = NTempest::C3Vector::Dot(qvec, edge2) / det;
    return 1;
}

void __fastcall CWorldMath::TransformAABox(const NTempest::C33Matrix& matrix,
                                           const NTempest::CAaBox& box,
                                           NTempest::CAaBox& result) {
    TransformBox(matrix, box, result);
}

void __fastcall CWorldMath::TransformAABox(const NTempest::C34Matrix& matrix,
                                           const NTempest::CAaBox& box,
                                           NTempest::CAaBox& result) {
    TransformBox(matrix, box, result);
}

void __fastcall CWorldMath::TransformAABox(const NTempest::C44Matrix& matrix,
                                           const NTempest::CAaBox& box,
                                           NTempest::CAaBox& result) {
    NTempest::C34Matrix affine;
    affine.a0 = matrix.a0; affine.a1 = matrix.a1; affine.a2 = matrix.a2;
    affine.b0 = matrix.b0; affine.b1 = matrix.b1; affine.b2 = matrix.b2;
    affine.c0 = matrix.c0; affine.c1 = matrix.c1; affine.c2 = matrix.c2;
    affine.d0 = matrix.d0; affine.d1 = matrix.d1; affine.d2 = matrix.d2;
    TransformBox(affine, box, result);
}

int __fastcall CWorldMath::VectorIntersectAABox2(const NTempest::CAaBox& box,
                                                 const NTempest::C3Vector& start,
                                                 const NTempest::C3Vector& end) {
    const NTempest::C3Vector direction = end - start;
    float tMin = 0.0F;
    float tMax = 1.0F;
    for (unsigned axis = 0; axis < 3; ++axis) {
        const float origin = start[axis];
        const float delta = direction[axis];
        if (std::fabs(delta) < kEpsilon) {
            if (origin < box.b[axis] || origin > box.t[axis]) return 0;
            continue;
        }
        float t1 = (box.b[axis] - origin) / delta;
        float t2 = (box.t[axis] - origin) / delta;
        if (t1 > t2) std::swap(t1, t2);
        tMin = (std::max)(tMin, t1);
        tMax = (std::min)(tMax, t2);
        if (tMin > tMax) return 0;
    }
    return 1;
}

int __fastcall CWorldMath::VectorIntersectAABox2(const NTempest::CAaBox& box,
                                                 const NTempest::C3Segment& segment) {
    const auto* vectors = reinterpret_cast<const NTempest::C3Vector*>(&segment);
    return VectorIntersectAABox2(box, vectors[0], vectors[1]);
}

int __fastcall CWorldMath::SphereIntersectAABox(const NTempest::C3Vector& center,
                                                const NTempest::CAaBox& box,
                                                float radius) {
    float distanceSquared = 0.0F;
    for (unsigned axis = 0; axis < 3; ++axis) {
        float distance = 0.0F;
        if (center[axis] < box.b[axis]) distance = box.b[axis] - center[axis];
        else if (center[axis] > box.t[axis]) distance = center[axis] - box.t[axis];
        distanceSquared += distance * distance;
    }
    return distanceSquared <= radius * radius;
}

unsigned __fastcall CWorldMath::AABoxIntersectPlane(const NTempest::C4Plane& plane,
                                                    const NTempest::CAaBox& box) {
    NTempest::C3Vector nearPoint, farPoint;
    for (unsigned axis = 0; axis < 3; ++axis) {
        if (plane.n[axis] >= 0.0F) { nearPoint[axis] = box.b[axis]; farPoint[axis] = box.t[axis]; }
        else { nearPoint[axis] = box.t[axis]; farPoint[axis] = box.b[axis]; }
    }
    const float nearDistance = NTempest::C3Vector::Dot(plane.n, nearPoint) + plane.d;
    const float farDistance = NTempest::C3Vector::Dot(plane.n, farPoint) + plane.d;
    if ((nearDistance < 0.0F) != (farDistance < 0.0F) ||
        std::fabs(nearDistance) < 0.019444443F || std::fabs(farDistance) < 0.019444443F)
        return 0;
    return nearDistance <= 0.019444443F ? 2u : 1u;
}

long double __fastcall CWorldMath::TriSqrDistance(const NTempest::C3Vector& origin,
                                                  const NTempest::C3Vector& a,
                                                  const NTempest::C3Vector& edge0,
                                                  const NTempest::C3Vector& edge1) {
    const NTempest::C3Vector b = a + edge0;
    const NTempest::C3Vector c = a + edge1;
    const NTempest::C3Vector normal = NTempest::C3Vector::Cross(edge0, edge1);
    const float normalSquared = normal.SquaredMag();
    if (normalSquared > kEpsilon) {
        const float signedDistance = NTempest::C3Vector::Dot(origin - a, normal) / std::sqrt(normalSquared);
        const NTempest::C3Vector projection = origin - normal * (signedDistance / std::sqrt(normalSquared));
        const NTempest::C3Vector ab = b - a;
        const NTempest::C3Vector bc = c - b;
        const NTempest::C3Vector ca = a - c;
        const NTempest::C3Vector ap = projection - a;
        const NTempest::C3Vector bp = projection - b;
        const NTempest::C3Vector cp = projection - c;
        const float e0 = NTempest::C3Vector::Dot(NTempest::C3Vector::Cross(ab, ap), normal);
        const float e1 = NTempest::C3Vector::Dot(NTempest::C3Vector::Cross(bc, bp), normal);
        const float e2 = NTempest::C3Vector::Dot(NTempest::C3Vector::Cross(ca, cp), normal);
        if ((e0 >= 0.0F && e1 >= 0.0F && e2 >= 0.0F) ||
            (e0 <= 0.0F && e1 <= 0.0F && e2 <= 0.0F))
            return static_cast<long double>(signedDistance * signedDistance);
    }
    return static_cast<long double>((std::min)({PointSegmentSqrDistance(origin, a, b),
                                                 PointSegmentSqrDistance(origin, b, c),
                                                 PointSegmentSqrDistance(origin, c, a)}));
}

int _E2_371() { return _E1_371(); }
int _E1_371() { INFINITY_371 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_342() { _E4_342(); return 0; } void _E4_342() { TWO_PI_330 = 6.2831854F; }
int _E8_332() { _E7_342(); return 0; } void _E7_342() { OO_TWO_PI_331 = 1.0F / TWO_PI_330; }
void _E13_14() {} int _E14_23() { _E13_14(); return 0; }
void _E16_29() {} int _E17_26() { _E16_29(); return 0; }
void _E19_56() {} int _E20_74() { _E19_56(); return 0; }
void _E22_64() {} int _E23_76() { _E22_64(); return 0; }
void _E25_64() {} int _E26_73() { _E25_64(); return 0; }
void _E28_100() {} int _E29_72() { _E28_100(); return 0; }
void _E31_98() {} int _E32_64() { _E31_98(); return 0; }
void _E34_75() {} int _E35_77() { _E34_75(); return 0; }
void _E37_70() {} int _E38_75() { _E37_70(); return 0; }
void _E40_71() {} int _E41_72() { _E40_71(); return 0; }
void _E43_68() {} int _E44_74() { _E43_68(); return 0; }
void _E46_67() {} int _E47_75() { _E46_67(); return 0; }
void _E52_66() {} int _E53_66() { _E52_66(); return 0; }
void _E55_58() {} int _E56_41() { _E55_58(); return 0; }
void _E58_59() {} int _E59_56() { _E58_59(); return 0; }
void _E61_57() {} int _E62_53() { _E61_57(); return 0; }
void _E64_47() {} int _E65_49() { _E64_47(); return 0; }
