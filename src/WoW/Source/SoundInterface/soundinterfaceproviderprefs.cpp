#include "WoW/Source/SoundInterface/soundinterfaceproviderprefs.h"

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Os/W32/ossound.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "WoW/Source/DB/DBClient/dbclient.h"
#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cmath>
#include <limits>

float INFINITY_72 = 0.0F;
float TWO_PI_72 = 0.0F;
float OO_TWO_PI_72 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_4 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_4 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_4 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_4 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_4 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_4 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_4 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_4 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_4 = 0.0F;
float SECONDS_PER_HEARTBEAT_4 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_4 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_4 = 0.0F;

bool g_underWater = false;
SoundProviderDesc s_currentProviderDesc;

namespace {
SoundProviderDesc s_startProviderDesc;
SoundProviderDesc s_targetProviderDesc;
SoundProviderDesc s_desc;
SoundProviderDesc s_descUnderwater;
unsigned s_flags = 0;
unsigned s_providerPrefFadeStartTime = 0;
unsigned s_providerPrefFadeEndTime = 0;
unsigned s_providerPrefFadeDuration = 0;
bool s_idleRunning = false;

int __fastcall EnvironmentListHandlerCommand(const char*, const char*) {
    return EnvironmentListHandler();
}

SoundProviderDesc DefaultDesc() { return {}; }

void ApplyReverb() {
    Sound::SetReverbProperties(s_currentProviderDesc.DecayTime,
                               s_currentProviderDesc.RoomRolloffFactor,
                               s_currentProviderDesc.AirAbsorptionHF);
}
}

void SaveDesc(SoundProviderDesc& destination, const SoundProviderPreferencesRec& record) {
    destination.Environment = record.m_EAXEnvironmentSelection;
    destination.EnvSize = record.m_EAX2EnvironmentSize;
    destination.EnvDiffusion = record.m_EAX2EnvironmentDiffusion;
    destination.Room = record.m_EAX2Room;
    destination.RoomHF = record.m_EAX2RoomHF;
    destination.RoomLF = record.m_EAX3RoomLF;
    destination.DecayTime = record.m_EAXDecayTime;
    destination.DecayHFRatio = record.m_EAX2DecayHFRatio;
    destination.DecayLFRatio = record.m_EAX3DecayLFRatio;
    destination.Reflections = record.m_EAX2Reflections;
    destination.ReflectionsDelay = record.m_EAX2ReflectionsDelay;
    destination.Reverb = record.m_EAX2Reverb;
    destination.ReverbDelay = record.m_EAX2ReverbDelay;
    destination.EchoTime = record.m_EAX3EchoTime;
    destination.EchoDepth = record.m_EAX3EchoDepth;
    destination.ModulationTime = record.m_EAX3ModulationTime;
    destination.ModulationDepth = record.m_EAX3ModulationDepth;
    destination.AirAbsorptionHF = record.m_EAX2AirAbsorption;
    destination.HFReference = record.m_EAX3HFReference;
    destination.LFReference = record.m_EAX3LFReference;
    destination.RoomRolloffFactor = record.m_EAX2RoomRolloff;
    destination.Flags = static_cast<unsigned>(record.m_Flags);
}

double InterpFloat(float progress, float start, float end) {
    return static_cast<double>(start) + (static_cast<double>(end) - start) * progress;
}

std::int64_t InterpInt(int start, int end, float progress) {
    return static_cast<std::int64_t>(std::llround(static_cast<double>(start) +
        (static_cast<double>(end) - start) * progress));
}

void StopWorldIdleHandler(EVENTID eventId) {
    if (s_idleRunning) {
        EventUnregister(eventId, WorldIdleHandler);
        s_idleRunning = false;
    }
}

int __fastcall WorldIdleHandler(const void* eventData, void*) {
    const auto* idle = static_cast<const EVENT_DATA_IDLE*>(eventData);
    const unsigned now = idle ? idle->time : OsGetAsyncTimeMs();
    SndInterfaceFadeProviderPrefs(now);
    return 1;
}

void SndInterfaceFadeProviderPrefs(unsigned now) {
    if (!s_idleRunning) return;
    if (now >= s_providerPrefFadeEndTime || s_providerPrefFadeDuration == 0) {
        s_currentProviderDesc = s_targetProviderDesc;
        StopWorldIdleHandler();
        ApplyReverb();
        return;
    }
    const float progress = std::clamp(static_cast<float>(now - s_providerPrefFadeStartTime) /
                                      static_cast<float>(s_providerPrefFadeDuration), 0.0F, 1.0F);
    s_currentProviderDesc.EnvSize = static_cast<float>(InterpFloat(progress, s_startProviderDesc.EnvSize, s_targetProviderDesc.EnvSize));
    s_currentProviderDesc.EnvDiffusion = static_cast<float>(InterpFloat(progress, s_startProviderDesc.EnvDiffusion, s_targetProviderDesc.EnvDiffusion));
    s_currentProviderDesc.DecayTime = static_cast<float>(InterpFloat(progress, s_startProviderDesc.DecayTime, s_targetProviderDesc.DecayTime));
    s_currentProviderDesc.DecayHFRatio = static_cast<float>(InterpFloat(progress, s_startProviderDesc.DecayHFRatio, s_targetProviderDesc.DecayHFRatio));
    s_currentProviderDesc.DecayLFRatio = static_cast<float>(InterpFloat(progress, s_startProviderDesc.DecayLFRatio, s_targetProviderDesc.DecayLFRatio));
    s_currentProviderDesc.ReflectionsDelay = static_cast<float>(InterpFloat(progress, s_startProviderDesc.ReflectionsDelay, s_targetProviderDesc.ReflectionsDelay));
    s_currentProviderDesc.ReverbDelay = static_cast<float>(InterpFloat(progress, s_startProviderDesc.ReverbDelay, s_targetProviderDesc.ReverbDelay));
    s_currentProviderDesc.RoomRolloffFactor = static_cast<float>(InterpFloat(progress, s_startProviderDesc.RoomRolloffFactor, s_targetProviderDesc.RoomRolloffFactor));
    s_currentProviderDesc.AirAbsorptionHF = static_cast<float>(InterpFloat(progress, s_startProviderDesc.AirAbsorptionHF, s_targetProviderDesc.AirAbsorptionHF));
    s_currentProviderDesc.EchoTime = static_cast<float>(InterpFloat(progress, s_startProviderDesc.EchoTime, s_targetProviderDesc.EchoTime));
    s_currentProviderDesc.EchoDepth = static_cast<float>(InterpFloat(progress, s_startProviderDesc.EchoDepth, s_targetProviderDesc.EchoDepth));
    s_currentProviderDesc.ModulationTime = static_cast<float>(InterpFloat(progress, s_startProviderDesc.ModulationTime, s_targetProviderDesc.ModulationTime));
    s_currentProviderDesc.ModulationDepth = static_cast<float>(InterpFloat(progress, s_startProviderDesc.ModulationDepth, s_targetProviderDesc.ModulationDepth));
    s_currentProviderDesc.HFReference = static_cast<float>(InterpFloat(progress, s_startProviderDesc.HFReference, s_targetProviderDesc.HFReference));
    s_currentProviderDesc.LFReference = static_cast<float>(InterpFloat(progress, s_startProviderDesc.LFReference, s_targetProviderDesc.LFReference));
    s_currentProviderDesc.Room = static_cast<int>(InterpInt(s_startProviderDesc.Room, s_targetProviderDesc.Room, progress));
    s_currentProviderDesc.RoomHF = static_cast<int>(InterpInt(s_startProviderDesc.RoomHF, s_targetProviderDesc.RoomHF, progress));
    s_currentProviderDesc.RoomLF = static_cast<int>(InterpInt(s_startProviderDesc.RoomLF, s_targetProviderDesc.RoomLF, progress));
    s_currentProviderDesc.Reflections = static_cast<int>(InterpInt(s_startProviderDesc.Reflections, s_targetProviderDesc.Reflections, progress));
    s_currentProviderDesc.Reverb = static_cast<int>(InterpInt(s_startProviderDesc.Reverb, s_targetProviderDesc.Reverb, progress));
    ApplyReverb();
}

void StartWorldIdleHandler() {
    if (!s_idleRunning) {
        EventRegisterEx(EVENT_ID_IDLE, WorldIdleHandler, nullptr, 0.0F);
        s_idleRunning = true;
    }
}

void StartProviderPrefFade(const SoundProviderDesc& target, unsigned duration) {
    s_startProviderDesc = s_currentProviderDesc;
    s_targetProviderDesc = target;
    s_providerPrefFadeStartTime = OsGetAsyncTimeMs();
    s_providerPrefFadeDuration = duration;
    s_providerPrefFadeEndTime = s_providerPrefFadeStartTime + duration;
    StartWorldIdleHandler();
}

void StopProviderPrefFade() {
    StopWorldIdleHandler();
    ApplyReverb();
}

void SndInterfaceSetProviderPrefs(int normalId, int underwaterId, unsigned transitionDuration) {
    s_flags = 0;
    const auto* normal = g_soundProviderPreferencesDB.Lookup(normalId);
    const auto* underwater = g_soundProviderPreferencesDB.Lookup(underwaterId);
    if (normal) { SaveDesc(s_desc, *normal); s_flags |= 1; }
    if (underwater) { SaveDesc(s_descUnderwater, *underwater); s_flags |= 2; }
    if ((g_underWater ? (s_flags & 2) : (s_flags & 1)) && (s_flags & 3) == 3) {
        const auto& target = g_underWater ? s_descUnderwater : s_desc;
        if (transitionDuration) StartProviderPrefFade(target, transitionDuration);
        else { s_currentProviderDesc = target; StopProviderPrefFade(); }
    } else {
        s_currentProviderDesc = DefaultDesc();
        StopProviderPrefFade();
    }
}

void SndInterfaceSetProviderPrefs(const SoundProviderDesc& normal, const SoundProviderDesc& underwater) {
    s_desc = normal;
    s_descUnderwater = underwater;
    s_currentProviderDesc = g_underWater ? s_descUnderwater : s_desc;
    StopProviderPrefFade();
}

void SndInterfaceClearProviderPrefs(int indoor) {
    const auto* record = indoor ? ClientDBGetDefaultIndoorProviderPrefs() : ClientDBGetDefaultOutdoorProviderPrefs();
    if (record) {
        SaveDesc(s_currentProviderDesc, *record);
        StopProviderPrefFade();
    } else {
        s_currentProviderDesc = DefaultDesc();
        StopProviderPrefFade();
    }
}

void SndSetRoomType(EVENTID, int room) {
    s_startProviderDesc.Room = room;
    s_currentProviderDesc.Room = room;
    s_targetProviderDesc.Room = room;
    if (!s_idleRunning) StopProviderPrefFade();
}

void ProviderPrefInitialize(const char*) {
    s_flags = 0;
    ConsoleCommandRegister(nullptr, "environment", CLIENT_COMMAND_CATEGORY_DEBUG, EnvironmentHandler);
    ConsoleCommandRegister(nullptr, "environmentlist", CLIENT_COMMAND_CATEGORY_DEBUG, EnvironmentListHandlerCommand);
}

void ProviderPrefShutdown(const char*) {
    ConsoleCommandUnregister("environment");
    ConsoleCommandUnregister("environmentlist");
    StopProviderPrefFade();
}

int __fastcall EnvironmentHandler(const char*, const char* value) {
    if (value && *value) {
        const int id = SStrToInt(value);
        SndInterfaceSetProviderPrefs(id, id, 0);
    }
    return 1;
}

int EnvironmentListHandler() {
    for (const auto& record : g_soundProviderPreferencesDB.m_records)
        ConsolePrintf("[%d]  %s", record.m_ID, record.m_Description ? record.m_Description : "");
    return 1;
}

void SndInterfaceProviderPrefsUnderwaterChanged() {
    if (g_underWater && (s_flags & 2)) s_currentProviderDesc = s_descUnderwater;
    else if (!g_underWater && (s_flags & 1)) s_currentProviderDesc = s_desc;
    else s_currentProviderDesc = DefaultDesc();
    StopProviderPrefFade();
}

int _E2_72() { return _E1_72(); }
int _E1_72() { INFINITY_72 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_72() { _E4_72(); return 0; } void _E4_72() { TWO_PI_72 = 6.2831854F; }
int _E8_72() { _E7_72(); return 0; } void _E7_72() { OO_TWO_PI_72 = 1.0F / TWO_PI_72; }
#define PREF_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
PREF_CONST_PAIR(_E21_7, _E20_11, MAX_DUEL_DISTANCE_SQUARED_4, 100.0F)
PREF_CONST_PAIR(_E24_10, _E23_14, MAX_TRADE_DISTANCE_SQUARED_4, 123.45679F)
PREF_CONST_PAIR(_E27_7, _E26_10, MAX_SHOP_DISTANCE_SQUARED_4, 30.864195F)
PREF_CONST_PAIR(_E30_5, _E29_9, MAX_INSPECT_DISTANCE_SQUARED_4, 30.864195F)
PREF_CONST_PAIR(_E33_3, _E32_7, MAX_LOOT_DISTANCE_SQUARED_4, 25.0F)
PREF_CONST_PAIR(_E36_2, _E35_8, MAX_BIND_DISTANCE_SQUARED_4, 100.0F)
PREF_CONST_PAIR(_E39_4, _E38_8, MAX_SITCHAIR_DISTANCE_SQUARED_4, 0.25F)
PREF_CONST_PAIR(_E42_3, _E41_6, MAX_SITCHAIRUSE_DISTANCE_SQUARED_4, 9.0F)
PREF_CONST_PAIR(_E45_2, _E44_8, MAX_SHARE_DISTANCE_SQUARED_4, 10000.0F)
PREF_CONST_PAIR(_E48_7, _E47_10, SECONDS_PER_HEARTBEAT_4, 5.0F)
PREF_CONST_PAIR(_E51_1, _E50_11, SPELL_FOCUS_DISTANCE_SQUARED_4, 2500.0F)
PREF_CONST_PAIR(_E54_6, _E53_9, MAX_OBJ_INTEREST_RADIUS_SQUARED_4, 10000.0F)
#undef PREF_CONST_PAIR
