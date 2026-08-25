#include "WoW/Source/SoundInterface/soundinterface.h"

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Os/W32/ossound.h"
#include "WoW/Source/Console/consolevar.h"
#include "WoW/Source/DB/DBClient/dbclient.h"
#include "WoW/Source/SoundInterface/soundinterfacedoodad.h"
#include "WoW/Source/SoundInterface/soundinterfacegluemusic.h"
#include "WoW/Source/SoundInterface/soundinterfacemidi.h"
#include "WoW/Source/SoundInterface/soundinterfaceproviderprefs.h"
#include "WoW/Source/SoundInterface/soundinterfacevocal.h"
#include "WoW/Source/SoundInterface/soundinterfacewater.h"
#include "WoW/Source/SoundInterface/soundinterfacezoneintro.h"
#include "WoW/Source/SoundInterface/soundinterfacezonemusic.h"
#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>
#include <string>

extern "C" {
#include "lauxlib.h"
}

float INFINITY_71 = 0.0F;
float TWO_PI_71 = 0.0F;
float OO_TWO_PI_71 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_3 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_3 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_3 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_3 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_3 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_3 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_3 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_3 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_3 = 0.0F;
float SECONDS_PER_HEARTBEAT_3 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_3 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_3 = 0.0F;
float MIXRATE = 44100.0F;
unsigned g_sndInterfaceFlags = 0;

namespace {
bool s_initialized = false;
bool s_scriptsRegistered = false;
unsigned s_elapsed = 0;
unsigned s_footstepRequest = 0;
unsigned s_footstepAccept = 0;
std::string s_interfaceSound;
std::string s_musicSound;
std::array<std::array<unsigned, 3>, NUM_VOCALUISOUNDS> s_vocalUISounds{};
VUISOUNDTYPE s_currentVocalUISoundType = VUISOUNDTYPE_NORMAL;
unsigned s_lastPlayedVocalUISound = NUM_VOCALUISOUNDS;
unsigned s_vocalUISoundPlayCount = 0;
}

WEAPONSOUNDS& WEAPONSOUNDS::operator=(const WEAPONSOUNDS& rhs) {
    if (this != &rhs) { soundList[0] = rhs.soundList[0]; soundList[1] = rhs.soundList[1]; }
    return *this;
}
WEAPONSOUNDS::~WEAPONSOUNDS() { Clear(); }
void WEAPONSOUNDS::Clear() { soundList[0] = soundList[1] = 0; }
IMPACTSOUNDDESC::~IMPACTSOUNDDESC() = default;

double SOUNDDEFINITION::GetVolume(float volumeScale, unsigned char) const {
    return std::clamp(static_cast<double>(m_volume) * volumeScale, 0.0, 1.0);
}
void SOUNDDEFINITION::SetFrequencyAndVolume(Sound* sound, float volumeScaler, unsigned char neverVary) {
    if (!sound) return;
    sound->SetVolume(static_cast<float>(GetVolume(volumeScaler, neverVary)));
    if (m_flags & 0x400U) sound->SetFrequency(static_cast<unsigned>(MIXRATE));
}
void SOUNDDEFINITION::Set3DParams(Sound* sound, const NTempest::C3Vector* position) {
    if (!sound) return;
    if (position) sound->SetPosition(*position);
    sound->SetDistances(m_minDistance, m_maxDistance);
    sound->SetCutoffDistanceSquared(m_distanceCutoffSquared);
}
int SOUNDDEFINITION::GetOsFlags() const { return (m_flags & 0x20U) ? 1 : 0; }

static Sound* StartSound(SOUNDDEFINITION* definition, const NTempest::C3Vector* position,
                         bool looped, float volume) {
    if (!definition) return nullptr;
    Sound* sound = Sound::Alloc();
    if (!sound) return nullptr;
    definition->SetFrequencyAndVolume(sound, volume, 0);
    const bool playing = position ? (looped ? sound->Play3DLooped() : sound->Play3D())
                                  : (looped ? sound->Play2DLooped() : sound->Play2D());
    if (!playing) { Sound::KillSound(sound); return nullptr; }
    definition->Set3DParams(sound, position);
    return sound;
}

void SndInterfaceInitialize() {
    RegisterCVars(nullptr);
    s_initialized = Sound::Initialize();
    if (s_initialized) {
        InitializeGlueMusic();
        ProviderPrefInitialize();
        SndSetObstructionCallback();
    }
}

int FootstepTerrainInitialize() { return 1; }

long double __fastcall ObstructionCallback(const NTempest::C3Vector* source,
                                            const NTempest::C3Vector* target) {
    if (!source || !target) return 0.0L;
    const float distance = (*target - *source).SquaredMag();
    return distance > 10000.0F ? 0.75L : std::sqrt(distance) * 0.0075L;
}

static unsigned char __fastcall MusicVolumeCVarHandler(CVar*, const char*, const char*, void*);
static unsigned char __fastcall SoundVolumeCVarHandler(CVar*, const char*, const char*, void*);
static unsigned char __fastcall MasterVolumeCVarHandler(CVar*, const char*, const char*, void*);
static unsigned char __fastcall EnableMusicCVarHandler(CVar*, const char*, const char*, void*);

CVar* RegisterCVars(const char*) {
    CVar::Register("musicvolume", "musicvolume", 0, "0.25", MusicVolumeCVarHandler, 7, 0, nullptr);
    CVar::Register("soundvolume", "soundvolume", 0, "1.0", SoundVolumeCVarHandler, 7, 0, nullptr);
    CVar::Register("mastervolume", "mastervolume", 0, "1.0", MasterVolumeCVarHandler, 7, 0, nullptr);
    CVar::Register("enablemusic", "enablemusic", 0, "1", EnableMusicCVarHandler, 7, 0, nullptr);
    return CVar::Register("enablesound", "enablesound", 0, "1", EnableSoundHandler, 7, 0, nullptr);
}

static unsigned char __fastcall MusicVolumeCVarHandler(CVar*, const char*, const char* value, void*) { return MusicVolumeHandler(value, nullptr); }
static unsigned char __fastcall SoundVolumeCVarHandler(CVar*, const char*, const char* value, void*) { return SoundVolumeHandler(value, nullptr); }
static unsigned char __fastcall MasterVolumeCVarHandler(CVar*, const char*, const char* value, void*) { return MasterVolumeHandler(value, nullptr); }
static unsigned char __fastcall EnableMusicCVarHandler(CVar*, const char*, const char* value, void*) { return EnableMusicHandler(value, nullptr); }

unsigned char __stdcall MusicVolumeHandler(const char* value, void*) {
    Sound::SetMusicVolume(value ? std::strtof(value, nullptr) : 0.0F); return 1;
}
unsigned char __stdcall SoundVolumeHandler(const char* value, void*) {
    Sound::SetSoundVolume(value ? std::strtof(value, nullptr) : 0.0F); return 1;
}
unsigned char __stdcall MasterVolumeHandler(const char* value, void*) {
    Sound::SetMasterVolume(value ? std::strtof(value, nullptr) : 0.0F); return 1;
}
unsigned char __stdcall EnableMusicHandler(const char* value, void*) {
    const bool enabled = value && std::strtol(value, nullptr, 0) != 0;
    SndInterfacePauseZoneMusic(!enabled);
    if (!enabled) SndInterfaceStopGlueMusic(0.0F);
    return 1;
}
unsigned char __fastcall EnableSoundHandler(CVar*, const char*, const char* value, void*) {
    Sound::MuteSFX(!(value && std::strtol(value, nullptr, 0) != 0)); return 1;
}

unsigned char __fastcall SoundGetParamValueInt(const char* parameter, int* value) {
    const auto* cvar = CVar::Lookup(parameter); if (!cvar || !value) return 0; *value = cvar->m_intValue; return 1;
}
unsigned char __fastcall SoundGetParamValueFloat(const char* parameter, float* value) {
    const auto* cvar = CVar::Lookup(parameter); if (!cvar || !value) return 0; *value = cvar->m_floatValue; return 1;
}
unsigned char __fastcall SoundGetParamValueString(const char* parameter, char** value) {
    auto* cvar = CVar::Lookup(parameter); if (!cvar || !value) return 0; *value = cvar->m_stringValue; return 1;
}

void SndInterfaceDestroy() {
    SndSetObstructionCallback();
    ProviderPrefShutdown();
    ShutdownGlueMusic();
    Sound::Shutdown();
    s_initialized = false;
}

void SndInterfaceWorldInitialize() {
    SoundInterfaceRegisterWorldCVars();
    InitializeZoneMusic();
    SoundInterfaceInitializeWorldMIDI();
    InitializeWaterAmbiences();
    SoundInterfaceDoodadInitialize();
    EventRegisterEx(EVENT_ID_IDLE, WorldIdle, nullptr, 0.0F);
    SndInterfaceZoneIntroInitialize();
}

void SoundInterfaceRegisterWorldCVars(const char*) {
    CVar::Register("enablegroupsounds", "enablegroupsounds", 0, "1", nullptr, 7, 0, nullptr);
    CVar::Register("enableerrorsounds", "enableerrorsounds", 0, "1", nullptr, 7, 0, nullptr);
    SoundInterfaceInitializeWorldMIDICVars();
}

int __fastcall WorldIdle(const void* eventData, void*) {
    const auto* idle = static_cast<const EVENT_DATA_IDLE*>(eventData);
    s_elapsed += idle ? static_cast<unsigned>(idle->elapsedSec * 1000.0F) : 0;
    if (s_elapsed >= 1000) { s_elapsed -= 1000; SndInterfaceZoneIntroIdler(); }
    return 1;
}

void SndInterfaceWorldDestroy(EVENTID) {
    EventUnregister(EVENT_ID_IDLE, WorldIdle);
    ShutdownZoneMusic();
    ShutdownWaterAmbiences();
    SoundInterfaceShutdownWorldMIDI();
    SoundInterfaceDoodadDestroy();
    SndInterfaceZoneIntroDestroy();
}

void DetermineWeaponTypeAndMaterial(unsigned* weaponType, unsigned* subclass, PARRYMATERIALS* material) {
    if (!subclass || !material) return;
    if (!weaponType) { *subclass = 0; *material = PARRY_MATERIAL_WOOD; return; }
    *subclass = weaponType[1];
    *material = PARRY_MATERIAL_WOOD;
}
void SndInterfacePlayParrySound(const void*, const std::uint8_t*, int, const NTempest::C3Vector* position) {
    SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(0xCBF), position, -1, 1.0F);
}
void SndInterfacePlayHitSound(unsigned*, int, int, const NTempest::C3Vector* position) {
    SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(0xCBF), position, -1, 1.0F);
}
void SndInterfacePlayDeflectedSound(const NTempest::C3Vector* position) {
    SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(0xCBF), position, -1, 1.0F);
}
void SndInterfacePlayWeaponSwooshSound(int, int, const NTempest::C3Vector* position, int) {
    SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(0xCBF), position, -1, 1.0F);
}
void SndInterfacePlaySpellSound(const NTempest::C3Vector* position, void*) {
    SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(0xCBF), position, -1, 1.0F);
}
void SndInterfacePlayItemSound(int itemDisplayID, int) {
    InternalPlaySound(nullptr, itemDisplayID, -1);
}
void SndInterfacePlayInterfaceSound(const char* soundType) {
    if (soundType) { s_interfaceSound = soundType; Sound* sound = Sound::Alloc(); if (sound) sound->Play2D(); }
}
void SndInterfaceInitializeVocalUISounds(unsigned, unsigned) { s_vocalUISounds = {}; s_lastPlayedVocalUISound = NUM_VOCALUISOUNDS; s_vocalUISoundPlayCount = 0; }
void SndInterfacePlayVocalUISound(const char* soundType, VOCALUISOUNDS sound) {
    const auto* cvar = CVar::Lookup(soundType); if (!cvar || cvar->m_intValue == 0 || sound >= NUM_VOCALUISOUNDS) return;
    ++s_vocalUISoundPlayCount; s_lastPlayedVocalUISound = sound; InternalPlaySound(nullptr, static_cast<int>(sound), -1);
}

bool InternalPlaySound(const char* soundID, int soundIndex, int forceIndex) {
    auto* definition = ISndInterfaceGetSndEntry(soundIndex); if (!definition) return false;
    auto* filename = definition->GetRandomFileName(forceIndex); if (!filename || filename->fileName.empty()) return false;
    auto* sound = StartSound(definition, nullptr, false, 1.0F); if (!sound) return false;
    if (soundID) s_interfaceSound = soundID; return true;
}
void SndInterfacePlayFootstepSound(const NTempest::C3Vector*, unsigned terrainID, int splashing) {
    ++s_footstepRequest; const unsigned soundID = GetFootstepTerrain(terrainID, terrainID, splashing); if (soundID) { ++s_footstepAccept; InternalPlaySound(nullptr, soundID, -1); }
}
unsigned GetFootstepTerrain(unsigned terrainID, int, int) { return terrainID; }
void SndInterfacePlayFoleySound(int position) { InternalPlaySound(nullptr, position, -1); }
void SndInterfacePlaySheatheSound(const std::uint8_t* sheathing, int, const NTempest::C3Vector* position) { if (sheathing) SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(sheathing[2]), position, -1, 1.0F); }
void SndInterfacePlayImmuneSound(const NTempest::C3Vector* position) { SndInterfacePlaySound(reinterpret_cast<const NTempest::C3Vector*>(0xD06), position, -1, 1.0F); }
void SndInterfacePlayAbsorbedSound(const NTempest::C3Vector* position) { SndInterfacePlayImmuneSound(position); }
unsigned SndInterfaceGetSoundVariations(int soundID) { auto* d = ISndInterfaceGetSndEntry(soundID); return d ? static_cast<unsigned>(d->m_fileNames.size()) : 0; }
int Script_PlaySound(lua_State* state) {
    if (!lua_isstring(state, 1))
        return luaL_error(state, "Usage: PlaySound(\"soundType\")");
    SndInterfacePlayInterfaceSound(lua_tostring(state, 1));
    return 0;
}
int Script_PlayMusic(lua_State* state) {
    if (!lua_isstring(state, 1))
        return luaL_error(state, "Usage: PlayMusic(\"filename\")");
    s_musicSound = lua_tostring(state, 1);
    Sound* sound = Sound::Alloc();
    if (sound && !sound->Play2DLooped()) Sound::KillSound(sound);
    return 0;
}
void SoundRegisterScriptFunctions() {
    if (s_scriptsRegistered) return;
    FrameScript_RegisterFunction("PlaySound", Script_PlaySound);
    FrameScript_RegisterFunction("PlayMusic", Script_PlayMusic);
    s_scriptsRegistered = true;
}
void SoundUnregisterScriptFunctions() {
    if (!s_scriptsRegistered) return;
    FrameScript_UnregisterFunction("PlaySound");
    FrameScript_UnregisterFunction("PlayMusic");
    s_scriptsRegistered = false;
}

void SndInterfaceSetUnderwater(char underwater) {
    if ((g_sndInterfaceFlags & 1U) && g_underWater == (underwater != 0)) return;
    g_sndInterfaceFlags |= 1U; g_underWater = underwater != 0;
    SndInterfaceMIDIUnderwaterChanged(); WaterAmbiencesUnderwaterChanged(); SndInterfaceProviderPrefsUnderwaterChanged();
}
unsigned char SoundInterfaceIsSoundLooping(unsigned char* soundID, bool* looping) {
    auto* d = ISndInterfaceGetSndEntry(static_cast<int>(reinterpret_cast<std::uintptr_t>(soundID))); if (!d || !looping) return 0; *looping = (d->m_flags & 0x200U) != 0; return 1;
}
bool SndInterfacePlaySound(int soundID, int forceIndex) { return InternalPlaySound(nullptr, soundID, forceIndex); }
unsigned char SndInterfacePlaySound(const NTempest::C3Vector* soundID, const NTempest::C3Vector* position, int forceIndex, float volumeScaler) {
    auto* d = ISndInterfaceGetSndEntry(static_cast<int>(reinterpret_cast<std::uintptr_t>(soundID))); if (!d) return 0; auto* f = d->GetRandomFileName(forceIndex); if (!f || f->fileName.empty()) return 0; return StartSound(d, position, false, volumeScaler) ? 1 : 0;
}
unsigned char SndInterfacePlaySplashSound(const NTempest::C3Vector* soundID, const NTempest::C3Vector* position) { return SndInterfacePlaySound(soundID, position, -1, 1.0F); }
unsigned char InternalPlaySound_0(const char*, int soundIndex, const NTempest::C3Vector* position, int forceIndex, float volumeScaler) {
    auto* d = ISndInterfaceGetSndEntry(soundIndex); if (!d) return 0; return StartSound(d, position, false, volumeScaler) ? 1 : 0;
}
Sound* SndInterfacePlayLoopedSound(int soundID, unsigned) { auto* d = ISndInterfaceGetSndEntry(soundID); return StartSound(d, nullptr, true, 1.0F); }
Sound* SndInterfacePlayLoopedSound(const NTempest::C3Vector* soundID, const NTempest::C3Vector* position, unsigned) { auto* d = ISndInterfaceGetSndEntry(static_cast<int>(reinterpret_cast<std::uintptr_t>(soundID))); return StartSound(d, position, true, 1.0F); }
void SndInterfacePlaySpellFizzleSound(int caster, void*) { InternalPlaySound(nullptr, caster, -1); }
void SndInterfaceAssociateSoundWithObject(Sound* sound, std::uint64_t) { if (sound) sound->Set3DUpdateHandle(0); }
Sound* SndInterfaceCreateSound(int soundID, int forceIndex, float fadeInRate, unsigned char) { auto* d = ISndInterfaceGetSndEntry(soundID); auto* sound = StartSound(d, nullptr, true, 1.0F); if (sound) sound->SetFadeIn(static_cast<unsigned>(std::max(0.0F, fadeInRate))); if (sound && !sound->IsPlaying()) { Sound::KillSound(sound); return nullptr; } return sound; }
void _E143() {}
unsigned char SndInterfacePlaySound(Sound* sound, float fadeInRate) { if (!sound) return 0; sound->SetFadeIn(static_cast<unsigned>(std::max(0.0F, fadeInRate))); return sound->Play() ? 1 : 0; }
void SndInterfaceSetPositionCallback() {}
int SoundPositionCallback(NTempest::C3Vector*, std::uint64_t) { return 0; }
void SndInterfaceClearPositionCallback() {}
void SndSetObstructionCallback() {}

int _E2_71() { return _E1_71(); } int _E1_71() { INFINITY_71 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_71() { _E4_71(); return 0; } void _E4_71() { TWO_PI_71 = 6.2831854F; }
int _E8_71() { _E7_71(); return 0; } void _E7_71() { OO_TWO_PI_71 = 1.0F / TWO_PI_71; }
#define CORE_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
CORE_CONST_PAIR(_E21_6, _E20_10, MAX_DUEL_DISTANCE_SQUARED_3, 100.0F)
CORE_CONST_PAIR(_E24_9, _E23_13, MAX_TRADE_DISTANCE_SQUARED_3, 123.45679F)
CORE_CONST_PAIR(_E27_6, _E26_9, MAX_SHOP_DISTANCE_SQUARED_3, 30.864195F)
CORE_CONST_PAIR(_E30_4, _E29_8, MAX_INSPECT_DISTANCE_SQUARED_3, 30.864195F)
CORE_CONST_PAIR(_E33_2, _E32_6, MAX_LOOT_DISTANCE_SQUARED_3, 25.0F)
CORE_CONST_PAIR(_E36_1, _E35_7, MAX_BIND_DISTANCE_SQUARED_3, 100.0F)
CORE_CONST_PAIR(_E39_3, _E38_7, MAX_SITCHAIR_DISTANCE_SQUARED_3, 0.25F)
CORE_CONST_PAIR(_E42_2, _E41_5, MAX_SITCHAIRUSE_DISTANCE_SQUARED_3, 9.0F)
CORE_CONST_PAIR(_E45_1, _E44_7, MAX_SHARE_DISTANCE_SQUARED_3, 10000.0F)
CORE_CONST_PAIR(_E48_6, _E47_9, SECONDS_PER_HEARTBEAT_3, 5.0F)
CORE_CONST_PAIR(_E51_0, _E50_10, SPELL_FOCUS_DISTANCE_SQUARED_3, 2500.0F)
CORE_CONST_PAIR(_E54_5, _E53_8, MAX_OBJ_INTEREST_RADIUS_SQUARED_3, 10000.0F)
CORE_CONST_PAIR(_E60_5, _E59_3, MIXRATE, 44100.0F)
#undef CORE_CONST_PAIR
int _E63_2() { _E62_2(); return 0; } void _E62_2() {}
int _E66() { _E65_1(); return 0; } void _E65_1() {}
int _E69_1() { _E68_1(); return 0; } void _E68_1() {}
int _E72_1() { _E71_0(); return 0; } void _E71_0() {}
int _E140() { _E137(); return _E139(); } void _E137() {} int _E139() { return 0; }
void _E138() {}
