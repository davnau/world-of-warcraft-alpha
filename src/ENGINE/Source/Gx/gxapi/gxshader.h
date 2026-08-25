#pragma once

#define WIN32_LEAN_AND_MEAN
#include <d3d9.h>

#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/c4vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"

#include <cstdint>
#include <string>
#include <vector>

namespace RegisterCombiners {

struct CombinerVariable {
    unsigned input = 0;
    unsigned mapping = 0;
    unsigned component = 0;
};

struct CombinerOutput {
    unsigned abOutput = 0;
    unsigned cdOutput = 0;
    unsigned sumOutput = 0;
    unsigned scale = 0;
    unsigned bias = 0;
    unsigned abDotProduct = 0;
    unsigned cdDotProduct = 0;
    unsigned muxSum = 0;
};

struct CombinerPortion {
    CombinerVariable variable[4]{};
    CombinerOutput output{};
    void Realize(unsigned stage, unsigned portion) const;
};

struct GeneralCombiner {
    CombinerPortion portion[2]{};
    float constants[2][4]{};
    void Realize(unsigned stage, bool perStageConstants) const;
};

struct FinalCombiner {
    CombinerVariable variable[7]{};
    void Realize() const;
};

} // namespace RegisterCombiners

class CGxPixelShader {
public:
    std::vector<std::uint8_t> code;
    IDirect3DPixelShader9* apiSpecific = nullptr;
    unsigned glApiSpecific = 0;
    unsigned refCount = 1;
    bool valid = false;
    enum Target : int {
        Target_None = -1,
        Target_ps_1_1 = 0,
        Target_ps_1_2 = 1,
        Target_ps_1_3 = 2,
        Target_ps_1_4 = 3,
        Target_ps_2_0 = 4,
        Target_nvrc = 5,
        Target_nvts = 6,
        Target_nvts2 = 7,
        Target_nvts3 = 8,
        Target_atifs = 9,
        Target_arbfp1 = 10,
    } target = Target_None;
    std::vector<RegisterCombiners::GeneralCombiner> combiners;
    RegisterCombiners::FinalCombiner finalCombiner{};
};

class CGxVertexShader {
public:
    enum Target : int {
        Target_default = -2,
        Target_gx = -1,
        Target_vs_1_1 = 0,
        Target_vs_2_0 = 1,
        Target_arbvp1 = 2,
        Targets_Last = 3,
    };
    IDirect3DVertexShader9* apiSpecific = nullptr;
    unsigned refCount = 1;
};

struct CGxShaderParam {
    std::string name;
    unsigned index = 0;
    unsigned vectorCount = 1;
    bool dirty = true;
    float values[16]{};

    void Set(unsigned newIndex, unsigned newVectorCount, const float* newValues);
    void Set(const NTempest::C4Vector& value);
    void Set(const NTempest::C34Matrix& matrix);
    void Set(const NTempest::C44Matrix& matrix);
};

class CGxShader {
public:
    virtual ~CGxShader() = default;

    void SetParam(unsigned index, unsigned vectorCount, const float* values);
    void SetParam(CGxShaderParam* parameter, const NTempest::C4Vector& value);
    void SetParam(CGxShaderParam* parameter, const NTempest::C34Matrix& matrix);
    void SetParam(CGxShaderParam* parameter, const NTempest::C44Matrix& matrix);
    CGxShaderParam* GetParam(unsigned index);
    const CGxShaderParam* GetParam(unsigned index) const;
    CGxShaderParam* GetParam(const char* parameterName);
    CGxShaderParam* GetFirstParam();
    CGxShaderParam* GetNextParam(CGxShaderParam* parameter);

    std::vector<CGxShaderParam> params;
    bool paramsDirty = false;
};
