#include "ENGINE/Source/Services/particlemisc.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
namespace { unsigned char Byte(float v){return static_cast<unsigned char>(std::clamp(v,0.0F,255.0F));} }
CParticleKey::CParticleKey(){m_startColor=NTempest::CImVector(0xFFFFFFFFU);m_endColor=m_startColor;}
void CParticleKey::Interpolate(float time,NTempest::CImVector*c,int*h,int*tail,float*s)const{if(!c||!h||!tail||!s)throw std::invalid_argument("particle key output");float t=std::clamp((time-m_startTime)*m_ooSegLength*0.99F+0.005F,0.0F,1.0F);c->a=Byte(m_startColor.a+m_deltaColor[0]*t);c->r=Byte(m_startColor.r+m_deltaColor[1]*t);c->g=Byte(m_startColor.g+m_deltaColor[2]*t);c->b=Byte(m_startColor.b+m_deltaColor[3]*t);*s=m_startScale+m_deltaScale*t;float cellT=m_repeat==1.0F?t:std::fmod(t*m_repeat,1.0F);*h=static_cast<int>(m_initialHead+m_deltaHead*cellT);*tail=static_cast<int>(m_initialTail+m_deltaTail*cellT);}
void CParticleKey::SetSegment(float a,float b){if(a<0||a>=b||std::abs(a-b)<1e-7F)throw std::invalid_argument("particle key segment");m_normStartTime=a;m_normEndTime=b;SetLifeSpan(m_lifeSpan);}
void CParticleKey::SetLifeSpan(float life){if(life<=1e-7F)throw std::invalid_argument("particle lifespan");m_lifeSpan=life;m_startTime=life*m_normStartTime;m_endTime=life*m_normEndTime;m_ooSegLength=1.0F/(m_endTime-m_startTime);}
void CParticleKey::SetRepeat(float repeat){if(repeat<1)throw std::invalid_argument("particle repeat");m_repeat=repeat;}
void CParticleKey::SetColors(NTempest::CImVector a,NTempest::CImVector b){m_startColor=a;m_endColor=b;m_deltaColor[0]=b.a-a.a;m_deltaColor[1]=b.r-a.r;m_deltaColor[2]=b.g-a.g;m_deltaColor[3]=b.b-a.b;}
void CParticleKey::SetHeadCells(int a,int b){m_headStart=a;m_headEnd=b;m_initialHead=b<a?a+1:a;m_deltaHead=b<a?b-a-1:b-a+1;}
void CParticleKey::SetTailCells(int a,int b){m_tailStart=a;m_tailEnd=b;m_initialTail=b<a?a+1:a;m_deltaTail=b<a?b-a-1:b-a+1;}
void CParticleKey::SetScales(float a,float b){m_startScale=a;m_endScale=b;m_deltaScale=b-a;}
void CParticleKey::Segment(float*a,float*b)const{if(a)*a=m_startTime;if(b)*b=m_endTime;}void CParticleKey::Repeat(float*v)const{if(v)*v=m_repeat;}void CParticleKey::LifeSpan(float*v)const{if(v)*v=m_lifeSpan;}void CParticleKey::Colors(NTempest::CImVector*a,NTempest::CImVector*b)const{if(a)*a=m_startColor;if(b)*b=m_endColor;}void CParticleKey::HeadCells(int*a,int*b)const{if(a)*a=m_headStart;if(b)*b=m_headEnd;}void CParticleKey::TailCells(int*a,int*b)const{if(a)*a=m_tailStart;if(b)*b=m_tailEnd;}void CParticleKey::Scales(float*a,float*b)const{if(a)*a=m_startScale;if(b)*b=m_endScale;}
