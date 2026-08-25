#include "ENGINE/Source/Services/particlesystem2.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Services/texture.h"
#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>
#include <numeric>
#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif
namespace {
NTempest::C3Vector g_particleCameraWorldPosition{};
}

void ParticleSystem2SetCameraWorldPosition(
    const NTempest::C3Vector& position) {
    g_particleCameraWorldPosition = position;
}

const NTempest::C3Vector& ParticleSystem2GetCameraWorldPosition() {
    return g_particleCameraWorldPosition;
}

CParticle2::CParticle2()=default;CParticle2::~CParticle2()=default;CParticle2_Model::CParticle2_Model()=default;CParticle2_Model::~CParticle2_Model()=default;
void CParticleEmitter2::IRenderParticle(unsigned){}
void CParticleEmitter2::IRenderVertices(){}
void CParticleEmitter2::IRenderIndices(){}
void CParticleEmitter2::BufRenderParticles(){RenderParticles();}
void CParticleEmitter2::RenderParticles()
{
    if (!m_hTex || m_alive.Empty() || !m_particleHasHead && !m_particleHasTail) return;
    auto* texture = TextureGetGxTex(m_hTex, 1, nullptr);
    if (!texture) return;

    NTempest::C44Matrix view;
    GxXformView(&view);
    NTempest::C3Vector right{view.a0, view.b0, view.c0};
    NTempest::C3Vector up{view.a1, view.b1, view.c1};
    NTempest::C3Vector forward{view.a2, view.b2, view.c2};
    if (m_xyQuads) { right={1,0,0}; up={0,1,0}; forward={0,0,1}; }
    if (right.SquaredMag() > 0.0F) right.Normalize();
    if (up.SquaredMag() > 0.0F) up.Normalize();
    if (forward.SquaredMag() > 0.0F) forward.Normalize();

    std::vector<unsigned> particles;
    particles.reserve(m_alive.m_stackPointer);
    for (unsigned i=0;i<m_alive.m_stackPointer;++i)
        if (m_alive.m_stack[i] < m_particles.size()) particles.push_back(m_alive.m_stack[i]);
    if (m_sortZ) std::stable_sort(particles.begin(),particles.end(),[&](unsigned l,unsigned r){
        const auto& a=m_particles[l].m_position;const auto& b=m_particles[r].m_position;
        const float az=view.a2*a.x+view.b2*a.y+view.c2*a.z+view.d2;
        const float bz=view.a2*b.x+view.b2*b.y+view.c2*b.z+view.d2;
        return az>bz;
    });
    if (particles.size()>4096U) particles.resize(4096U);

    std::vector<NTempest::C3Vector> positions;
    std::vector<NTempest::C3Vector> normals;
    std::vector<NTempest::CImVector> colors;
    std::vector<NTempest::C2Vector> texCoords;
    std::vector<std::uint16_t> indices;
    const unsigned quadsPerParticle=unsigned(m_particleHasHead)+unsigned(m_particleHasTail);
    positions.reserve(particles.size()*quadsPerParticle*4U);
    normals.reserve(positions.capacity());colors.reserve(positions.capacity());
    texCoords.reserve(positions.capacity());indices.reserve(particles.size()*quadsPerParticle*6U);
    auto appendQuad=[&](const std::array<NTempest::C3Vector,4>&p,const NTempest::CImVector&color,
                        float u0,float v0,float u1,float v1){
        if(positions.size()>65531U)return;
        const auto base=static_cast<std::uint16_t>(positions.size());
        positions.insert(positions.end(),p.begin(),p.end());
        for(unsigned i=0;i<4;++i){normals.push_back({-forward.x,-forward.y,-forward.z});colors.push_back(color);}
        texCoords.insert(texCoords.end(),{{u0,v0},{u0,v1},{u1,v1},{u1,v0}});
        indices.insert(indices.end(),{base,static_cast<std::uint16_t>(base+1),static_cast<std::uint16_t>(base+2),base,static_cast<std::uint16_t>(base+2),static_cast<std::uint16_t>(base+3)});
    };

    for(const auto particleIndex:particles){
        const auto& particle=m_particles[particleIndex];
        const unsigned keyIndex=particle.m_age>=m_particleKeys[0].m_endTime?1U:0U;
        NTempest::CImVector color;int headCell=0,tailCell=0;float scale=1.0F;
        m_particleKeys[keyIndex].Interpolate(particle.m_age,&color,&headCell,&tailCell,&scale);
        if(m_twinkleOnOff<1.0F){const float phase=std::fmod(std::fabs(std::sin((particleIndex+1U)*12.9898F+particle.m_age*m_twinkleFPS))*43758.5453F,1.0F);if(phase>m_twinkleOnOff)continue;}
        if(m_twinkleScaleRange!=0.0F){const float phase=0.5F+0.5F*std::sin((particleIndex+1U)*7.13F+particle.m_age*m_twinkleFPS);scale*=m_twinkleScaleMin+phase*m_twinkleScaleRange;}
        NTempest::C3Vector center=particle.m_position;
        NTempest::C3Vector velocity=particle.m_velocity;
        if(m_useModelSpace){center=m_modelToWorld*center;velocity={m_modelToWorld.a0*velocity.x+m_modelToWorld.b0*velocity.y+m_modelToWorld.c0*velocity.z,m_modelToWorld.a1*velocity.x+m_modelToWorld.b1*velocity.y+m_modelToWorld.c1*velocity.z,m_modelToWorld.a2*velocity.x+m_modelToWorld.b2*velocity.y+m_modelToWorld.c2*velocity.z};}
        center-=m_cameraWorldPos;
        auto cellUv=[&](int cell){const unsigned c=static_cast<unsigned>(std::max(0,cell));const float u0=float(c%m_textureColumns)/float(m_textureColumns);const float v0=float(c/m_textureColumns)/float(m_textureRows);return std::array<float,4>{u0,v0,u0+1.0F/float(m_textureColumns),v0+1.0F/float(m_textureRows)};};
        if(m_particleHasHead){const float angle=m_particleAngularVelocity*particle.m_age;const float cs=std::cos(angle),sn=std::sin(angle);const auto rr=right*cs+up*sn;const auto uu=up*cs-right*sn;const auto uv=cellUv(headCell);appendQuad({center-rr*scale+uu*scale,center-rr*scale-uu*scale,center+rr*scale-uu*scale,center+rr*scale+uu*scale},color,uv[0],uv[1],uv[2],uv[3]);}
        if(m_particleHasTail){NTempest::C3Vector tailVector=velocity*std::min(m_particleTailLength,particle.m_age);if(tailVector.SquaredMag()<1.0e-8F)tailVector=up*(scale*2.0F);NTempest::C3Vector side=NTempest::C3Vector::Cross(forward,tailVector);if(side.SquaredMag()<1.0e-8F)side=right;side.Normalize();side*=scale;const auto end=center-tailVector;const auto uv=cellUv(tailCell);appendQuad({center-side,end-side,end+side,center+side},color,uv[0],uv[1],uv[2],uv[3]);}
    }
    if(positions.empty())return;
    GxVertexShaderSelect(GxVS_PassThru);GxRsPush();
    GxRsSet(GxRs_Texture0,texture);GxRsSet(GxRs_TexBlend0,1);
    const int blend=std::clamp(m_particleMaterial.alpha,0,int(GxBlends_Last)-1);
    GxRsSet(GxRs_Blend,static_cast<EGxBlend>(blend));GxRsSet(GxRs_Lighting,m_particleMaterial.enableLighting?1:0);GxRsSet(GxRs_Fog,m_particleMaterial.enableFog?1:0);GxRsSet(GxRs_DepthTest,1);GxRsSet(GxRs_DepthWrite,m_particleMaterial.enableDepthWrites?1:0);GxRsSet(GxRs_Culling,0);
    GxPrimLockVertexPtrs(static_cast<unsigned>(positions.size()),positions.data(),sizeof(NTempest::C3Vector),normals.data(),sizeof(NTempest::C3Vector),colors.data(),sizeof(NTempest::CImVector),nullptr,0,texCoords.data(),sizeof(NTempest::C2Vector),nullptr,0);
    GxPrimDrawElements(GxPrim_Triangles,static_cast<unsigned>(indices.size()),indices.data());GxPrimUnlockVertexPtrs();GxRsPop();
}
void CParticleEmitter2::RenderParticle(unsigned i){IRenderParticle(i);}
void CParticleEmitter2::RenderParticleModels(){}
void CParticleStack::Push(unsigned v){if(m_stackPointer==m_stack.size())m_stack.push_back(v);else m_stack[m_stackPointer]=v;++m_stackPointer;}unsigned CParticleStack::Pop(){if(!m_stackPointer)throw std::underflow_error("particle stack");return m_stack[--m_stackPointer];}unsigned CParticleStack::Top()const{if(!m_stackPointer)throw std::underflow_error("particle stack");return m_stack[m_stackPointer-1];}
CParticleEmitter2::CParticleEmitter2()=default;CParticleEmitter2::~CParticleEmitter2()=default;CParticleEmitter2*CParticleEmitter2::Clone()const{return new CParticleEmitter2(*this);}void CParticleEmitter2::CreateParticle(CParticle2*p,float,const NTempest::C34Matrix*xform){if(!p)return;const NTempest::C3Vector local{0,0,m_particleZsource};p->m_position=m_useModelSpace||!xform?local:(*xform)*local;p->m_velocity=CalcVelocity({0,0,1});p->m_age=0;p->m_flags=CParticle2::F_BORN;}
void CParticleEmitter2::SetModel(HMODEL__*m){m_model=m;}NTempest::C3Vector CParticleEmitter2::CalcVelocity(const NTempest::C3Vector&dir)const{return dir*m_particleVelocity;}void CParticleEmitter2::Sync(const CParticleEmitter2&o){m_numNew=o.m_numNew;m_priorityPlane=o.m_priorityPlane;m_particleType=o.m_particleType;m_elapsedTime=o.m_elapsedTime;m_particleEmissionRate=o.m_particleEmissionRate;m_particleLifeSpan=o.m_particleLifeSpan;m_particleTailLength=o.m_particleTailLength;m_particleKeys=o.m_particleKeys;m_particleVelocity=o.m_particleVelocity;m_particleAcceleration=o.m_particleAcceleration;m_particleVelocityVariation=o.m_particleVelocityVariation;m_particleZsource=o.m_particleZsource;m_particleAngularVelocity=o.m_particleAngularVelocity;m_particleMaterial=o.m_particleMaterial;m_textureRows=o.m_textureRows;m_textureColumns=o.m_textureColumns;m_hTex=o.m_hTex;m_replaceableId=o.m_replaceableId;m_sourceFlags=o.m_sourceFlags;m_enabled=o.m_enabled;m_enabled2=o.m_enabled2;m_particleHasHead=o.m_particleHasHead;m_particleHasTail=o.m_particleHasTail;m_sortZ=o.m_sortZ;m_needSquirt=o.m_needSquirt;m_paused=o.m_paused;m_useModelSpace=o.m_useModelSpace;m_inheritScale=o.m_inheritScale;m_instantVelLin=o.m_instantVelLin;m_0XKill=o.m_0XKill;m_extrude=o.m_extrude;m_xyQuads=o.m_xyQuads;m_zvelOnly=o.m_zvelOnly;m_tumbler=o.m_tumbler;m_tailGrows=o.m_tailGrows;m_project=o.m_project;m_follow=o.m_follow;m_twinkleFPS=o.m_twinkleFPS;m_twinkleOnOff=o.m_twinkleOnOff;m_twinkleScaleMin=o.m_twinkleScaleMin;m_twinkleScaleMax=o.m_twinkleScaleMax;m_twinkleScaleRange=o.m_twinkleScaleRange;m_ivelScale=o.m_ivelScale;m_tumblex=o.m_tumblex;m_tumbley=o.m_tumbley;m_tumblez=o.m_tumblez;m_drag=o.m_drag;m_windVector=o.m_windVector;m_windTime=o.m_windTime;m_followSpeed1=o.m_followSpeed1;m_followScale1=o.m_followScale1;m_followSpeed2=o.m_followSpeed2;m_followScale2=o.m_followScale2;m_followB=o.m_followB;m_followM=o.m_followM;m_modelToWorld=o.m_modelToWorld;m_cameraWorldPos=o.m_cameraWorldPos;m_frameScale=o.m_frameScale;}void CParticleEmitter2::SyncReserve(unsigned n){m_particles.reserve(n);}void CParticleEmitter2::SyncAllocation(){m_dead.m_stack.clear();m_dead.m_stackPointer=0;for(unsigned i=0;i<m_particles.size();++i)m_dead.Push(i);}void CParticleEmitter2::ProjectParticle(CParticle2&p){if(p.m_position.z<0){p.m_position.z=0;p.m_velocity.z=std::abs(p.m_velocity.z);}}
void CParticleEmitter2::MoveParticle(CParticle2&p,float dt){if(p.m_age<m_windTime)p.m_velocity+=m_windVector*dt;p.m_position.x+=p.m_velocity.x*dt;p.m_position.y+=p.m_velocity.y*dt;p.m_position.z+=p.m_velocity.z*dt-0.5F*m_particleAcceleration*dt*dt;p.m_velocity.z-=m_particleAcceleration*dt;const float drag=std::min(1.0F,std::max(0.0F,m_drag*dt));p.m_velocity*=1.0F-drag;}void CParticleEmitter2::Render(const NTempest::C3Vector*){if(!m_alive.Empty()){if(m_particleType==PT_QUAD)RenderParticles();else RenderParticleModels();for(unsigned i=0;i<m_childCount;++i)if(m_childEmitter[i])m_childEmitter[i]->Render();}}void CParticleEmitter2::DestroyParticle(unsigned i){if(i<m_particles.size()){m_particles[i].m_flags=0;m_dead.Push(i);}}
void CParticleEmitter2::SetEnabled(int v){m_enabled=v!=0;}void CParticleEmitter2::SetEnabled2(int v){m_enabled2=v!=0;}void CParticleEmitter2::SetEmissionRate(float v){m_particleEmissionRate=std::max(0.0F,v);}void CParticleEmitter2::SetLifeSpan(float v){m_particleLifeSpan=std::max(0.0001F,v);for(auto&k:m_particleKeys)k.SetLifeSpan(m_particleLifeSpan);}void CParticleEmitter2::SetVelocity(float v){m_particleVelocity=v;}void CParticleEmitter2::SetAcceleration(float v){m_particleAcceleration=v;}void CParticleEmitter2::SetVelocityVariation(float v){m_particleVelocityVariation=v;}void CParticleEmitter2::SetMaterial(const CParticleMat&v){m_particleMaterial=v;}void CParticleEmitter2::MaterialDisableLight(int v){m_particleMaterial.enableLighting=!v;}void CParticleEmitter2::MaterialDisableFog(int v){m_particleMaterial.enableFog=!v;}void CParticleEmitter2::SetTexture(HTEXTURE__*v){m_hTex=v;}void CParticleEmitter2::SetReplaceableId(unsigned v){m_replaceableId=v;}void CParticleEmitter2::SetKey(unsigned i,const CParticleKey&v){if(i>=2)throw std::out_of_range("particle key");m_particleKeys[i]=v;}void CParticleEmitter2::SetTextureDimensions(unsigned r,unsigned c){m_textureRows=std::max(1U,r);m_textureColumns=std::max(1U,c);}void CParticleEmitter2::SetParticleStyle(PARTICLE_TYPE v){m_particleType=v;}void CParticleEmitter2::SetParticleStyle(PARTICLE_TYPE v,bool head,bool tail,float tailLength,bool tailGrows){m_particleType=v;m_particleHasHead=head;m_particleHasTail=tail;m_particleTailLength=tailLength;m_tailGrows=tailGrows;}void CParticleEmitter2::SetSourceFlags(std::uint32_t flags){m_sourceFlags=flags;m_sortZ=(flags&0x00010000U)!=0;m_useModelSpace=(flags&0x00080000U)!=0;m_inheritScale=(flags&0x00100000U)!=0;m_instantVelLin=(flags&0x00200000U)!=0;m_0XKill=(flags&0x00400000U)!=0;m_zvelOnly=(flags&0x00800000U)!=0;m_tumbler=(flags&0x01000000U)!=0;m_tailGrows=(flags&0x02000000U)!=0;m_extrude=(flags&0x04000000U)!=0;m_xyQuads=(flags&0x08000000U)!=0;m_project=(flags&0x10000000U)!=0;m_follow=(flags&0x20000000U)!=0;}
int CParticleEmitter2::Enabled()const{return m_enabled;}int CParticleEmitter2::Enabled2()const{return m_enabled2;}double CParticleEmitter2::EmissionRate()const{return m_particleEmissionRate;}double CParticleEmitter2::LifeSpan()const{return m_particleLifeSpan;}double CParticleEmitter2::Velocity()const{return m_particleVelocity;}double CParticleEmitter2::Acceleration()const{return m_particleAcceleration;}double CParticleEmitter2::VelocityVariation()const{return m_particleVelocityVariation;}unsigned CParticleEmitter2::ReplaceableId()const{return m_replaceableId;}void CParticleEmitter2::Key(unsigned i,CParticleKey*k)const{if(!k||i>=2)throw std::out_of_range("particle key");*k=m_particleKeys[i];}void CParticleEmitter2::TextureDimensions(unsigned*r,unsigned*c)const{if(r)*r=m_textureRows;if(c)*c=m_textureColumns;}CParticleEmitter2::PARTICLE_TYPE CParticleEmitter2::ParticleStyle()const{return m_particleType;}
void CParticleEmitter2::SingletonMgrUpdate(float d){if(!m_updated)Update(d);m_updated=false;}void CParticleEmitter2::UpdateXform(const NTempest::C34Matrix&m){m_cameraWorldPos=ParticleSystem2GetCameraWorldPosition();m_modelToWorld=m;m_modelToWorld.d0+=m_cameraWorldPos.x;m_modelToWorld.d1+=m_cameraWorldPos.y;m_modelToWorld.d2+=m_cameraWorldPos.z;const float scaleSquared=m.a0*m.a0+m.a1*m.a1+m.a2*m.a2;m_frameScale=std::sqrt((std::max)(0.0F,scaleSquared));}void CParticleEmitter2::Update(float dt){if(!m_enabled||m_paused)return;EmitNewParticles(dt);InternalUpdate(dt);m_updated=true;}void CParticleEmitter2::EmitNewParticles(float dt){m_numNew+=m_particleEmissionRate*dt;if(m_needSquirt)m_numNew=std::max(m_numNew,m_particleEmissionRate);unsigned count=static_cast<unsigned>(m_numNew);m_numNew-=count;m_needSquirt=false;while(count--){unsigned i;if(m_dead.Empty()){i=static_cast<unsigned>(m_particles.size());m_particles.emplace_back();}else i=m_dead.Pop();CreateParticle(&m_particles[i],dt,&m_modelToWorld);m_alive.Push(i);}}void CParticleEmitter2::InternalUpdate(float dt){dt=std::max(0.0F,dt);std::vector<unsigned>alive;while(!m_alive.Empty())alive.push_back(m_alive.Pop());for(unsigned i:alive){auto&particle=m_particles[i];particle.m_age+=dt;if(particle.m_age>=m_particleLifeSpan){DestroyParticle(i);continue;}particle.m_keyFrame=particle.m_age>m_particleKeys[0].m_endTime?1:0;MoveParticle(particle,dt);if(m_project)ProjectParticle(particle);m_alive.Push(i);}for(unsigned n=0;n<m_childCount;++n)if(m_childEmitter[n])m_childEmitter[n]->Update(dt);}void CParticleEmitter2::StepUpdate(float d){InternalUpdate(d);}void CParticleEmitter2::Squirt(){m_needSquirt=true;}void CParticleEmitter2::Flush(){m_particles.clear();m_modelParticles.clear();m_alive={};m_dead={};}void CParticleEmitter2::SetZsource(float v){m_particleZsource=v;}void CParticleEmitter2::SetSortZ(int v){m_sortZ=v!=0;}int CParticleEmitter2::SortZ()const{return m_sortZ;}void CParticleEmitter2::SetFollowParams(float b,float m){m_followB=b;m_followM=m;m_followSpeed1=b;m_followScale1=m;}void CParticleEmitter2::SetFollowParams(float speed1,float scale1,float speed2,float scale2){m_followSpeed1=speed1;m_followScale1=scale1;m_followSpeed2=speed2;m_followScale2=scale2;m_followB=speed1;m_followM=scale1;}void CParticleEmitter2::AddChildEmitter(CParticleEmitter2*e){if(m_childCount>=4)throw std::length_error("child emitters");m_childEmitter[m_childCount++]=e;}unsigned CParticleEmitter2::AddRef(){return++m_refCount;}unsigned CParticleEmitter2::DecRef(){if(!m_refCount)return 0;unsigned n=--m_refCount;if(!n)delete this;return n;}
