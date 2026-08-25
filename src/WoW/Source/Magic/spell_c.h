#pragma once

#include "WoW/Source/Magic/spellnet.h"

#include <cstdint>
#include <unordered_map>

class SpellRec;
struct CGUnit_C;
struct CGGameObject_C;
using SPELL_FAILED_REASON = int;

class SpellHistory {
public:
    void ClearHistory();
    void GarbageCollect(unsigned timestamp);
    void SetCooldown(int spellID, unsigned readyTimestamp);
    bool IsOnCooldown(int spellID, unsigned timestamp) const;
    unsigned CooldownEnd(int spellID) const;
    unsigned m_lastTimestamp = 0;
    std::unordered_map<int, unsigned> m_cooldowns;
};
void Spell_C_SpellFailed(int reason, char notify, int a3, int a4, int a5);
const void* FindAnyItemSubclassRec(unsigned subclassMask, int value);
char* GetStringReason(char reason);
void SpellMissingItemCallback(const std::uint64_t* guid, int arg, char granted, int flags);
void ItemCheckCooldownCallback(std::uint64_t* guid, void* arg, unsigned char granted);
int Spell_C_GetSpellByName(const char* name);
unsigned Spell_C_GetSpellLevel(int isPet, int spellID);
int Spell_C_GetManaCost(int isPet, int spellID);
unsigned Spell_C_GetManaCostPerSecond(int isPet, int spellID);
int Spell_C_GetCastTime(int isPet, int spellID);
void Spell_C_GetMinMaxRange(int spellID, int* minRange, float* maxRange);
void Spell_C_GetMinMaxPoints(int* effectIndex, int spellID, int* min, int* max, int level, int isPet);
int Spell_C_GetModalSpell();
std::uint64_t* Spell_C_GetModalItem();
bool Spell_C_IsModal();
SpellCast* Spell_C_GetCurrentCaster();
std::uint64_t* Spell_C_GetCurrentTarget();
bool Spell_C_HaveSpellTokens(const SpellRec*, int*, unsigned char report);
bool Spell_C_IsTargeting();
int Spell_C_GetTargettingSpell();
void Spell_C_StopTargeting();
void Spell_C_CancelSpell(unsigned char notifyServer, char reason, SPELL_FAILED_REASON failure);
void Spell_C_SetModal(const void* item, int flags);
unsigned char Spell_C_TargetSpell(const SpellRec*, int flags);
void SendCast(void*, int flags);
bool RangeCheckSelected(CGUnit_C*, int flags);
bool RangeCheck(void* target, int flags, int spellID);
void GameObjectStatsCallback(const std::uint64_t*, float, int, int);
unsigned char Spell_C_HandleSpriteClick(std::uint64_t* object);
char Spell_C_HandleSpriteClick(const CGUnit_C* object);
bool Spell_C_CanTargetObject(CGGameObject_C* object);
bool Spell_C_CanTargetObjects(); bool Spell_C_CanTargetUnits(); bool Spell_C_CanTargetMe();
bool Spell_C_CanTargetParty(); bool Spell_C_CanTargetFriends(); bool Spell_C_CanTargetEnemies();
bool Spell_C_CanTargetDead(); bool Spell_C_CanTargetItems(); bool Spell_C_CanTargetTerrain();
double Spell_C_GetSpellRadius(); bool Spell_C_HandleSpriteRay(unsigned* checkRange, char flags); bool Spell_C_HandleTerrainRay(float* checkRange, char flags);
bool Spell_C_WaitingForStringInput(); unsigned Spell_C_WorldObjectCursor(); double Spell_C_WorldObjectFacing(); bool Spell_C_WorldObjectHousing();
void Spell_C_WorldObjectRotate(); int Script_SpellIsTargeting(); int Script_SpellCanTargetUnit(int); int Script_SpellTargetUnit(int); int Script_SpellStopTargeting(int);
void SpellRegisterScriptFunctions(); void SpellUnregisterScriptFunctions(); void Spell_C_CancelCombatSpell(); void Spell_C_CancelAura(int);
unsigned Spell_C_GetPowerDisplayMod(int); void Spell_C_Initialize(); void Spell_C_Destroy(); bool IsSpellAura(int);
int CastResultHandler(unsigned, CDataStore*); int SpellDelayed(unsigned, CDataStore*); int SpellChannelStart(unsigned, CDataStore*); int SpellChannelUpdate(unsigned, CDataStore*);
int SpellAddDynamicTarget(unsigned, CDataStore*); int SpellStartHandler(unsigned, int, unsigned, CDataStore*); void SpellGo(void*, void*, double, int, CDataStore*);
void SetItemCooldown(int, int, unsigned, int); void ItemStatsCooldownCallback(unsigned, int, int, int); int SpellFailedHandler(unsigned, int); int PetSpellFailedHandler(unsigned, CDataStore*);
int SpellCooldownHandler(unsigned, CDataStore*); int ItemCooldownHandler(unsigned, int); int CooldownEvent(unsigned, int, unsigned, CDataStore*); void Spell_C_CooldownEventTriggered(int, unsigned, int, int);
void Spell_C_ClearCooldowns(int); int CooldownCheat(unsigned, CDataStore*); int PetTameFailure(unsigned, CDataStore*); int PlaySpellVisualKit(unsigned, CDataStore*);
int CCommand_Learn(const char*, const char*); int CCommand_Cooldown(); int CCommand_CooldownPet(); int CCommand_UseSkill(const char*, char*); int CCommand_SetSkill(const char*, const char*); int CCommand_CancelAura(const char*, const char*);

extern SpellHistory g_spellHistory;
extern float INFINITY_375;
extern SpellCast g_currentSpellCast;
int _E2_375(); int _E1_375(); int _E5_345(); void _E4_345(); int _E8_335(); void _E7_345();
