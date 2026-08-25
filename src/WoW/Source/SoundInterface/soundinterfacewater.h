#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

class Sound;
class SoundEntriesRec;

struct LIQUIDINFO {
    unsigned m_subTypes[3]{};
    NTempest::C3Vector m_positionOffset[3]{};
    const SoundEntriesRec* m_soundRecords[3]{};
    const SoundEntriesRec* m_currentRecord = nullptr;
    Sound* m_sound = nullptr;
    unsigned m_currentPlayingSound = 0;

    void StopSound(int immediate);
    int Update(const NTempest::C3Vector* listenerPos);
    void UpdateVolume();
    void Tick();
    void StartSound(unsigned subType, const NTempest::C3Vector* listenerPos);
};

extern LIQUIDINFO s_liquidInfo[4];
extern int s_paused;
extern float s_volume_0;

void InitializeWaterAmbiences();
unsigned char __stdcall ToggleCallback(const char* newValue, void* arg);
void ClearAllSounds(int immediate);
int __fastcall WaterHandler(const void* param, void* unused);
void HandleWaterAmbiences();
void ShutdownWaterAmbiences();
void WaterAmbiencesUnderwaterChanged();
void SndInterfaceWaterSetPaused(char paused);
void SndInterfaceWaterUpdateVolume(float volume);

int _E2_79(); int _E1_79(); int _E5_79(); void _E4_79(); int _E8_79(); void _E7_79();
int _E21_14(); void _E20_18(); int _E24_17(); void _E23_21(); int _E27_14(); void _E26_17();
int _E30_12(); void _E29_16(); int _E33_10(); void _E32_14(); int _E36_9(); void _E35_15();
int _E39_11(); void _E38_15(); int _E42_10(); void _E41_13(); int _E45_9(); void _E44_15();
int _E48_14(); void _E47_17(); int _E51_8(); void _E50_18(); int _E54_13(); void _E53_16();
int _E60_10(); void _E59_8(); int _E63_3(); void _E62_7(); int _E66_1(); void _E65_3();
int _E69_3(); void _E68_2(); int _E72_3(); void _E71_2(); int _E138_0(); void _E137_0();
int _E143_0(); int _E140_0(); int _E142(); void _E141();
