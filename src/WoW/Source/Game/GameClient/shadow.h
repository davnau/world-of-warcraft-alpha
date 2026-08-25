#pragma once
#include "ENGINE/Source/Tempest/caabox.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include <cstdint>
#include <vector>
class CGxTex; class CGxBuf; struct CGxBufCommand; struct HMODEL__; struct HOBJECT__;

struct ShadowProjectionVertexPC {
    NTempest::C3Vector position{};
    NTempest::CImVector color{};
};

struct ShadowProjectionVertexPN {
    NTempest::C3Vector position{};
    NTempest::C3Vector normal{0.0F, 0.0F, 1.0F};
};

struct ShadowProjectionState {
    NTempest::CAaBox worldBox{};
    NTempest::C44Matrix basis{};
    NTempest::CImVector color{};
    float fadeOffset = 0.0F;
    bool active = false;
    bool rendered = false;
    unsigned renderCount = 0;
};

double FeetToWorld(float); CGxTex* ProjectTex2dGetFade(); void ProjectTex2d(NTempest::CAaBox*,const NTempest::C44Matrix*,CGxBuf*,float); void ProjectTexRenderPC(CGxBufCommand*,CGxBuf*); void ProjectTexRenderVerticesPC(CGxBuf*,int); void ProjectTexRenderIndices(CGxBuf*,int); void ProjectTexRenderPN(CGxBufCommand*,CGxBuf*,int); void ProjectTexRenderVerticesPN(CGxBuf*,int); void ShadowRender(const NTempest::C44Matrix*,HMODEL__*); void ShadowRender_LOD1(void*,float*,void*); void ShadowInit(); void ShadowDestroy(HOBJECT__*); int ConsoleCommand_ShadowLOD(const char*,const char*);
void ProjectTex2dMakeMatrices(NTempest::C44Matrix*, NTempest::C44Matrix*,
                              const NTempest::CAaBox*, const NTempest::C44Matrix*,
                              float, int);
void s_BlobFadeTex_0(unsigned, unsigned, unsigned, unsigned, unsigned, void*, unsigned*, const void**);
void s_ProjFadeTex(unsigned, unsigned, unsigned, unsigned, unsigned, void*, unsigned*, const void**);
extern float INFINITY_370; extern float BLOB_BELOW; extern float BLOB_ABOVE;
extern int s_shadowLOD;
extern ShadowProjectionState s_shadowProjectionState;
extern std::vector<ShadowProjectionVertexPC> s_shadowPCVertices;
extern std::vector<ShadowProjectionVertexPN> s_shadowPNVertices;
extern std::vector<std::uint16_t> s_shadowIndices;

int _E2_370(); int _E1_370(); int _E157_3(); void _E156_4(); int _E162_3(); int _E159_5(); int _E161_4(); void _E160_3(); int _E167_2(); int _E164_4(); int _E166_2(); void _E165_1(); int _E172_2(); void _E169_4(); int _E171_3(); void _E170_3(); int _E177_4(); void _E174_4(); int _E176_2(); void _E175_3();
