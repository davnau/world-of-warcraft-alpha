#pragma once
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c4vector.h"
namespace NTempest { class C4Quaternion;
class C34Matrix { public:
 float a0,a1,a2,b0,b1,b2,c0,c1,c2,d0,d1,d2;
 constexpr C34Matrix():a0(1),a1(0),a2(0),b0(0),b1(1),b2(0),c0(0),c1(0),c2(1),d0(0),d1(0),d2(0){}
 float* Data(){return &a0;} const float* Data()const{return &a0;}
 C34Matrix& operator+=(const C34Matrix&); C34Matrix& operator-=(const C34Matrix&); C34Matrix& operator*=(const C34Matrix&); C34Matrix& operator*=(float); C34Matrix& operator/=(float);
 C34Matrix AffineInverse()const; C34Matrix AffineInverse(float)const; C34Matrix AffineInverse(const C3Vector&)const;
 static C34Matrix Rotation(const C3Vector&,float,bool=true); void Translate(const C3Vector&); void Scale(const C3Vector&); void Scale(float); void Rotate(const C3Vector&,const C3Vector&,char); void Rotate(const C4Quaternion&);
}; static_assert(sizeof(C34Matrix)==48,"C34Matrix layout mismatch");
bool operator==(const C34Matrix&,const C34Matrix&); bool operator!=(const C34Matrix&,const C34Matrix&); C34Matrix operator+(C34Matrix,const C34Matrix&); C34Matrix operator-(C34Matrix,const C34Matrix&); C34Matrix operator*(const C34Matrix&,const C34Matrix&); C34Matrix operator*(C34Matrix,float); C34Matrix operator*(float,C34Matrix); C34Matrix operator/(C34Matrix,float); C3Vector operator*(const C34Matrix&,const C3Vector&); C4Vector operator*(const C34Matrix&,const C4Vector&);
}
