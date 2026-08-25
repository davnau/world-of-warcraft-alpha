#include "WoW/Source/SoundInterface/soundinterfacegluemusic.h"

#include "ENGINE/Source/Os/W32/ossound.h"

#include <limits>
#include <string>

float INFINITY_73 = 0.0F;
float TWO_PI_73 = 0.0F;
float OO_TWO_PI_73 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_5 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_5 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_5 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_5 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_5 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_5 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_5 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_5 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_5 = 0.0F;
float SECONDS_PER_HEARTBEAT_5 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_5 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_5 = 0.0F;

bool s_glueMusicInitialized = false;
const char* s_musicFile = nullptr;

namespace {
std::string s_musicFileStorage;
Sound* s_glueMusic = nullptr;
}

void InitializeGlueMusic() { s_glueMusicInitialized = true; }

void ShutdownGlueMusic() {
    SndInterfaceStopGlueMusic(0.0F);
    s_musicFileStorage.clear();
    s_musicFile = nullptr;
    s_glueMusicInitialized = false;
}

void SndInterfaceStopGlueMusic(float) {
    if (s_glueMusic != nullptr) {
        s_glueMusic->Stop();
        Sound::KillSound(s_glueMusic);
        s_glueMusic = nullptr;
    }
}

void SndInterfaceSetGlueMusic(const char* fileName) {
    if (!s_glueMusicInitialized || fileName == nullptr || *fileName == '\0') {
        SndInterfaceStopGlueMusic(3.0F);
        return;
    }
    if (s_musicFileStorage == fileName && s_glueMusic != nullptr)
        return;
    s_musicFileStorage = fileName;
    s_musicFile = s_musicFileStorage.c_str();
    SndInterfaceStopGlueMusic(3.0F);
    s_glueMusic = Sound::Alloc();
    if (s_glueMusic != nullptr) {
        s_glueMusic->SetVolume(1.0F);
        s_glueMusic->Play2DLooped();
    }
}

int _E2_73() { return _E1_73(); }
int _E1_73() { INFINITY_73 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_73() { _E4_73(); return 0; } void _E4_73() { TWO_PI_73 = 6.2831854F; }
int _E8_73() { _E7_73(); return 0; } void _E7_73() { OO_TWO_PI_73 = 1.0F / TWO_PI_73; }

#define SOUND_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
SOUND_CONST_PAIR(_E21_8, _E20_12, MAX_DUEL_DISTANCE_SQUARED_5, 100.0F)
SOUND_CONST_PAIR(_E24_11, _E23_15, MAX_TRADE_DISTANCE_SQUARED_5, 123.45679F)
SOUND_CONST_PAIR(_E27_8, _E26_11, MAX_SHOP_DISTANCE_SQUARED_5, 30.864195F)
SOUND_CONST_PAIR(_E30_6, _E29_10, MAX_INSPECT_DISTANCE_SQUARED_5, 30.864195F)
SOUND_CONST_PAIR(_E33_4, _E32_8, MAX_LOOT_DISTANCE_SQUARED_5, 25.0F)
SOUND_CONST_PAIR(_E36_3, _E35_9, MAX_BIND_DISTANCE_SQUARED_5, 100.0F)
SOUND_CONST_PAIR(_E39_5, _E38_9, MAX_SITCHAIR_DISTANCE_SQUARED_5, 0.25F)
SOUND_CONST_PAIR(_E42_4, _E41_7, MAX_SITCHAIRUSE_DISTANCE_SQUARED_5, 9.0F)
SOUND_CONST_PAIR(_E45_3, _E44_9, MAX_SHARE_DISTANCE_SQUARED_5, 10000.0F)
SOUND_CONST_PAIR(_E48_8, _E47_11, SECONDS_PER_HEARTBEAT_5, 5.0F)
SOUND_CONST_PAIR(_E51_2, _E50_12, SPELL_FOCUS_DISTANCE_SQUARED_5, 2500.0F)
SOUND_CONST_PAIR(_E54_7, _E53_10, MAX_OBJ_INTEREST_RADIUS_SQUARED_5, 10000.0F)
#undef SOUND_CONST_PAIR
