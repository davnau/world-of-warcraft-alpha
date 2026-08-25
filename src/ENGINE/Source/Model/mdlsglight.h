#pragma once
#include "ENGINE/Source/Model/model.h"
#include <span>

struct CModelLight {
    std::uint32_t type = 0;
    std::int32_t bone = -1;
    NTempest::C3Vector position{};
    NTempest::C3Vector ambientColor{};
    float ambientIntensity = 0.0F;
    NTempest::C3Vector diffuseColor{1.0F};
    float diffuseIntensity = 1.0F;
    float attenuationStart = 0.0F;
    float attenuationEnd = 1.0F;
    bool enabled = true;
};

std::uintptr_t CreateGxLight(const CModelLight& light);
bool MdlReadLoadLights(CModelComplex& model, std::span<const std::uint8_t> bytes);
void MdxReadLights(const std::uint8_t* fileBytes, std::size_t byteCount, CModelComplex& model);
const CModelLight* ModelGetLight(std::uintptr_t handle);
void ModelDestroyLights(CModelComplex& model);

