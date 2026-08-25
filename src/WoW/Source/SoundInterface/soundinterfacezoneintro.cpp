#include "WoW/Source/SoundInterface/soundinterfacezoneintro.h"

#include "ENGINE/Source/Os/W32/ossound.h"

#include <limits>

float INFINITY_77 = 0.0F;
float TWO_PI_77 = 0.0F;
float OO_TWO_PI_77 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_9 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_9 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_9 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_9 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_9 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_9 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_9 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_9 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_9 = 0.0F;
float SECONDS_PER_HEARTBEAT_9 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_9 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_9 = 0.0F;

namespace {
Sound* s_zoneIntroSound = nullptr;
int s_zoneIntroPriority = 0;
unsigned s_lastPlayTime = 0;
}

void SndInterfaceZoneIntroIdler()
{
    if (s_zoneIntroSound && !s_zoneIntroSound->IsPlaying()) {
        Sound::KillSound(s_zoneIntroSound);
        s_zoneIntroSound = nullptr;
    }
}

void SndInterfaceZoneIntroInitialize(const char*)
{
    s_lastPlayTime = 0;
}

int CCommand_ZoneIntroReset()
{
    s_lastPlayTime = 0;
    return 1;
}

void SndInterfaceZoneIntroDestroy()
{
    if (s_zoneIntroSound) {
        Sound::KillSound(s_zoneIntroSound);
        s_zoneIntroSound = nullptr;
    }
    s_lastPlayTime = 0;
    s_zoneIntroPriority = 0;
}

void SndInterfaceRegisterNewZoneIntro(int priority, int)
{
    if (priority == 0) {
        SndInterfaceZoneIntroStop();
        return;
    }
    if (s_zoneIntroSound && s_zoneIntroPriority > priority)
        return;
    SndInterfaceZoneIntroStop();
    s_zoneIntroSound = Sound::Alloc();
    if (s_zoneIntroSound && s_zoneIntroSound->Play2D()) {
        s_zoneIntroPriority = priority;
        s_lastPlayTime = 1;
    } else {
        SndInterfaceZoneIntroStop();
    }
}

void SndInterfaceZoneIntroStop()
{
    if (s_zoneIntroSound) {
        Sound::KillSound(s_zoneIntroSound);
        s_zoneIntroSound = nullptr;
    }
}

int _E2_77() { return _E1_77(); }
int _E1_77() { INFINITY_77 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_77() { _E4_77(); return 0; }
void _E4_77() { TWO_PI_77 = 3.1415927F + 3.1415927F; }
int _E8_77() { _E7_77(); return 0; }
void _E7_77() { OO_TWO_PI_77 = 1.0F / TWO_PI_77; }

#define ZONEINTRO_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
ZONEINTRO_CONST_PAIR(_E21_12, _E20_16, MAX_DUEL_DISTANCE_SQUARED_9, 100.0F)
ZONEINTRO_CONST_PAIR(_E24_15, _E23_19, MAX_TRADE_DISTANCE_SQUARED_9, 123.45679F)
ZONEINTRO_CONST_PAIR(_E27_12, _E26_15, MAX_SHOP_DISTANCE_SQUARED_9, 30.864195F)
ZONEINTRO_CONST_PAIR(_E30_10, _E29_14, MAX_INSPECT_DISTANCE_SQUARED_9, 30.864195F)
ZONEINTRO_CONST_PAIR(_E33_8, _E32_12, MAX_LOOT_DISTANCE_SQUARED_9, 25.0F)
ZONEINTRO_CONST_PAIR(_E36_7, _E35_13, MAX_BIND_DISTANCE_SQUARED_9, 100.0F)
ZONEINTRO_CONST_PAIR(_E39_9, _E38_13, MAX_SITCHAIR_DISTANCE_SQUARED_9, 0.25F)
ZONEINTRO_CONST_PAIR(_E42_8, _E41_11, MAX_SITCHAIRUSE_DISTANCE_SQUARED_9, 9.0F)
ZONEINTRO_CONST_PAIR(_E45_7, _E44_13, MAX_SHARE_DISTANCE_SQUARED_9, 10000.0F)
ZONEINTRO_CONST_PAIR(_E48_12, _E47_15, SECONDS_PER_HEARTBEAT_9, 5.0F)
ZONEINTRO_CONST_PAIR(_E51_6, _E50_16, SPELL_FOCUS_DISTANCE_SQUARED_9, 2500.0F)
ZONEINTRO_CONST_PAIR(_E54_11, _E53_14, MAX_OBJ_INTEREST_RADIUS_SQUARED_9, 10000.0F)
#undef ZONEINTRO_CONST_PAIR

