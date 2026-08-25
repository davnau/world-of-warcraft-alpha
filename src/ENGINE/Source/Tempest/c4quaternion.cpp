#include "ENGINE/Source/Tempest/c4quaternion.h"
#include "ENGINE/Source/Tempest/c33matrix.h"
#include <algorithm>
#include <cmath>
namespace NTempest {
namespace {
float QDot(const C4Quaternion&a,const C4Quaternion&b){return a.x*b.x+a.y*b.y+a.z*b.z+a.w*b.w;}
C4Quaternion QScale(const C4Quaternion&q,float s){return {q.w*s,q.x*s,q.y*s,q.z*s};}
C4Quaternion QAdd(const C4Quaternion&a,const C4Quaternion&b){return {a.w+b.w,a.x+b.x,a.y+b.y,a.z+b.z};}
C4Quaternion QMul(const C4Quaternion&a,const C4Quaternion&b){return {a.w*b.w-a.x*b.x-a.y*b.y-a.z*b.z,a.w*b.x+a.x*b.w+a.y*b.z-a.z*b.y,a.w*b.y-a.x*b.z+a.y*b.w+a.z*b.x,a.w*b.z+a.x*b.y-a.y*b.x+a.z*b.w};}
C4Quaternion QNorm(const C4Quaternion&q){float m=std::sqrt(QDot(q,q));return m?QScale(q,1/m):C4Quaternion();}
}
float C4Vector::SquaredMag()const{return x*x+y*y+z*z+w*w;}
void C4Quaternion::FromRotationMatrix(const C33Matrix&m){float tr=m.a0+m.b1+m.c2;if(tr>0){float s=std::sqrt(tr+1)*2;w=.25F*s;x=(m.b2-m.c1)/s;y=(m.c0-m.a2)/s;z=(m.a1-m.b0)/s;}else if(m.a0>m.b1&&m.a0>m.c2){float s=std::sqrt(1+m.a0-m.b1-m.c2)*2;w=(m.b2-m.c1)/s;x=.25F*s;y=(m.b0+m.a1)/s;z=(m.c0+m.a2)/s;}else if(m.b1>m.c2){float s=std::sqrt(1+m.b1-m.a0-m.c2)*2;w=(m.c0-m.a2)/s;x=(m.b0+m.a1)/s;y=.25F*s;z=(m.c1+m.b2)/s;}else{float s=std::sqrt(1+m.c2-m.a0-m.b1)*2;w=(m.a1-m.b0)/s;x=(m.c0+m.a2)/s;y=(m.c1+m.b2)/s;z=.25F*s;}*this=QNorm(*this);}
void C4Quaternion::FromRotationMatrixInv(const C33Matrix&r){FromRotationMatrix(r);*this=Conjugate();}
void C4Quaternion::FromAngleAxis(float angle,const C3Vector&axis){C3Vector n=axis;if(n.SquaredMag()!=0)n.Normalize();float h=angle*.5F,s=std::sin(h);x=n.x*s;y=n.y*s;z=n.z*s;w=std::cos(h);}
void C4Quaternion::ToAngleAxis(float&angle,C3Vector&axis)const{C4Quaternion q=QNorm(*this);angle=2*std::acos(std::clamp(q.w,-1.0F,1.0F));float s=std::sqrt(std::max(0.0F,1-q.w*q.w));axis=s<1e-6F?C3Vector(1,0,0):C3Vector(q.x/s,q.y/s,q.z/s);}
C4Quaternion C4Quaternion::Conjugate()const{return {w,-x,-y,-z};} C4Quaternion C4Quaternion::Inverse()const{float m=SquaredMag();return m?QScale(Conjugate(),1/m):C4Quaternion();}
C4Quaternion C4Quaternion::Exp()const{float a=std::sqrt(x*x+y*y+z*z),s=a>1e-6F?std::sin(a)/a:1;float ew=std::exp(w);return {ew*std::cos(a),ew*x*s,ew*y*s,ew*z*s};}
C4Quaternion C4Quaternion::Log()const{float m=std::sqrt(SquaredMag()),a=std::acos(std::clamp(w/m,-1.0F,1.0F)),s=std::sqrt(x*x+y*y+z*z);float k=s>1e-6F?a/s:0;return {std::log(m),x*k,y*k,z*k};}
C4Quaternion C4Quaternion::Slerp(const C4Quaternion&p,const C4Quaternion&q,float t){C4Quaternion e=q;float d=QDot(p,q);if(d<0){e=QScale(q,-1);d=-d;}if(d>.9995F)return QNorm(QAdd(QScale(p,1-t),QScale(e,t)));float a=std::acos(std::clamp(d,-1.0F,1.0F)),s=std::sin(a);return QAdd(QScale(p,std::sin((1-t)*a)/s),QScale(e,std::sin(t*a)/s));}
void C4Quaternion::SquadInterm(const C4Quaternion&q0,const C4Quaternion&q1,const C4Quaternion&q2,C4Quaternion&a,C4Quaternion&b){C4Quaternion inv=q1.Inverse();C4Quaternion e=QScale(QAdd(QMul(inv,q0).Log(),QMul(inv,q2).Log()),-.25F).Exp();a=QMul(q1,e);b=a;}
void C4Quaternion::SquadIntermMaxCompat(const C4Quaternion&q0,const C4Quaternion&q1,const C4Quaternion&q2,C4Quaternion&a,C4Quaternion&b){SquadInterm(q0,q1,q2,a,b);}
void C4Quaternion::SquadIntermTCB(const C4Quaternion&q0,const C4Quaternion&q1,const C4Quaternion&q2,float,float,float,float,float,float,C4Quaternion&a,C4Quaternion&b){SquadInterm(q0,q1,q2,a,b);}
C4Quaternion C4Quaternion::Squad(const C4Quaternion&p,const C4Quaternion&q,const C4Quaternion&a,const C4Quaternion&b,float t){return Slerp(Slerp(p,q,t),Slerp(a,b,t),2*t*(1-t));}
} // namespace NTempest
