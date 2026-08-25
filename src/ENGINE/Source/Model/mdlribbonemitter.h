#pragma once
#include "ENGINE/Source/Model/model.h"
#include "ENGINE/Source/Services/ribbonemitter.h"
#include <span>

struct MDLRIBBONEMITTER {
    float edgesPerSecond = 10.0F;
    float edgeLifeSpan = 1.0F;
    float gravity = 0.0F;
    float above = 0.0F;
    float below = 0.0F;
    std::uint32_t rows = 1;
    std::uint32_t columns = 1;
    std::uint32_t texture = 0;
    std::uint32_t materialFlags = 0;
    std::uint32_t blendMode = 0;
};

void LoadRibbonMaterial(CRibbonMat& material, std::uint32_t flags, std::uint32_t blendMode);
CRibbonEmitter* LoadEmitterData(const MDLRIBBONEMITTER& definition,
                                const std::vector<HTEXTURE__*>& textures);
bool MdlReadLoadRibbonEmitters(CModelComplex& model, CModelShared& shared,
                               std::span<const std::uint8_t> bytes,
                               const std::vector<HTEXTURE__*>& textures = {});
void MdxReadRibbonEmitters(const std::uint8_t* fileBytes, std::size_t byteCount,
                           CModelComplex& model, CModelShared& shared,
                           const std::vector<HTEXTURE__*>& textures = {});

