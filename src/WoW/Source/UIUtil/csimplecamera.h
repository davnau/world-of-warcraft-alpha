#pragma once

#include "ENGINE/Source/Tempest/c33matrix.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/crect.h"

namespace NTempest {
class C3Vector;
}

class CSimpleCamera {
public:
    CSimpleCamera();
    CSimpleCamera(float nearZ, float farZ, float fov);
    virtual ~CSimpleCamera() = default;

    virtual void SetFacing(const NTempest::C3Vector& forward);
    virtual void SetFacing(const NTempest::C3Vector& forward,
                           const NTempest::C3Vector& up);
    virtual void SetFacing(float yaw, float pitch, float roll);
    virtual void SetGxProjectionAndView(const NTempest::CRect* projectionRect);
    virtual NTempest::C3Vector* Forward(NTempest::C3Vector* result);
    virtual NTempest::C3Vector* Right(NTempest::C3Vector* result);
    virtual NTempest::C3Vector* Up(NTempest::C3Vector* result);

    const NTempest::C3Vector& Position() const { return m_position; }
    const NTempest::C33Matrix& Facing() const { return m_facing; }
    float NearZ() const { return m_nearZ; }
    float FarZ() const { return m_farZ; }
    float Fov() const { return m_fov; }
    float Aspect() const { return m_aspect; }

    NTempest::C3Vector m_position{};
    NTempest::C33Matrix m_facing{};
    float m_nearZ = 0.11111111F;
    float m_farZ = 277.77777F;
    float m_fov = 1.5707964F;
    float m_aspect = 1.0F;
};

void BuildBillboardMatrix(const NTempest::C3Vector& forward,
                          NTempest::C33Matrix* rotation);
void FaceDirection(const NTempest::C3Vector& forward,
                   NTempest::C3Vector* xprime,
                   NTempest::C3Vector* yprime,
                   NTempest::C3Vector* zprime);
void BuildBillboardMatrixWithRoll(const NTempest::C3Vector& forward,
                                  const NTempest::C3Vector& up,
                                  NTempest::C33Matrix* rotation);
void FaceDirectionWithRoll(const NTempest::C3Vector& forward,
                           const NTempest::C3Vector& up,
                           NTempest::C3Vector* xprime,
                           NTempest::C3Vector* yprime,
                           NTempest::C3Vector* zprime);

extern float INFINITY_133;
extern float TWO_PI_133;
extern float OO_TWO_PI_133;
extern float MAX_DUEL_DISTANCE_SQUARED_64;
extern float MAX_TRADE_DISTANCE_SQUARED_64;
extern float MAX_SHOP_DISTANCE_SQUARED_64;
extern float MAX_INSPECT_DISTANCE_SQUARED_64;
extern float MAX_LOOT_DISTANCE_SQUARED_64;
extern float MAX_BIND_DISTANCE_SQUARED_64;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_64;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_64;
extern float MAX_SHARE_DISTANCE_SQUARED_64;
extern float SECONDS_PER_HEARTBEAT_64;
extern float SPELL_FOCUS_DISTANCE_SQUARED_63;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_64;

int _E2_133(); int _E1_133();
int _E5_133(); void _E4_133();
int _E8_133(); void _E7_133();
int _E28_57(); void _E27_54();
int _E31_54(); void _E30_52();
int _E34_55(); void _E33_50();
int _E37_54(); void _E36_49();
int _E40_55(); void _E39_51();
int _E43_54(); void _E42_50();
int _E46_52(); void _E45_49();
int _E49_54(); void _E48_54();
int _E52_53(); void _E51_48();
int _E55_49(); void _E54_53();
int _E58_48(); void _E57_41();
int _E61_49(); void _E60_49();
int _E68_15(); void _E67_45();
int _E71_7(); void _E70_37();
int _E74_4(); void _E73_37();
int _E77_3(); void _E76_36();
int _E80_7(); void _E79_3();
