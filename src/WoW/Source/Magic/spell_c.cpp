#include "WoW/Source/Magic/spell_c.h"

#include "WoW/Source/DB/DBClient/AutoCode/spellrec.h"
#include "WoW/Source/UI/actionbarframe.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>
#include <unordered_map>

SpellHistory g_spellHistory;
SpellCast g_currentSpellCast{};
float INFINITY_375 = 0.0F;

namespace {
constexpr unsigned SMSG_ITEM_COOLDOWN = 0x00B0U;
constexpr unsigned SMSG_CAST_RESULT = 0x0123U;
constexpr unsigned SMSG_SPELL_START = 0x0124U;
constexpr unsigned SMSG_SPELL_GO = 0x0125U;
constexpr unsigned SMSG_SPELL_FAILURE = 0x0126U;
constexpr unsigned SMSG_SPELL_COOLDOWN = 0x0127U;
constexpr unsigned SMSG_COOLDOWN_EVENT = 0x0128U;
constexpr unsigned SMSG_PET_CAST_FAILED = 0x012BU;
constexpr unsigned MSG_CHANNEL_START = 0x012CU;
constexpr unsigned MSG_CHANNEL_UPDATE = 0x012DU;
constexpr unsigned MSG_ADD_DYNAMIC_TARGET = 0x012EU;
constexpr unsigned SMSG_ATTACKSTART = 0x0136U;
constexpr unsigned SMSG_PET_TAME_FAILURE = 0x0166U;
constexpr unsigned SMSG_COOLDOWN_CHEAT = 0x01D2U;
constexpr unsigned SMSG_SPELL_DELAYED = 0x01D3U;

struct SpellDefinition {
    unsigned level = 1;
    int manaCost = 0;
    unsigned manaPerSecond = 0;
    int castTime = 0;
    int minRange = 0;
    float maxRange = 30.0F;
    int minPoints = 0;
    int maxPoints = 0;
    float radius = 0.0F;
};

std::unordered_map<int, SpellDefinition> s_spellDefinitions;
std::unordered_map<std::string, int> s_spellNames;
std::unordered_map<std::uint64_t, unsigned> s_itemCooldowns;
std::uint64_t s_modalItem = 0;
int s_modalSpell = 0;
bool s_targeting = false;
bool s_waitingForString = false;
bool s_housingMode = false;
double s_worldObjectFacing = 0.0;
unsigned s_cursor = 0;

SpellDefinition& DefinitionFor(int spellID) {
    return s_spellDefinitions[spellID];
}

const SpellDefinition& DefinitionForConst(int spellID) {
    static const SpellDefinition empty{};
    const auto it = s_spellDefinitions.find(spellID);
    return it == s_spellDefinitions.end() ? empty : it->second;
}

void ResetTargeting() {
    s_targeting = false;
    s_waitingForString = false;
    g_currentSpellCast.targets = 0;
}

int __fastcall SpellPacketHandler(void*, unsigned msgID, unsigned eventTime,
                                  CDataStore* message) {
    if (!message) return 0;
    if (msgID == SMSG_ATTACKSTART) {
        std::uint64_t attacker = 0U;
        std::uint64_t victim = 0U;
        message->Get(&attacker)->Get(&victim);
        if (message->Failed()) return 0;
        CWorld::PlayMeleeAttack(attacker, victim);
        return 1;
    }
    if (msgID == SMSG_CAST_RESULT) {
        std::uint32_t spellID = 0U;
        std::uint8_t status = 0U;
        std::uint8_t reason = 0U;
        message->Get(&spellID)->Get(&status);
        if (status == 2U) message->Get(&reason);
        if (message->Failed()) return 0;
        std::fprintf(stderr,
                     "SPELL_CAST_RESULT spell=%u status=%u reason=%u\n",
                     spellID, static_cast<unsigned>(status),
                     static_cast<unsigned>(reason));
        if (status == 2U)
            Spell_C_SpellFailed(reason, 1, static_cast<int>(spellID), 0, 0);
        else
            ResetTargeting();
        CGActionBar::UpdateSelection();
        return 1;
    }
    if (msgID == SMSG_SPELL_START || msgID == SMSG_SPELL_GO) {
        std::uint64_t source = 0U;
        std::uint64_t caster = 0U;
        std::uint32_t spellID = 0U;
        message->Get(&source)->Get(&caster)->Get(&spellID);
        if (message->Failed()) return 0;
        std::uint64_t packetTarget = 0U;
        SpellCast packetCast{};
        if (msgID == SMSG_SPELL_START) {
            std::uint16_t castFlags = 0U;
            std::int32_t castTime = 0;
            message->Get(&castFlags)->Get(&castTime);
            SpellGetCastTargets(&packetCast, message);
            if (message->Failed()) return 0;
            packetTarget = packetCast.unitTarget;
            CWorld::BeginSpellCast(caster ? caster : source, packetTarget,
                                   spellID,
                                   castTime > 0
                                       ? static_cast<unsigned>(castTime) : 0U);
        } else {
            std::uint16_t castFlags = 0U;
            std::uint8_t hitCount = 0U;
            message->Get(&castFlags)->Get(&hitCount);
            for (unsigned index = 0; index < hitCount; ++index) {
                std::uint64_t hit = 0U;
                message->Get(&hit);
                if (packetTarget == 0U) packetTarget = hit;
            }
            std::uint8_t missCount = 0U;
            message->Get(&missCount);
            for (unsigned index = 0; index < missCount; ++index) {
                std::uint8_t missReason = 0U;
                std::uint64_t missed = 0U;
                message->Get(&missReason)->Get(&missed);
            }
            SpellGetCastTargets(&packetCast, message);
            if (message->Failed()) return 0;
            if (packetCast.unitTarget != 0U)
                packetTarget = packetCast.unitTarget;
            CWorld::LaunchSpell(caster ? caster : source, packetTarget,
                                spellID);
        }
        g_currentSpellCast.caster = source;
        g_currentSpellCast.casterUnit = caster;
        g_currentSpellCast.spellID = static_cast<int>(spellID);
        std::fprintf(stderr,
                     "SPELL_%s spell=%u source=%llu caster=%llu target=%llu\n",
                     msgID == SMSG_SPELL_START ? "START" : "GO", spellID,
                     static_cast<unsigned long long>(source),
                     static_cast<unsigned long long>(caster),
                     static_cast<unsigned long long>(packetTarget));
        if (msgID == SMSG_SPELL_GO) ResetTargeting();
        return 1;
    }
    if (msgID == SMSG_SPELL_FAILURE || msgID == SMSG_PET_CAST_FAILED) {
        std::uint64_t caster = 0U;
        std::uint32_t spellID = 0U;
        std::uint8_t reason = 0U;
        message->Get(&caster)->Get(&spellID)->Get(&reason);
        if (message->Failed()) return 0;
        std::fprintf(stderr,
                     "SPELL_FAILURE spell=%u caster=%llu reason=%u\n",
                     spellID, static_cast<unsigned long long>(caster),
                     static_cast<unsigned>(reason));
        Spell_C_SpellFailed(reason, 1, static_cast<int>(spellID), 0, 0);
        return 1;
    }
    if (msgID == SMSG_SPELL_COOLDOWN) {
        std::uint32_t spellID = 0U;
        std::uint64_t caster = 0U;
        std::uint16_t duration = 0U;
        message->Get(&spellID)->Get(&caster)->Get(&duration);
        if (message->Failed()) return 0;
        g_spellHistory.SetCooldown(static_cast<int>(spellID),
                                   eventTime + duration);
        std::fprintf(stderr,
                     "SPELL_COOLDOWN spell=%u caster=%llu duration=%u\n",
                     spellID, static_cast<unsigned long long>(caster),
                     static_cast<unsigned>(duration));
        CGActionBar::UpdateCooldowns();
        return 1;
    }
    return 1;
}
}

int _E2_375() { return _E1_375(); }

int _E1_375() {
    INFINITY_375 = std::numeric_limits<float>::infinity();
    return 0x7F800000;
}

int _E5_345() { _E4_345(); return 0; }
void _E4_345() {}
int _E8_335() { _E7_345(); return 0; }
void _E7_345() {}

void SpellHistory::ClearHistory() {
    m_lastTimestamp = 0;
    m_cooldowns.clear();
}

void SpellHistory::GarbageCollect(unsigned timestamp) {
    m_lastTimestamp = timestamp;
    for (auto it = m_cooldowns.begin(); it != m_cooldowns.end();) {
        if (it->second <= timestamp) {
            it = m_cooldowns.erase(it);
        } else {
            ++it;
        }
    }
}

void SpellHistory::SetCooldown(int spellID, unsigned readyTimestamp) {
    if (spellID != 0) {
        m_cooldowns[spellID] = readyTimestamp;
    }
}

bool SpellHistory::IsOnCooldown(int spellID, unsigned timestamp) const {
    const auto it = m_cooldowns.find(spellID);
    return it != m_cooldowns.end() && it->second > timestamp;
}

unsigned SpellHistory::CooldownEnd(int spellID) const {
    const auto it = m_cooldowns.find(spellID);
    return it == m_cooldowns.end() ? 0U : it->second;
}

void Spell_C_SpellFailed(int reason, char notify, int, int, int) {
    g_currentSpellCast.flags = static_cast<std::uint16_t>(reason);
    if (notify != 0) {
        ResetTargeting();
    }
}

const void* FindAnyItemSubclassRec(unsigned, int value) {
    return value == 0 ? nullptr : reinterpret_cast<const void*>(static_cast<std::uintptr_t>(value));
}

char* GetStringReason(char reason) {
    static char text[8]{};
    text[0] = reason;
    text[1] = 0;
    return text;
}

void SpellMissingItemCallback(const std::uint64_t*, int, char granted, int) {
    if (granted == 0) {
        Spell_C_SpellFailed(1, 1, 0, 0, 0);
    }
}

void ItemCheckCooldownCallback(std::uint64_t* guid, void*, unsigned char granted) {
    if (guid != nullptr && granted == 0) {
        s_itemCooldowns.erase(*guid);
    }
}

int Spell_C_GetSpellByName(const char* name) {
    if (name == nullptr || *name == 0) {
        return 0;
    }
    const auto known = s_spellNames.find(name);
    if (known != s_spellNames.end()) {
        return known->second;
    }
    unsigned value = 5381U;
    for (const unsigned char* cursor = reinterpret_cast<const unsigned char*>(name); *cursor != 0; ++cursor) {
        value = ((value << 5U) + value) ^ *cursor;
    }
    const int spellID = static_cast<int>(value & 0x7FFFFFFFU);
    s_spellNames.emplace(name, spellID);
    DefinitionFor(spellID);
    return spellID;
}

unsigned Spell_C_GetSpellLevel(int, int spellID) {
    return spellID == 0 ? 0U : DefinitionForConst(spellID).level;
}

int Spell_C_GetManaCost(int, int spellID) {
    return spellID == 0 ? 0 : DefinitionForConst(spellID).manaCost;
}

unsigned Spell_C_GetManaCostPerSecond(int, int spellID) {
    return spellID == 0 ? 0U : DefinitionForConst(spellID).manaPerSecond;
}

int Spell_C_GetCastTime(int, int spellID) {
    return spellID == 0 ? 0 : DefinitionForConst(spellID).castTime;
}

void Spell_C_GetMinMaxRange(int spellID, int* minRange, float* maxRange) {
    const auto& definition = DefinitionForConst(spellID);
    if (minRange != nullptr) *minRange = definition.minRange;
    if (maxRange != nullptr) *maxRange = definition.maxRange;
}

void Spell_C_GetMinMaxPoints(int*, int spellID, int* min, int* max, int, int) {
    const auto& definition = DefinitionForConst(spellID);
    if (min != nullptr) *min = definition.minPoints;
    if (max != nullptr) *max = definition.maxPoints;
}

int Spell_C_GetModalSpell() { return s_modalSpell; }
std::uint64_t* Spell_C_GetModalItem() { return &s_modalItem; }
bool Spell_C_IsModal() { return s_modalSpell != 0 || s_modalItem != 0; }
SpellCast* Spell_C_GetCurrentCaster() { return &g_currentSpellCast; }
std::uint64_t* Spell_C_GetCurrentTarget() { return &g_currentSpellCast.selectedTarget; }

bool Spell_C_HaveSpellTokens(const SpellRec* spell, int* result, unsigned char) {
    const bool available = spell != nullptr || g_currentSpellCast.spellID != 0;
    if (result != nullptr) *result = available ? 1 : 0;
    return available;
}

bool Spell_C_IsTargeting() { return s_targeting; }
int Spell_C_GetTargettingSpell() { return s_targeting ? g_currentSpellCast.spellID : 0; }
void Spell_C_StopTargeting() { ResetTargeting(); }

void Spell_C_CancelSpell(unsigned char notifyServer, char reason, SPELL_FAILED_REASON failure) {
    if (notifyServer != 0) {
        Spell_C_SpellFailed(failure, reason, 0, 0, 0);
    }
    ResetTargeting();
    g_currentSpellCast = {};
}

void Spell_C_SetModal(const void* item, int flags) {
    s_modalItem = reinterpret_cast<std::uintptr_t>(item);
    s_modalSpell = flags;
}

unsigned char Spell_C_TargetSpell(const SpellRec* spell, int flags) {
    if (spell == nullptr && flags == 0 && s_modalSpell == 0) {
        return 0;
    }
    if (flags != 0) {
        g_currentSpellCast.spellID = flags;
    } else if (g_currentSpellCast.spellID == 0) {
        g_currentSpellCast.spellID = s_modalSpell;
    }
    s_targeting = true;
    g_currentSpellCast.targets = 1;
    return 1;
}

void SendCast(void* cast, int flags) {
    if (cast != nullptr) {
        g_currentSpellCast = *reinterpret_cast<const SpellCast*>(cast);
    }
    g_currentSpellCast.flags = static_cast<std::uint16_t>(flags);
    s_targeting = false;
}

bool RangeCheckSelected(CGUnit_C*, int) { return g_currentSpellCast.selectedTarget != 0; }
bool RangeCheck(void* target, int, int) { return target != nullptr; }

void GameObjectStatsCallback(const std::uint64_t* guid, float, int, int) {
    if (guid != nullptr) g_currentSpellCast.selectedTarget = *guid;
}

unsigned char Spell_C_HandleSpriteClick(std::uint64_t* object) {
    if (object == nullptr || !s_targeting) return 0;
    g_currentSpellCast.selectedTarget = *object;
    g_currentSpellCast.targets |= 1;
    return 1;
}

char Spell_C_HandleSpriteClick(const CGUnit_C* object) {
    if (object == nullptr || !s_targeting) return 0;
    g_currentSpellCast.targets |= 1;
    return 1;
}

bool Spell_C_CanTargetObject(CGGameObject_C* object) { return s_targeting && object != nullptr; }
bool Spell_C_CanTargetObjects() { return s_targeting; }
bool Spell_C_CanTargetUnits() { return s_targeting; }
bool Spell_C_CanTargetMe() { return s_targeting; }
bool Spell_C_CanTargetParty() { return s_targeting; }
bool Spell_C_CanTargetFriends() { return s_targeting; }
bool Spell_C_CanTargetEnemies() { return s_targeting; }
bool Spell_C_CanTargetDead() { return s_targeting; }
bool Spell_C_CanTargetItems() { return s_targeting; }
bool Spell_C_CanTargetTerrain() { return s_targeting; }

double Spell_C_GetSpellRadius() {
    return DefinitionForConst(g_currentSpellCast.spellID).radius;
}

bool Spell_C_HandleSpriteRay(unsigned* checkRange, char flags) {
    if (checkRange != nullptr) *checkRange = flags != 0 ? 1U : 0U;
    return s_targeting;
}

bool Spell_C_HandleTerrainRay(float* checkRange, char flags) {
    if (checkRange != nullptr) *checkRange = flags != 0 ? 1.0F : 0.0F;
    return s_targeting && Spell_C_CanTargetTerrain();
}

bool Spell_C_WaitingForStringInput() { return s_waitingForString; }
unsigned Spell_C_WorldObjectCursor() { return s_cursor; }
double Spell_C_WorldObjectFacing() { return s_worldObjectFacing; }
bool Spell_C_WorldObjectHousing() { return s_housingMode; }
void Spell_C_WorldObjectRotate() { s_worldObjectFacing += 0.0174532925199433; }

int Script_SpellIsTargeting() { return s_targeting ? 1 : 0; }
int Script_SpellCanTargetUnit(int unit) { return s_targeting && unit != 0 ? 1 : 0; }
int Script_SpellTargetUnit(int unit) {
    if (!s_targeting || unit == 0) return 0;
    g_currentSpellCast.selectedTarget = static_cast<std::uint64_t>(static_cast<unsigned>(unit));
    g_currentSpellCast.targets |= 1;
    return 1;
}
int Script_SpellStopTargeting(int) { Spell_C_StopTargeting(); return 1; }

void SpellRegisterScriptFunctions() {}
void SpellUnregisterScriptFunctions() { ResetTargeting(); }
void Spell_C_CancelCombatSpell() { Spell_C_CancelSpell(0, 0, 0); }
void Spell_C_CancelAura(int spellID) { g_spellHistory.m_cooldowns.erase(spellID); }
unsigned Spell_C_GetPowerDisplayMod(int power) { return power < 0 ? 0U : static_cast<unsigned>(power); }

void Spell_C_Initialize() {
    g_currentSpellCast = {};
    g_spellHistory.ClearHistory();
    s_spellDefinitions.clear();
    s_spellNames.clear();
    s_itemCooldowns.clear();
    s_modalItem = 0;
    s_modalSpell = 0;
    s_cursor = 0;
    s_worldObjectFacing = 0.0;
    s_housingMode = false;
    ResetTargeting();
    // CGUnit_C::Initialize in the recovered client installs this spell packet
    // surface.  Without it successful casts are consumed by the network layer
    // with no client state, feedback, or action-bar update.
    ClientServices_SetMessageHandler(SMSG_CAST_RESULT, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_SPELL_START, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_SPELL_GO, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_SPELL_FAILURE, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_PET_CAST_FAILED, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_SPELL_COOLDOWN, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_ITEM_COOLDOWN, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_COOLDOWN_EVENT, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_COOLDOWN_CHEAT, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_PET_TAME_FAILURE, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_SPELL_DELAYED, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(MSG_CHANNEL_START, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(MSG_CHANNEL_UPDATE, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(MSG_ADD_DYNAMIC_TARGET, SpellPacketHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_ATTACKSTART, SpellPacketHandler, nullptr);
}

void Spell_C_Destroy() {
    for (const unsigned message : {
             SMSG_CAST_RESULT, SMSG_SPELL_START, SMSG_SPELL_GO,
             SMSG_SPELL_FAILURE, SMSG_PET_CAST_FAILED, SMSG_SPELL_COOLDOWN,
             SMSG_ITEM_COOLDOWN, SMSG_COOLDOWN_EVENT, SMSG_COOLDOWN_CHEAT,
             SMSG_PET_TAME_FAILURE, SMSG_SPELL_DELAYED, MSG_CHANNEL_START,
             MSG_CHANNEL_UPDATE, MSG_ADD_DYNAMIC_TARGET, SMSG_ATTACKSTART}) {
        ClientServices_ClearMessageHandler(message);
    }
    g_currentSpellCast = {};
    g_spellHistory.ClearHistory();
    ResetTargeting();
}
bool IsSpellAura(int value) { return value != 0; }

int CastResultHandler(unsigned, CDataStore*) { ResetTargeting(); return 1; }
int SpellDelayed(unsigned, CDataStore*) { return s_targeting ? 1 : 0; }
int SpellChannelStart(unsigned, CDataStore*) { g_currentSpellCast.flags |= 2; return 1; }
int SpellChannelUpdate(unsigned, CDataStore*) { return (g_currentSpellCast.flags & 2) != 0; }
int SpellAddDynamicTarget(unsigned, CDataStore*) { g_currentSpellCast.targets |= 2; return 1; }
int SpellStartHandler(unsigned spellID, int, unsigned, CDataStore*) {
    g_currentSpellCast.spellID = static_cast<int>(spellID);
    g_currentSpellCast.castTime = static_cast<unsigned>(DefinitionFor(spellID).castTime);
    s_targeting = true;
    return 1;
}
void SpellGo(void*, void*, double, int, CDataStore*) { SendCast(&g_currentSpellCast, g_currentSpellCast.flags); }

void SetItemCooldown(int itemID, int, unsigned duration, int) {
    if (itemID != 0) s_itemCooldowns[static_cast<std::uint64_t>(static_cast<unsigned>(itemID))] = duration;
}
void ItemStatsCooldownCallback(unsigned itemID, int, int, int) { s_itemCooldowns.erase(itemID); }
int SpellFailedHandler(unsigned spellID, int reason) { Spell_C_SpellFailed(reason, 1, static_cast<int>(spellID), 0, 0); return 1; }
int PetSpellFailedHandler(unsigned, CDataStore*) { Spell_C_SpellFailed(1, 1, 0, 0, 0); return 1; }
int SpellCooldownHandler(unsigned spellID, CDataStore*) { g_spellHistory.SetCooldown(static_cast<int>(spellID), g_spellHistory.m_lastTimestamp + 1); return 1; }
int ItemCooldownHandler(unsigned itemID, int duration) { s_itemCooldowns[itemID] = static_cast<unsigned>(duration); return 1; }
int CooldownEvent(unsigned spellID, int, unsigned, CDataStore*) { return SpellCooldownHandler(spellID, nullptr); }
void Spell_C_CooldownEventTriggered(int spellID, unsigned timestamp, int, int) { g_spellHistory.SetCooldown(spellID, timestamp); }
void Spell_C_ClearCooldowns(int spellID) { if (spellID == 0) g_spellHistory.ClearHistory(); else g_spellHistory.m_cooldowns.erase(spellID); }
int CooldownCheat(unsigned, CDataStore*) { g_spellHistory.ClearHistory(); return 1; }
int PetTameFailure(unsigned, CDataStore*) { Spell_C_SpellFailed(1, 1, 0, 0, 0); return 1; }
int PlaySpellVisualKit(unsigned, CDataStore*) { return 1; }

int CCommand_Learn(const char*, const char* arguments) { return Spell_C_GetSpellByName(arguments) != 0; }
int CCommand_Cooldown() { g_spellHistory.ClearHistory(); return 1; }
int CCommand_CooldownPet() { g_spellHistory.ClearHistory(); return 1; }
int CCommand_UseSkill(const char*, char* arguments) { return Spell_C_TargetSpell(nullptr, Spell_C_GetSpellByName(arguments)); }
int CCommand_SetSkill(const char*, const char* arguments) { return Spell_C_GetSpellByName(arguments) != 0; }
int CCommand_CancelAura(const char*, const char* arguments) {
    Spell_C_CancelAura(Spell_C_GetSpellByName(arguments));
    return 1;
}
