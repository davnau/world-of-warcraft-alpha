#include "ENGINE/Source/Services/gfxsingletonmanager.h"

#include <algorithm>

ParticleSystemManager* ParticleSystemManager::s_instance = nullptr;
float ParticleSystemManager::scaler = 1.0F;
RibbonManager* RibbonManager::s_instance = nullptr;

ParticleSystemManager* ParticleSystemManager::GetInstance()
{
    if (!s_instance) s_instance = new ParticleSystemManager;
    return s_instance;
}

void ParticleSystemManager::Destroy()
{
    delete s_instance;
    s_instance = nullptr;
}

void ParticleSystemManager::SetScaler(float value)
{
    scaler = std::clamp(value, 0.0F, 1.0F);
}

double ParticleSystemManager::GetScaler() { return scaler; }

ParticleSystemManager::~ParticleSystemManager()
{
    Flush();
    for (auto* emitter : deletedEmitters) delete emitter;
    for (auto* emitter : deletedEmitter2s) if (emitter) emitter->DecRef();
}

void ParticleSystemManager::Flush()
{
    for (auto* emitter : emitters) delete emitter;
    for (auto* emitter : emitter2s) if (emitter) emitter->DecRef();
    emitters.clear();
    emitter2s.clear();
}

void ParticleSystemManager::UpdateEmitters(float elapsed,
                                           const NTempest::C3Vector*, void*)
{
    for (auto* emitter : emitters) emitter->Update(elapsed * scaler);
    for (auto* emitter : emitter2s) emitter->SingletonMgrUpdate(elapsed * scaler);
    for (auto* emitter : deletedEmitters) delete emitter;
    for (auto* emitter : deletedEmitter2s) if (emitter) emitter->DecRef();
    deletedEmitters.clear();
    deletedEmitter2s.clear();
}

void ParticleSystemManager::RenderParticleEmitter(CParticleEmitter* emitter)
{
    if (emitter) emitter->Render();
}

void ParticleSystemManager::RenderParticleEmitter2(CParticleEmitter2* emitter)
{
    if (emitter) emitter->Render();
}

void ParticleSystemManager::RenderEmitters()
{
    for (auto* emitter : emitters) RenderParticleEmitter(emitter);
    for (auto* emitter : emitter2s) RenderParticleEmitter2(emitter);
}

CParticleEmitter* ParticleSystemManager::CreateModelEmitter()
{
    auto* emitter = new CParticleEmitter;
    emitters.push_back(emitter);
    return emitter;
}

CPlaneParticleEmitter* ParticleSystemManager::CreateQuadEmitter()
{
    auto* emitter = new CPlaneParticleEmitter;
    emitter->AddRef(); // manager reference; the initial reference is returned
    emitter2s.push_back(emitter);
    return emitter;
}

CSphereParticleEmitter* ParticleSystemManager::CreateSphereEmitter()
{
    auto* emitter = new CSphereParticleEmitter;
    emitter->AddRef();
    emitter2s.push_back(emitter);
    return emitter;
}

CSplineParticleEmitter* ParticleSystemManager::CreateSplineEmitter()
{
    auto* emitter = new CSplineParticleEmitter;
    emitter->AddRef();
    emitter2s.push_back(emitter);
    return emitter;
}

CParticleEmitter2* ParticleSystemManager::DuplicateEmitter(const CParticleEmitter2* source)
{
    if (!source) return nullptr;
    auto* emitter = source->Clone();
    emitter->AddRef();
    emitter2s.push_back(emitter);
    return emitter;
}

void ParticleSystemManager::DeleteModelEmitter(CParticleEmitter* emitter)
{
    emitters.erase(std::remove(emitters.begin(), emitters.end(), emitter), emitters.end());
    if (emitter) deletedEmitters.push_back(emitter);
}

void ParticleSystemManager::DeleteEmitter2(CParticleEmitter2* emitter)
{
    if (!emitter) return;
    const auto found = std::find(emitter2s.begin(), emitter2s.end(), emitter);
    if (found == emitter2s.end()) {
        emitter->DecRef();
        return;
    }
    emitter->DecRef(); // release the caller/model reference
    emitter2s.erase(found);
    deletedEmitter2s.push_back(emitter); // manager reference released after update
}

RibbonManager* RibbonManager::GetInstance()
{
    if (!s_instance) s_instance = new RibbonManager;
    return s_instance;
}

void RibbonManager::Destroy()
{
    delete s_instance;
    s_instance = nullptr;
}

RibbonManager::~RibbonManager()
{
    Flush();
    for (auto* emitter : deletedEmitters) delete emitter;
}

void RibbonManager::Flush()
{
    for (auto* emitter : emitters) delete emitter;
    emitters.clear();
}

void RibbonManager::UpdateEmitters(float elapsed, const NTempest::C3Vector* camera,
                                   const NTempest::C3Vector*)
{
    for (auto* emitter : emitters) emitter->SingletonMgrUpdate(elapsed, camera, 0);
    for (auto* emitter : deletedEmitters) delete emitter;
    deletedEmitters.clear();
}

void RibbonManager::RenderEmitter(CRibbonEmitter* emitter)
{
    if (emitter) emitter->Render();
}

void RibbonManager::RenderEmitters()
{
    for (auto* emitter : emitters) RenderEmitter(emitter);
}

CRibbonEmitter* RibbonManager::CreateEmitter()
{
    auto* emitter = new CRibbonEmitter;
    emitters.push_back(emitter);
    return emitter;
}

CRibbonEmitter* RibbonManager::DuplicateEmitter(const CRibbonEmitter* source)
{
    if (!source) return nullptr;
    auto* emitter = new CRibbonEmitter(*source);
    emitters.push_back(emitter);
    return emitter;
}

void RibbonManager::DeleteEmitter(CRibbonEmitter* emitter)
{
    emitters.erase(std::remove(emitters.begin(), emitters.end(), emitter), emitters.end());
    if (emitter) deletedEmitters.push_back(emitter);
}
