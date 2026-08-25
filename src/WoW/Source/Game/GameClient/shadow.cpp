#include "WoW/Source/Game/GameClient/shadow.h"

#include "ENGINE/Source/Gx/gxapi/gxbuffer.h"
#include "ENGINE/Source/Gx/gxapi/gxtexture.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <limits>

float INFINITY_370 = 0.0F;
float BLOB_BELOW = 0.0F;
float BLOB_ABOVE = 0.0F;
int s_shadowLOD = 1;
ShadowProjectionState s_shadowProjectionState{};
std::vector<ShadowProjectionVertexPC> s_shadowPCVertices;
std::vector<ShadowProjectionVertexPN> s_shadowPNVertices;
std::vector<std::uint16_t> s_shadowIndices;

namespace {
CGxTex s_fadeTexture{};
std::array<NTempest::CImVector, 512> s_fadeTexels{};
NTempest::C3Vector s_zup{0.0F, 0.0F, 1.0F};
NTempest::CImVector s_shadowColor{};
unsigned s_fadeTexelCount = 0;
bool s_initialized = false;

void BuildBlobFadeTexture() {
    constexpr unsigned width = 64;
    constexpr unsigned height = 8;
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const float t = width > 1 ? static_cast<float>(x) / static_cast<float>(width - 1) : 0.0F;
            const float scaled = t * 12.0F;
            float alpha = 0.0F;
            if (scaled < 2.0F) {
                alpha = scaled * 0.5F;
            } else if (scaled < 10.0F) {
                alpha = 1.0F;
            } else {
                alpha = std::fmax(0.0F, (12.0F - scaled) * 0.5F);
            }
            s_fadeTexels[y * width + x] = NTempest::CImVector(
                static_cast<std::uint32_t>(std::clamp(alpha, 0.0F, 1.0F) * 255.0F) << 24);
        }
    }
    s_fadeTexelCount = width * height;
}

void BuildProjectedFadeTexture() {
    constexpr unsigned width = 64;
    constexpr unsigned height = 8;
    const unsigned fadeCount = width * 0.05F < 1.0F
        ? 1U
        : static_cast<unsigned>(width * 0.05F);
    const unsigned fadeBase = width > fadeCount ? width - fadeCount - 1 : 0;
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            unsigned alpha = 255;
            if (x == 0 || x == width - 1) {
                alpha = 0;
            } else if (x >= fadeBase && fadeCount != 0) {
                alpha = static_cast<unsigned>(
                    std::clamp((1.0F - static_cast<float>(x - fadeBase) /
                                           static_cast<float>(fadeCount)) * 255.0F,
                               0.0F, 255.0F));
            }
            s_fadeTexels[y * width + x] = NTempest::CImVector(alpha << 24);
        }
    }
    s_fadeTexelCount = width * height;
}

void ResetProjection() {
    s_shadowProjectionState = {};
    s_shadowPCVertices.clear();
    s_shadowPNVertices.clear();
    s_shadowIndices.clear();
}

template <typename T>
void CopyBufferBytes(CGxBuf* buffer, const std::vector<T>& values) {
    if (buffer == nullptr) {
        return;
    }
    const auto bytes = values.size() * sizeof(T);
    buffer->vertexData.resize(bytes);
    if (bytes != 0) {
        std::memcpy(buffer->vertexData.data(), values.data(), bytes);
    }
    buffer->vertexStride = sizeof(T);
    buffer->vertexCount = static_cast<unsigned>(values.size());
}

void CopyIndexBytes(CGxBuf* buffer) {
    if (buffer == nullptr) {
        return;
    }
    buffer->indexData.resize(s_shadowIndices.size() * sizeof(std::uint16_t));
    if (!s_shadowIndices.empty()) {
        std::memcpy(buffer->indexData.data(), s_shadowIndices.data(), buffer->indexData.size());
    }
    buffer->indexCount = static_cast<unsigned>(s_shadowIndices.size());
}
}

int _E2_370() { return _E1_370(); }

int _E1_370() {
    INFINITY_370 = std::numeric_limits<float>::infinity();
    return 0x7F800000;
}

int _E157_3() { return _E156_4(), 0; }

void _E156_4() {
    BLOB_BELOW = static_cast<float>(FeetToWorld(5.0F));
    BLOB_ABOVE = static_cast<float>(FeetToWorld(3.0F));
}

double FeetToWorld(float ft) { return static_cast<double>(ft) * 0.33333334; }

int _E162_3() { return _E159_5(), _E161_4(); }

int _E159_5() {
    s_fadeTexels.fill(NTempest::CImVector(0));
    s_fadeTexelCount = 512;
    return 0;
}

int _E161_4() { return std::atexit(_E160_3); }

void _E160_3() { s_fadeTexels.fill(NTempest::CImVector(0)); }

int _E167_2() { return _E164_4(), _E166_2(); }

int _E164_4() {
    ResetProjection();
    return 0;
}

int _E166_2() { return std::atexit(_E165_1); }

void _E165_1() { ResetProjection(); }

int _E172_2() { return _E169_4(), _E171_3(); }

void _E169_4() { s_zup = NTempest::C3Vector(0.0F, 0.0F, 1.0F); }

int _E171_3() { return std::atexit(_E170_3); }

void _E170_3() {}

int _E177_4() { return _E174_4(), _E176_2(); }

void _E174_4() { s_shadowColor = NTempest::CImVector(0); }

int _E176_2() { return std::atexit(_E175_3); }

void _E175_3() {}

CGxTex* ProjectTex2dGetFade() { return &s_fadeTexture; }

void ProjectTex2dMakeMatrices(NTempest::C44Matrix* textureMatrix,
                              NTempest::C44Matrix* fadeMatrix,
                              const NTempest::CAaBox* box,
                              const NTempest::C44Matrix* basis,
                              float fadeOffset, int inWorldSpace) {
    if (textureMatrix == nullptr || fadeMatrix == nullptr || box == nullptr) {
        return;
    }
    const float width = box->t.x - box->b.x;
    const float height = box->t.y - box->b.y;
    const float depth = box->t.z - box->b.z;
    *textureMatrix = NTempest::C44Matrix();
    *fadeMatrix = NTempest::C44Matrix();
    if (width <= 0.001F || height <= 0.001F || depth <= 0.001F) {
        return;
    }
    textureMatrix->a0 = 1.0F / width;
    textureMatrix->b1 = 1.0F / height;
    textureMatrix->d0 = 0.5F - box->b.x * textureMatrix->a0;
    textureMatrix->d1 = 0.5F - box->b.y * textureMatrix->b1;
    fadeMatrix->a0 = 1.0F / depth;
    fadeMatrix->c2 = fadeMatrix->a0;
    fadeMatrix->d0 = fadeOffset;
    fadeMatrix->d1 = 0.5F;
    fadeMatrix->d2 = fadeOffset;
    if (basis != nullptr) {
        *textureMatrix *= *basis;
    }
    if (inWorldSpace == 0) {
        textureMatrix->d2 = -box->b.z * fadeMatrix->c2;
    }
}

void s_BlobFadeTex_0(unsigned, unsigned width, unsigned height, unsigned,
                     unsigned mipLevel, void*, unsigned* stride, const void** texels) {
    if (stride == nullptr || texels == nullptr || width == 0 || mipLevel != 0) {
        return;
    }
    BuildBlobFadeTexture();
    *stride = width * sizeof(NTempest::CImVector);
    *texels = s_fadeTexels.data();
    if (height != 0 && width != 64) {
        *stride = width * sizeof(NTempest::CImVector);
    }
}

void s_ProjFadeTex(unsigned, unsigned width, unsigned height, unsigned,
                   unsigned mipLevel, void*, unsigned* stride, const void** texels) {
    if (stride == nullptr || texels == nullptr || width == 0 || mipLevel != 0) {
        return;
    }
    BuildProjectedFadeTexture();
    *stride = width * sizeof(NTempest::CImVector);
    *texels = s_fadeTexels.data();
    (void)height;
}

void ProjectTex2d(NTempest::CAaBox* basis, const NTempest::C44Matrix* matrix,
                  CGxBuf* color, float fadeOffset) {
    ResetProjection();
    if (basis == nullptr) {
        return;
    }
    s_shadowProjectionState.worldBox = *basis;
    if (matrix != nullptr) {
        s_shadowProjectionState.basis = *matrix;
    }
    s_shadowProjectionState.fadeOffset = fadeOffset;
    s_shadowProjectionState.color = s_shadowColor;
    s_shadowProjectionState.active = true;

    const auto& b = basis->b;
    const auto& t = basis->t;
    s_shadowPCVertices = {
        {{b.x, b.y, b.z}, s_shadowColor},
        {{t.x, b.y, b.z}, s_shadowColor},
        {{t.x, t.y, b.z}, s_shadowColor},
        {{b.x, t.y, b.z}, s_shadowColor},
    };
    s_shadowPNVertices = {
        {{b.x, b.y, b.z}, s_zup},
        {{t.x, b.y, b.z}, s_zup},
        {{t.x, t.y, b.z}, s_zup},
        {{b.x, t.y, b.z}, s_zup},
    };
    s_shadowIndices = {0, 1, 2, 0, 2, 3};
    CopyBufferBytes(color, s_shadowPCVertices);
    CopyIndexBytes(color);
}

void ProjectTexRenderPC(CGxBufCommand*, CGxBuf* buf) {
    ProjectTexRenderVerticesPC(buf, 0);
    ProjectTexRenderIndices(buf, 0);
}

void ProjectTexRenderVerticesPC(CGxBuf* buf, int) {
    CopyBufferBytes(buf, s_shadowPCVertices);
}

void ProjectTexRenderIndices(CGxBuf* buf, int) { CopyIndexBytes(buf); }

void ProjectTexRenderPN(CGxBufCommand*, CGxBuf* buf, int) {
    ProjectTexRenderVerticesPN(buf, 0);
    ProjectTexRenderIndices(buf, 0);
}

void ProjectTexRenderVerticesPN(CGxBuf* buf, int) {
    CopyBufferBytes(buf, s_shadowPNVertices);
}

void ShadowRender(const NTempest::C44Matrix* basis, HMODEL__* model) {
    if (basis != nullptr && model != nullptr && s_shadowLOD != 0) {
        ShadowRender_LOD1(model, const_cast<float*>(reinterpret_cast<const float*>(basis)), model);
    }
}

void ShadowRender_LOD1(void*, float* matrix, void*) {
    if (matrix == nullptr || !s_shadowProjectionState.active) {
        return;
    }
    s_shadowProjectionState.basis = *reinterpret_cast<NTempest::C44Matrix*>(matrix);
    s_shadowProjectionState.rendered = true;
    ++s_shadowProjectionState.renderCount;
}

void ShadowInit() {
    _E1_370();
    _E156_4();
    _E159_5();
    _E169_4();
    _E174_4();
    BuildBlobFadeTexture();
    s_fadeTexture.width = 64;
    s_fadeTexture.height = 8;
    s_fadeTexture.target = GxTex_2d;
    s_fadeTexture.format = CGxTex::Argb8888;
    s_fadeTexture.dataFormat = CGxTex::Argb8888;
    s_fadeTexture.userArg = nullptr;
    s_initialized = true;
}

void ShadowDestroy(HOBJECT__*) {
    ResetProjection();
    s_fadeTexture = {};
    s_initialized = false;
}

int ConsoleCommand_ShadowLOD(const char*, const char* arguments) {
    if (arguments == nullptr) {
        return 0;
    }
    char* end = nullptr;
    const long requested = std::strtol(arguments, &end, 10);
    if (end == arguments || requested < 0 || requested > 2) {
        return 0;
    }
    s_shadowLOD = static_cast<int>(requested);
    if (s_shadowLOD == 2) {
        BuildProjectedFadeTexture();
    } else {
        BuildBlobFadeTexture();
    }
    return 1;
}
