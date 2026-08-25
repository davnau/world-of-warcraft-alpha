#pragma once
#include "ENGINE/Source/Tempest/c4vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
namespace NTempest {
class C33Matrix;
class C4Quaternion : public C4Vector {
public:
    constexpr C4Quaternion() : C4Vector(0,0,0,1) {}
    constexpr C4Quaternion(float w_,float x_,float y_,float z_) : C4Vector(x_,y_,z_,w_) {}
    void FromRotationMatrix(const C33Matrix& r); void FromRotationMatrixInv(const C33Matrix& r);
    void FromAngleAxis(float angle,const C3Vector& axis); void ToAngleAxis(float& angle,C3Vector& axis) const;
    C4Quaternion Inverse() const; C4Quaternion Conjugate() const; C4Quaternion Exp() const; C4Quaternion Log() const;
    static C4Quaternion Slerp(const C4Quaternion& p,const C4Quaternion& q,float t);
    static void SquadInterm(const C4Quaternion& q0,const C4Quaternion& q1,const C4Quaternion& q2,C4Quaternion& a,C4Quaternion& b);
    static void SquadIntermMaxCompat(const C4Quaternion& q0,const C4Quaternion& q1,const C4Quaternion& q2,C4Quaternion& a,C4Quaternion& b);
    static void SquadIntermTCB(const C4Quaternion& q0,const C4Quaternion& q1,const C4Quaternion& q2,float time0,float time1,float time2,float tension,float continuity,float bias,C4Quaternion& a,C4Quaternion& b);
    static C4Quaternion Squad(const C4Quaternion& p,const C4Quaternion& q,const C4Quaternion& a,const C4Quaternion& b,float t);
};
static_assert(sizeof(C4Quaternion)==16,"C4Quaternion layout mismatch");
} // namespace NTempest
