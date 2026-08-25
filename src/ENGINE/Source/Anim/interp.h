#pragma once

#include "ENGINE/Source/Services/datamgr.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c4quaternion.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <type_traits>
#include <vector>

enum KEYTYPE : std::uint32_t { KEYTYPE_NONE, KEYTYPE_LINEAR, KEYTYPE_HERMITE, KEYTYPE_BEZIER };

struct CKeyFrame { int time = 0; };
struct CKeySeq { unsigned start = 0, count = 0; };
struct CKeyTrackStatus { unsigned currKey = 0, nextKey = 0; int timepastkey = 0; };
struct CAnimSequence;

namespace NTempest {
struct C4QuaternionCompressed {
    std::uint64_t m_data = 0;
    static constexpr std::uint64_t kComponentMask = 0x1FFFFFULL;
    static std::int32_t SignExtend21(std::uint32_t value) {
        return static_cast<std::int32_t>(value << 11U) >> 11U;
    }
    float GetW(float x, float y, float z) const {
        const float squared = x * x + y * y + z * z;
        return std::fabs(squared - 1.0F) < 0.00000095367432F
            ? 0.0F : std::sqrt((std::max)(0.0F, 1.0F - squared));
    }
    float GetW() const {
        const auto quaternion = static_cast<C4Quaternion>(*this);
        return quaternion.w;
    }
    void Set(const C4Quaternion& q) {
        const std::int64_t sign = q.w >= 0.0F ? 1 : -1;
        const auto x = sign * static_cast<std::int64_t>(q.x * 2097152.0F);
        const auto y = sign * static_cast<std::int64_t>(q.y * 1048576.0F);
        const auto z = sign * static_cast<std::int64_t>(q.z * 1048576.0F);
        m_data = (static_cast<std::uint64_t>(x) << 42U) |
                 ((static_cast<std::uint64_t>(y) & kComponentMask) << 21U) |
                 (static_cast<std::uint64_t>(z) & kComponentMask);
    }
    operator C4Quaternion() const {
        const float x = static_cast<float>(static_cast<std::int64_t>(m_data) >> 42U) / 2097152.0F;
        const float y = static_cast<float>(SignExtend21(static_cast<std::uint32_t>(m_data >> 21U))) / 1048576.0F;
        const float z = static_cast<float>(SignExtend21(static_cast<std::uint32_t>(m_data))) / 1048576.0F;
        return {GetW(x, y, z), x, y, z};
    }
};
static_assert(sizeof(C4QuaternionCompressed) == 8, "recovered packed quaternion layout");
}

struct CKeyFrameTrackBase {
    std::vector<CKeyFrame> m_keyFrames;
    std::vector<CKeySeq> m_indices;
    unsigned m_keyFrameSize = 0;
    unsigned m_globalSeqId = ~0U;
    void SetNumKeys(unsigned count,unsigned keySize=sizeof(CKeyFrame));
    void AddKey(int time);
    void SetSequenceIndices(unsigned sequence, unsigned start, unsigned count);
    void SetSequenceIndices(const std::vector<CAnimSequence>& sequences);
    bool SetAnimTime(unsigned sequence, int time, int duration, CKeyTrackStatus& status) const;
    bool ISetAnimTime(unsigned sequence,int time,int duration,CKeyTrackStatus&status)const{return SetAnimTime(sequence,time,duration,status);}
    bool ISetAnimTimeConstSeq(unsigned sequence,int time,int duration,CKeyTrackStatus&status)const{return SetAnimTime(sequence,time,duration,status);}
    bool JustPastKeyForward(const CKeyTrackStatus& status, unsigned key) const;
    bool JustPastKeyBackward(const CKeyTrackStatus& status, unsigned key) const;
    bool JustPastKey(const CKeyTrackStatus& status, unsigned key, bool forward) const;
    unsigned NextKey(unsigned key, unsigned sequence) const;
    const CKeyFrame* GetKeyFrame(unsigned key) const;
    int TimeDiff(unsigned first, unsigned second) const;
    int TimeDiff(unsigned first,unsigned second,unsigned sequenceTime) const;
    unsigned FindKeyForTime(unsigned sequence, int time) const;
    unsigned FindKeyForTime(unsigned sequence,unsigned currentKey,int time) const;
    unsigned FindKeyForTimeConstSeq(unsigned sequence, int time) const;
    unsigned FindKeyForTimeConstSeq(unsigned currentKey,int time,bool) const;
};

template<class T> inline T AnimLerp(const T& a,const T& b,float t){return a+(b-a)*t;}
template<> inline float AnimLerp(const float&a,const float&b,float t){return a+(b-a)*t;}
template<> inline unsigned AnimLerp(const unsigned&a,const unsigned&b,float t){return t<1.0F?a:b;}
template<> inline C3Color AnimLerp(const C3Color&a,const C3Color&b,float t){return {a.r+(b.r-a.r)*t,a.g+(b.g-a.g)*t,a.b+(b.b-a.b)*t};}
template<> inline NTempest::C4Quaternion AnimLerp(const NTempest::C4Quaternion&a,const NTempest::C4Quaternion&b,float t){return NTempest::C4Quaternion::Slerp(a,b,t);}
template<class T> inline T AnimHermite(const T&a,const T&out,const T&b,const T&in,float t){float t2=t*t,t3=t2*t;return a*(2*t3-3*t2+1)+out*(t3-2*t2+t)+b*(-2*t3+3*t2)+in*(t3-t2);}
template<> inline float AnimHermite(const float&a,const float&out,const float&b,const float&in,float t){float t2=t*t,t3=t2*t;return a*(2*t3-3*t2+1)+out*(t3-2*t2+t)+b*(-2*t3+3*t2)+in*(t3-t2);}
template<> inline unsigned AnimHermite(const unsigned&a,const unsigned&,const unsigned&b,const unsigned&,float t){return AnimLerp(a,b,t);}
template<> inline C3Color AnimHermite(const C3Color&a,const C3Color&out,const C3Color&b,const C3Color&in,float t){return{AnimHermite(a.r,out.r,b.r,in.r,t),AnimHermite(a.g,out.g,b.g,in.g,t),AnimHermite(a.b,out.b,b.b,in.b,t)};}
template<> inline NTempest::C4Quaternion AnimHermite(const NTempest::C4Quaternion&a,const NTempest::C4Quaternion&out,const NTempest::C4Quaternion&b,const NTempest::C4Quaternion&in,float t){return NTempest::C4Quaternion::Squad(a,b,out,in,t);}
template<class T> inline T AnimBezier(const T&a,const T&out,const T&b,const T&in,float t){const float u=1.0F-t;return a*(u*u*u)+out*(3.0F*u*u*t)+in*(3.0F*u*t*t)+b*(t*t*t);}
template<> inline float AnimBezier(const float&a,const float&out,const float&b,const float&in,float t){const float u=1.0F-t;return a*(u*u*u)+out*(3.0F*u*u*t)+in*(3.0F*u*t*t)+b*(t*t*t);}
template<> inline unsigned AnimBezier(const unsigned&a,const unsigned&,const unsigned&b,const unsigned&,float t){return AnimLerp(a,b,t);}
template<> inline C3Color AnimBezier(const C3Color&a,const C3Color&out,const C3Color&b,const C3Color&in,float t){return{AnimBezier(a.r,out.r,b.r,in.r,t),AnimBezier(a.g,out.g,b.g,in.g,t),AnimBezier(a.b,out.b,b.b,in.b,t)};}
template<> inline NTempest::C4Quaternion AnimBezier(const NTempest::C4Quaternion&a,const NTempest::C4Quaternion&out,const NTempest::C4Quaternion&b,const NTempest::C4Quaternion&in,float t){return NTempest::C4Quaternion::Squad(a,b,out,in,t);}

template<class Stored,class Value=Stored> struct CAnimKey { int time=0; Stored value{}; Value inTan{}; Value outTan{}; };

template<class Stored,class Value=Stored> class CKeyFrameTrack : public CKeyFrameTrackBase {
public:
    KEYTYPE m_trackType=KEYTYPE_NONE;
    std::vector<CAnimKey<Stored,Value>> keys;
    void SetNumKeys(unsigned n){keys.reserve(n);CKeyFrameTrackBase::SetNumKeys(n,sizeof(CAnimKey<Stored,Value>));}
    void AddKey(int time,const Stored& value){keys.push_back({time,value,{},{}});CKeyFrameTrackBase::AddKey(time);}
    void AddKey(int time,const Stored& value,const Value& in,const Value& out){keys.push_back({time,value,in,out});CKeyFrameTrackBase::AddKey(time);}
    Value GetLinearKey(unsigned i)const{return Convert(keys.at(i).value);}
    Value GetSplineKey(unsigned i)const{return Convert(keys.at(i).value);}
    Value ToLinearKey(unsigned i)const{return GetLinearKey(i);}
    Value ToSplineKey(unsigned i)const{return GetSplineKey(i);}
    Value InterpolateLinear(unsigned i,unsigned j,float t)const{return AnimLerp(Convert(keys.at(i).value),Convert(keys.at(j).value),t);}
    Value InterpolateHermite(unsigned i,unsigned j,float t)const{return AnimHermite(Convert(keys.at(i).value),keys.at(i).outTan,Convert(keys.at(j).value),keys.at(j).inTan,t);}
    Value InterpolateBezier(unsigned i,unsigned j,float t)const{return AnimBezier(Convert(keys.at(i).value),keys.at(i).outTan,Convert(keys.at(j).value),keys.at(j).inTan,t);}
    bool InterpolateVolatileFewKeys(unsigned sequence,int time,Value&out,CKeyTrackStatus*s=nullptr)const{return Interpolate(sequence,time,out,s);}
    bool InterpolateRetainedFewKeys(unsigned sequence,int time,Value&out,CKeyTrackStatus*s=nullptr)const{return Interpolate(sequence,time,out,s);}
    bool Interpolate(unsigned sequence,int time,Value& out,CKeyTrackStatus* status=nullptr)const{
        if(keys.empty())return false;
        const unsigned sequenceId=m_globalSeqId==~0U?sequence:0U;
        if(sequenceId>=m_indices.size()||m_indices[sequenceId].count==0)return false;
        unsigned i=FindKeyForTime(sequenceId,time);unsigned j=NextKey(i,sequenceId);if(j>=keys.size())j=i;
        const int span=keys[j].time-keys[i].time;float t=span>0?std::clamp(float(time-keys[i].time)/span,0.0F,1.0F):0.0F;
        Value a=Convert(keys[i].value),b=Convert(keys[j].value);
        if(m_trackType==KEYTYPE_HERMITE)out=AnimHermite(a,keys[i].outTan,b,keys[j].inTan,t);else if(m_trackType==KEYTYPE_BEZIER)out=AnimBezier(a,keys[i].outTan,b,keys[j].inTan,t);else out=AnimLerp(a,b,t);
        if(status){status->currKey=i;status->nextKey=j;status->timepastkey=time-keys[i].time;}return true;
    }
private:
    static Value Convert(const Stored& v){if constexpr(std::is_same_v<Stored,NTempest::C4QuaternionCompressed>)return static_cast<NTempest::C4Quaternion>(v);else return v;}
};

float EvaluateCubicPolynomial(float a,float b,float c,float d,float t);
float Blend(float a,float b,float amount);
NTempest::C3Vector Blend(const NTempest::C3Vector&,const NTempest::C3Vector&,float);
