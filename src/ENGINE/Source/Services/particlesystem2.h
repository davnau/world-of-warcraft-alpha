#pragma once
#include "ENGINE/Source/Gx/gxapi/gxbase.h"
#include "ENGINE/Source/Services/particlemisc.h"
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/c4quaternion.h"
#include <array>
#include <cstdint>
#include <vector>
class CTexture;using HTEXTURE__=CTexture;struct HMODEL__;
void ParticleSystem2SetCameraWorldPosition(const NTempest::C3Vector& position);
const NTempest::C3Vector& ParticleSystem2GetCameraWorldPosition();
struct CParticle2 { enum{F_BORN=1};CParticle2();virtual~CParticle2();NTempest::C3Vector m_position{};std::uint8_t m_keyFrame=0,m_flags=0,m_filler[2]{};NTempest::C3Vector m_velocity{};float m_age=0; };
struct CParticle2_Model: CParticle2 { CParticle2_Model();~CParticle2_Model()override;NTempest::C4Quaternion m_rotation{};NTempest::C3Vector m_rotVelocity{}; };
class CParticleStack { public:void Push(unsigned);unsigned Pop();unsigned Top()const;bool Empty()const{return m_stackPointer==0;}std::vector<unsigned>m_stack;unsigned m_stackPointer=0; };
struct CParticleMat { int alpha=0;bool enableLighting=true,enableFog=true,enableDepthWrites=true; };
class CParticleEmitter2 {
public:
 enum PARTICLE_EMITTER_TYPE{PET_BASE_EMITTER,PET_PLANE_EMITTER,PET_SPHERE_EMITTER,PET_SPLINE_EMITTER,PET_NUMS_PETS};enum PARTICLE_TYPE{PT_QUAD,PT_MODEL};
 CParticleEmitter2();virtual ~CParticleEmitter2();virtual CParticleEmitter2* Clone()const;virtual void CreateParticle(CParticle2*,float,const NTempest::C34Matrix*);
 void SetModel(HMODEL__*);NTempest::C3Vector CalcVelocity(const NTempest::C3Vector&)const;void Sync(const CParticleEmitter2&);void SyncReserve(unsigned);void SyncAllocation();void ProjectParticle(CParticle2&);
 virtual void MoveParticle(CParticle2&,float);virtual void Render(const NTempest::C3Vector* camera=nullptr);void IRenderParticle(unsigned);void IRenderVertices();void IRenderIndices();void BufRenderParticles();void RenderParticles();void RenderParticle(unsigned);void RenderParticleModels();void DestroyParticle(unsigned);void SetEnabled(int);void SetEnabled2(int);virtual void SetEmissionRate(float);void SetLifeSpan(float);void SetVelocity(float);void SetAcceleration(float);void SetVelocityVariation(float);void SetMaterial(const CParticleMat&);void MaterialDisableLight(int);void MaterialDisableFog(int);void SetTexture(HTEXTURE__*);void SetReplaceableId(unsigned);void SetKey(unsigned,const CParticleKey&);void SetTextureDimensions(unsigned,unsigned);void SetParticleStyle(PARTICLE_TYPE);void SetParticleStyle(PARTICLE_TYPE,bool,bool,float,bool);void SetSourceFlags(std::uint32_t);
 int Enabled()const;int Enabled2()const;double EmissionRate()const;double LifeSpan()const;double Velocity()const;double Acceleration()const;double VelocityVariation()const;unsigned ReplaceableId()const;void Key(unsigned,CParticleKey*)const;void TextureDimensions(unsigned*,unsigned*)const;PARTICLE_TYPE ParticleStyle()const;
 void SingletonMgrUpdate(float);void UpdateXform(const NTempest::C34Matrix&);void Update(float);void EmitNewParticles(float);void InternalUpdate(float);void StepUpdate(float);void Squirt();void Flush();void SetZsource(float);void SetSortZ(int);int SortZ()const;void SetFollowParams(float,float);void SetFollowParams(float,float,float,float);void AddChildEmitter(CParticleEmitter2*);unsigned AddRef();unsigned DecRef();
 unsigned m_refCount=1;float m_numNew=0;int m_priorityPlane=0;PARTICLE_EMITTER_TYPE m_emitterType=PET_BASE_EMITTER;PARTICLE_TYPE m_particleType=PT_QUAD;std::vector<CParticle2>m_particles;std::vector<CParticle2_Model>m_modelParticles;CParticleStack m_alive,m_dead;std::array<CParticleEmitter2*,4>m_childEmitter{};unsigned m_childCount=0;HMODEL__*m_model=nullptr;float m_elapsedTime=0,m_particleEmissionRate=0,m_particleLifeSpan=1,m_particleTailLength=0;std::array<CParticleKey,2>m_particleKeys{};float m_particleVelocity=0,m_particleAcceleration=0,m_particleVelocityVariation=0,m_particleZsource=0,m_particleAngularVelocity=0;CParticleMat m_particleMaterial{};unsigned m_textureRows=1,m_textureColumns=1;HTEXTURE__*m_hTex=nullptr;unsigned m_replaceableId=0;std::uint32_t m_sourceFlags=0;bool m_enabled=true,m_enabled2=true,m_particleHasHead=true,m_particleHasTail=false,m_sortZ=false,m_needSquirt=false,m_paused=false,m_useModelSpace=false,m_inheritScale=false,m_instantVelLin=false,m_0XKill=false,m_extrude=false,m_xyQuads=false,m_zvelOnly=false,m_tumbler=false,m_tailGrows=false,m_project=false,m_follow=false;float m_twinkleFPS=10,m_twinkleOnOff=1,m_twinkleScaleMin=1,m_twinkleScaleMax=1,m_twinkleScaleRange=0,m_ivelScale=1;NTempest::C2Vector m_tumblex{},m_tumbley{},m_tumblez{};float m_drag=0;NTempest::C3Vector m_windVector{};float m_windTime=0,m_followSpeed1=0,m_followScale1=0,m_followSpeed2=0,m_followScale2=0,m_followB=0,m_followM=0;NTempest::C34Matrix m_modelToWorld{};NTempest::C3Vector m_cameraWorldPos{};float m_frameScale=1.0F;
 bool m_updated=false;
};
