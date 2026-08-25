#pragma once
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c4vector.h"
namespace NTempest { class C4Quaternion;
class C44Matrix { public:
 float a0,a1,a2,a3,b0,b1,b2,b3,c0,c1,c2,c3,d0,d1,d2,d3;
 constexpr C44Matrix():a0(1),a1(0),a2(0),a3(0),b0(0),b1(1),b2(0),b3(0),c0(0),c1(0),c2(1),c3(0),d0(0),d1(0),d2(0),d3(1){}
 constexpr C44Matrix(float A0,float A1,float A2,float A3,float B0,float B1,float B2,float B3,float C0,float C1,float C2,float C3,float D0,float D1,float D2,float D3):a0(A0),a1(A1),a2(A2),a3(A3),b0(B0),b1(B1),b2(B2),b3(B3),c0(C0),c1(C1),c2(C2),c3(C3),d0(D0),d1(D1),d2(D2),d3(D3){}
 float* Data(){return &a0;} const float* Data()const{return &a0;}
 static double Det(float,float,float,float,float,float,float,float,float); double Determinant()const; C44Matrix Cofactors()const; C44Matrix Adjoint()const; C44Matrix Inverse(float epsilon=1e-8F)const; C44Matrix Transpose()const; C44Matrix AffineInverse()const; C44Matrix AffineInverse(float)const; C44Matrix AffineInverse(const C3Vector&)const;
 C44Matrix& operator+=(const C44Matrix&);C44Matrix& operator-=(const C44Matrix&);C44Matrix& operator*=(const C44Matrix&);C44Matrix& operator*=(float);C44Matrix& operator/=(float);
 static C44Matrix Rotation(const C3Vector&,float,bool=true);void Translate(const C3Vector&);void Scale(const C3Vector&);void Scale(float);void Rotate(const C3Vector&,const C3Vector&,char);void Rotate(const C4Quaternion&);
}; static_assert(sizeof(C44Matrix)==64,"C44Matrix layout mismatch");
bool operator==(const C44Matrix&,const C44Matrix&);bool operator!=(const C44Matrix&,const C44Matrix&);C44Matrix operator+(C44Matrix,const C44Matrix&);C44Matrix operator-(C44Matrix,const C44Matrix&);C44Matrix operator*(const C44Matrix&,const C44Matrix&);C44Matrix operator*(C44Matrix,float);C44Matrix operator*(float,C44Matrix);C44Matrix operator/(C44Matrix,float);C3Vector operator*(const C44Matrix&,const C3Vector&);C4Vector operator*(const C44Matrix&,const C4Vector&);
}
