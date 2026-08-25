#include "ENGINE/Source/Tempest/c33matrix.h"
#include "ENGINE/Source/Tempest/c4quaternion.h"
#include <algorithm>
#include <cmath>
namespace NTempest {
namespace {
C33Matrix Rx(float v){float c=std::cos(v),s=std::sin(v);return {1,0,0,0,c,s,0,-s,c};}
C33Matrix Ry(float v){float c=std::cos(v),s=std::sin(v);return {c,0,-s,0,1,0,s,0,c};}
C33Matrix Rz(float v){float c=std::cos(v),s=std::sin(v);return {c,s,0,-s,c,0,0,0,1};}
void Euler(C33Matrix& out,char p0,char p1,char p2,float v0,float v1,float v2){auto r=[](char p,float v){return p=='X'?Rx(v):p=='Y'?Ry(v):Rz(v);};out=r(p0,v0)*r(p1,v1)*r(p2,v2);}
}
C33Matrix __fastcall operator*(const C33Matrix& l,const C33Matrix& r){C33Matrix o(0,0,0,0,0,0,0,0,0);for(int c=0;c<3;++c)for(int row=0;row<3;++row)for(int k=0;k<3;++k)o.Data()[c*3+row]+=l.Data()[k*3+row]*r.Data()[c*3+k];return o;}
C3Vector __fastcall operator*(const C33Matrix& m,const C3Vector& v){return {m.a0*v.x+m.b0*v.y+m.c0*v.z,m.a1*v.x+m.b1*v.y+m.c1*v.z,m.a2*v.x+m.b2*v.y+m.c2*v.z};}
double C33Matrix::Determinant()const{return a0*(b1*c2-b2*c1)-b0*(a1*c2-a2*c1)+c0*(a1*b2-a2*b1);}
C33Matrix C33Matrix::Cofactors()const{return {b1*c2-b2*c1,-(b0*c2-b2*c0),b0*c1-b1*c0,-(a1*c2-a2*c1),a0*c2-a2*c0,-(a0*c1-a1*c0),a1*b2-a2*b1,-(a0*b2-a2*b0),a0*b1-a1*b0};}
C33Matrix C33Matrix::Adjoint()const{C33Matrix c=Cofactors();return {c.a0,c.b0,c.c0,c.a1,c.b1,c.c1,c.a2,c.b2,c.c2};}
C33Matrix C33Matrix::Inverse(float epsilon)const{float d=static_cast<float>(Determinant());if(std::fabs(d)<=epsilon)return {};C33Matrix o=Adjoint();for(float& v:*reinterpret_cast<float(*)[9]>(o.Data()))v/=d;return o;}
C33Matrix C33Matrix::AffineInverse()const{return Inverse();} C33Matrix C33Matrix::AffineInverse(float scale)const{C33Matrix o=Adjoint();for(float&v:*reinterpret_cast<float(*)[9]>(o.Data()))v*=scale;return o;} C33Matrix C33Matrix::AffineInverse(const C3Vector& s)const{C33Matrix o=Adjoint();o.Scale(s);return o;}
C33Matrix& C33Matrix::Rotation(float angle){*this=Rz(angle);return *this;}
C33Matrix C33Matrix::Rotation(const C3Vector& axis,float angle,bool unit){C3Vector n=axis;if(!unit)n.Normalize();float c=std::cos(angle),s=std::sin(angle),t=1-c;return {t*n.x*n.x+c,t*n.x*n.y+s*n.z,t*n.x*n.z-s*n.y,t*n.x*n.y-s*n.z,t*n.y*n.y+c,t*n.y*n.z+s*n.x,t*n.x*n.z+s*n.y,t*n.y*n.z-s*n.x,t*n.z*n.z+c};}
void C33Matrix::Scale(float s){for(float&v:*reinterpret_cast<float(*)[9]>(Data()))v*=s;} void C33Matrix::Scale(float x,float y,float z){a0*=x;a1*=x;a2*=x;b0*=y;b1*=y;b2*=y;c0*=z;c1*=z;c2*=z;} void C33Matrix::Scale(const C3Vector&s){Scale(s.x,s.y,s.z);} void C33Matrix::Scale(float x,float y){a0*=x;a1*=x;b0*=y;b1*=y;} void C33Matrix::Scale(const C2Vector&s){Scale(s.x,s.y);}
void C33Matrix::Rotate(float a){*this=*this*Rz(a);} void C33Matrix::Rotate(const C3Vector& axis,const C3Vector& angles,char unit){*this=*this*Rotation(axis,angles.x,unit!=0);} void C33Matrix::Rotate(const C4Quaternion&q){C33Matrix r;r.a0=1-2*(q.y*q.y+q.z*q.z);r.a1=2*(q.x*q.y+q.z*q.w);r.a2=2*(q.x*q.z-q.y*q.w);r.b0=2*(q.x*q.y-q.z*q.w);r.b1=1-2*(q.x*q.x+q.z*q.z);r.b2=2*(q.y*q.z+q.x*q.w);r.c0=2*(q.x*q.z+q.y*q.w);r.c1=2*(q.y*q.z-q.x*q.w);r.c2=1-2*(q.x*q.x+q.y*q.y);*this=*this*r;}
void C33Matrix::Translate(const C2Vector&m){c0+=a0*m.x+b0*m.y;c1+=a1*m.x+b1*m.y;}
bool C33Matrix::ToEulerAnglesXYZ(float&x,float&y,float&z)const{y=std::asin(std::clamp(-a2,-1.0F,1.0F));float c=std::cos(y);if(std::fabs(c)>1e-6F){x=std::atan2(b2,c2);z=std::atan2(a1,a0);return true;}x=std::atan2(-c1,b1);z=0;return false;}
#define TO_EULER(Name,A,B,C) bool C33Matrix::ToEulerAngles##Name(float&A,float&B,float&C)const{return ToEulerAnglesXYZ(A,B,C);}
TO_EULER(XZY,x,z,y) TO_EULER(YXZ,y,x,z) TO_EULER(YZX,y,z,x) TO_EULER(ZXY,z,x,y) TO_EULER(ZYX,z,y,x)
#undef TO_EULER
void C33Matrix::FromEulerAnglesXYZ(float x,float y,float z){Euler(*this,'X','Y','Z',x,y,z);} void C33Matrix::FromEulerAnglesXZY(float x,float z,float y){Euler(*this,'X','Z','Y',x,z,y);} void C33Matrix::FromEulerAnglesYXZ(float y,float x,float z){Euler(*this,'Y','X','Z',y,x,z);} void C33Matrix::FromEulerAnglesYZX(float y,float z,float x){Euler(*this,'Y','Z','X',y,z,x);} void C33Matrix::FromEulerAnglesZXY(float z,float x,float y){Euler(*this,'Z','X','Y',z,x,y);} void C33Matrix::FromEulerAnglesZYX(float z,float y,float x){Euler(*this,'Z','Y','X',z,y,x);}
} // namespace NTempest
