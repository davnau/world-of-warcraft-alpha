#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c4vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/cimvector.h"

#include <cstdint>
#include <vector>

class CVar;

enum WORLDTEXTTYPE : int {
    WT_DAMAGE = 0,
    WT_ABSORB = 1,
    WT_CRIT = 2,
    WT_HEALSPELL = 3,
    WT_MISS = 4,
    WT_XPGAIN = 5,
    WT_QUEST = 6,
    NUM_WORLDTEXTTYPES = 7,
};

struct WORLDTEXTCREATEPARAMS {
    float ascendDistance = 0.0F;
    unsigned totalTime = 0;
    unsigned fadeInTime = 0;
    unsigned fadeOutTime = 0;
    NTempest::CImVector fontColor;
    NTempest::C2Vector shadowOffset;
    NTempest::CImVector shadowColor;
    float charSpacing = 0.0F;
    float heightScale = 1.0F;
    float zOffset = 0.0F;
    NTempest::C2Vector border;
    float startFontHeight = 0.0F;
    float endFontHeight = 0.0F;
    unsigned enlargeTime = 0;
    unsigned shrinkTime = 0;
    unsigned flags = 0;
    char fontName[260]{};
    float fontHeight = 0.0F;

    WORLDTEXTCREATEPARAMS();
    void Defaults();
};

struct WORLDTEXTSTRING {
    WORLDTEXTTYPE worldTextType = WT_DAMAGE;
    WORLDTEXTCREATEPARAMS params;
    unsigned elapsedTime = 0;
    unsigned totalTime = 0;
    std::uint64_t object = 0;
    float textWidth = 0.0F;
    float textHeight = 0.0F;
    float heightScale = 1.0F;
    float zOffset = 0.0F;
    int hidden = 0;
    unsigned m_flags = 0;
    void* string = nullptr;
    float savedStringHeight = 0.0F;
    char savedStringText[64]{};

    void Hide(int hide);
    void Render();
    void Update(float elapsed, const NTempest::C44Matrix* matrix,
                const NTempest::C3Vector* basePosition);
    void Reset();
    void CalculateNewPosition(const NTempest::C4Vector* worldPosition,
                              unsigned elapsedTime, NTempest::C4Vector* textPos,
                              const NTempest::C44Matrix* matrix,
                              int worldPositionSpecified);
    void UpdatePosition(const NTempest::C4Vector* worldPosition,
                        unsigned elapsedTime, NTempest::C4Vector* textPos);
    void CalculateTextHeight(unsigned elapsedTime);
    void RecreateString();
    void UpdateStringHeight(float height);
    void CalculateNewColor(unsigned elapsed);
    void InitTextFrame(const char* text);
};

using HWORLDTEXT__ = WORLDTEXTSTRING;

extern WORLDTEXTCREATEPARAMS s_worldTextParams[NUM_WORLDTEXTTYPES];

void WorldTextInitialize();
CVar* InitConsoleVariables();
void WorldTextShutdown();
void WorldTextClearStrings();
void WorldTextGetColor(int color, NTempest::CImVector* result);
HWORLDTEXT__* WorldTextCreate(int text, const char* value, std::uint64_t object,
                               const NTempest::CImVector* colorOverride);
void WorldTextUpdate(const NTempest::C44Matrix* matrix, float elapsed);
void WorldTextUpdate(WORLDTEXTSTRING* text, const NTempest::C44Matrix* matrix,
                     float elapsed, const NTempest::C3Vector* position);
int WorldTextIsTextDone(WORLDTEXTSTRING* text);
void WorldTextShow(WORLDTEXTSTRING* text, int show);
void WorldTextRender(WORLDTEXTSTRING* text);
NTempest::CImVector* ColorInterp(NTempest::CImVector* result, float range,
                                 NTempest::CImVector startColor,
                                 NTempest::CImVector endColor);
std::int64_t IntInterp(int end, int value, float progress);

int _E2_369();
int _E1_369();
int _E188_8();
unsigned _E185_15();
int _E187_10();
void _E186_8();
int _E193_8();
float* _E190_8();
int _E192_9();
void _E191_6();
void _E196_5();
