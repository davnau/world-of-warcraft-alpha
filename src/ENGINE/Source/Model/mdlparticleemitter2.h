#pragma once

#include "ENGINE/Source/Model/model.h"
#include "ENGINE/Source/Services/particlesystem2.h"

#include <array>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

// Static portion of the early PRE2 MDLX particle-emitter record. Animation
// tracks are decoded by Anim; these values construct the service emitter that
// the tracks drive at run time.
struct MDLPARTICLEEMITTER2 {
    std::uint32_t objectId = 0;
    std::uint32_t parentId = ~0U;
    std::uint32_t flags = 0;
    std::uint32_t emitterType = 0;

    float staticSpeed = 0.0F;
    float staticVariation = 0.0F;
    float staticLatitude = 0.0F;
    float staticLongitude = 0.0F;
    float staticGravity = 0.0F;
    float staticZsource = 0.0F;
    float staticLife = 0.0F;
    float staticEmissionRate = 0.0F;
    float staticLength = 0.0F;
    float staticWidth = 0.0F;

    std::uint32_t rows = 1;
    std::uint32_t columns = 1;
    std::uint32_t particleType = 0;
    float tailLength = 0.0F;
    float middleTime = 0.5F;
    std::array<NTempest::C3Vector, 3> colors{{{1.0F, 1.0F, 1.0F},
                                              {1.0F, 1.0F, 1.0F},
                                              {1.0F, 1.0F, 1.0F}}};
    std::array<std::uint8_t, 3> alpha{};
    std::array<float, 3> scale{{1.0F, 1.0F, 1.0F}};
    std::array<std::uint32_t, 3> lifespanUV{};
    std::array<std::uint32_t, 3> decayUV{};
    std::array<std::uint32_t, 3> tailUV{};
    std::array<std::uint32_t, 3> tailDecayUV{};
    std::uint32_t blendMode = 0;
    std::uint32_t textureId = 0;
    std::int32_t priorityPlane = 0;
    std::uint32_t replaceableId = 0;
    std::string geometryModel;
    std::string recursionModel;

    float twinkleFPS = 10.0F;
    float twinkleOnOff = 1.0F;
    float twinkleScaleMin = 1.0F;
    float twinkleScaleMax = 1.0F;
    float inverseVelocityScale = 1.0F;
    std::array<float, 2> tumbleX{};
    std::array<float, 2> tumbleY{};
    std::array<float, 2> tumbleZ{};
    float drag = 0.0F;
    float spin = 0.0F;
    NTempest::C3Vector windVector{};
    float windTime = 0.0F;
    float followSpeed1 = 0.0F;
    float followScale1 = 0.0F;
    float followSpeed2 = 0.0F;
    float followScale2 = 0.0F;
    std::vector<NTempest::C3Vector> spline;
    std::uint32_t squirts = 0;
};

CParticleEmitter2* CreateEmitter(const MDLPARTICLEEMITTER2& definition,
                                 const CModelComplex* model = nullptr);
CParticleEmitter2* CreateEmitterObject(std::uint32_t emitterType);
void SetParticleStyle(CParticleEmitter2& emitter, std::uint32_t style);
void SetParticleKeys(CParticleEmitter2& emitter, const MDLPARTICLEEMITTER2& definition);
void SetKeyColors(CParticleKey& key1, const MDLPARTICLEEMITTER2& definition,
                  CParticleKey& key2);
void CreateParticleMaterial(CParticleMat& material, std::uint32_t blendMode,
                            std::uint32_t flags);
void SetMaterialBlendMode(CParticleMat& material, std::uint32_t blendMode);
void CreateChildEmitter(std::uint32_t flags, CParticleEmitter2& parent);
std::uint32_t GetEmitterFlags(const CParticleEmitter2& emitter);
void SetParticleTumble(CParticleEmitter2& emitter, const NTempest::C3Vector& tumble);
void LoadC3Vector(NTempest::C3Vector& vector, const float* values);
HTEXTURE__* LoadModelTexture(const CModelComplex& model, std::uint32_t textureIndex);
bool MdlReadLoadEmitters2(CModelComplex& model, CModelShared& shared,
                          std::span<const std::uint8_t> bytes);
void MdxReadEmitters2(const std::uint8_t* fileBytes, std::size_t byteCount,
                      CModelComplex& model, CModelShared& shared);
