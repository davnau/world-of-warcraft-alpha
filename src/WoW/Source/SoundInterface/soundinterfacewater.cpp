#include "WoW/Source/SoundInterface/soundinterfacewater.h"

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Os/W32/ossound.h"
#include "WoW/Source/DB/DBClient/dbclient.h"
#include "WoW/Source/SoundInterface/soundinterfaceproviderprefs.h"

#include <algorithm>
#include <cmath>
#include <limits>

float INFINITY_79 = 0.0F;
float TWO_PI_79 = 0.0F;
float OO_TWO_PI_79 = 0.0F;
float OOSMOLTILE_SIZE_2 = 0.0F;
float PANNING_DIST_SQUARED = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_11 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_11 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_11 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_11 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_11 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_11 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_11 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_11 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_11 = 0.0F;
float SECONDS_PER_HEARTBEAT_11 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_11 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_11 = 0.0F;

LIQUIDINFO s_liquidInfo[4];
int s_paused = -1;
float s_volume_0 = 1.0F;

namespace {
unsigned s_flags_1 = 0;
bool s_mapWaterSounds = true;

void ClearSlot(LIQUIDINFO& info, int immediate) {
    info.StopSound(immediate);
    info.m_currentRecord = nullptr;
    info.m_currentPlayingSound = 0;
    for (auto& subtype : info.m_subTypes) subtype = 0;
    for (auto& position : info.m_positionOffset) position = {};
}
}

void LIQUIDINFO::StopSound(int) {
    if (m_sound) {
        m_sound->Stop();
        Sound::KillSound(m_sound);
        m_sound = nullptr;
    }
    m_currentRecord = nullptr;
}

void LIQUIDINFO::StartSound(unsigned subType, const NTempest::C3Vector* listenerPos) {
    if (subType >= 3 || !listenerPos || s_paused > 0 || !m_soundRecords[subType]) return;
    m_currentPlayingSound = subType;
    m_currentRecord = m_soundRecords[subType];
    if (m_sound) {
        NTempest::C3Vector position = *listenerPos + m_positionOffset[subType];
        m_sound->SetPosition(position);
        return;
    }
    m_sound = Sound::Alloc();
    if (!m_sound) return;
    m_sound->SetVolume(s_volume_0);
    m_sound->Play3DLooped();
    NTempest::C3Vector position = *listenerPos + m_positionOffset[subType];
    m_sound->SetPosition(position);
    m_sound->SetFadeIn(5);
}

void LIQUIDINFO::Tick() {
    if (!m_sound || m_currentPlayingSound >= 3) return;
    const auto& offset = m_positionOffset[m_currentPlayingSound];
    const float pan = std::clamp(1.0F - offset.SquaredMag() / PANNING_DIST_SQUARED, 0.0F, 1.0F);
    m_sound->SetPanning(pan);
    m_sound->SetVolume(s_volume_0);
}

void LIQUIDINFO::UpdateVolume() {
    if (m_sound) {
        m_sound->SetVolume(s_volume_0);
        Tick();
    }
}

int LIQUIDINFO::Update(const NTempest::C3Vector* listenerPos) {
    unsigned subtype = 0;
    while (subtype < 3 && m_subTypes[subtype] == 0) ++subtype;
    if (subtype >= 3) {
        StopSound(0);
        return 0;
    }
    if (m_sound && m_currentPlayingSound != subtype) StopSound(0);
    StartSound(subtype, listenerPos);
    Tick();
    return m_sound != nullptr;
}

void InitializeWaterAmbiences() {
    for (auto& info : s_liquidInfo) ClearSlot(info, 1);
    for (const auto& record : g_soundWaterTypeDB.m_records) {
        if (record.m_soundType < 0 || record.m_soundType >= 4) continue;
        const unsigned subtype = static_cast<unsigned>((record.m_soundSubtype >> 2) & 3);
        if (subtype >= 3) continue;
        auto* soundRecord = g_soundEntriesDB.Lookup(record.m_SoundID);
        s_liquidInfo[record.m_soundType].m_soundRecords[subtype] = soundRecord;
    }
    s_flags_1 |= 1;
    EventRegisterEx(EVENT_ID_IDLE, WaterHandler, nullptr, 0.0F);
    s_paused = 0;
}

unsigned char __stdcall ToggleCallback(const char* newValue, void*) {
    s_mapWaterSounds = newValue == nullptr || std::atoi(newValue) != 0;
    if (!s_mapWaterSounds) ClearAllSounds(0);
    return 1;
}

void ClearAllSounds(int immediate) {
    for (auto& info : s_liquidInfo) ClearSlot(info, immediate);
}

int __fastcall WaterHandler(const void*, void*) {
    HandleWaterAmbiences();
    return 1;
}

void HandleWaterAmbiences() {
    if (!(s_flags_1 & 1) || !s_mapWaterSounds || s_paused > 0 || g_underWater) return;
    const auto listener = Sound::GetListenerPosition();
    unsigned active = 0;
    for (auto& info : s_liquidInfo) {
        if (active < 2 && info.Update(&listener)) ++active;
        else if (active >= 2) info.StopSound(0);
    }
    s_flags_1 &= ~2U;
}

void ShutdownWaterAmbiences() {
    s_paused = -1;
    ClearAllSounds(1);
    if (s_flags_1 & 1) EventUnregister(EVENT_ID_IDLE, WaterHandler);
    s_flags_1 = 0;
}

void WaterAmbiencesUnderwaterChanged() {
    if (g_underWater) ClearAllSounds(1);
    else s_flags_1 |= 2;
}

void SndInterfaceWaterSetPaused(char paused) {
    const int newState = paused != 0;
    if (newState && newState != s_paused) ClearAllSounds(1);
    s_paused = newState;
}

void SndInterfaceWaterUpdateVolume(float volume) {
    s_volume_0 = volume;
    for (auto& info : s_liquidInfo) info.UpdateVolume();
}

int _E2_79() { return _E1_79(); }
int _E1_79() { INFINITY_79 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_79() { _E4_79(); return 0; } void _E4_79() { TWO_PI_79 = 6.2831854F; }
int _E8_79() { _E7_79(); return 0; } void _E7_79() { OO_TWO_PI_79 = 1.0F / TWO_PI_79; }
#define WATER_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
WATER_CONST_PAIR(_E21_14, _E20_18, MAX_DUEL_DISTANCE_SQUARED_11, 100.0F)
WATER_CONST_PAIR(_E24_17, _E23_21, MAX_TRADE_DISTANCE_SQUARED_11, 123.45679F)
WATER_CONST_PAIR(_E27_14, _E26_17, MAX_SHOP_DISTANCE_SQUARED_11, 30.864195F)
WATER_CONST_PAIR(_E30_12, _E29_16, MAX_INSPECT_DISTANCE_SQUARED_11, 30.864195F)
WATER_CONST_PAIR(_E33_10, _E32_14, MAX_LOOT_DISTANCE_SQUARED_11, 25.0F)
WATER_CONST_PAIR(_E36_9, _E35_15, MAX_BIND_DISTANCE_SQUARED_11, 100.0F)
WATER_CONST_PAIR(_E39_11, _E38_15, MAX_SITCHAIR_DISTANCE_SQUARED_11, 0.25F)
WATER_CONST_PAIR(_E42_10, _E41_13, MAX_SITCHAIRUSE_DISTANCE_SQUARED_11, 9.0F)
WATER_CONST_PAIR(_E45_9, _E44_15, MAX_SHARE_DISTANCE_SQUARED_11, 10000.0F)
WATER_CONST_PAIR(_E48_14, _E47_17, SECONDS_PER_HEARTBEAT_11, 5.0F)
WATER_CONST_PAIR(_E51_8, _E50_18, SPELL_FOCUS_DISTANCE_SQUARED_11, 2500.0F)
WATER_CONST_PAIR(_E54_13, _E53_16, MAX_OBJ_INTEREST_RADIUS_SQUARED_11, 10000.0F)
#undef WATER_CONST_PAIR
int _E60_10() { _E59_8(); return 0; } void _E59_8() { OOSMOLTILE_SIZE_2 = 1.0F / 4.1666665F; }
int _E63_3() { _E62_7(); return 0; } void _E62_7() { PANNING_DIST_SQUARED = 10000.0F; }
int _E66_1() { _E65_3(); return 0; } void _E65_3() {}
int _E69_3() { _E68_2(); return 0; } void _E68_2() {}
int _E72_3() { _E71_2(); return 0; } void _E71_2() {}
int _E138_0() { _E137_0(); return 0; } void _E137_0() { PANNING_DIST_SQUARED = 10000.0F; }
int _E143_0() { _E140_0(); return _E142(); }
int _E140_0() { for (auto& info : s_liquidInfo) info = {}; return 0; }
int _E142() { return 0; }
void _E141() { for (auto& info : s_liquidInfo) info.StopSound(1); }
