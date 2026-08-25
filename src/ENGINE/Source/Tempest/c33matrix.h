#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c2vector.h"

namespace NTempest {
class C4Quaternion;

class C33Matrix {
public:
    float a0, a1, a2, b0, b1, b2, c0, c1, c2;
    constexpr C33Matrix() : a0(1),a1(0),a2(0),b0(0),b1(1),b2(0),c0(0),c1(0),c2(1) {}
    constexpr C33Matrix(float a0_,float a1_,float a2_,float b0_,float b1_,float b2_,float c0_,float c1_,float c2_)
        : a0(a0_),a1(a1_),a2(a2_),b0(b0_),b1(b1_),b2(b2_),c0(c0_),c1(c1_),c2(c2_) {}
    float* Data() { return &a0; } const float* Data() const { return &a0; }
    double Determinant() const;
    C33Matrix Cofactors() const; C33Matrix Adjoint() const; C33Matrix Inverse(float epsilon=1.0e-8F) const;
    C33Matrix AffineInverse() const; C33Matrix AffineInverse(float scale) const; C33Matrix AffineInverse(const C3Vector& scale) const;
    C33Matrix& Rotation(float angle); static C33Matrix Rotation(const C3Vector& axis,float angle,bool unit=true);
    void Scale(float s); void Scale(float x,float y,float z); void Scale(const C3Vector& scale);
    void Scale(float x,float y); void Scale(const C2Vector& scale);
    void Rotate(float angle); void Rotate(const C3Vector& angle,const C3Vector& axis,char unit); void Rotate(const C4Quaternion& rotation);
    void Translate(const C2Vector& move);
    bool ToEulerAnglesXYZ(float& x,float& y,float& z) const; bool ToEulerAnglesXZY(float& x,float& z,float& y) const;
    bool ToEulerAnglesYXZ(float& y,float& x,float& z) const; bool ToEulerAnglesYZX(float& y,float& z,float& x) const;
    bool ToEulerAnglesZXY(float& z,float& x,float& y) const; bool ToEulerAnglesZYX(float& z,float& y,float& x) const;
    void FromEulerAnglesXYZ(float x,float y,float z); void FromEulerAnglesXZY(float x,float z,float y);
    void FromEulerAnglesYXZ(float y,float x,float z); void FromEulerAnglesYZX(float y,float z,float x);
    void FromEulerAnglesZXY(float z,float x,float y); void FromEulerAnglesZYX(float z,float y,float x);
};
static_assert(sizeof(C33Matrix)==36,"C33Matrix layout mismatch");
C33Matrix __fastcall operator*(const C33Matrix& l,const C33Matrix& r);
C3Vector __fastcall operator*(const C33Matrix& m,const C3Vector& v);
} // namespace NTempest
