#include "WoW/Source/loadingscreen.h"

#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Scrn/scrn.h"
#include "ENGINE/Source/Services/texture.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <limits>

#ifdef LoadImage
#undef LoadImage
#endif

bool s_loadingScreenEnabled = false;
int s_xmlLoaded = 0;
int s_xmlTotal = 1;
int s_worldLoaded = 0;
float s_progress = 0.0F;
CGxTex* s_textureHandles[NUM_LOADING_TEXTURES]{};
MipBits* s_mipBits[NUM_LOADING_TEXTURES]{};
TEXTUREINFO s_textureInfo[NUM_LOADING_TEXTURES] = {
    {"Interface\\Glues\\loading", false, 0.5F, 0.5F, 1.0F, 1.0F, GxBlend_Alpha},
    {"Interface\\Glues\\LoadingBar\\Loading-BarFill", true, 0.5F, 0.075F, 0.525F, 0.025F, GxBlend_Alpha},
    {"Interface\\Glues\\LoadingBar\\Loading-BarBorder", false, 0.5F, 0.075F, 0.6F, 0.05F, GxBlend_Alpha},
};
float INFINITY_2 = 0.0F;
float TWO_PI_2 = 0.0F;

namespace {
HTEXTURE__* s_loadingTextures[NUM_LOADING_TEXTURES]{};
HLAYER s_loadingScreenLayer = 0;

int __fastcall EatLoadingEvent(const void*, void*) { return EatEvent(); }

void __fastcall LoadingScreenLayerPaint(void*, const RECTF* rect,
                                        const RECTF*, float elapsed) {
    LoadingScreenPaint(rect, elapsed);
}
}

void EnableLoadingScreen() {
    DisableLoadingScreen();
    for (std::uintptr_t index = 0; index < NUM_LOADING_TEXTURES; ++index)
        LoadImage(reinterpret_cast<void*>(index));
    const RECTF rect{0.0F, 0.0F, 1.0F, 1.0F};
    ScrnLayerCreate(&rect, 6U, 9.0F, nullptr, LoadingScreenLayerPaint,
                    &s_loadingScreenLayer);
    RegisterHandlers();
    InitializeProgressBar();
    s_loadingScreenEnabled = true;
    // Present once before the synchronous world load starts. The recovered
    // progress routine paints and presents directly, independent of the next
    // normal screen-layer paint event.
    UpdateProgressBar();
}

void InitializeProgressBar() {
    s_xmlLoaded = 0;
    s_xmlTotal = 1;
    s_worldLoaded = 0;
    s_progress = 0.0F;
    FrameXML_RegisterLoadProgressCallback(FrameXMLProgressCallback);
}

void __fastcall FrameXMLProgressCallback(int loaded, int total) {
    s_xmlLoaded = loaded;
    s_xmlTotal = total;
    UpdateProgressBar();
}

void UpdateProgressBar() {
    UpdateProgress();
    if (!s_loadingScreenEnabled)
        return;

    float minX = 0.0F, maxX = 1.0F;
    float minY = 0.0F, maxY = 1.0F;
    float minZ = 0.0F, maxZ = 1.0F;
    NTempest::C44Matrix savedProjection;
    NTempest::C44Matrix savedView;
    GxXformViewport(&minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    GxXformProjection(&savedProjection);
    GxXformView(&savedView);

    const NTempest::C44Matrix identity;
    const NTempest::C44Matrix ortho(
        2.0F, 0.0F, 0.0F, 0.0F,
        0.0F, 2.0F, 0.0F, 0.0F,
        0.0F, 0.0F, 0.004F, 0.0F,
        -1.0F, -1.0F, -1.0F, 1.0F);
    GxXformSetViewport(0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F);
    GxXformSetView(identity);
    GxXformSetProjection(ortho);
    const RECTF rect{0.0F, 0.0F, 1.0F, 1.0F};
    LoadingScreenPaint(&rect, 0.0F);
    GxScenePresent(3U);

    GxXformSetProjection(savedProjection);
    GxXformSetView(savedView);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void UpdateProgress() {
    double progress = 0.0;
    if (s_xmlTotal != 0)
        progress = static_cast<double>(s_xmlLoaded) / static_cast<double>(s_xmlTotal) * 0.75;
    if (s_worldLoaded != 0) progress += 0.25;
    s_progress = static_cast<float>(std::clamp(progress, 0.0, 1.0));
}

void RegisterHandlers() {
    constexpr std::array<EVENTID, 7> events{{
        EVENT_ID_CHAR, EVENT_ID_KEYDOWN, EVENT_ID_KEYUP,
        EVENT_ID_KEYDOWN_REPEATING, EVENT_ID_MOUSEDOWN,
        EVENT_ID_MOUSEMOVE_RELATIVE, EVENT_ID_MOUSEUP}};
    for (EVENTID event : events)
        EventRegisterEx(event, EatLoadingEvent, nullptr, 8.0F);
}
int EatEvent() { return 0; }

void __stdcall LoadingScreenPaint(const RECTF*, float) {
    static constexpr std::uint16_t indices[]{0, 1, 2, 3};
    static const NTempest::C3Vector normal{0.0F, 0.0F, 1.0F};
    static constexpr std::array<NTempest::C2Vector, 4> texCoord{{
        {0.0F, 1.0F}, {1.0F, 1.0F}, {0.0F, 0.0F}, {1.0F, 0.0F}}};

    static bool reportedPaint = false;
    if (!reportedPaint) {
        std::fprintf(stderr, "LOADING_PAINT textures=%p,%p,%p progress=%.3f\n",
                     static_cast<void*>(s_textureHandles[0]),
                     static_cast<void*>(s_textureHandles[1]),
                     static_cast<void*>(s_textureHandles[2]), s_progress);
        reportedPaint = true;
    }
    GxRsPush();
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxVertexShaderSelect(GxVS_PassThru);

    for (int index = 0; index < NUM_LOADING_TEXTURES; ++index) {
        CGxTex* texture = s_textureHandles[index];
        if (!texture)
            continue;
        const TEXTUREINFO& info = s_textureInfo[index];
        GxRsSet(GxRs_Blend, static_cast<EGxBlend>(info.blend));
        const float halfWidth = info.width * 0.5F;
        const float halfHeight = info.height * 0.5F;
        float left = info.centerX - halfWidth;
        float right = info.centerX + halfWidth;
        if (info.scaleWithProgress)
            right = left + info.width * s_progress;
        const float bottom = info.centerY - halfHeight;
        const float top = info.centerY + halfHeight;
        const std::array<NTempest::C3Vector, 4> positions{{
            {left, bottom, 0.0F}, {right, bottom, 0.0F},
            {left, top, 0.0F}, {right, top, 0.0F}}};

        GxRsSet(GxRs_Texture0, texture);
        GxPrimLockVertexPtrs(4, positions.data(), sizeof(NTempest::C3Vector),
                             &normal, 0, nullptr, 0, nullptr, 0,
                             texCoord.data(), sizeof(NTempest::C2Vector), nullptr, 0);
        GxPrimDrawElements(GxPrim_TriangleStrip, 4, indices);
        GxPrimUnlockVertexPtrs();
    }
    GxRsPop();
}

void __fastcall LoadImage(void* textureType) {
    const auto index = static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(textureType));
    if (index < NUM_LOADING_TEXTURES) {
        s_loadingTextures[index] = TextureCreate(s_textureInfo[index].name, nullptr, 0U);
        s_textureHandles[index] = s_loadingTextures[index]
            ? TextureGetGxTex(s_loadingTextures[index], 1, nullptr) : nullptr;
        std::fprintf(stderr, "LOADING_IMAGE index=%u file=%s handle=%p gx=%p\n", index,
                     s_textureInfo[index].name,
                     static_cast<void*>(s_loadingTextures[index]),
                     static_cast<void*>(s_textureHandles[index]));
    }
}

void __fastcall TextureCallback(unsigned int width, unsigned int, unsigned int,
                                unsigned int, void*, unsigned int* stride,
                                const void** texels) {
    if (width == 1 && stride != nullptr) *stride = 0;
    if (texels != nullptr) *texels = nullptr;
}

std::uint8_t __fastcall DrawingLoadingScreen() {
    return s_loadingScreenEnabled ? 1 : 0;
}

void DisableLoadingScreen() {
    if (!s_loadingScreenEnabled) return;
    if (s_loadingScreenLayer) {
        ScrnLayerDestroy(s_loadingScreenLayer);
        s_loadingScreenLayer = 0;
    }
    for (int i = 0; i < NUM_LOADING_TEXTURES; ++i) {
        if (s_loadingTextures[i])
            TextureRelease(s_loadingTextures[i]);
        s_loadingTextures[i] = nullptr;
        s_textureHandles[i] = nullptr;
        s_mipBits[i] = nullptr;
    }
    CleanupProgressBar();
    UnregisterHandlers();
    s_loadingScreenEnabled = false;
}

void CleanupProgressBar() {
    FrameXML_RegisterLoadProgressCallback(nullptr);
    s_progress = 0.0F;
}
void UnregisterHandlers() {
    constexpr std::array<EVENTID, 7> events{{
        EVENT_ID_CHAR, EVENT_ID_KEYDOWN, EVENT_ID_KEYUP,
        EVENT_ID_KEYDOWN_REPEATING, EVENT_ID_MOUSEDOWN,
        EVENT_ID_MOUSEMOVE_RELATIVE, EVENT_ID_MOUSEUP}};
    for (EVENTID event : events)
        EventUnregister(event, EatLoadingEvent);
}

void LoadingScreenRegisterWorldLoaded() {
    s_worldLoaded = 1;
    UpdateProgressBar();
}

int _E1_2() { INFINITY_2 = std::numeric_limits<float>::infinity(); return 0x7F800000; }
int _E2_2() { return _E1_2(); }
void _E4_2() { TWO_PI_2 = 3.1415927F + 3.1415927F; }
int _E5_2() { _E4_2(); return 0; }
void _E7_2() {}
int _E8_2() { _E7_2(); return 0; }
void _E19_2() {}
int _E20_2() { _E19_2(); return 0; }
void _E22_2() {}
int _E23_2() { _E22_2(); return 0; }
void _E25_2() {}
int _E26_2() { _E25_2(); return 0; }
void _E28_2() {}
int _E29_2() { _E28_2(); return 0; }
void _E31_2() {}
int _E32_2() { _E31_2(); return 0; }
void _E34_2() {}
int _E35_2() { _E34_2(); return 0; }
void _E37_2() {}
int _E38_2() { _E37_2(); return 0; }
void _E40_2() {}
int _E41_2() { _E40_2(); return 0; }
void _E43_2() {}
int _E44_2() { _E43_2(); return 0; }
void _E46_2() {}
int _E47_2() { _E46_2(); return 0; }
void _E49_2() {}
int _E50_2() { _E49_2(); return 0; }
void _E52_2() {}
int _E53_2() { _E52_2(); return 0; }
void _E87_2() {}
int _E88_2() { _E87_2(); return 0; }
void _E89_2() {}
int _E90_2() { _E89_2(); return 0; }
void _E94_1() {}
void _E95() {}
