#include "WoW/Source/SoundInterface/snddebug.h"

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "ENGINE/Source/Os/W32/ossound.h"
#include "WoW/Source/SoundInterface/soundinterface.h"
#include "WoW/Source/SoundInterface/snddebugindoors.h"
#include "WoW/Source/SoundInterface/snddebugoutdoors.h"
#include "WoW/Source/clientruntime.h"

#include <cstdlib>
#include <limits>

float INFINITY_75 = 0.0F;
float TWO_PI_75 = 0.0F;
float OO_TWO_PI_75 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_10 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_10 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_10 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_10 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_10 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_10 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_10 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_10 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_10 = 0.0F;
float SECONDS_PER_HEARTBEAT_10 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_10 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_10 = 0.0F;

int s_pingSound = 0;
int s_pingFrequency = 100;
unsigned s_lastPingTime = 0;
NTempest::C3Vector s_pingPosition;

void SndDebugInitialize(const char*) {
    s_pingSound = 0; s_pingFrequency = 100; s_lastPingTime = 0;
    ConsoleCommandRegister(nullptr, "snddebugping", CLIENT_COMMAND_CATEGORY_DEBUG, PingSound);
    ConsoleCommandRegister(nullptr, "snddebugroomtype", CLIENT_COMMAND_CATEGORY_DEBUG, RoomType);
    EventRegisterEx(EVENT_ID_IDLE, DebugTickHandler, nullptr, 0.0F);
}

int __fastcall PingSound(const char*, const char* value) {
    if (value && *value) {
        char* end = nullptr;
        s_pingSound = static_cast<int>(std::strtol(value, &end, 0));
        if (end && *end) s_pingFrequency = std::max(100, static_cast<int>(std::strtol(end, nullptr, 0)));
        s_pingPosition = Sound::GetListenerPosition();
        ConsolePrintf("Ping sound %d set for interval of %d milliseconds", s_pingSound, s_pingFrequency);
    } else { s_pingSound = 0; ConsolePrintf("Disabling ping sound"); }
    return 1;
}

int __fastcall RoomType(const char*, const char* value) {
    const unsigned type = value ? static_cast<unsigned>(std::strtoul(value, nullptr, 0)) : 0;
    SndSetRoomType(EVENT_ID_IDLE, static_cast<int>(std::min(type, static_cast<unsigned>(SNDROOMTYPE_PSYCHOTIC))));
    return 1;
}

int __fastcall DebugTickHandler(const void*, void*) { SndDebugTick(); return 1; }
char SndDebugTick() {
    if (!s_pingSound) return 0;
    const unsigned now = OsGetAsyncTimeMs();
    if (now - s_lastPingTime >= static_cast<unsigned>(std::max(100, s_pingFrequency))) {
        InternalPlaySound(nullptr, s_pingSound, -1);
        s_lastPingTime = now;
    }
    return 1;
}

void SndDebugShutdown(const char*) {
    ConsoleCommandUnregister("snddebugping"); ConsoleCommandUnregister("snddebugroomtype");
    IndoorsShutdown(); OutdoorsShutdown(); s_pingSound = 0;
    EventUnregister(EVENT_ID_IDLE, DebugTickHandler);
}
void SndDebugDungeonTransition(const char*, unsigned continent, unsigned mapId) {
    if (continent == 0) SndDebugRegisterContinent(mapId);
}

int _E2_75() { return _E1_75(); } int _E1_75() { INFINITY_75 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E62_4() { _E59_5(); return _E61_2(); } void _E59_5() { s_pingPosition = {}; } int _E61_2() { return 0; } void _E60_7() {}
int _E5_75() { _E4_75(); return 0; } void _E4_75() { TWO_PI_75 = 6.2831854F; }
int _E8_75() { _E7_75(); return 0; } void _E7_75() { OO_TWO_PI_75 = 1.0F / TWO_PI_75; }
#define DEBUG_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
DEBUG_CONST_PAIR(_E21_10, _E20_14, MAX_DUEL_DISTANCE_SQUARED_10, 100.0F)
DEBUG_CONST_PAIR(_E24_13, _E23_17, MAX_TRADE_DISTANCE_SQUARED_10, 123.45679F)
DEBUG_CONST_PAIR(_E27_10, _E26_13, MAX_SHOP_DISTANCE_SQUARED_10, 30.864195F)
DEBUG_CONST_PAIR(_E30_8, _E29_12, MAX_INSPECT_DISTANCE_SQUARED_10, 30.864195F)
DEBUG_CONST_PAIR(_E33_6, _E32_10, MAX_LOOT_DISTANCE_SQUARED_10, 25.0F)
DEBUG_CONST_PAIR(_E36_5, _E35_11, MAX_BIND_DISTANCE_SQUARED_10, 100.0F)
DEBUG_CONST_PAIR(_E39_7, _E38_11, MAX_SITCHAIR_DISTANCE_SQUARED_10, 0.25F)
DEBUG_CONST_PAIR(_E42_6, _E41_9, MAX_SITCHAIRUSE_DISTANCE_SQUARED_10, 9.0F)
DEBUG_CONST_PAIR(_E45_5, _E44_11, MAX_SHARE_DISTANCE_SQUARED_10, 10000.0F)
DEBUG_CONST_PAIR(_E48_10, _E47_13, SECONDS_PER_HEARTBEAT_10, 5.0F)
DEBUG_CONST_PAIR(_E51_4, _E50_14, SPELL_FOCUS_DISTANCE_SQUARED_10, 2500.0F)
DEBUG_CONST_PAIR(_E54_9, _E53_12, MAX_OBJ_INTEREST_RADIUS_SQUARED_10, 10000.0F)
#undef DEBUG_CONST_PAIR
