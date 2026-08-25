#include "WoW/Source/SoundInterface/soundinterfacedoodad.h"

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Os/W32/ossound.h"

#include <algorithm>
#include <cmath>
#include <limits>

float INFINITY_78 = 0.0F;
float TWO_PI_78 = 0.0F;
float OO_TWO_PI_78 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_13 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_13 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_13 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_13 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_13 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_13 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_13 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_13 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_13 = 0.0F;
float SECONDS_PER_HEARTBEAT_13 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_13 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_13 = 0.0F;

LOOPEDDOODADDESC s_doodadLoopedInfo[8];

namespace {
std::uint64_t s_elapsed_1 = 0;

void StopDoodadSound(LOOPEDDOODADDESC& info) {
    if (info.sound) {
        info.sound->Stop();
        Sound::KillSound(info.sound);
        info.sound = nullptr;
    }
}
}

int LOOPEDDOODADDESC::FindFreeSlot() const {
    for (int slot = 0; slot < 8; ++slot)
        if ((posInUseFlags & (1U << slot)) == 0) return slot;
    return 0;
}

int LOOPEDDOODADDESC::GetClosestIndex(const NTempest::C3Vector* listener) const {
    if (!listener) return -1;
    int closest = -1;
    float closestDistance = 0.0F;
    for (int i = 0; i < 8; ++i) {
        if (!(posInUseFlags & (1U << i))) continue;
        const float distance = (*listener - pos[i]).SquaredMag();
        if (closest < 0 || distance < closestDistance) {
            closest = i;
            closestDistance = distance;
        }
    }
    return closest;
}

void LOOPEDDOODADDESC::Update(const NTempest::C3Vector* listener) {
    if (!posInUseFlags || soundID < 0) {
        StopDoodadSound(*this);
        return;
    }
    const int closest = GetClosestIndex(listener);
    if (closest < 0) return;
    currentIndex = closest;
    if (!sound) {
        sound = Sound::Alloc();
        if (!sound) return;
        sound->SetVolume(1.0F);
        if (!sound->Play3DLooped()) { StopDoodadSound(*this); return; }
    }
    sound->SetPosition(pos[closest]);
}

int __fastcall DoodadLoopHandler(const void* eventData, void*) {
    const auto* idle = static_cast<const EVENT_DATA_IDLE*>(eventData);
    if (idle) s_elapsed_1 += static_cast<std::uint64_t>(idle->elapsedSec * 1000.0F);
    const auto listener = Sound::GetListenerPosition();
    for (auto& info : s_doodadLoopedInfo) info.Update(&listener);
    return 1;
}

void SoundInterfaceDoodadInitialize() {
    for (auto& info : s_doodadLoopedInfo) {
        StopDoodadSound(info);
        info = {};
        info.soundID = -1;
        info.currentIndex = -1;
    }
    EventRegisterEx(EVENT_ID_IDLE, DoodadLoopHandler, nullptr, 0.0F);
}

void SoundInterfaceDoodadDestroy() {
    EventUnregister(EVENT_ID_IDLE, DoodadLoopHandler);
    for (auto& info : s_doodadLoopedInfo) {
        StopDoodadSound(info);
        info.soundID = -1;
        info.posInUseFlags = 0;
    }
}

LOOPEDDOODADDESC* FindFreeDoodadLoop(int* freeSlot, const NTempest::C3Vector* soundId, int* soundIndex) {
    if (!freeSlot || !soundIndex) return nullptr;
    int unused = -1;
    for (int i = 0; i < 8; ++i) {
        if (s_doodadLoopedInfo[i].soundID < 0) unused = i;
        if (soundId && s_doodadLoopedInfo[i].soundID == static_cast<int>(soundId->x) &&
            s_doodadLoopedInfo[i].posInUseFlags != 0xFF) {
            *freeSlot = s_doodadLoopedInfo[i].FindFreeSlot();
            *soundIndex = i;
            return &s_doodadLoopedInfo[i];
        }
    }
    if (unused < 0) { *freeSlot = 0; *soundIndex = 0; return nullptr; }
    auto& info = s_doodadLoopedInfo[unused];
    StopDoodadSound(info);
    info = {};
    info.soundID = soundId ? static_cast<int>(soundId->x) : -1;
    info.currentIndex = -1;
    *freeSlot = 0;
    *soundIndex = unused;
    return &info;
}

std::uint32_t SndInterfaceHandleDoodadLoopStart(const NTempest::C3Vector* pos,
                                                 const NTempest::C3Vector* position) {
    if (!pos || !position) return 0;
    int freeSlot = 0;
    int soundIndex = 0;
    auto* info = FindFreeDoodadLoop(&freeSlot, pos, &soundIndex);
    if (!info) return 0;
    info->posInUseFlags |= 1U << freeSlot;
    info->pos[freeSlot] = *position;
    return (static_cast<std::uint32_t>(soundIndex) << 16) | static_cast<std::uint32_t>(freeSlot + 1);
}

void SndInterfaceHandleDoodadLoopStop(std::uint32_t handle) {
    if (!handle) return;
    const unsigned slot = (handle & 0xFFFFU) - 1U;
    const unsigned index = handle >> 16;
    if (index < 8 && slot < 8) s_doodadLoopedInfo[index].posInUseFlags &= ~(1U << slot);
}

void SndInterfaceHandleDoodadOneShot(unsigned, const NTempest::C3Vector* position) {
    if (!position) return;
    Sound* sound = Sound::Alloc();
    if (!sound) return;
    sound->SetPosition(*position);
    if (!sound->Play3D()) Sound::KillSound(sound);
}

int _E2_78() { return _E1_78(); }
int _E1_78() { INFINITY_78 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_78() { _E4_78(); return 0; } void _E4_78() { TWO_PI_78 = 6.2831854F; }
int _E8_78() { _E7_78(); return 0; } void _E7_78() { OO_TWO_PI_78 = 1.0F / TWO_PI_78; }
#define DOODAD_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
DOODAD_CONST_PAIR(_E21_13, _E20_17, MAX_DUEL_DISTANCE_SQUARED_13, 100.0F)
DOODAD_CONST_PAIR(_E24_16, _E23_20, MAX_TRADE_DISTANCE_SQUARED_13, 123.45679F)
DOODAD_CONST_PAIR(_E27_13, _E26_16, MAX_SHOP_DISTANCE_SQUARED_13, 30.864195F)
DOODAD_CONST_PAIR(_E30_11, _E29_15, MAX_INSPECT_DISTANCE_SQUARED_13, 30.864195F)
DOODAD_CONST_PAIR(_E33_9, _E32_13, MAX_LOOT_DISTANCE_SQUARED_13, 25.0F)
DOODAD_CONST_PAIR(_E36_8, _E35_14, MAX_BIND_DISTANCE_SQUARED_13, 100.0F)
DOODAD_CONST_PAIR(_E39_10, _E38_14, MAX_SITCHAIR_DISTANCE_SQUARED_13, 0.25F)
DOODAD_CONST_PAIR(_E42_9, _E41_12, MAX_SITCHAIRUSE_DISTANCE_SQUARED_13, 9.0F)
DOODAD_CONST_PAIR(_E45_8, _E44_14, MAX_SHARE_DISTANCE_SQUARED_13, 10000.0F)
DOODAD_CONST_PAIR(_E48_13, _E47_16, SECONDS_PER_HEARTBEAT_13, 5.0F)
DOODAD_CONST_PAIR(_E51_7, _E50_17, SPELL_FOCUS_DISTANCE_SQUARED_13, 2500.0F)
DOODAD_CONST_PAIR(_E54_12, _E53_15, MAX_OBJ_INTEREST_RADIUS_SQUARED_13, 10000.0F)
#undef DOODAD_CONST_PAIR
int _E62_6() { _E59_7(); return _E61_4(); }
int _E59_7() { for (auto& info : s_doodadLoopedInfo) { info = {}; info.soundID = -1; info.currentIndex = -1; } return 0; }
int _E61_4() { return 0; }
void _E60_9() { SoundInterfaceDoodadDestroy(); }
