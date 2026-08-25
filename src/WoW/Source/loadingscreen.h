#pragma once

#include "ENGINE/Source/Base/coordinate.h"

#include <cstdint>

class CGxTex;
struct MipBits;
class CStatus;

enum TEXTURETYPE : int {
    TEXTURE_LOADING_BACKGROUND = 0,
    TEXTURE_LOADING_PROGRESS = 1,
    TEXTURE_LOADING_LOGO = 2,
    NUM_LOADING_TEXTURES = 3,
};

struct TEXTUREINFO {
    const char* name = nullptr;
    bool scaleWithProgress = false;
    float centerX = 0.5F;
    float centerY = 0.5F;
    float width = 1.0F;
    float height = 1.0F;
    int blend = 0;
};

extern bool s_loadingScreenEnabled;
extern int s_xmlLoaded;
extern int s_xmlTotal;
extern int s_worldLoaded;
extern float s_progress;
extern CGxTex* s_textureHandles[NUM_LOADING_TEXTURES];
extern MipBits* s_mipBits[NUM_LOADING_TEXTURES];
extern TEXTUREINFO s_textureInfo[NUM_LOADING_TEXTURES];
extern float INFINITY_2;
extern float TWO_PI_2;

void EnableLoadingScreen();
void InitializeProgressBar();
void __fastcall FrameXMLProgressCallback(int loaded, int total);
void UpdateProgressBar();
void UpdateProgress();
void RegisterHandlers();
int EatEvent();
void __stdcall LoadingScreenPaint(const RECTF* rect, float elapsed);
void __fastcall LoadImage(void* textureType);
void __fastcall TextureCallback(unsigned int width, unsigned int height, unsigned int depth,
                                unsigned int mipLevel, void* userArg,
                                unsigned int* texelStrideInBytes, const void** texels);
std::uint8_t __fastcall DrawingLoadingScreen();
void DisableLoadingScreen();
void CleanupProgressBar();
void UnregisterHandlers();
void LoadingScreenRegisterWorldLoaded();

int _E1_2();
int _E2_2();
void _E4_2();
int _E5_2();
void _E7_2();
int _E8_2();
void _E19_2();
int _E20_2();
void _E22_2();
int _E23_2();
void _E25_2();
int _E26_2();
void _E28_2();
int _E29_2();
void _E31_2();
int _E32_2();
void _E34_2();
int _E35_2();
void _E37_2();
int _E38_2();
void _E40_2();
int _E41_2();
void _E43_2();
int _E44_2();
void _E46_2();
int _E47_2();
void _E49_2();
int _E50_2();
void _E52_2();
int _E53_2();
void _E87_2();
int _E88_2();
void _E89_2();
int _E90_2();
void _E94_1();
void _E95();
