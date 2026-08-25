#include "WoW/Source/UI/paperdollinfoframe.h"

#include "ENGINE/Source/Frame/framescript.h"
#include "WoW/Source/DB/DBClient/dbcacheinstances.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/UI/actionbarframe.h"

extern "C" {
#include "lauxlib.h"
}

#include <array>
#include <cstdio>
#include <cstring>

namespace {
const CLNT_ITEM_OBJECT* GetInventoryItem(lua_State* state, int unitArg,
                                         int slotArg) {
    const char* unit = lua_tostring(state, unitArg);
    if (!unit || _stricmp(unit, "player") != 0 ||
        !lua_isnumber(state, slotArg)) return nullptr;
    const int luaSlot = static_cast<int>(lua_tonumber(state, slotArg));
    return luaSlot >= 1 && luaSlot <= 23
        ? ClntObjMgrGetInventoryItem(static_cast<unsigned>(luaSlot - 1))
        : nullptr;
}
}

int Script_GetInventoryItemTexture(lua_State* state) {
    const CLNT_ITEM_OBJECT* item = GetInventoryItem(state, 1, 2);
    const char* texture = item
        ? CGActionBar::GetItemTexture(static_cast<int>(item->entryId)) : nullptr;
    if (texture) lua_pushstring(state, texture);
    else lua_pushnil(state);
    return 1;
}

int Script_GetInventoryItemCount(lua_State* state) {
    const CLNT_ITEM_OBJECT* item = GetInventoryItem(state, 1, 2);
    lua_pushnumber(state, item ? static_cast<double>(item->stackCount) : 0.0);
    return 1;
}

int Script_GetInventoryItemQuality(lua_State* state) {
    const CLNT_ITEM_OBJECT* item = GetInventoryItem(state, 1, 2);
    const ItemStats_C* stats = item
        ? g_itemDBCache.GetRecord(static_cast<int>(item->entryId)) : nullptr;
    if (stats) lua_pushnumber(state, stats->m_overallQualityID);
    else lua_pushnil(state);
    return 1;
}

int Script_GetInventoryItemLink(lua_State* state) {
    const CLNT_ITEM_OBJECT* item = GetInventoryItem(state, 1, 2);
    const ItemStats_C* stats = item
        ? g_itemDBCache.GetRecord(static_cast<int>(item->entryId)) : nullptr;
    if (!item || !stats) return 0;
    char link[1024]{};
    std::snprintf(link, sizeof(link), "|Hitem:%u|h[%s]|h",
                  item->entryId, stats->m_displayName[0]);
    lua_pushstring(state, link);
    return 1;
}

int Script_GetInventoryItemCooldown(lua_State* state) {
    const CLNT_ITEM_OBJECT* item = GetInventoryItem(state, 1, 2);
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, item ? 1.0 : 0.0);
    return 3;
}

int Script_IsInventoryItemLocked(lua_State* state) {
    const CLNT_ITEM_OBJECT* item = GetInventoryItem(state, 1, 2);
    if (item && (item->flags & 1U) != 0U) lua_pushnumber(state, 1.0);
    else lua_pushnil(state);
    return 1;
}

int Script_GetInventorySlotInfo(lua_State* state) {
    struct Slot { const char* name; int id; };
    static constexpr Slot slots[] = {
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
    for (const Slot& slot : slots) {
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

int Script_GetGuildInfo(lua_State* state) { lua_pushnil(state); return 1; }
int Script_GetSkillByIndex(lua_State* state) { lua_pushnumber(state, 0.0); return 1; }
int Script_GetSkillLineInfo(lua_State* state) {
    for (int i = 0; i < 5; ++i) lua_pushnumber(state, 0.0);
    return 5;
}
int Script_NoResult(lua_State*) { return 0; }

namespace {
struct ScriptFunction { const char* name; lua_CFunction function; };
constexpr std::array<ScriptFunction, 18> kCharacterFunctions{{
    {"CursorCanGoInSlot", Script_NoResult},
    {"GetGuildInfo", Script_GetGuildInfo},
    {"GetInventoryItemCooldown", Script_GetInventoryItemCooldown},
    {"GetInventoryItemCount", Script_GetInventoryItemCount},
    {"GetInventoryItemLink", Script_GetInventoryItemLink},
    {"GetInventoryItemQuality", Script_GetInventoryItemQuality},
    {"GetInventoryItemTexture", Script_GetInventoryItemTexture},
    {"GetInventorySlotInfo", Script_GetInventorySlotInfo},
    {"GetSkillByIndex", Script_GetSkillByIndex},
    {"GetSkillLineInfo", Script_GetSkillLineInfo},
    {"IsInventoryItemLocked", Script_IsInventoryItemLocked},
    {"PickupBagFromSlot", Script_NoResult},
    {"PickupInventoryItem", Script_NoResult},
    {"PutItemInBackpack", Script_NoResult},
    {"PutItemInBag", Script_NoResult},
    {"SetInventoryPortaitTexture", Script_NoResult},
    {"ShowInventorySellCursor", Script_NoResult},
    {"UseInventoryItem", Script_NoResult},
}};
}

void CGCharacterInfo::EnterWorld() { }
void CGCharacterInfo::GetSkillInfoByIndex() { }
void CGCharacterInfo::GetSkillOffsetFromString() { }
void CGCharacterInfo::InitializeGame() { }
void CGCharacterInfo::InstallMirrorHandlers() { }
void CGCharacterInfo::LeaveWorld() { }
void CGCharacterInfo::OrderProficiencies() { }
void CGCharacterInfo::OrderSkillLines() { }
void CGCharacterInfo::PickupBag() { }
void CGCharacterInfo::PickupItem() { }
void CGCharacterInfo::PutItemInBackpack() { }
void CGCharacterInfo::PutItemInBag() { }
void CGCharacterInfo::RemoveMirrorHandlers() { }
void CGCharacterInfo::ShutdownGame() { }
void CGCharacterInfo::UpdateAllSkillLines() { }
void CGCharacterInfo::UpdateItem() { }
void CGCharacterInfo::UseItem() { }
void CharacterInfoRegisterScriptFunctions() {
    for (const ScriptFunction& function : kCharacterFunctions)
        FrameScript_RegisterFunction(function.name, function.function);
}
void CharacterInfoUnregisterScriptFunctions() {
    for (const ScriptFunction& function : kCharacterFunctions)
        FrameScript_UnregisterFunction(function.name);
}
void FindItemSubClassRecord() { }
void GetCharBaseInfo() { }
void GetSlotFromLua() { }
void GuildNameCallback() { }
void PlayerCharacterPointsUpdateHandler() { }
void QSortCompareByCategoryAndLevel() { }
void QSortCompareProficiency() { }
