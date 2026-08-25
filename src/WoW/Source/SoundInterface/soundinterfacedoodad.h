#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

class Sound;

struct LOOPEDDOODADDESC {
    NTempest::C3Vector pos[8]{};
    int soundID = -1;
    unsigned posInUseFlags = 0;
    int currentIndex = -1;
    Sound* sound = nullptr;

    int FindFreeSlot() const;
    void Update(const NTempest::C3Vector* listener);
    int GetClosestIndex(const NTempest::C3Vector* listener) const;
};

extern LOOPEDDOODADDESC s_doodadLoopedInfo[8];

int __fastcall DoodadLoopHandler(const void* eventData, void* parameter);
void SoundInterfaceDoodadInitialize();
void SoundInterfaceDoodadDestroy();
std::uint32_t SndInterfaceHandleDoodadLoopStart(const NTempest::C3Vector* pos,
                                                 const NTempest::C3Vector* position);
LOOPEDDOODADDESC* FindFreeDoodadLoop(int* freeSlot, const NTempest::C3Vector* soundId,
                                     int* soundIndex);
void SndInterfaceHandleDoodadLoopStop(std::uint32_t handle);
void SndInterfaceHandleDoodadOneShot(unsigned soundID, const NTempest::C3Vector* position);

int _E2_78(); int _E1_78(); int _E5_78(); void _E4_78(); int _E8_78(); void _E7_78();
int _E21_13(); void _E20_17(); int _E24_16(); void _E23_20(); int _E27_13(); void _E26_16();
int _E30_11(); void _E29_15(); int _E33_9(); void _E32_13(); int _E36_8(); void _E35_14();
int _E39_10(); void _E38_14(); int _E42_9(); void _E41_12(); int _E45_8(); void _E44_14();
int _E48_13(); void _E47_16(); int _E51_7(); void _E50_17(); int _E54_12(); void _E53_15();
int _E62_6(); int _E59_7(); int _E61_4(); void _E60_9();
