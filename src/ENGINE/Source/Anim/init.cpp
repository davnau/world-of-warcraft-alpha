#include "ENGINE/Source/Anim/init.h"
#include <cstring>
#include <stdexcept>
namespace{template<class T>T*Create(CAnimData&d,ANIM_OBJECT_TYPE type){auto p=std::make_unique<T>();p->type=type;T*r=p.get();d.obj.push_back(r);d.ownedObjs.push_back(std::move(p));return r;}template<class S,class V>void SetTrack(CKeyFrameTrack<S,V>&t,KEYTYPE type,const std::vector<CAnimKey<S,V>>&keys){t.m_trackType=type;t.keys=keys;t.m_keyFrames.clear();for(const auto&k:keys)t.CKeyFrameTrackBase::AddKey(k.time);}}
bool CAnimTransform::Animates()const{return!translation.keys.empty()||!rotation.keys.empty()||!scale.keys.empty();}bool CAnimData::Animates()const{for(auto*o:obj)if(o&&o->Animates())return true;return!tex.empty()||!cameraObjs.empty();}bool CAnimData::Moves()const{for(auto*o:obj)if(o&&!o->translation.keys.empty())return true;return false;}
CAnimBoneObj::CAnimBoneObj()=default;CAnimLightObj::CAnimLightObj()=default;CAnimModelObj::CAnimModelObj()=default;CAnimEmitter2Obj::CAnimEmitter2Obj()=default;CAnimMaterialLayer::CAnimMaterialLayer()=default;CAnimObjStatus::CAnimObjStatus()=default;CAnimObjBlendStatus::CAnimObjBlendStatus()=default;CAnimObjBlendStatus::~CAnimObjBlendStatus()=default;CAnimLayerStatus::CAnimLayerStatus()=default;CSeqOrdering::CSeqOrdering()=default;
std::shared_ptr<CAnimData>AnimCreate(){auto d=std::make_shared<CAnimData>();AnimInit(*d);return d;}void AnimInit(CAnimData&d){d.flags=0;d.obj.clear();d.ownedObjs.clear();d.pivots.clear();d.headarray.clear();}
std::shared_ptr<CAnimData> AnimDuplicate(const CAnimData& s)
{
    auto d=AnimCreate();
    d->seq=s.seq;
    d->globalSeqLength=s.globalSeqLength;
    d->objectOrder=s.objectOrder;
    d->cameraOrder=s.cameraOrder;
    d->sequenceOrder=s.sequenceOrder;
    d->pivots=s.pivots;
    d->geo=s.geo;
    d->tex=s.tex;
    d->cameraObjs=s.cameraObjs;
    d->layers=s.layers;
    d->flags=s.flags;

    for(const auto* o:s.obj){
        if(!o)continue;
        CAnimObj* n=nullptr;
        switch(o->type){
        case ANIM_OBJ_BONE:{
            auto* value=AnimObjectCreateBone(*d);
            *value=*static_cast<const CAnimBoneObj*>(o);
            n=value;
            break;
        }
        case ANIM_OBJ_LIGHT:{
            auto* value=AnimObjectCreateLight(*d);
            *value=*static_cast<const CAnimLightObj*>(o);
            n=value;
            break;
        }
        case ANIM_OBJ_MODEL:{
            auto* value=Create<CAnimModelObj>(*d,ANIM_OBJ_MODEL);
            *value=*static_cast<const CAnimModelObj*>(o);
            n=value;
            break;
        }
        case ANIM_OBJ_EMITTER2:{
            auto* value=AnimObjectCreateEmitter2(*d);
            *value=*static_cast<const CAnimEmitter2Obj*>(o);
            n=value;
            break;
        }
        case ANIM_OBJ_RIBBON:{
            auto* value=AnimObjectCreateRibbon(*d);
            *value=*static_cast<const CAnimRibbonObj*>(o);
            n=value;
            break;
        }
        case ANIM_OBJ_EVENT:{
            auto* value=AnimObjectCreateEvent(*d);
            *value=*static_cast<const CAnimEventObj*>(o);
            n=value;
            break;
        }
        case ANIM_OBJ_ATTACHMENT:{
            auto* value=AnimObjectCreateAttachment(*d);
            *value=*static_cast<const CAnimModelObj*>(o);
            n=value;
            break;
        }
        default:
            n=AnimObjectCreateHelper(*d);
            *n=*o;
            break;
        }
        // Copy assignment necessarily copied links into the source hierarchy;
        // those pointers are rebuilt against the duplicated object array below.
        n->parent=nullptr;
        n->childarray.clear();
    }
    for(std::size_t i=0;i<s.obj.size()&&i<d->obj.size();++i){
        if(!s.obj[i]||!s.obj[i]->parent)continue;
        const auto parent=std::find(s.obj.begin(),s.obj.end(),s.obj[i]->parent);
        if(parent!=s.obj.end())
            AnimObjectSetParent(*d,*d->obj[i],d->obj[std::distance(s.obj.begin(),parent)]);
    }
    BuildHierarchy(*d);
    return d;
}
CAnimObj*AnimObjectCreateHelper(CAnimData&d){return Create<CAnimObj>(d,ANIM_OBJ_HELPER);}CAnimLightObj*AnimObjectCreateLight(CAnimData&d){return Create<CAnimLightObj>(d,ANIM_OBJ_LIGHT);}CAnimBoneObj*AnimObjectCreateBone(CAnimData&d){return Create<CAnimBoneObj>(d,ANIM_OBJ_BONE);}CAnimEmitter2Obj*AnimObjectCreateEmitter2(CAnimData&d){return Create<CAnimEmitter2Obj>(d,ANIM_OBJ_EMITTER2);}CAnimRibbonObj*AnimObjectCreateRibbon(CAnimData&d){return Create<CAnimRibbonObj>(d,ANIM_OBJ_RIBBON);}CAnimEventObj*AnimObjectCreateEvent(CAnimData&d){return Create<CAnimEventObj>(d,ANIM_OBJ_EVENT);}CAnimModelObj*AnimObjectCreateAttachment(CAnimData&d){return Create<CAnimModelObj>(d,ANIM_OBJ_ATTACHMENT);}
void AnimObjectSetIndex(CAnimData&d,CAnimObj&o,unsigned i){if(i>=d.obj.size())throw std::out_of_range("animation object index");o.animObjId=i;}CAnimObj*GetNodeByIndex(CAnimData&d,unsigned i){return i<d.obj.size()?d.obj[i]:nullptr;}void AnimObjectSetParent(CAnimData&d,CAnimObj&o,CAnimObj*p){if(o.parent){auto&c=o.parent->childarray;c.erase(std::remove(c.begin(),c.end(),&o),c.end());}o.parent=p;if(p)p->childarray.push_back(&o);else if(std::find(d.headarray.begin(),d.headarray.end(),&o)==d.headarray.end())d.headarray.push_back(&o);}
void AnimObjectSetTranslation(CAnimObj&o,KEYTYPE t,const std::vector<CAnimKey<NTempest::C3Vector>>&k){SetTrack(o.translation,t,k);}void AnimObjectSetRotation(CAnimObj&o,KEYTYPE t,const std::vector<CAnimKey<NTempest::C4QuaternionCompressed,NTempest::C4Quaternion>>&k){SetTrack(o.rotation,t,k);}void AnimObjectSetScaling(CAnimObj&o,KEYTYPE t,const std::vector<CAnimKey<NTempest::C3Vector>>&k){SetTrack(o.scale,t,k);}void AnimObjectSetEventTrack(CAnimEventObj&o,const std::vector<int>&times){o.events.m_keyFrames.clear();for(int t:times)o.events.AddKey(t);}
void AnimObjectSetVisibilityTrack(CAnimVisibleObj&o,KEYTYPE t,const std::vector<CAnimKey<float>>&k){SetTrack(o.visibility,t,k);}void AnimObjectSetAttenuation(CAnimLightObj&o,KEYTYPE t,const std::vector<CAnimKey<float>>&a,const std::vector<CAnimKey<float>>&b){SetTrack(o.attenstart,t,a);SetTrack(o.attenend,t,b);}void AnimObjectSetColor(CAnimLightObj&o,KEYTYPE t,const std::vector<CAnimKey<C3Color>>&k){SetTrack(o.color,t,k);}void AnimObjectSetIntensity(CAnimLightObj&o,KEYTYPE t,const std::vector<CAnimKey<float>>&k){SetTrack(o.intensity,t,k);}void AnimObjectSetAmbColor(CAnimLightObj&o,KEYTYPE t,const std::vector<CAnimKey<C3Color>>&k){SetTrack(o.ambColor,t,k);}void AnimObjectSetAmbIntensity(CAnimLightObj&o,KEYTYPE t,const std::vector<CAnimKey<float>>&k){SetTrack(o.ambIntensity,t,k);}
#define FLOAT_SETTER(fn,member,type) void fn(type&o,KEYTYPE t,const std::vector<CAnimKey<float>>&k){SetTrack(o.member,t,k);}
FLOAT_SETTER(AnimObjectSetParticleEmissionRate2,emissionRate,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleGravity2,gravity,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleVariation2,variation,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetEmitterLongitude2,longitude,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetEmitterLatitude2,latitude,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleSpeed2,particleSpeed,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleLength2,length,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleWidth2,width,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleZsource2,zsource,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetParticleLifeSpan2,lifeSpan,CAnimEmitter2Obj) FLOAT_SETTER(AnimObjectSetRibbonHeightAbove,heightAbove,CAnimRibbonObj) FLOAT_SETTER(AnimObjectSetRibbonHeightBelow,heightBelow,CAnimRibbonObj) FLOAT_SETTER(AnimObjectSetRibbonAlpha,alpha,CAnimRibbonObj)
#undef FLOAT_SETTER
void AnimObjectSetRibbonSlot(CAnimRibbonObj&o,KEYTYPE t,const std::vector<CAnimKey<unsigned>>&k){SetTrack(o.slot,t,k);}void AnimObjectSetRibbonColor(CAnimRibbonObj&o,KEYTYPE t,const std::vector<CAnimKey<C3Color>>&k){SetTrack(o.color,t,k);}
void AnimAddSequences(CAnimData&d,const std::vector<CAnimSequence>&v){d.seq.insert(d.seq.end(),v.begin(),v.end());AnimSetSequenceOrderingDefault(d);}unsigned AnimAddGeoset(CAnimData&d,const CAnimGeoset&v){d.geo.push_back(v);return static_cast<unsigned>(d.geo.size()-1);}void AnimAddGeosets(CAnimData&d,const std::vector<CAnimGeoset>&v){d.geo.insert(d.geo.end(),v.begin(),v.end());}unsigned AnimAddCamera(CAnimData&d,const CAnimCameraObj&v){d.cameraObjs.push_back(v);return static_cast<unsigned>(d.cameraObjs.size()-1);}void AnimAddCameras(CAnimData&d,const std::vector<CAnimCameraObj>&v){d.cameraObjs.insert(d.cameraObjs.end(),v.begin(),v.end());AnimResetCameraOrdering(d);}unsigned AnimAddTextureAnim(CAnimData&d,const CAnimTransform&v){d.tex.push_back(v);return static_cast<unsigned>(d.tex.size()-1);}void AnimAddTextureAnims(CAnimData&d,const std::vector<CAnimTransform>&v){d.tex.insert(d.tex.end(),v.begin(),v.end());}unsigned AnimAddMaterialLayer(CAnimData&d,const CAnimMaterialLayer&v){d.layers.push_back(v);return static_cast<unsigned>(d.layers.size()-1);}void AnimAddMaterialLayers(CAnimData&d,const std::vector<CAnimMaterialLayer>&v){d.layers.insert(d.layers.end(),v.begin(),v.end());}KEYTYPE GetTrackType(unsigned v){return v<=KEYTYPE_BEZIER?static_cast<KEYTYPE>(v):KEYTYPE_NONE;}unsigned GetObjectFlags(const CAnimObj&o){return o.flags;}unsigned GetGenObjectCount(const CAnimData&d){return static_cast<unsigned>(d.obj.size());}void BuildHierarchy(CAnimData&d){d.headarray.clear();for(auto*o:d.obj)if(o&&!o->parent)d.headarray.push_back(o);}void ResolveStatusPtrs(CAnim&a){if(a.hdata)a.status.resize(a.hdata->obj.size());}
void SetObjectParent(CAnimData&d,CAnimObj&o,CAnimObj*p){AnimObjectSetParent(d,o,p);}unsigned SetTransformationFlags(const CAnimObj&o){unsigned f=!o.translation.keys.empty()?1U:0U;if(!o.rotation.keys.empty())f|=4U;if(!o.scale.keys.empty())f|=2U;return f;}void ValidateInheritanceFlags(CAnimData&d){for(auto*o:d.obj){if(!o)continue;unsigned inherited=0;for(auto*p=o->parent;p;p=p->parent)inherited|=SetTransformationFlags(*p);o->flags=static_cast<std::uint8_t>(o->flags|(inherited&7U));}}
