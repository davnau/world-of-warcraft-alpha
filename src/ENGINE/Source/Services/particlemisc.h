#pragma once
#include "ENGINE/Source/Tempest/cimvector.h"

class CParticleKey {
public:
    CParticleKey();
    CParticleKey& operator=(const CParticleKey&) = default;
    void Interpolate(float time,NTempest::CImVector* color,int* headCell,int* tailCell,float* scale) const;
    void SetSegment(float normStartTime,float normEndTime);void SetLifeSpan(float lifeSpan);void SetRepeat(float repeat);
    void SetColors(NTempest::CImVector start,NTempest::CImVector end);void SetHeadCells(int start,int end);void SetTailCells(int start,int end);void SetScales(float start,float end);
    void Segment(float* start,float* end)const;void Repeat(float*)const;void LifeSpan(float*)const;void Colors(NTempest::CImVector*,NTempest::CImVector*)const;void HeadCells(int*,int*)const;void TailCells(int*,int*)const;void Scales(float*,float*)const;
    NTempest::CImVector m_startColor{};int m_deltaColor[4]{};int m_initialHead=0,m_deltaHead=0,m_initialTail=0,m_deltaTail=0;float m_startScale=1,m_deltaScale=0,m_startTime=0,m_ooSegLength=1,m_endTime=1;NTempest::CImVector m_endColor{};int m_headStart=0,m_headEnd=0,m_tailStart=0,m_tailEnd=0;float m_endScale=1,m_repeat=1,m_normStartTime=0,m_normEndTime=1,m_lifeSpan=1;
};
