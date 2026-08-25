#include "WoW/Source/SoundInterface/isoundinterface.h"

#include "ENGINE/Source/Os/W32/ossound.h"
#include "WoW/Source/DB/DBClient/dbclient.h"

#include <algorithm>
#include <array>
#include <limits>
#include <random>
#include <unordered_map>

float INFINITY_74 = 0.0F;
float TWO_PI_74 = 0.0F;
float OO_TWO_PI_74 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_2 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_2 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_2 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_2 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_2 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_2 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_2 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_2 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_2 = 0.0F;
float SECONDS_PER_HEARTBEAT_2 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_2 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_2 = 0.0F;

namespace {
std::unordered_map<int, SOUNDDEFINITION> s_soundDefinitions;
std::vector<FSOUND_REVERB_CHANNELPROPERTIES> s_reverbTable;
std::uint32_t s_randomState = 0x13579BDFU;
}

SOUNDDEFINITION::SOUNDDEFINITION() = default;
SOUNDDEFINITION::SOUNDDEFINITION(const SOUNDDEFINITION& rhs) = default;
SOUNDDEFINITION& SOUNDDEFINITION::operator=(const SOUNDDEFINITION& rhs) = default;
SOUNDDEFINITION::~SOUNDDEFINITION() { Clear(); }
void SOUNDDEFINITION::Clear() { m_fileNames.clear(); m_totalFrequency = 0; m_lastPlayed = 0; m_loopCounter = 0; }

FILENAMEENTRY* SOUNDDEFINITION::GetRandomFileName(int index) {
    if (m_fileNames.empty() || m_totalFrequency == 0) return nullptr;
    std::size_t selected = 0;
    if (index >= 0) selected = std::min<std::size_t>(static_cast<std::size_t>(index), m_fileNames.size() - 1);
    else if (m_equalFreqs) selected = (m_lastPlayed + 1) % m_fileNames.size();
    else {
        s_randomState = s_randomState * 1664525U + 1013904223U;
        const unsigned target = s_randomState % m_totalFrequency;
        while (selected + 1 < m_fileNames.size() && target >= m_fileNames[selected].accumulatedFreq) ++selected;
    }
    m_lastPlayed = static_cast<unsigned>(selected);
    m_loopCounter = index < 0 ? m_loopCounter + 1 : 0;
    return &m_fileNames[selected];
}

void ISndInterfaceInitialize() {
    s_soundDefinitions.clear();
    s_reverbTable.clear();
    ReadFiles();
    InitializeInterfaceSounds();
}

int ReadFiles() {
    for (const auto& record : g_soundEntriesDB.m_records) {
        SOUNDDEFINITION definition;
        definition.m_flags = static_cast<unsigned>(record.m_flags);
        definition.m_volume = record.m_volumeFloat;
        definition.m_pitch = record.m_pitch;
        definition.m_pitchVariation = record.m_pitchVariation;
        definition.m_priority = record.m_priority;
        definition.m_channel = record.m_channel;
        definition.m_minDistance = record.m_minDistance;
        definition.m_maxDistance = record.m_maxDistance;
        definition.m_distanceCutoffSquared = record.m_distanceCutoff * record.m_distanceCutoff;
        unsigned total = 0;
        for (std::size_t i = 0; i < record.m_File.size(); ++i) {
            if (record.m_File[i].empty()) continue;
            FILENAMEENTRY entry;
            entry.fileName = record.m_File[i];
            entry.accumulatedFreq = record.m_Freq[i] ? record.m_Freq[i] : 1U;
            total += entry.accumulatedFreq;
            definition.m_fileNames.push_back(std::move(entry));
        }
        unsigned accumulated = 0;
        for (auto& entry : definition.m_fileNames) { accumulated += entry.accumulatedFreq; entry.accumulatedFreq = accumulated; }
        definition.m_totalFrequency = accumulated;
        definition.m_equalFreqs = !definition.m_fileNames.empty() &&
            std::all_of(definition.m_fileNames.begin(), definition.m_fileNames.end(), [&](const auto& e) { return e.accumulatedFreq == 0 || e.accumulatedFreq == definition.m_fileNames.front().accumulatedFreq; });
        s_soundDefinitions[record.m_ID] = std::move(definition);
    }
    return 1;
}

int InitializeInterfaceSounds() { return 1; }
int InitializeUISounds() { return 1; }
int InitializeSheatheSounds() { return 1; }
int InitializeUnitCombatSounds() { GenerateWeaponSwingCombatSounds(); InitializeWeaponImpactCombatSounds(); return 1; }
void GenerateWeaponSwingCombatSounds() {}
void InitializeWeaponImpactCombatSounds() {}
void ParseWeaponImpactArmorField(void*) {}

void ISndInterfaceShutdown() { s_soundDefinitions.clear(); s_reverbTable.clear(); }

SOUNDDEFINITION* __fastcall ISndInterfaceGetSndEntry(int soundID) {
    const auto found = s_soundDefinitions.find(soundID);
    return found == s_soundDefinitions.end() ? nullptr : &found->second;
}

FSOUND_REVERB_CHANNELPROPERTIES* GetReverbType(int, int preference) {
    if (preference < 0) return nullptr;
    if (static_cast<std::size_t>(preference) >= s_reverbTable.size()) s_reverbTable.resize(static_cast<std::size_t>(preference) + 1);
    return &s_reverbTable[preference];
}

unsigned char __fastcall InitializePrefTable(int index) {
    return index >= 0 ? 1 : 0;
}

#define ISOUND_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
int _E2_74() { return _E1_74(); }
int _E1_74() { INFINITY_74 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E62_3() { _E59_4(); return _E61_1(); } void _E59_4() {} int _E61_1() { return 0; }
void _E60_6() { s_soundDefinitions.clear(); }
int _E67_0() { _E64_1(); return _E66_0(); } int _E64_1() { return 0; } int _E66_0() { return 0; } void _E65_2() {}
int _E72_2() { _E69_2(); return _E71_1(); } void _E69_2() {} int _E71_1() { return 0; } void _E70_0() {}
int _E77_0() { _E74_1(); return _E76(); } int _E74_1() { return 0; } int _E76() { return 0; } void _E75_0() {}
int _E82_0() { _E81(); return 0; } int _E81() { return 0; } void _E80_0() {}
int _E87_4() { _E84_0(); return _E86(); } void _E84_0() {} int _E86() { return 0; } void _E85_0() {}
int _E5_74() { _E4_74(); return 0; } void _E4_74() { TWO_PI_74 = 6.2831854F; }
int _E8_74() { _E7_74(); return 0; } void _E7_74() { OO_TWO_PI_74 = 1.0F / TWO_PI_74; }
ISOUND_CONST_PAIR(_E21_9, _E20_13, MAX_DUEL_DISTANCE_SQUARED_2, 100.0F)
ISOUND_CONST_PAIR(_E24_12, _E23_16, MAX_TRADE_DISTANCE_SQUARED_2, 123.45679F)
ISOUND_CONST_PAIR(_E27_9, _E26_12, MAX_SHOP_DISTANCE_SQUARED_2, 30.864195F)
ISOUND_CONST_PAIR(_E30_7, _E29_11, MAX_INSPECT_DISTANCE_SQUARED_2, 30.864195F)
ISOUND_CONST_PAIR(_E33_5, _E32_9, MAX_LOOT_DISTANCE_SQUARED_2, 25.0F)
ISOUND_CONST_PAIR(_E36_4, _E35_10, MAX_BIND_DISTANCE_SQUARED_2, 100.0F)
ISOUND_CONST_PAIR(_E39_6, _E38_10, MAX_SITCHAIR_DISTANCE_SQUARED_2, 0.25F)
ISOUND_CONST_PAIR(_E42_5, _E41_8, MAX_SITCHAIRUSE_DISTANCE_SQUARED_2, 9.0F)
ISOUND_CONST_PAIR(_E45_4, _E44_10, MAX_SHARE_DISTANCE_SQUARED_2, 10000.0F)
ISOUND_CONST_PAIR(_E48_9, _E47_12, SECONDS_PER_HEARTBEAT_2, 5.0F)
ISOUND_CONST_PAIR(_E51_3, _E50_13, SPELL_FOCUS_DISTANCE_SQUARED_2, 2500.0F)
ISOUND_CONST_PAIR(_E54_8, _E53_11, MAX_OBJ_INTEREST_RADIUS_SQUARED_2, 10000.0F)
#undef ISOUND_CONST_PAIR
