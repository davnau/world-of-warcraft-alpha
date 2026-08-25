#pragma once

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class CVar;
class Sound;
struct lua_State;

struct FILENAMEENTRY {
    std::string fileName;
    unsigned accumulatedFreq = 0;
};

struct SOUNDDEFINITION {
    std::vector<FILENAMEENTRY> m_fileNames;
    float m_pitch = 1.0F;
    float m_pitchVariation = 0.0F;
    unsigned m_priority = 0;
    unsigned m_channel = 0;
    unsigned m_flags = 0;
    float m_volume = 1.0F;
    float m_minDistance = 0.0F;
    float m_maxDistance = 100.0F;
    float m_distanceCutoffSquared = 10000.0F;
    unsigned m_totalFrequency = 0;
    unsigned m_lastPlayed = 0;
    unsigned m_loopCounter = 0;
    unsigned m_primeStepIndex = 0;
    bool m_equalFreqs = false;
    int m_reverbPrefIndex = -1;

    SOUNDDEFINITION();
    SOUNDDEFINITION(const SOUNDDEFINITION& rhs);
    SOUNDDEFINITION& operator=(const SOUNDDEFINITION& rhs);
    ~SOUNDDEFINITION();
    FILENAMEENTRY* GetRandomFileName(int index);
    void Clear();
    double GetVolume(float volumeScale, unsigned char neverVary) const;
    void SetFrequencyAndVolume(Sound* sound, float volumeScaler, unsigned char neverVaryVolume);
    void Set3DParams(Sound* sound, const NTempest::C3Vector* position);
    int GetOsFlags() const;
};

struct WEAPONSOUNDS {
    unsigned soundList[2]{};
    WEAPONSOUNDS() = default;
    WEAPONSOUNDS(const WEAPONSOUNDS&) = default;
    WEAPONSOUNDS& operator=(const WEAPONSOUNDS& rhs);
    ~WEAPONSOUNDS();
    void Clear();
};

struct IMPACTSOUNDDESC {
    WEAPONSOUNDS materialSounds[4];
    ~IMPACTSOUNDDESC();
};

enum PARRYMATERIALS : unsigned { PARRY_MATERIAL_WOOD = 0, PARRY_MATERIAL_METAL = 1 };
enum VUISOUNDTYPE : unsigned { VUISOUNDTYPE_NORMAL = 0, VUISOUNDTYPE_PISSED = 1 };
enum VOCALUISOUNDS : unsigned { NUM_VOCALUISOUNDS = 66 };

struct FSOUND_REVERB_CHANNELPROPERTIES {
    float Direct = 1.0F;
    float DirectHF = 1.0F;
    float Room = 0.0F;
    float RoomHF = 1.0F;
    float Obstruction = 0.0F;
    float ObstructionLFRatio = 0.0F;
    float Occlusion = 0.0F;
    float OcclusionLFRatio = 0.0F;
};

void ISndInterfaceInitialize();
int ReadFiles();
int InitializeInterfaceSounds();
int InitializeUISounds();
int InitializeSheatheSounds();
int InitializeUnitCombatSounds();
void GenerateWeaponSwingCombatSounds();
void InitializeWeaponImpactCombatSounds();
void ParseWeaponImpactArmorField(void* record);
void ISndInterfaceShutdown();
SOUNDDEFINITION* __fastcall ISndInterfaceGetSndEntry(int soundID);
FSOUND_REVERB_CHANNELPROPERTIES* GetReverbType(int index, int preference);
unsigned char __fastcall InitializePrefTable(int index);

extern bool g_underWater;
extern unsigned g_sndInterfaceFlags;

void SndInterfaceInitialize();
int FootstepTerrainInitialize();
long double __fastcall ObstructionCallback(const NTempest::C3Vector* source, const NTempest::C3Vector* target);
CVar* RegisterCVars(const char* previous = nullptr);
unsigned char __stdcall MusicVolumeHandler(const char* newValue, void* userArg);
unsigned char __stdcall SoundVolumeHandler(const char* newValue, void* userArg);
unsigned char __stdcall MasterVolumeHandler(const char* newValue, void* userArg);
unsigned char __stdcall EnableMusicHandler(const char* newValue, void* userArg);
unsigned char __fastcall EnableSoundHandler(CVar* cvar, const char* oldValue, const char* newValue, void* userArg);
unsigned char __fastcall SoundGetParamValueInt(const char* parameter, int* value);
unsigned char __fastcall SoundGetParamValueFloat(const char* parameter, float* value);
unsigned char __fastcall SoundGetParamValueString(const char* parameter, char** value);
void SndInterfaceDestroy();
void SndInterfaceWorldInitialize();
void SoundInterfaceRegisterWorldCVars(const char* previous = nullptr);
int __fastcall WorldIdle(const void* eventData, void* parameter);
void SndInterfaceWorldDestroy(EVENTID eventId = EVENT_ID_IDLE);

void SndInterfacePlayParrySound(const void* defendingItem, const std::uint8_t* objectType,
                                int criticalHit, const NTempest::C3Vector* position);
void DetermineWeaponTypeAndMaterial(unsigned* weaponType, unsigned* subclass, PARRYMATERIALS* material);
void SndInterfacePlayHitSound(unsigned* defendingItemType, int soundType, int criticalHit,
                              const NTempest::C3Vector* position);
void SndInterfacePlayDeflectedSound(const NTempest::C3Vector* position);
void SndInterfacePlayWeaponSwooshSound(int criticalHit, int offhand,
                                       const NTempest::C3Vector* position, int missed);
void SndInterfacePlaySpellSound(const NTempest::C3Vector* position, void* unit);
void SndInterfacePlayItemSound(int itemDisplayID, int soundType);
void SndInterfacePlayInterfaceSound(const char* soundType);
void SndInterfaceInitializeVocalUISounds(unsigned sex, unsigned voice);
void SndInterfacePlayVocalUISound(const char* soundType, VOCALUISOUNDS sound);
bool InternalPlaySound(const char* soundID, int soundIndex, int forceIndex);
void SndInterfacePlayFootstepSound(const NTempest::C3Vector* position, unsigned terrainID, int splashing);
unsigned GetFootstepTerrain(unsigned terrainID, int terrainSoundID, int splashing);
void SndInterfacePlayFoleySound(int position);
void SndInterfacePlaySheatheSound(const std::uint8_t* sheathing, int unsheathing,
                                  const NTempest::C3Vector* position);
void SndInterfacePlayImmuneSound(const NTempest::C3Vector* position);
void SndInterfacePlayAbsorbedSound(const NTempest::C3Vector* position);
unsigned SndInterfaceGetSoundVariations(int soundID);
int Script_PlaySound(lua_State* state);
int Script_PlayMusic(lua_State* state);
void SoundRegisterScriptFunctions();
void SoundUnregisterScriptFunctions();
void SndInterfaceSetUnderwater(char underwater);
unsigned char SoundInterfaceIsSoundLooping(unsigned char* soundID, bool* looping);
bool SndInterfacePlaySound(int soundID, int forceIndex);
unsigned char SndInterfacePlaySound(const NTempest::C3Vector* soundID,
                                    const NTempest::C3Vector* position, int forceIndex, float volumeScaler);
unsigned char SndInterfacePlaySplashSound(const NTempest::C3Vector* soundID,
                                          const NTempest::C3Vector* position);
unsigned char InternalPlaySound_0(const char* soundID, int soundIndex,
                                  const NTempest::C3Vector* position, int forceIndex, float volumeScaler);
Sound* SndInterfacePlayLoopedSound(int soundID, unsigned loopCount);
Sound* SndInterfacePlayLoopedSound(const NTempest::C3Vector* soundID,
                                   const NTempest::C3Vector* position, unsigned loopCount);
void SndInterfacePlaySpellFizzleSound(int caster, void* unit);
void SndInterfaceAssociateSoundWithObject(Sound* sound, std::uint64_t objectHandle);
Sound* SndInterfaceCreateSound(int soundID, int forceIndex, float fadeInRate, unsigned char doNotKeepAlive);
void _E143();
unsigned char SndInterfacePlaySound(Sound* sound, float fadeInRate);
void SndInterfaceSetPositionCallback();
int SoundPositionCallback(NTempest::C3Vector* position, std::uint64_t handle);
void SndInterfaceClearPositionCallback();
void SndSetObstructionCallback();

int _E2_71(); int _E1_71(); int _E5_71(); void _E4_71(); int _E8_71(); void _E7_71();
int _E21_6(); void _E20_10(); int _E24_9(); void _E23_13(); int _E27_6(); void _E26_9();
int _E30_4(); void _E29_8(); int _E33_2(); void _E32_6(); int _E36_1(); void _E35_7();
int _E39_3(); void _E38_7(); int _E42_2(); void _E41_5(); int _E45_1(); void _E44_7();
int _E48_6(); void _E47_9(); int _E51_0(); void _E50_10(); int _E54_5(); void _E53_8();
int _E60_5(); void _E59_3(); int _E63_2(); void _E62_2(); int _E66(); void _E65_1();
int _E69_1(); void _E68_1(); int _E72_1(); void _E71_0(); int _E140(); void _E137();
int _E139(); void _E138();
