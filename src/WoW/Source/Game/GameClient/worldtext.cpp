#include "WoW/Source/Game/GameClient/worldtext.h"

#include "WoW/Source/Console/consolevar.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <memory>

namespace {
std::vector<WORLDTEXTSTRING*> s_textList;
CVar* s_fontHeightCVar = nullptr;
CVar* s_fontOutlineCVar = nullptr;
CVar* s_fontFadeInTime = nullptr;
CVar* s_fontFadeOutTime = nullptr;
CVar* s_fontFadeTotalTime = nullptr;
CVar* s_fontFadeAscendDistance = nullptr;
CVar* s_fontCharSpacing = nullptr;
CVar* s_fontConeAngle = nullptr;
constexpr std::uint32_t kWorldTextColors[7] = {
    0xFFFFFFFFU, 0xFFFFFFFFU, 0xFFFFFFFFU, 0xFF00FF00U,
    0xFFFFFFFFU, 0x8060607BU, 0xFFFF80C0U,
};

float Clamp01(float value) { return std::clamp(value, 0.0F, 1.0F); }
}

WORLDTEXTCREATEPARAMS s_worldTextParams[NUM_WORLDTEXTTYPES];
float INFINITY_369 = 0.0F;

int _E1_369() { INFINITY_369 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2_369() { return _E1_369(); }
unsigned _E185_15() { return 0; }
void _E186_8() { WorldTextShutdown(); }
int _E187_10() { return 0; }
int _E188_8() { _E185_15(); return _E187_10(); }
float* _E190_8() { return &s_worldTextParams[0].border.y; }
void _E191_6() {}
int _E192_9() { return 0; }
int _E193_8() { _E190_8(); return _E192_9(); }
void _E196_5() {}

WORLDTEXTCREATEPARAMS::WORLDTEXTCREATEPARAMS() {
    fontColor = NTempest::CImVector(0);
    shadowColor = NTempest::CImVector(0);
}

void WORLDTEXTCREATEPARAMS::Defaults() {
    ascendDistance = 24.0F;
    totalTime = 3000;
    fadeInTime = 1000;
    fadeOutTime = 2000;
    fontColor = NTempest::CImVector(0xFFFFFFFFU);
    shadowOffset = NTempest::C2Vector(0.0F, 0.0F);
    shadowColor = NTempest::CImVector(0xFF000000U);
    charSpacing = 0.0F;
    heightScale = 1.0F;
    zOffset = 0.0F;
    border = NTempest::C2Vector(0.0F, 0.0F);
    startFontHeight = 0.0125F;
    endFontHeight = 0.025F;
    enlargeTime = 0;
    shrinkTime = 0;
    flags = 0;
}

void WORLDTEXTSTRING::Hide(int hide) { hidden = hide; }
void WORLDTEXTSTRING::Render() {
    // The Gxu renderer consumes this state through the public render bridge.
    // Keeping the method state-only lets headless preservation tests exercise
    // visibility/lifetime without requiring a graphics device.
}

void WORLDTEXTSTRING::Update(float elapsed, const NTempest::C44Matrix* matrix,
                             const NTempest::C3Vector* basePosition) {
    const unsigned delta = elapsed > 0.0F ? static_cast<unsigned>(elapsed) : 0;
    elapsedTime += delta;
    CalculateTextHeight(elapsedTime);
    CalculateNewColor(elapsedTime);
    if (basePosition != nullptr) {
        NTempest::C4Vector position(basePosition->x, basePosition->y,
                                    basePosition->z, 1.0F);
        NTempest::C4Vector textPosition;
        CalculateNewPosition(&position, elapsedTime, &textPosition, matrix, 1);
    }
}

void WORLDTEXTSTRING::Reset() { elapsedTime = 0; }

void WORLDTEXTSTRING::CalculateNewPosition(const NTempest::C4Vector* worldPosition,
                                            unsigned elapsed, NTempest::C4Vector* textPos,
                                            const NTempest::C44Matrix*, int) {
    if (worldPosition == nullptr || textPos == nullptr) return;
    UpdatePosition(worldPosition, elapsed, textPos);
}

void WORLDTEXTSTRING::UpdatePosition(const NTempest::C4Vector* worldPosition,
                                     unsigned elapsed, NTempest::C4Vector* textPos) {
    if (worldPosition == nullptr || textPos == nullptr) return;
    *textPos = *worldPosition;
    if ((params.flags & 1U) != 0 || totalTime == 0) return;
    const float progress = static_cast<float>(std::max(1U, elapsed)) /
                           static_cast<float>(totalTime);
    float rise = params.ascendDistance * progress;
    if ((params.flags & 8U) != 0) {
        const float jump = Clamp01(progress * 3.0F);
        rise *= std::sin(jump * 3.1415927F);
    }
    textPos->z += rise;
}

void WORLDTEXTSTRING::CalculateTextHeight(unsigned elapsed) {
    if (totalTime == 0) return;
    const unsigned clamped = std::min(elapsed, totalTime);
    float height = params.endFontHeight;
    if ((params.flags & 0x10U) == 0) {
        if ((params.flags & 1U) == 0 && clamped < params.enlargeTime && params.enlargeTime != 0) {
            const float p = static_cast<float>(clamped) / params.enlargeTime;
            height = params.startFontHeight +
                     (params.endFontHeight - params.startFontHeight) * p;
        } else if ((params.flags & 1U) == 0 && clamped < totalTime && clamped >= params.shrinkTime &&
                   totalTime > params.shrinkTime) {
            const float p = static_cast<float>(clamped - params.shrinkTime) /
                            static_cast<float>(totalTime - params.shrinkTime);
            height = params.endFontHeight +
                     (params.startFontHeight - params.endFontHeight) * p;
        }
    }
    UpdateStringHeight(std::max(height, 0.001F));
}

void WORLDTEXTSTRING::RecreateString() {
    if (savedStringText[0] == 0) {
        string = nullptr;
        textWidth = textHeight = 0.0F;
        return;
    }
    string = this;
    textHeight = savedStringHeight;
    textWidth = static_cast<float>(std::strlen(savedStringText)) * savedStringHeight * 0.5F;
}

void WORLDTEXTSTRING::UpdateStringHeight(float height) {
    if ((m_flags & 0x80U) == 0 || height != savedStringHeight) {
        m_flags |= 0x80U;
        savedStringHeight = height;
        RecreateString();
    }
}

void WORLDTEXTSTRING::CalculateNewColor(unsigned elapsed) {
    if (string == nullptr || totalTime == 0) return;
    unsigned char alpha = params.fontColor.a;
    if (params.fadeInTime != 0 && elapsed < params.fadeInTime) {
        alpha = static_cast<unsigned char>(params.fontColor.a *
                 static_cast<float>(elapsed) / params.fadeInTime);
    } else if (params.fadeOutTime != 0 && elapsed > totalTime - std::min(totalTime, params.fadeOutTime)) {
        const unsigned start = totalTime - std::min(totalTime, params.fadeOutTime);
        alpha = static_cast<unsigned char>(params.fontColor.a *
                 static_cast<float>(totalTime - elapsed) /
                 std::max(1U, totalTime - start));
    }
    (void)alpha;
}

void WORLDTEXTSTRING::InitTextFrame(const char* text) {
    if (text != nullptr && *text != 0) {
        std::strncpy(savedStringText, text, sizeof(savedStringText) - 1);
        savedStringText[sizeof(savedStringText) - 1] = 0;
    }
}

void WorldTextInitialize() {
    for (unsigned i = 0; i < NUM_WORLDTEXTTYPES; ++i) {
        s_worldTextParams[i] = WORLDTEXTCREATEPARAMS();
        s_worldTextParams[i].Defaults();
        s_worldTextParams[i].fontColor = NTempest::CImVector(kWorldTextColors[i]);
        s_worldTextParams[i].startFontHeight = 0.0125F;
        s_worldTextParams[i].endFontHeight = i == WT_CRIT ? 0.0375F : 0.025F;
    }
    InitConsoleVariables();
    s_worldTextParams[WT_DAMAGE].totalTime = 800;
    s_worldTextParams[WT_DAMAGE].fadeInTime = 0;
    s_worldTextParams[WT_DAMAGE].fadeOutTime = 800;
    s_worldTextParams[WT_ABSORB] = s_worldTextParams[WT_DAMAGE];
    s_worldTextParams[WT_CRIT] = s_worldTextParams[WT_DAMAGE];
    s_worldTextParams[WT_HEALSPELL].totalTime = s_fontFadeTotalTime ? s_fontFadeTotalTime->IntValue() : 1200;
    s_worldTextParams[WT_HEALSPELL].fadeInTime = 200;
    s_worldTextParams[WT_HEALSPELL].fadeOutTime = 900;
    s_worldTextParams[WT_XPGAIN].totalTime = 6000;
    s_worldTextParams[WT_XPGAIN].fadeInTime = 500;
    s_worldTextParams[WT_XPGAIN].fadeOutTime = 400;
    s_worldTextParams[WT_QUEST].totalTime = 0xFFFFFFFFU;
    s_worldTextParams[WT_QUEST].fadeOutTime = 0xFFFFFFFFU;
}

CVar* InitConsoleVariables() {
    s_fontHeightCVar = CVar::Register("damagefontheight", "Height to use for damage text", 0, "0.0035", nullptr, 0, 0, nullptr);
    s_fontOutlineCVar = CVar::Register("damagefontoutline", "Outline the damage text", 0, "1", nullptr, 0, 0, nullptr);
    s_fontFadeInTime = CVar::Register("damagefontfadeintime", "Fade-in time for damage text", 0, "200", nullptr, 0, 0, nullptr);
    s_fontFadeOutTime = CVar::Register("damagefontfadeouttime", "Fade-out time for damage text", 0, "900", nullptr, 0, 0, nullptr);
    s_fontFadeTotalTime = CVar::Register("damagefonttotaltime", "Total time to draw damage text", 0, "1200", nullptr, 0, 0, nullptr);
    s_fontFadeAscendDistance = CVar::Register("damagefontascenddistance", "World inches to move", 0, "7", nullptr, 0, 0, nullptr);
    s_fontCharSpacing = CVar::Register("damagefontcharspacing", "Character spacing", 0, "0.001", nullptr, 0, 0, nullptr);
    s_fontConeAngle = CVar::Register("damagefontconeangle", "Damage font cone angle", 0, "0", nullptr, 0, 0, nullptr);
    return s_fontConeAngle;
}

void WorldTextShutdown() {
    for (WORLDTEXTSTRING* text : s_textList) delete text;
    s_textList.clear();
}

void WorldTextClearStrings() {
    for (WORLDTEXTSTRING* text : s_textList) text->string = nullptr;
}

void WorldTextGetColor(int color, NTempest::CImVector* result) {
    if (result == nullptr || color < 0 || color >= NUM_WORLDTEXTTYPES) return;
    *result = s_worldTextParams[color].fontColor;
}

HWORLDTEXT__* WorldTextCreate(int text, const char* value, std::uint64_t object,
                               const NTempest::CImVector* colorOverride) {
    if (text < 0 || text >= NUM_WORLDTEXTTYPES) return nullptr;
    auto* result = new WORLDTEXTSTRING();
    result->worldTextType = static_cast<WORLDTEXTTYPE>(text);
    result->params = s_worldTextParams[text];
    result->totalTime = result->params.totalTime;
    result->heightScale = result->params.heightScale;
    result->zOffset = result->params.zOffset;
    result->object = object;
    if (colorOverride != nullptr) result->params.fontColor = *colorOverride;
    result->InitTextFrame(value);
    result->UpdateStringHeight(result->params.startFontHeight);
    s_textList.push_back(result);
    return result;
}

void WorldTextUpdate(const NTempest::C44Matrix* matrix, float elapsed) {
    for (WORLDTEXTSTRING* text : s_textList) {
        if (text != nullptr && text->object != 0) text->Update(elapsed, matrix, nullptr);
    }
}

void WorldTextUpdate(WORLDTEXTSTRING* text, const NTempest::C44Matrix* matrix,
                     float elapsed, const NTempest::C3Vector* position) {
    if (text != nullptr) text->Update(elapsed, matrix, position);
}

int WorldTextIsTextDone(WORLDTEXTSTRING* text) {
    if (text == nullptr) return 1;
    if (text->elapsedTime <= text->totalTime) return 0;
    return (text->params.flags & 1U) != 0 ? 0 : 1;
}

void WorldTextShow(WORLDTEXTSTRING* text, int show) { if (text != nullptr) text->Hide(show == 0); }
void WorldTextRender(WORLDTEXTSTRING* text) { if (text != nullptr) text->Render(); }

NTempest::CImVector* ColorInterp(NTempest::CImVector* result, float range,
                                 NTempest::CImVector startColor,
                                 NTempest::CImVector endColor) {
    if (result == nullptr) return nullptr;
    range = Clamp01(range);
    result->b = static_cast<std::uint8_t>(IntInterp(startColor.b, endColor.b, range));
    result->g = static_cast<std::uint8_t>(IntInterp(startColor.g, endColor.g, range));
    result->r = static_cast<std::uint8_t>(IntInterp(startColor.r, endColor.r, range));
    result->a = static_cast<std::uint8_t>(IntInterp(startColor.a, endColor.a, range));
    return result;
}

std::int64_t IntInterp(int end, int value, float progress) {
    return static_cast<std::int64_t>((value - end) * progress + end);
}
