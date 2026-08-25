#include "ENGINE/Source/Tempest/c3spline.h"

#include <algorithm>
#include <cmath>

namespace NTempest {
namespace { float Clamp01(float t){return std::clamp(t,0.0F,1.0F);} }

void C3Spline::SetPoint(unsigned i,const C3Vector&p){if(i<points.Count()){points[i]=p;ValidateCache();}}
void C3Spline::SetPoints(const C3Vector*p,unsigned n){points.Set(p,n);ValidateCache();}
C3Vector C3Spline::Pos(float t,EvalType type)const{if(type==EVAL_ARCLENGTH&&cachedLength>0)t=std::clamp(t/cachedLength,0.0F,1.0F);return Evaluate(t);}
C3Vector C3Spline::Vel(float t,EvalType type)const{if(type==EVAL_ARCLENGTH&&cachedLength>0)t=std::clamp(t/cachedLength,0.0F,1.0F);return EvaluateDer1(t);}
void C3Spline::Frame(float t,C3Vector&tan,C3Vector&normal,C3Vector&binormal)const{tan=EvaluateDer1(t);if(tan.SquaredMag())tan.Normalize();C3Vector d2=EvaluateDer2(t);binormal=C3Vector::Cross(tan,d2);if(binormal.SquaredMag())binormal.Normalize();normal=C3Vector::Cross(binormal,tan);}
float C3Spline::Curvature(float t)const{C3Vector d1=EvaluateDer1(t),d2=EvaluateDer2(t);float m=d1.SquaredMag();return m>0?std::sqrt(C3Vector::Cross(d1,d2).SquaredMag())/std::pow(m,1.5F):0;}
float C3Spline::SegLength(unsigned seg,unsigned steps)const{float sum=0;C3Vector last=Evaluate(float(seg));for(unsigned i=1;i<=steps;++i){C3Vector p=Evaluate(float(seg)+float(i)/steps);sum+=std::sqrt((p-last).SquaredMag());last=p;}return sum;}
float C3Spline::ILength()const{return cachedLength;}float C3Spline::ParametricSegT(float t,unsigned&seg)const{unsigned n=std::max(1U,points.Count()>1?points.Count()-1:1);float s=Clamp01(t)*n;seg=std::min(unsigned(s),n-1);return s-seg;}float C3Spline::ArclengthSegT(float d,unsigned&seg)const{if(!cachedSegLength.Count()){seg=0;return 0;}d=std::clamp(d,0.0F,cachedLength);float prior=0;for(seg=0;seg<cachedSegLength.Count();++seg){float next=prior+cachedSegLength[seg];if(d<=next)return cachedSegLength[seg]?((d-prior)/cachedSegLength[seg]):0;prior=next;}seg=cachedSegLength.Count()-1;return 1;}
void C3Spline::ValidateCache(){unsigned segs=points.Count()>1?points.Count()-1:0;cachedSegLength.SetCount(segs);cachedLength=0;for(unsigned i=0;i<segs;++i){cachedSegLength[i]=SegLength(i);cachedLength+=cachedSegLength[i];}}

C3Vector C3Spline_Bezier3::Evaluate(float t)const{if(points.Count()<4)return {};unsigned seg;float u=ParametricSegT(t,seg);unsigned i=std::min(seg*3,points.Count()-4);float v=1-u;return points[i]*(v*v*v)+points[i+1]*(3*v*v*u)+points[i+2]*(3*v*u*u)+points[i+3]*(u*u*u);}
C3Vector C3Spline_Bezier3::EvaluateDer1(float t)const{if(points.Count()<4)return {};unsigned seg;float u=ParametricSegT(t,seg);unsigned i=std::min(seg*3,points.Count()-4);float v=1-u;return (points[i+1]-points[i])*(3*v*v)+(points[i+2]-points[i+1])*(6*v*u)+(points[i+3]-points[i+2])*(3*u*u);}
C3Vector C3Spline_Bezier3::EvaluateDer2(float t)const{if(points.Count()<4)return {};unsigned seg;float u=ParametricSegT(t,seg);unsigned i=std::min(seg*3,points.Count()-4);return (points[i+2]-points[i+1]*2.0F+points[i])*(6*(1-u))+(points[i+3]-points[i+2]*2.0F+points[i+1])*(6*u);}
C3Vector C3Spline_Bezier3::IPosParametric(float t)const{return Evaluate(t);}C3Vector C3Spline_Bezier3::IVelParametric(float t)const{return EvaluateDer1(t);}C3Vector C3Spline_Bezier3::IPosArclength(float t)const{return Pos(t,EVAL_ARCLENGTH);}C3Vector C3Spline_Bezier3::IVelArclength(float t)const{return Vel(t,EVAL_ARCLENGTH);}void C3Spline_Bezier3::IFrameArclength(float t,C3Vector&a,C3Vector&b,C3Vector&c)const{Frame(t,a,b,c);}void C3Spline_Bezier3::ISetPoints(const C3Vector*p,unsigned n){SetPoints(p,n);}void C3Spline_Bezier3::IValidateCache(){ValidateCache();}

C3Vector C3Spline_CatmullRom::Evaluate(float t)const{if(points.Count()<2)return points.Count()?points[0]:C3Vector();unsigned seg;float u=ParametricSegT(t,seg);auto at=[&](int i)->const C3Vector&{return points[std::clamp(i,0,int(points.Count()-1))];};const auto&p0=at(int(seg)-1),&p1=at(seg),&p2=at(seg+1),&p3=at(seg+2);return (p1*2+(p2-p0)*u+(p0*2-p1*5+p2*4-p3)*(u*u)+(-1.0F*p0+p1*3-p2*3+p3)*(u*u*u))*.5F;}
C3Vector C3Spline_CatmullRom::EvaluateDer1(float t)const{float e=1e-3F;return (Evaluate(Clamp01(t+e))-Evaluate(Clamp01(t-e)))/(2*e);}C3Vector C3Spline_CatmullRom::EvaluateDer2(float t)const{float e=1e-3F;return (Evaluate(Clamp01(t+e))-Evaluate(t)*2.0F+Evaluate(Clamp01(t-e)))/(e*e);}
C3Vector C3Spline_CatmullRom::IPosParametric(float t)const{return Evaluate(t);}C3Vector C3Spline_CatmullRom::IVelParametric(float t)const{return EvaluateDer1(t);}C3Vector C3Spline_CatmullRom::IPosArclength(float t)const{return Pos(t,EVAL_ARCLENGTH);}C3Vector C3Spline_CatmullRom::IVelArclength(float t)const{return Vel(t,EVAL_ARCLENGTH);}void C3Spline_CatmullRom::IFrameArclength(float t,C3Vector&a,C3Vector&b,C3Vector&c)const{Frame(t,a,b,c);}void C3Spline_CatmullRom::ISetPoints(const C3Vector*p,unsigned n){SetPoints(p,n);}void C3Spline_CatmullRom::IValidateCache(){ValidateCache();}
} // namespace NTempest
