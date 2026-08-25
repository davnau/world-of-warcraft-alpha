#include "WoW/Source/SoundInterface/soundinterfacezonemusic.h"

#include "ENGINE/Source/Os/W32/ossound.h"
#include "ENGINE/Source/Os/W32/time.h"

#include <cstdlib>
#include <limits>

float INFINITY_76 = 0.0F;
float TWO_PI_76 = 0.0F;
float OO_TWO_PI_76 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_8 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_8 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_8 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_8 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_8 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_8 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_8 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_8 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_8 = 0.0F;
float SECONDS_PER_HEARTBEAT_8 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_8 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_8 = 0.0F;

namespace {
unsigned s_flags = 0;
unsigned s_nextPlay = 0;
unsigned s_minimumInterval = 5000;
unsigned s_maximumInterval = 15000;
int s_currentZone = 0;
Sound* s_sound = nullptr;
std::uint32_t s_randomState = 0x9e3779b9U;
}

void InitializeZoneMusic()
{
    s_flags |= 2U;
    s_nextPlay = 0;
}

int ZoneMusicIdle(unsigned now)
{
    if ((s_flags & 1U) == 0 && s_currentZone != 0) {
        if (s_sound && !s_sound->IsPlaying()) {
            Sound::KillSound(s_sound);
            s_sound = nullptr;
            s_nextPlay = GetNextPlayTime();
        }
        if (!s_sound && (s_nextPlay == 0 || now >= s_nextPlay))
            PlayMusic();
    }
    return 1;
}

std::uint32_t GetNextPlayTime()
{
    const unsigned span = s_maximumInterval > s_minimumInterval
        ? s_maximumInterval - s_minimumInterval : 1U;
    s_randomState = s_randomState * 1664525U + 1013904223U;
    return OsGetAsyncTimeMs() + s_minimumInterval + (s_randomState % span);
}

void PlayMusic()
{
    if ((s_flags & 1U) != 0 || s_currentZone == 0)
        return;
    if (s_sound)
        Sound::KillSound(s_sound);
    s_sound = Sound::Alloc();
    if (s_sound) {
        s_sound->SetVolume(1.0F);
        if (!s_sound->Play2DLooped()) {
            Sound::KillSound(s_sound);
            s_sound = nullptr;
        }
    }
}

void ShutdownZoneMusic(int)
{
    s_flags &= ~2U;
    s_currentZone = 0;
    if (s_sound) {
        Sound::KillSound(s_sound);
        s_sound = nullptr;
    }
}

void SndInterfaceRegisterNewZone(int zoneId)
{
    if (s_currentZone != zoneId && s_sound)
        s_flags |= 4U;
    s_currentZone = zoneId;
}

void SndInterfacePauseZoneMusic(int paused)
{
    if (paused) {
        if (s_sound) {
            Sound::KillSound(s_sound);
            s_sound = nullptr;
        }
        s_flags |= 1U;
    } else {
        s_flags &= ~1U;
    }
}

int SndInterfaceIsZoneMusicPaused() { return (s_flags & 1U) != 0; }
void SndInterfaceSetZoneMusicIntervals(unsigned minimumMs, unsigned maximumMs)
{
    s_minimumInterval = minimumMs;
    s_maximumInterval = maximumMs < minimumMs ? minimumMs : maximumMs;
}

int _E2_76() { return _E1_76(); }
int _E1_76() { INFINITY_76 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_76() { _E4_76(); return 0; }
void _E4_76() { TWO_PI_76 = 3.1415927F + 3.1415927F; }
int _E8_76() { _E7_76(); return 0; }
void _E7_76() { OO_TWO_PI_76 = 1.0F / TWO_PI_76; }

#define ZONEMUSIC_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
ZONEMUSIC_CONST_PAIR(_E21_11, _E20_15, MAX_DUEL_DISTANCE_SQUARED_8, 100.0F)
ZONEMUSIC_CONST_PAIR(_E24_14, _E23_18, MAX_TRADE_DISTANCE_SQUARED_8, 123.45679F)
ZONEMUSIC_CONST_PAIR(_E27_11, _E26_14, MAX_SHOP_DISTANCE_SQUARED_8, 30.864195F)
ZONEMUSIC_CONST_PAIR(_E30_9, _E29_13, MAX_INSPECT_DISTANCE_SQUARED_8, 30.864195F)
ZONEMUSIC_CONST_PAIR(_E33_7, _E32_11, MAX_LOOT_DISTANCE_SQUARED_8, 25.0F)
ZONEMUSIC_CONST_PAIR(_E36_6, _E35_12, MAX_BIND_DISTANCE_SQUARED_8, 100.0F)
ZONEMUSIC_CONST_PAIR(_E39_8, _E38_12, MAX_SITCHAIR_DISTANCE_SQUARED_8, 0.25F)
ZONEMUSIC_CONST_PAIR(_E42_7, _E41_10, MAX_SITCHAIRUSE_DISTANCE_SQUARED_8, 9.0F)
ZONEMUSIC_CONST_PAIR(_E45_6, _E44_12, MAX_SHARE_DISTANCE_SQUARED_8, 10000.0F)
ZONEMUSIC_CONST_PAIR(_E48_11, _E47_14, SECONDS_PER_HEARTBEAT_8, 5.0F)
ZONEMUSIC_CONST_PAIR(_E51_5, _E50_15, SPELL_FOCUS_DISTANCE_SQUARED_8, 2500.0F)
ZONEMUSIC_CONST_PAIR(_E54_10, _E53_13, MAX_OBJ_INTEREST_RADIUS_SQUARED_8, 10000.0F)
#undef ZONEMUSIC_CONST_PAIR

int _E62_5() { _E59_6(); return _E61_3(); }
void _E59_6() { s_randomState = 0x9e3779b9U; }
int _E61_3() { return std::atexit(_E60_8); }
void _E60_8() {}

