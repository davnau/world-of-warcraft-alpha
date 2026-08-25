#include "WoW/Source/UIUtil/csimplecamera.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"

#include <algorithm>
#include <cmath>

float INFINITY_133 = 0.0F;
float TWO_PI_133 = 0.0F;
float OO_TWO_PI_133 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_64 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_64 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_64 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_64 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_64 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_64 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_64 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_64 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_64 = 0.0F;
float SECONDS_PER_HEARTBEAT_64 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_63 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_64 = 0.0F;

namespace {
constexpr float kEpsilon = 0.00000023841858F;

NTempest::C3Vector SafeNormalize(NTempest::C3Vector value,
                                  const NTempest::C3Vector& fallback) {
    const float lengthSquared = value.SquaredMag();
    if (lengthSquared < kEpsilon)
        return fallback;
    value *= 1.0F / std::sqrt(lengthSquared);
    return value;
}

NTempest::C44Matrix CreateProjection(float fov, float aspect,
                                     float nearZ, float farZ) {
    const float safeAspect = aspect > kEpsilon ? aspect : kEpsilon;
    // GxuXformCreateProjection treats the preserved field of view as a
    // diagonal angle.  The square-root term is present in the original
    // routine and is what keeps wide displays from stretching the scene.
    const float halfHeight =
        std::tan((fov / std::sqrt(safeAspect * safeAspect + 1.0F)) * 0.5F) * nearZ;
    const float reciprocalWidth = nearZ /
        (safeAspect * (halfHeight > kEpsilon ? halfHeight : kEpsilon));
    const float reciprocalHeight = nearZ /
        (halfHeight > kEpsilon ? halfHeight : kEpsilon);
    const float depth = farZ - nearZ;
    NTempest::C44Matrix result;
    result.a0 = reciprocalWidth;
    result.b1 = reciprocalHeight;
    result.c2 = (nearZ + farZ) / depth;
    result.d2 = -2.0F * nearZ * farZ / depth;
    result.c3 = 1.0F;
    result.d3 = 0.0F;
    return result;
}

NTempest::C44Matrix CreateView(const NTempest::C33Matrix& facing) {
    // The preserved client keeps world geometry close to the origin by
    // subtracting the camera position in each object's world transform.  Its
    // CSimpleCamera view therefore contains orientation only.
    const NTempest::C3Vector forward{facing.a0, facing.a1, facing.a2};
    const NTempest::C3Vector right{facing.b0, facing.b1, facing.b2};
    const NTempest::C3Vector up{facing.c0, facing.c1, facing.c2};

    NTempest::C44Matrix result;
    result.a0 = right.x;
    result.a1 = up.x;
    result.a2 = forward.x;
    result.b0 = right.y;
    result.b1 = up.y;
    result.b2 = forward.y;
    result.c0 = right.z;
    result.c1 = up.z;
    result.c2 = forward.z;
    return result;
}
}

CSimpleCamera::CSimpleCamera() {
    SetFacing(0.0F, 0.0F, 0.0F);
}

CSimpleCamera::CSimpleCamera(float nearZ, float farZ, float fov)
    : m_nearZ(nearZ), m_farZ(farZ), m_fov(fov) {
    SetFacing(0.0F, 0.0F, 0.0F);
}

void CSimpleCamera::SetFacing(const NTempest::C3Vector& forward) {
    BuildBillboardMatrix(forward, &m_facing);
}

void CSimpleCamera::SetFacing(const NTempest::C3Vector& forward,
                              const NTempest::C3Vector& up) {
    BuildBillboardMatrixWithRoll(forward, up, &m_facing);
}

void CSimpleCamera::SetFacing(float yaw, float pitch, float roll) {
    m_facing.FromEulerAnglesZYX(yaw, pitch, roll);
}

void CSimpleCamera::SetGxProjectionAndView(const NTempest::CRect* projectionRect) {
    if (projectionRect == nullptr)
        return;
    const float width = projectionRect->r - projectionRect->l;
    const float height = projectionRect->b - projectionRect->t;
    if (std::fabs(height) < kEpsilon)
        return;
    m_aspect = width / height;
    GxXformSetProjection(CreateProjection(m_fov, m_aspect, m_nearZ, m_farZ));
    GxXformSetView(CreateView(m_facing));
}

NTempest::C3Vector* CSimpleCamera::Forward(NTempest::C3Vector* result) {
    if (result != nullptr)
        *result = {m_facing.a0, m_facing.a1, m_facing.a2};
    return result;
}

NTempest::C3Vector* CSimpleCamera::Right(NTempest::C3Vector* result) {
    if (result != nullptr)
        *result = {m_facing.b0, m_facing.b1, m_facing.b2};
    return result;
}

NTempest::C3Vector* CSimpleCamera::Up(NTempest::C3Vector* result) {
    if (result != nullptr)
        *result = {m_facing.c0, m_facing.c1, m_facing.c2};
    return result;
}

void FaceDirection(const NTempest::C3Vector& forward,
                   NTempest::C3Vector* xprime,
                   NTempest::C3Vector* yprime,
                   NTempest::C3Vector* zprime) {
    if (xprime == nullptr || yprime == nullptr || zprime == nullptr)
        return;
    *xprime = SafeNormalize(forward, {1.0F, 0.0F, 0.0F});
    if (std::fabs(xprime->x) + std::fabs(xprime->y) < kEpsilon) {
        *yprime = {1.0F, 0.0F, 0.0F};
    } else {
        *yprime = {-xprime->y, xprime->x, 0.0F};
        *yprime = SafeNormalize(*yprime, {1.0F, 0.0F, 0.0F});
    }
    *zprime = NTempest::C3Vector::Cross(*xprime, *yprime);
    *zprime = SafeNormalize(*zprime, {0.0F, 0.0F, 1.0F});
}

void BuildBillboardMatrix(const NTempest::C3Vector& forward,
                          NTempest::C33Matrix* rotation) {
    if (rotation == nullptr)
        return;
    NTempest::C3Vector xprime, yprime, zprime;
    FaceDirection(forward, &xprime, &yprime, &zprime);
    rotation->a0 = xprime.x; rotation->a1 = xprime.y; rotation->a2 = xprime.z;
    rotation->b0 = yprime.x; rotation->b1 = yprime.y; rotation->b2 = yprime.z;
    rotation->c0 = zprime.x; rotation->c1 = zprime.y; rotation->c2 = zprime.z;
}

void FaceDirectionWithRoll(const NTempest::C3Vector& forward,
                           const NTempest::C3Vector& up,
                           NTempest::C3Vector* xprime,
                           NTempest::C3Vector* yprime,
                           NTempest::C3Vector* zprime) {
    if (xprime == nullptr || yprime == nullptr || zprime == nullptr)
        return;
    *xprime = SafeNormalize(forward, {1.0F, 0.0F, 0.0F});
    *yprime = up - (*xprime) * NTempest::C3Vector::Dot(up, *xprime);
    if (yprime->SquaredMag() < kEpsilon) {
        NTempest::C3Vector unused;
        FaceDirection(*xprime, xprime, yprime, &unused);
    } else {
        *yprime = SafeNormalize(*yprime, {0.0F, 0.0F, 1.0F});
    }
    *zprime = NTempest::C3Vector::Cross(*xprime, *yprime);
    *zprime = SafeNormalize(*zprime, {0.0F, 0.0F, 1.0F});
    *yprime = NTempest::C3Vector::Cross(*zprime, *xprime);
}

void BuildBillboardMatrixWithRoll(const NTempest::C3Vector& forward,
                                  const NTempest::C3Vector& up,
                                  NTempest::C33Matrix* rotation) {
    if (rotation == nullptr)
        return;
    NTempest::C3Vector xprime, yprime, zprime;
    FaceDirectionWithRoll(forward, up, &xprime, &yprime, &zprime);
    rotation->a0 = xprime.x; rotation->a1 = xprime.y; rotation->a2 = xprime.z;
    rotation->b0 = yprime.x; rotation->b1 = yprime.y; rotation->b2 = yprime.z;
    rotation->c0 = zprime.x; rotation->c1 = zprime.y; rotation->c2 = zprime.z;
}

int _E2_133() { return _E1_133(); }
int _E1_133() { INFINITY_133 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_133() { _E4_133(); return 0; }
void _E4_133() { TWO_PI_133 = 3.1415927F + 3.1415927F; }
int _E8_133() { _E7_133(); return 0; }
void _E7_133() { OO_TWO_PI_133 = 1.0F / TWO_PI_133; }

#define CAMERA_CONST_THUNK_PAIR(N, THUNK, BODY) \
    int N##THUNK() { N##BODY(); return 0; } \
    void N##BODY()

void _E27_54() { MAX_DUEL_DISTANCE_SQUARED_64 = 10.0F * 10.0F; }
int _E28_57() { _E27_54(); return 0; }
void _E30_52() { MAX_TRADE_DISTANCE_SQUARED_64 = 11.111111F * 11.111111F; }
int _E31_54() { _E30_52(); return 0; }
void _E33_50() { MAX_SHOP_DISTANCE_SQUARED_64 = 5.5555553F * 5.5555553F; }
int _E34_55() { _E33_50(); return 0; }
void _E36_49() { MAX_INSPECT_DISTANCE_SQUARED_64 = 5.5555553F * 5.5555553F; }
int _E37_54() { _E36_49(); return 0; }
void _E39_51() { MAX_LOOT_DISTANCE_SQUARED_64 = 5.0F * 5.0F; }
int _E40_55() { _E39_51(); return 0; }
void _E42_50() { MAX_BIND_DISTANCE_SQUARED_64 = 10.0F * 10.0F; }
int _E43_54() { _E42_50(); return 0; }
void _E45_49() { MAX_SITCHAIR_DISTANCE_SQUARED_64 = 0.5F * 0.5F; }
int _E46_52() { _E45_49(); return 0; }
void _E48_54() { MAX_SITCHAIRUSE_DISTANCE_SQUARED_64 = 3.0F * 3.0F; }
int _E49_54() { _E48_54(); return 0; }
void _E51_48() { MAX_SHARE_DISTANCE_SQUARED_64 = 100.0F * 100.0F; }
int _E52_53() { _E51_48(); return 0; }
void _E54_53() { SECONDS_PER_HEARTBEAT_64 = 5.0F; }
int _E55_49() { _E54_53(); return 0; }
void _E57_41() { SPELL_FOCUS_DISTANCE_SQUARED_63 = 50.0F * 50.0F; }
int _E58_48() { _E57_41(); return 0; }
void _E60_49() { MAX_OBJ_INTEREST_RADIUS_SQUARED_64 = 100.0F * 100.0F; }
int _E61_49() { _E60_49(); return 0; }

void _E67_45() {}
int _E68_15() { _E67_45(); return 0; }
void _E70_37() {}
int _E71_7() { _E70_37(); return 0; }
void _E73_37() {}
int _E74_4() { _E73_37(); return 0; }
void _E76_36() {}
int _E77_3() { _E76_36(); return 0; }
void _E79_3() {}
int _E80_7() { _E79_3(); return 0; }
