#include "WoW/Source/UI/gameui.h"
#include "WoW/Source/UI/actionbarframe.h"
#include "WoW/Source/UI/charactermodelbase.h"
#include "WoW/Source/UI/chatframe.h"
#include "WoW/Source/UI/containerframe.h"
#include "WoW/Source/UI/minimapframe.h"
#include "WoW/Source/UI/paperdollinfoframe.h"
#include "WoW/Source/UI/worldframe.h"
#include "WoW/Source/UIUtil/tooltip.h"

#include "ENGINE/Source/Base/status.h"
#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Frame/frame.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/Glue/charselectinfo.h"
#include "WoW/Source/Console/consolevar.h"
#include "WoW/Source/DB/DBClient/dbcache.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/Object/objectclient/player_c.h"
#include "WoW/Source/UI/uibindings.h"
#include "WoW/Source/UIUtil/camera.h"
#include "WoW/Source/UIUtil/inputcontrol.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/clientruntime.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <array>
#include <ctime>
#include <cstring>
#include <cstdio>
#include <string>

int Script_Jump(lua_State* state);
void Script_AcceptResurrect();
void Script_DeclineResurrect();
void Script_RepopMe();

namespace {
CSimpleFrame* CreateWorldFrame(CSimpleFrame* parent) { return CGWorldFrame::Create(parent); }
CSimpleFrame* CreateTooltip(CSimpleFrame* parent) { return CGTooltip::Create(parent); }
CSimpleFrame* CreateMinimap(CSimpleFrame* parent) { return CGMinimapFrame::Create(parent); }
CSimpleFrame* CreatePlayerModel(CSimpleFrame* parent) { return CGCharacterModelBase::Create(parent); }
CSimpleFrame* CreateTabardModel(CSimpleFrame* parent) { return CGCharacterModelBase::Create(parent); }

constexpr const char* kScreenResolutions[] = {
    "640x480", "800x600", "1024x768", "1280x720", "1280x1024",
    "1600x900", "1920x1080"
};

int Script_GetTime_Impl(lua_State* state) {
    lua_pushnumber(state, static_cast<double>(OsGetAsyncTimeMs()) * 0.001);
    return 1;
}

int Script_GetGameTime_Impl(lua_State* state) {
    const std::time_t now = std::time(nullptr);
    std::tm local{};
    localtime_s(&local, &now);
    lua_pushnumber(state, local.tm_hour);
    lua_pushnumber(state, local.tm_min);
    return 2;
}

int Script_GetScreenWidth_Impl(lua_State* state) {
    NTempest::CRect screen{};
    GxCapsScreenSize(&screen);
    lua_pushnumber(state, std::max(1024.0f, screen.r - screen.l));
    return 1;
}

int Script_GetScreenHeight_Impl(lua_State* state) {
    NTempest::CRect screen{};
    GxCapsScreenSize(&screen);
    lua_pushnumber(state, std::max(768.0f, screen.b - screen.t));
    return 1;
}

int Script_ReturnZero(lua_State* state) {
    lua_pushnumber(state, 0.0);
    return 1;
}

int Script_ReturnNil(lua_State* state) {
    lua_pushnil(state);
    return 1;
}

int Script_ReturnEmpty(lua_State* state) {
    lua_pushstring(state, "");
    return 1;
}

int Script_NoResult(lua_State*) { return 0; }

int Script_ReturnThreeZero(lua_State* state) {
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    return 3;
}

int Script_ReturnTwoZero(lua_State* state) {
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    return 2;
}

int Script_ReturnFiveZero(lua_State* state) {
    for (int index = 0; index < 5; ++index) lua_pushnumber(state, 0.0);
    return 5;
}

std::uint64_t ResolveUnitGuid(const char* unit) {
    if (!unit || !*unit) return 0U;
    if (_stricmp(unit, "player") == 0)
        return ClntObjMgrGetActivePlayer();
    if (_stricmp(unit, "target") == 0) {
        std::uint64_t guid = 0U;
        ClientRuntime_GetSelectedTarget(&guid);
        return guid;
    }
    return 0U;
}

const CLNT_VISIBLE_OBJECT* ResolveUnit(lua_State* state, int argument) {
    if (!lua_isstring(state, argument)) return nullptr;
    return ClntObjMgrGetVisibleObject(
        ResolveUnitGuid(lua_tostring(state, argument)));
}

int PushScriptTruth(lua_State* state, bool value) {
    if (value) lua_pushnumber(state, 1.0);
    else lua_pushnil(state);
    return 1;
}

bool UnitsAreFriendly(const CLNT_VISIBLE_OBJECT* left,
                      const CLNT_VISIBLE_OBJECT* right) {
    if (!left || !right) return false;
    if (left->guid == right->guid) return true;
    return left->faction != 0U && left->faction == right->faction;
}

std::uint32_t UnitPower(const CLNT_VISIBLE_OBJECT* unit, bool maximum) {
    if (!unit) return 0U;
    const std::size_t index = std::min<std::size_t>(unit->powerType, 3U);
    std::uint32_t value = maximum ? unit->maxPower[index] : unit->power[index];
    if (unit->powerType == 1U) value /= 10U;
    return value;
}

int Script_UnitExists_Impl(lua_State* state) {
    return PushScriptTruth(state, ResolveUnit(state, 1) != nullptr);
}

int Script_UnitIsUnit_Impl(lua_State* state) {
    const std::uint64_t left = ResolveUnitGuid(luaL_checkstring(state, 1));
    const std::uint64_t right = ResolveUnitGuid(luaL_checkstring(state, 2));
    return PushScriptTruth(state, left != 0U && left == right);
}

int Script_UnitIsPlayer_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    return PushScriptTruth(state, unit && unit->objectType == 4U);
}

int Script_UnitIsEnemy_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* left = ResolveUnit(state, 1);
    const CLNT_VISIBLE_OBJECT* right = ResolveUnit(state, 2);
    return PushScriptTruth(state, left && right && !UnitsAreFriendly(left, right));
}

int Script_UnitIsFriend_Impl(lua_State* state) {
    return PushScriptTruth(state,
        UnitsAreFriendly(ResolveUnit(state, 1), ResolveUnit(state, 2)));
}

int Script_UnitReaction_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* left = ResolveUnit(state, 1);
    const CLNT_VISIBLE_OBJECT* right = ResolveUnit(state, 2);
    if (!left || !right) lua_pushnil(state);
    else lua_pushnumber(state, UnitsAreFriendly(left, right) ? 5.0 : 2.0);
    return 1;
}

int Script_UnitHealth_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    lua_pushnumber(state, unit ? unit->health : 0U);
    return 1;
}

int Script_UnitHealthMax_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    lua_pushnumber(state, unit ? unit->maxHealth : 0U);
    return 1;
}

int Script_UnitMana_Impl(lua_State* state) {
    lua_pushnumber(state, UnitPower(ResolveUnit(state, 1), false));
    return 1;
}

int Script_UnitManaMax_Impl(lua_State* state) {
    lua_pushnumber(state, UnitPower(ResolveUnit(state, 1), true));
    return 1;
}

int Script_UnitPowerType_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    lua_pushnumber(state, unit ? unit->powerType : 0U);
    return 1;
}

int Script_UnitIsDead_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    return PushScriptTruth(state, unit && unit->maxHealth != 0U && unit->health == 0U);
}

int Script_SetPortraitTexture_Impl(lua_State* state) {
    auto* texture = dynamic_cast<CSimpleTexture*>(
        FrameScript_GetObject(state, 1));
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 2);
    if (!texture || !unit) return 0;
    // The recovered SetPortraitTexture renders a loaded unit model into a
    // cached portrait texture.  Its unloaded-model fallback is the matching
    // race/sex temporary portrait (or TemporaryPortrait-Monster for units),
    // not an action-bar icon.
    static constexpr const char* raceNames[] = {
        nullptr, "Human", "Orc", "Dwarf", "NightElf", "Scourge",
        "Tauren", "Gnome", "Troll"
    };
    std::string portrait = "Interface\\CharacterFrame\\TemporaryPortrait-Monster";
    if (unit->objectType == 4U && unit->race < std::size(raceNames) &&
        raceNames[unit->race]) {
        portrait = "Interface\\CharacterFrame\\TemporaryPortrait-";
        portrait += unit->sex == 1U ? "Female-" : "Male-";
        portrait += raceNames[unit->race];
    }
    texture->SetTexture(portrait.c_str());
    texture->SetTexCoord(0.0F, 1.0F, 0.0F, 1.0F);
    std::fprintf(stderr,
                 "UNIT_PORTRAIT guid=%llu display=%u file=%s\n",
                 static_cast<unsigned long long>(unit->guid), unit->displayId,
                 portrait.c_str());
    return 0;
}

int Script_TargetUnit_Impl(lua_State* state) {
    const std::uint64_t guid = ResolveUnitGuid(luaL_checkstring(state, 1));
    if (guid != 0U && ClntObjMgrGetVisibleObject(guid))
        CGGameUI::Target(guid);
    return 0;
}

int Script_ClearTarget_Impl(lua_State* state) {
    std::uint64_t previous = 0U;
    const bool hadTarget = ClientRuntime_GetSelectedTarget(&previous) != 0;
    CGGameUI::Target(0U);
    return PushScriptTruth(state, hadTarget);
}

int Script_GetScreenResolutions_Impl(lua_State* state) {
    for (const char* resolution : kScreenResolutions)
        lua_pushstring(state, resolution);
    return static_cast<int>(std::size(kScreenResolutions));
}

int Script_UnitRace_Impl(lua_State* state) {
    if (!lua_isstring(state, 1) || !CGlueMgr::m_characterInfo) {
        lua_pushnil(state);
        return 1;
    }
    static constexpr const char* raceNames[] = {
        nullptr, "Human", "Orc", "Dwarf", "Night Elf",
        "Undead", "Tauren", "Gnome", "Troll"
    };
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    const unsigned race = unit && unit->race ? unit->race
        : CGlueMgr::m_characterInfo->raceID;
    lua_pushstring(state, race < std::size(raceNames) && raceNames[race]
        ? raceNames[race] : "Human");
    return 1;
}

int Script_UnitClass_Impl(lua_State* state) {
    static constexpr const char* classNames[] = {
        nullptr, "Warrior", "Paladin", "Hunter", "Rogue", "Priest",
        nullptr, "Shaman", "Mage", "Warlock", nullptr, "Druid"
    };
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    const unsigned classID = unit && unit->classId ? unit->classId
        : (CGlueMgr::m_characterInfo ? CGlueMgr::m_characterInfo->classID : 0U);
    lua_pushstring(state, classID < std::size(classNames) && classNames[classID]
        ? classNames[classID] : "Warrior");
    return 1;
}

int Script_UnitLevel_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    lua_pushnumber(state, unit ? unit->level : (CGlueMgr::m_characterInfo
        ? CGlueMgr::m_characterInfo->experienceLevel : 0U));
    return 1;
}

std::uint32_t ExperienceToNextLevel(std::uint32_t level) {
    if (level == 0U) return 0U;
    const std::uint32_t difference = level > 31U ? 5U * (level - 30U)
        : level == 31U ? 6U : level == 30U ? 3U : level == 29U ? 1U : 0U;
    const std::uint32_t raw = (8U * level + difference) * (45U + 5U * level) + 1U;
    return ((raw + 50U) / 100U) * 100U;
}

int Script_UnitXP_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    std::fprintf(stderr, "UNIT_XP unit=%p level=%u xp=%u next=%u\n",
                 static_cast<const void*>(unit), unit ? unit->level : 0U,
                 unit ? unit->experience : 0U,
                 unit ? unit->nextLevelExperience : 0U);
    lua_pushnumber(state, unit ? unit->experience : 0U);
    return 1;
}

int Script_UnitXPMax_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    const std::uint32_t maximum = unit && unit->nextLevelExperience != 0U
        ? unit->nextLevelExperience
        : ExperienceToNextLevel(unit ? unit->level : 0U);
    std::fprintf(stderr, "UNIT_XP_MAX unit=%p level=%u value=%u packet=%u\n",
                 static_cast<const void*>(unit), unit ? unit->level : 0U,
                 maximum, unit ? unit->nextLevelExperience : 0U);
    lua_pushnumber(state, maximum);
    return 1;
}

int Script_GetPlayerBuff_Impl(lua_State* state) {
    lua_pushnumber(state, -1.0);
    return 1;
}

int Script_PlayerHasSpells_Impl(lua_State* state) {
    if (CGActionBar::HasKnownSpells()) lua_pushnumber(state, 1.0);
    else lua_pushnil(state);
    return 1;
}

int Script_GetInventorySlotInfo_Impl(lua_State* state) {
    struct InventorySlot { const char* name; int id; };
    static constexpr InventorySlot slots[] = {
        {"AmmoSlot", 0}, {"HeadSlot", 1}, {"NeckSlot", 2},
        {"ShoulderSlot", 3}, {"ShirtSlot", 4}, {"ChestSlot", 5},
        {"WaistSlot", 6}, {"LegsSlot", 7}, {"FeetSlot", 8},
        {"WristSlot", 9}, {"HandsSlot", 10}, {"Finger0Slot", 11},
        {"Finger1Slot", 12}, {"Trinket0Slot", 13}, {"Trinket1Slot", 14},
        {"BackSlot", 15}, {"MainHandSlot", 16}, {"SecondaryHandSlot", 17},
        {"RangedSlot", 18}, {"TabardSlot", 19}, {"Bag0Slot", 20},
        {"Bag1Slot", 21}, {"Bag2Slot", 22}, {"Bag3Slot", 23},
    };
    const char* requested = lua_tostring(state, 1);
    for (const auto& slot : slots) {
        if (requested && _stricmp(requested, slot.name) == 0) {
            lua_pushnumber(state, slot.id);
            lua_pushstring(state, "");
            return 2;
        }
    }
    lua_pushnumber(state, -1.0);
    lua_pushstring(state, "");
    return 2;
}

int Script_UnitName_Impl(lua_State* state) {
    const CLNT_VISIBLE_OBJECT* unit = ResolveUnit(state, 1);
    const char* name = "";
    if (unit && unit->objectType == 3U) {
        if (const CreatureStats_C* record =
                g_creatureDBCache.GetRecord(static_cast<int>(unit->entryId)))
            name = record->m_name[0];
    } else if (unit && unit->objectType == 4U &&
               unit->guid != ClntObjMgrGetActivePlayer()) {
        if (const NameCache* record = g_nameDBCache.GetRecord(unit->guid))
            name = record->m_name;
    } else if (CGlueMgr::m_characterInfo) {
        name = CGlueMgr::m_characterInfo->name;
    }
    lua_pushstring(state, name);
    return 1;
}

int Script_GetCVar_Impl(lua_State* state) {
    const char* name = lua_tostring(state, 1);
    const CVar* variable = CVar::Lookup(name);
    lua_pushstring(state, variable ? variable->StringValue() : "0");
    return 1;
}

int Script_SetCVar_Impl(lua_State* state) {
    const char* name = luaL_checkstring(state, 1);
    const char* value = luaL_checkstring(state, 2);
    if (CVar* variable = CVar::Lookup(name)) variable->Set(value);
    return 0;
}

int Script_GetCurrentResolution_Impl(lua_State* state) {
    const CVar* variable = CVar::Lookup("gxResolution");
    const char* current = variable ? variable->StringValue() : "1280x720";
    for (std::size_t index = 0; index < std::size(kScreenResolutions); ++index) {
        if (_stricmp(current, kScreenResolutions[index]) == 0) {
            lua_pushnumber(state, static_cast<double>(index + 1));
            return 1;
        }
    }
    lua_pushnumber(state, 4.0);
    return 1;
}

int Script_SetScreenResolution_Impl(lua_State* state) {
    const int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    if (index >= 0 && index < static_cast<int>(std::size(kScreenResolutions))) {
        if (CVar* variable = CVar::Lookup("gxResolution"))
            variable->Set(kScreenResolutions[index]);
    }
    return 0;
}

int Script_GetGamma_Impl(lua_State* state) {
    const CVar* variable = CVar::Lookup("Gamma");
    lua_pushnumber(state, variable ? variable->FloatValue() : 1.0);
    return 1;
}

int Script_SetGamma_Impl(lua_State* state) {
    const double gamma = luaL_checknumber(state, 1);
    char value[32]{};
    std::snprintf(value, sizeof(value), "%.6f", gamma);
    if (CVar* variable = CVar::Lookup("Gamma")) variable->Set(value);
    return 0;
}

int Script_GetLootMethod_Impl(lua_State* state) {
    lua_pushstring(state, "freeforall");
    lua_pushnil(state);
    return 2;
}

int Script_GetSkillByIndex_Impl(lua_State* state) {
    lua_pushstring(state, "");
    for (int index = 0; index < 4; ++index) lua_pushnumber(state, 0.0);
    return 5;
}

int Script_Quit_Impl(lua_State*) {
    EventInitiateShutdown();
    return 0;
}

int Script_GetNetStats_Impl(lua_State* state) {
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    return 4;
}

int Script_BankButtonIDToInvSlotID_Impl(lua_State* state) {
    const int button = static_cast<int>(lua_tonumber(state, 1));
    lua_pushnumber(state, 39 + (button > 0 ? button : 1));
    return 1;
}

int Script_IsShiftKeyDown_Impl(lua_State* state) {
    lua_pushboolean(state, EventIsKeyDown(KEY_SHIFT) != 0);
    return 1;
}

int Script_GetDefaultLanguage_Impl(lua_State* state) {
    lua_pushstring(state, "Common");
    return 1;
}

int Script_GetMapContinents_Impl(lua_State* state) {
    lua_pushstring(state, "Kalimdor");
    lua_pushstring(state, "Azeroth");
    return 2;
}

int Script_AcceptResurrect_Impl(lua_State*) {
    Script_AcceptResurrect();
    return 0;
}

int Script_DeclineResurrect_Impl(lua_State*) {
    Script_DeclineResurrect();
    return 0;
}

int Script_RepopMe_Impl(lua_State*) {
    Script_RepopMe();
    return 0;
}

int Script_GetReleaseTimeRemaining_Impl(lua_State* state) {
    // The recovered x86 call pushes the double 300.0 (high dword 0x4072C000).
    // StaticPopup.lua uses this as the five-minute automatic release timeout.
    lua_pushnumber(state, 300.0);
    return 1;
}

struct NativeScriptFunction {
    const char* name;
    lua_CFunction function;
};

const NativeScriptFunction kNativeScriptFunctions[] = {
    {"GetTime", Script_GetTime_Impl},
    {"GetGameTime", Script_GetGameTime_Impl},
    {"GetScreenWidth", Script_GetScreenWidth_Impl},
    {"GetScreenHeight", Script_GetScreenHeight_Impl},
    {"GetPlayerBuffTimeLeft", Script_ReturnZero},
    {"GetPlayerBuff", Script_GetPlayerBuff_Impl},
    {"UnitMoney", Script_ReturnZero},
    {"GetInventorySlotInfo", Script_GetInventorySlotInfo_Impl},
    {"GetActionTexture", Script_ReturnNil},
    {"BankButtonIDToInvSlotID", Script_BankButtonIDToInvSlotID_Impl},
    {"UnitName", Script_UnitName_Impl},
    {"HideSellCursor", Script_NoResult},
    {"GetBindingKey", Script_ReturnNil},
    {"GetCVar", Script_GetCVar_Impl},
    {"GetNetStats", Script_GetNetStats_Impl},
    {"SpellIsTargeting", Script_ReturnNil},
    {"IsShiftKeyDown", Script_IsShiftKeyDown_Impl},
    {"GetGuildInfo", Script_ReturnNil},
    {"UnitIsDead", Script_UnitIsDead_Impl},
    {"UnitIsUnit", Script_UnitIsUnit_Impl},
    {"UnitIsPlayer", Script_UnitIsPlayer_Impl},
    {"UnitIsEnemy", Script_UnitIsEnemy_Impl},
    {"UnitIsFriend", Script_UnitIsFriend_Impl},
    {"UnitReaction", Script_UnitReaction_Impl},
    {"UnitIsPlusMob", Script_ReturnNil},
    {"UnitIsCharmed", Script_ReturnNil},
    {"UnitIsPartyLeader", Script_ReturnNil},
    {"UnitInParty", Script_ReturnNil},
    {"SetWhoToUI", Script_NoResult},
    {"PlaySound", Script_NoResult},
    {"PlayerHasSpells", Script_PlayerHasSpells_Impl},
    {"GetZoneText", Script_ReturnEmpty},
    {"GetTabardCreationCost", Script_ReturnZero},
    {"GetReleaseTimeRemaining", Script_GetReleaseTimeRemaining_Impl},
    {"GetDefaultLanguage", Script_GetDefaultLanguage_Impl},
    {"GetBonusBarOffset", Script_ReturnZero},
    {"GetCurrentMapContinent", Script_ReturnZero},
    {"GetPetActionInfo", Script_ReturnNil},
    {"GetCurrentResolution", Script_GetCurrentResolution_Impl},
    {"GetNumBindings", Script_ReturnZero},
    {"GetMinimapZoneText", Script_ReturnEmpty},
    {"GetMapInfo", Script_ReturnNil},
    {"GetMapContinents", Script_GetMapContinents_Impl},
    {"GetLookingForGroup", Script_ReturnNil},
    {"GetSubZoneText", Script_ReturnEmpty},
    {"GetNumShapeshiftForms", Script_ReturnZero},
    {"UnitExists", Script_UnitExists_Impl},
    {"UnitHealth", Script_UnitHealth_Impl},
    {"UnitHealthMax", Script_UnitHealthMax_Impl},
    {"UnitLevel", Script_UnitLevel_Impl},
    {"GetCursorMoney", Script_ReturnZero},
    {"GetScreenResolutions", Script_GetScreenResolutions_Impl},
    {"SetPortraitTexture", Script_SetPortraitTexture_Impl},
    {"GetComboPoints", Script_ReturnZero},
    {"GetActionCount", Script_ReturnZero},
    {"GetInventoryItemTexture", Script_ReturnNil},
    {"UnitRace", Script_UnitRace_Impl},
    {"UnitClass", Script_UnitClass_Impl},
    {"GetPetActionCooldown", Script_ReturnThreeZero},
    {"PetHasActionBar", Script_ReturnNil},
    {"GetMapZones", Script_NoResult},
    {"GetNumMapLandmarks", Script_ReturnZero},
    {"GetQuestTimers", Script_NoResult},
    {"GetNumLaguages", Script_ReturnZero},
    {"GetNumLanguages", Script_ReturnZero},
    {"SetCursor", Script_NoResult},
    {"SpellCanTargetUnit", Script_UnitExists_Impl},
    {"GetPlayerTradeMoney", Script_ReturnZero},
    {"HasAction", Script_ReturnNil},
    {"GetCurrentMapZone", Script_ReturnZero},
    {"UnitMana", Script_UnitMana_Impl},
    {"UnitManaMax", Script_UnitManaMax_Impl},
    {"GetInventoryItemCooldown", Script_ReturnThreeZero},
    {"IsInventoryItemLocked", Script_ReturnNil},
    {"IsAttackAction", Script_ReturnNil},
    {"IsCurrentAction", Script_ReturnNil},
    {"IsUsableAction", Script_ReturnNil},
    {"GetActionCooldown", Script_ReturnThreeZero},
    {"GetTargetTradeMoney", Script_ReturnZero},
    {"UnitPowerType", Script_UnitPowerType_Impl},
    {"UnitXP", Script_UnitXP_Impl},
    {"UnitXPMax", Script_UnitXPMax_Impl},
    {"UnitCharacterPoints", Script_ReturnTwoZero},
    {"GetNumPartyMembers", Script_ReturnZero},
    {"GetPartyMember", Script_ReturnNil},
    {"GetPartyLeaderIndex", Script_ReturnZero},
    {"IsPartyLeader", Script_ReturnNil},
    {"GetLootMethod", Script_GetLootMethod_Impl},
    {"GetSkillLineInfo", Script_ReturnFiveZero},
    {"GetSkillByIndex", Script_GetSkillByIndex_Impl},
    {"SpellStopTargeting", Script_ReturnNil},
    {"ClearTarget", Script_ClearTarget_Impl},
    {"TargetUnit", Script_TargetUnit_Impl},
    {"SetCVar", Script_SetCVar_Impl},
    {"GetGamma", Script_GetGamma_Impl},
    {"SetGamma", Script_SetGamma_Impl},
    {"SetScreenResolution", Script_SetScreenResolution_Impl},
    {"Logout", Script_NoResult},
    {"Quit", Script_Quit_Impl},
    {"Jump", Script_Jump},
    {"AcceptResurrect", Script_AcceptResurrect_Impl},
    {"DeclineResurrect", Script_DeclineResurrect_Impl},
    {"RepopMe", Script_RepopMe_Impl},
};

std::array<const char*, 0x177> BuildGameScriptEvents() {
    std::array<const char*, 0x177> events{};
    events[0xA0] = "UNIT_HEALTH";
    events[0xA1] = "UNIT_MAXHEALTH";
    events[0xA2] = "UNIT_MANA";
    events[0xA3] = "UNIT_MAXMANA";
    events[0xA4] = "UNIT_DISPLAYPOWER";
    events[0xA5] = "UNIT_LEVEL";
    events[0xA6] = "UNIT_FACTION";
    events[0xB4] = "UNIT_NAME_UPDATE";
    events[0xB5] = "UNIT_PORTRAIT_UPDATE";
    events[0xB6] = "UNIT_MODEL_CHANGED";
    events[0xB7] = "UNIT_INVENTORY_CHANGED";
    events[0xB8] = "ITEM_LOCK_CHANGED";
    events[0xB9] = "PLAYER_XP_UPDATE";
    events[0xC9] = "ACTIONBAR_SHOWGRID";
    events[0xCA] = "ACTIONBAR_HIDEGRID";
    events[0xCB] = "ACTIONBAR_PAGE_CHANGED";
    events[0xCC] = "ACTIONBAR_SLOT_CHANGED";
    events[0xCD] = "ACTIONBAR_UPDATE_STATE";
    events[0xCE] = "ACTIONBAR_UPDATE_USABLE";
    events[0xCF] = "ACTIONBAR_UPDATE_COOLDOWN";
    events[0xD0] = "UPDATE_BONUS_ACTIONBAR";
    static constexpr const char* chatEvents[] = {
        "CHAT_MSG_SAY", "CHAT_MSG_PARTY", "CHAT_MSG_GUILD",
        "CHAT_MSG_OFFICER", "CHAT_MSG_YELL", "CHAT_MSG_WHISPER",
        "CHAT_MSG_WHISPER_INFORM", "CHAT_MSG_EMOTE",
        "CHAT_MSG_TEXT_EMOTE", "CHAT_MSG_SYSTEM", "CHAT_MSG_MONSTER_SAY",
        "CHAT_MSG_MONSTER_YELL", "CHAT_MSG_MONSTER_EMOTE",
        "CHAT_MSG_CHANNEL", "CHAT_MSG_CHANNEL_JOIN",
        "CHAT_MSG_CHANNEL_LEAVE", "CHAT_MSG_CHANNEL_LIST",
        "CHAT_MSG_CHANNEL_NOTICE", "CHAT_MSG_CHANNEL_NOTICE_USER",
        "CHAT_MSG_AFK", "CHAT_MSG_DND", "CHAT_MSG_COMBAT_LOG",
        "CHAT_MSG_IGNORED", "CHAT_MSG_SKILL", "CHAT_MSG_LOOT"
    };
    for (std::size_t index = 0; index < std::size(chatEvents); ++index)
        events[0xD9 + index] = chatEvents[index];
    events[0xF4] = "SPELLS_CHANGED";
    events[0xBF] = "PLAYER_TARGET_CHANGED";
    events[0xC5] = "MINIMAP_ZONE_CHANGED";
    events[0xC6] = "MINIMAP_UPDATE_ZOOM";
    events[0x152] = "CHAT_MSG_COMBAT_LOG_ENEMY";
    events[0x153] = "CHAT_MSG_COMBAT_LOG_SELF";
    events[0x154] = "CHAT_MSG_COMBAT_LOG_PARTY";
    events[0x155] = "CHAT_MSG_COMBAT_LOG_ERROR";
    events[0x156] = "CHAT_MSG_COMBAT_LOG_MISC_INFO";
    events[0xFD] = "PLAYER_ENTERING_WORLD";
    events[0xFE] = "PLAYER_LEAVING_WORLD";
    events[0xFF] = "PLAYER_ALIVE";
    events[0x100] = "PLAYER_DEAD";
    events[0x101] = "PLAYER_CAMPING";
    events[0x102] = "PLAYER_QUITING";
    events[0x103] = "PLAYER_STAND";
    events[0x104] = "PLAYER_SIT";
    events[0x105] = "RESURRECT_REQUEST";
    events[0x130] = "BAG_OPEN";
    events[0x131] = "BAG_UPDATE";
    events[0x132] = "BAG_CLOSED";
    events[0x133] = "BAG_UPDATE_COOLDOWN";
    return events;
}

void RegisterNativeScriptFunctions() {
    for (const auto& function : kNativeScriptFunctions)
        FrameScript_RegisterFunction(function.name, function.function);
}
}

void CGGameUI::AddErrorMessage() { }
void CGGameUI::AddPartyMember() { }
void CGGameUI::AssistByName() { }
void CGGameUI::BeginCinematic() { }
void CGGameUI::BeginCinematicInternal() { }
void CGGameUI::CancelGuildInvite() { }
void CGGameUI::CancelPartyInvite() { }
void CGGameUI::ClearClientControls() { }
void CGGameUI::ClearCursor() { }
void CGGameUI::ClearInteractTarget() { }
void CGGameUI::ClearLootSlot() { }
void CGGameUI::ClearTarget() { Target(0U); }
void CGGameUI::CloseInteraction() { }
void CGGameUI::CloseLoot() { }
void CGGameUI::ClosestObjectMatch() { }
void CGGameUI::DeleteCursorItem() { }
void CGGameUI::DisplayError() { }
void CGGameUI::DropCursorPetAction() { }
void CGGameUI::DropCursorSpell() { }
void CGGameUI::EnablePartyMember() { }
void CGGameUI::EnterWorld() {
    // SCRIPT_PLAYER_ENTERING_WORLD (0xFD) is the recovered event emitted after
    // the active player object finishes initialization.
    CGActionBar::EnterWorld();
    CGCharacterInfo::EnterWorld();
    CGContainerInfo::EnterWorld();
    FrameScript_SignalEvent(0xFD);
    CGMinimapFrame::Initialize(static_cast<int>(ClntObjMgrGetMapID()));
}
void CGGameUI::FilterMouseDown() { }
void CGGameUI::FollowByName() { }
void CGGameUI::GetCursorItem() { }
void CGGameUI::GetCursorSpell() { }
void CGGameUI::GetCursorVirtualItem() { }
void CGGameUI::GetLastErrorString() { }
void CGGameUI::GetPartyMember() { }
void CGGameUI::HandleDisplaySizeChanged() { }
int CGGameUI::HandleMouseDown() { return 0; }
int CGGameUI::HandleMouseUp() { return 0; }
void CGGameUI::HandleObjectTrackChange() { }
void CGGameUI::HandleScreenshot() { }
void CGGameUI::HandleSpriteClick() { }
void CGGameUI::HandleSpriteTrack() { }
void CGGameUI::HandleTerrainClick() { }
void CGGameUI::HandleWorldClick() { }
void CGGameUI::HideCursor() { }
void CGGameUI::Idle() { }
void CGGameUI::Initialize() {
    if (CSimpleTop::s_instance)
        delete CSimpleTop::s_instance;
    new CSimpleTop;

    FrameScript_Flush();
    FrameScript_LoadTextTables("Interface\\FrameXML\\GlobalStrings.lua");
    RegisterNativeScriptFunctions();
    ActionBarRegisterScriptFunctions();
    CharacterInfoRegisterScriptFunctions();
    ContainerRegisterScriptFunctions();
    ChatRegisterScriptFunctions();
    CGTooltip::RegisterScriptMethods();
    CGCharacterModelBase::RegisterScriptMethods();
    CGMinimapFrame::RegisterScriptMethods();
    InputControlRegisterScriptFunctions();
    CameraRegisterScriptFunctions();
    UIBindingsRegisterScriptFunctions();
    static const auto gameEvents = BuildGameScriptEvents();
    FrameScript_CreateEvents(gameEvents.data(), static_cast<unsigned>(gameEvents.size()));

    // The recovered CGGameUI::Initialize sequence initializes and loads the
    // binding manager after FrameScript_Flush/CreateEvents and before loading
    // FrameXML.  Rebuild its compiled Bindings.xml functions against this Lua
    // state; retaining references from the glue state leaves every gameplay
    // key command inert.
    CGUIBindings::Get()->Initialize();

    CStatus status;
    const int loaded = FrameXML_CreateFrames("Interface\\FrameXML\\FrameXML.toc", &status);
    if (!loaded || !status.IsEmpty())
        status.Display();
    std::fprintf(stderr, "GAMEUI_INITIALIZE frames=%d statusEmpty=%d\n",
                 loaded, status.IsEmpty() ? 1 : 0);
}
void CGGameUI::InitializeGame() {
    // The original initializes each gameplay UI module before constructing the
    // common FrameXML tree.  Those modules retain their own recovered entry
    // points; the shared tree is the required ownership/presentation handoff.
    CGActionBar::InitializeGame();
    CGCharacterInfo::InitializeGame();
    Initialize();
}
void CGGameUI::IsPartyMember() { }
void CGGameUI::LeaveWorld() { }
void CGGameUI::LockItem() { }
void CGGameUI::NamePlateClicked() { }
void CGGameUI::NewZoneFeedback() { }
void CGGameUI::NextCinematic() { }
void CGGameUI::NextCinematicInternal() { }
void CGGameUI::OnClientControlChanged() { }
void CGGameUI::OnItemPush() { }
void CGGameUI::OnSpriteLeftClick() { }
void CGGameUI::OnSpriteRightClick() { }
void CGGameUI::OnTargetContextAction() { }
void CGGameUI::OnTerrainClick() { }
void CGGameUI::OpenGuildInvite() { }
void CGGameUI::OpenLoot() { }
void CGGameUI::OpenPartyInvite() { }
void CGGameUI::OpenResurrectRequest(const char* name) {
    if (name && *name)
        FrameScript_SignalEvent(0x105U, "%s", name);
}
void CGGameUI::PlayerCombatModeChanged() { }
void CGGameUI::RegisterFrameFactories() {
    FrameXML_RegisterFactory("WorldFrame", CreateWorldFrame);
    FrameXML_RegisterFactory("GameTooltip", CreateTooltip);
    FrameXML_RegisterFactory("Minimap", CreateMinimap);
    FrameXML_RegisterFactory("PlayerModel", CreatePlayerModel);
    FrameXML_RegisterFactory("TabardModel", CreateTabardModel);
}
void CGGameUI::Reload() { }
void CGGameUI::RemoveAllPartyMembers() { }
void CGGameUI::ResetCamera() { }
void CGGameUI::ScaleUI() { }
void CGGameUI::SetCursorItem() { }
void CGGameUI::SetCursorMoney() { }
void CGGameUI::SetCursorPetAction() { }
void CGGameUI::SetCursorSpell() { }
void CGGameUI::SetCursorVirtualItem() { }
void CGGameUI::SetInteractTarget() { }
void CGGameUI::SetLootMethod() { }
void CGGameUI::SetMinimapZoneText() { }
void CGGameUI::SetPartyLeader() { }
void CGGameUI::ShowAutoFollowChange() { }
void CGGameUI::ShowCombatFeedback() { }
void CGGameUI::ShowCursor() { }
void CGGameUI::ShowHealingFeedback() { }
void CGGameUI::ShowSpellMissFeedback() { }
void CGGameUI::Shutdown() { }
void CGGameUI::ShutdownGame() { }
void CGGameUI::StartCinematic() { }
void CGGameUI::StartCinematicCamera() { }
void CGGameUI::StopCinematic() { }
void CGGameUI::StopCinematicInternal() { }
void CGGameUI::SysMsgDisplay() { }
void CGGameUI::Target(std::uint64_t guid, bool) {
    std::uint64_t previous = 0U;
    ClientRuntime_GetSelectedTarget(&previous);
    if (previous == guid) return;

    CDataStore message;
    message.Put<std::uint32_t>(0x130U)->Put<std::uint64_t>(guid);
    ClientServices_Send(&message);
    ClientRuntime_SetSelectedTarget(guid);
    FrameScript_SignalEvent(0xBFU);
    std::fprintf(stderr, "GAMEUI_TARGET old=%llu new=%llu\n",
                 static_cast<unsigned long long>(previous),
                 static_cast<unsigned long long>(guid));
}
void CGGameUI::TargetNearestEnemy() { }
void CGGameUI::UnitNameUpdate() { }
void CGGameUI::UnitPortraitUpdate() { }
void CGGameUI::UnlockAllItems() { }
void CGGameUI::UnlockItem() { }
void CGGameUI::UpdateActivePlayer() { }
void CGGameUI::UpdateInteractTarget() { }
void CGGameUI::UpdateObjectHighlightColor() { }
void CGTabardModelFrame::Create() { }
GAMEERRORDESC::GAMEERRORDESC() { }
void CCommand_ScaleUI() { }
void CCommand_Script() { }
void ClosestObjectMatchProc() { }
void GetCinematicStartingCameraPosition() { }
void ItemPushItemStatsCallback() { }
void LoadPlacedFrames() { }
void LoadScriptFunctions_0() {
    RegisterNativeScriptFunctions();
}
void PlaceFrame() { }
void PlacedFrameCallback() { }
void QSortCompareNearestEnemy() { }
void SavePlacedFrames() { }
void Script_AcceptGroup() { }
void Script_AcceptGuild() { }
void Script_AcceptResurrect() {
    CGPlayer_C player;
    player.AcceptResurrectRequest(true);
}
void Script_AssistByName() { }
void Script_AssistUnit() { }
void Script_AttackTarget() { }
void Script_AutoEquipCursorItem() { }
void Script_BeginTrade() { }
void Script_CancelLogout() { }
void Script_CancelPendingEquip() { }
void Script_CancelTrade() { }
void Script_CheckInteractDistance() { }
void Script_ClearTarget() { }
void Script_CursorHasItem() { }
void Script_CursorHasMoney() { }
void Script_CursorHasSpell() { }
void Script_DeclineGroup() { }
void Script_DeclineGuild() { }
void Script_DeclineResurrect() {
    CGPlayer_C player;
    player.AcceptResurrectRequest(false);
}
void Script_DeleteCursorItem() { }
void Script_DropCursorMoney() { }
void Script_EquipCursorItem() { }
void Script_EquipPendingItem() { }
void Script_FollowByName() { }
void Script_FollowUnit() { }
void Script_ForceLogout() { }
void Script_ForceQuit() { }
void Script_FrameXML_Debug() { }
void Script_GetBindZone() { }
void Script_GetBuildVersion() { }
void Script_GetCVar() { }
void Script_GetCurrentPosition() { }
void Script_GetCurrentResolution() { }
void Script_GetCursorMoney() { }
void Script_GetCursorPosition() { }
void Script_GetDamageBonusStat() { }
void Script_GetDate() { }
void Script_GetDebugStats() { }
void Script_GetDoodadAnim() { }
void Script_GetFarclip() { }
void Script_GetFramerate() { }
void Script_GetGamma() { }
void Script_GetMinimapZoneText() { }
void Script_GetNetStats() { }
void Script_GetReleaseTimeRemaining() { }
void Script_GetScreenHeight() { }
void Script_GetScreenResolutions() { }
void Script_GetScreenWidth() { }
void Script_GetSubZoneText() { }
void Script_GetTerrainMip() { }
void Script_GetTexLodBias() { }
void Script_GetWaterDetail() { }
void Script_GetWorldDetail() { }
void Script_GetZoneText() { }
void Script_GuildDemoteByName() { }
void Script_GuildDisband() { }
void Script_GuildInfo() { }
void Script_GuildInviteByName() { }
void Script_GuildLeave() { }
void Script_GuildPromoteByName() { }
void Script_GuildRoster() { }
void Script_GuildSetLeaderByName() { }
void Script_GuildSetMOTD() { }
void Script_GuildUninviteByName() { }
void Script_HasSoulstone() { }
void Script_HideNameplates() { }
void Script_HideSellCursor() { }
void Script_InitiateTrade() { }
void Script_InviteByName() { }
void Script_InviteToParty() { }
void Script_IsAltKeyDown() { }
void Script_IsControlKeyDown() { }
void Script_IsShiftKeyDown() { }
int Script_Jump(lua_State* state) {
    const unsigned timestamp = state && lua_isnumber(state, 1)
        ? static_cast<unsigned>(lua_tonumber(state, 1))
        : OsGetAsyncTimeMs();
    CWorld::Jump(timestamp);
    return 0;
}
void Script_Logout() { }
void Script_NotifyInspect() { }
void Script_OpeningCinematic() { }
void Script_PVPPort() { }
void Script_PickupPlayerMoney() { }
void Script_PromoteByName() { }
void Script_PromoteToPartyLeader() { }
void Script_Quit() { }
void Script_RandomRoll() { }
void Script_ReloadUI() { }
void Script_RepopMe() {
    CGPlayer_C player;
    player.HandleRepopRequest();
}
void Script_ReportBug() { }
void Script_ReportNote() { }
void Script_ReportSuggestion() { }
void Script_RequestTimePlayed() { }
void Script_ResetPerformanceValues() { }
void Script_RunScript() { }
void Script_Screenshot() { }
void Script_SetCVar() { }
void Script_SetCursor() { }
void Script_SetDoodadAnim() { }
void Script_SetFarclip() { }
void Script_SetGamma() { }
void Script_SetLayoutMode() { }
void Script_SetScreenResolution() { }
void Script_SetTerrainMip() { }
void Script_SetTexLodBias() { }
void Script_SetWaterDetail() { }
void Script_SetWorldDetail() { }
void Script_ShowNameplates() { }
void Script_SitOrStand() { }
void Script_SplitMoney() { }
void Script_StopCinematic() { }
void Script_Stuck() { }
void Script_TargetLastEnemy() { }
void Script_TargetNearestEnemy() { }
void Script_TargetUnit() { }
void Script_TargetUnitsPet() { }
void Script_ToggleCollision() { }
void Script_ToggleCollisionDisplay() { }
void Script_TogglePerformanceDisplay() { }
void Script_TogglePerformanceValues() { }
void Script_TogglePlayerBounds() { }
void Script_TogglePortals() { }
void Script_ToggleRun() { }
void Script_ToggleSheath() { }
void Script_ToggleTris() { }
void Script_UninviteByName() { }
void Script_UninviteFromParty() { }
void Script_UseSoulstone() { }
void TargetUpdateProc() { }
void UnloadScriptFunctions_0() { }
