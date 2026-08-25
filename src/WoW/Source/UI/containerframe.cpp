#include "WoW/Source/UI/containerframe.h"

#include "ENGINE/Source/Frame/framescript.h"
#include "WoW/Source/DB/DBClient/dbcacheinstances.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/UI/actionbarframe.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

extern "C" {
#include "lauxlib.h"
}

#include <array>
#include <cstdio>

namespace {
constexpr unsigned SCRIPT_BAG_OPEN = 0x130U;
constexpr unsigned SCRIPT_BAG_UPDATE = 0x131U;
constexpr unsigned SCRIPT_BAG_CLOSED = 0x132U;
constexpr unsigned SCRIPT_BAG_UPDATE_COOLDOWN = 0x133U;
constexpr unsigned CMSG_USE_ITEM = 0x00ABU;

const CLNT_ITEM_OBJECT* GetContainerItem(int container, int luaSlot) {
    // The recovered client stores the 16 backpack entries at player inventory
    // slots 23..38.  Equipped items occupy slots 0..22.
    if (container != 0 || luaSlot < 1 || luaSlot > 16) return nullptr;
    return ClntObjMgrGetInventoryItem(23U + static_cast<unsigned>(luaSlot - 1));
}
}

int Script_GetContainerNumSlots(lua_State* state) {
    if (!lua_isnumber(state, 1))
        luaL_error(state, "Usage: GetContainerNumSlots(container)");
    const int container = static_cast<int>(lua_tonumber(state, 1));
    const std::uint64_t activePlayer = ClntObjMgrGetActivePlayer();
    const double slots = activePlayer != 0U && container == 0 ? 16.0 : 0.0;
    std::fprintf(stderr,
                 "CONTAINER_NUM_SLOTS container=%d active=%llu result=%.0f\n",
                 container, static_cast<unsigned long long>(activePlayer), slots);
    lua_pushnumber(state, slots);
    return 1;
}

int Script_GetContainerItemInfo(lua_State* state) {
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        luaL_error(state, "Usage: GetContainerItemInfo(container, slot)");
    const int container = static_cast<int>(lua_tonumber(state, 1));
    const int slot = static_cast<int>(lua_tonumber(state, 2));
    const CLNT_ITEM_OBJECT* item = GetContainerItem(container, slot);
    static unsigned loggedCalls = 0U;
    if (loggedCalls < 64U) {
        std::fprintf(stderr,
                     "CONTAINER_ITEM_INFO container=%d slot=%d guid=%llu entry=%u "
                     "count=%u\n",
                     container, slot,
                     static_cast<unsigned long long>(item ? item->guid : 0U),
                     item ? item->entryId : 0U,
                     item ? item->stackCount : 0U);
        ++loggedCalls;
    }
    if (!item) return 0;

    const char* texture = CGActionBar::GetItemTexture(
        static_cast<int>(item->entryId));
    if (texture) lua_pushstring(state, texture);
    else lua_pushnil(state);
    lua_pushnumber(state, static_cast<double>(item->stackCount));
    if ((item->flags & 1U) != 0U) lua_pushnumber(state, 1.0);
    else lua_pushnil(state);
    const ItemStats_C* stats = g_itemDBCache.GetRecord(
        static_cast<int>(item->entryId));
    lua_pushnumber(state, stats && stats->m_inventoryType != 0
        ? static_cast<double>(stats->m_overallQualityID) : -1.0);
    return 4;
}

int Script_GetContainerItemLink(lua_State* state) {
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        luaL_error(state, "Usage: GetContainerItemLink(container, slot)");
    const CLNT_ITEM_OBJECT* item = GetContainerItem(
        static_cast<int>(lua_tonumber(state, 1)),
        static_cast<int>(lua_tonumber(state, 2)));
    if (!item) return 0;
    const ItemStats_C* stats = g_itemDBCache.GetRecord(
        static_cast<int>(item->entryId));
    if (!stats) return 0;
    char link[1024]{};
    std::snprintf(link, sizeof(link), "|Hitem:%u|h[%s]|h",
                  item->entryId, stats->m_displayName[0]);
    lua_pushstring(state, link);
    return 1;
}

int Script_GetContainerItemCooldown(lua_State* state) {
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        luaL_error(state, "Usage: GetContainerItemCooldown(container, slot)");
    const CLNT_ITEM_OBJECT* item = GetContainerItem(
        static_cast<int>(lua_tonumber(state, 1)),
        static_cast<int>(lua_tonumber(state, 2)));
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, item ? 1.0 : 0.0);
    return 3;
}

int Script_PickupContainerItem(lua_State*) { return 0; }
int Script_SplitContainerItem(lua_State*) { return 0; }
int Script_UseContainerItem(lua_State* state) {
    if (!lua_isnumber(state, 1) || !lua_isnumber(state, 2))
        luaL_error(state, "Usage: UseContainerItem(container, slot)");
    const int container = static_cast<int>(lua_tonumber(state, 1));
    const int luaSlot = static_cast<int>(lua_tonumber(state, 2));
    const CLNT_ITEM_OBJECT* item = GetContainerItem(container, luaSlot);
    if (!item) return 0;
    const unsigned inventorySlot = 23U + static_cast<unsigned>(luaSlot - 1);
    CDataStore message;
    message.Put(CMSG_USE_ITEM)
        ->Put(static_cast<std::uint8_t>(23U))
        ->Put(static_cast<std::uint8_t>(inventorySlot))
        ->Put(static_cast<std::uint8_t>(0U))
        ->Put(static_cast<std::uint16_t>(0U));
    ClientServices_Send(&message);
    return 0;
}
int Script_ShowContainerSellCursor(lua_State*) { return 0; }
int Script_SetBagPortaitTexture(lua_State*) { return 0; }

int Script_GetBagName(lua_State* state) {
    if (!lua_isnumber(state, 1))
        luaL_error(state, "Usage: GetBagName(container)");
    if (static_cast<int>(lua_tonumber(state, 1)) == 0 &&
        ClntObjMgrGetActivePlayer() != 0U) {
        lua_pushstring(state, "Backpack");
    } else {
        lua_pushnil(state);
    }
    return 1;
}

namespace {
struct ScriptFunction {
    const char* name;
    lua_CFunction function;
};

constexpr std::array<ScriptFunction, 10> kContainerFunctions{{
    {"GetContainerNumSlots", Script_GetContainerNumSlots},
    {"GetContainerItemInfo", Script_GetContainerItemInfo},
    {"GetContainerItemLink", Script_GetContainerItemLink},
    {"GetContainerItemCooldown", Script_GetContainerItemCooldown},
    {"PickupContainerItem", Script_PickupContainerItem},
    {"SplitContainerItem", Script_SplitContainerItem},
    {"UseContainerItem", Script_UseContainerItem},
    {"ShowContainerSellCursor", Script_ShowContainerSellCursor},
    {"SetBagPortaitTexture", Script_SetBagPortaitTexture},
    {"GetBagName", Script_GetBagName},
}};
}

void CGContainerInfo::EnterWorld() {
    FrameScript_SignalEvent(SCRIPT_BAG_UPDATE, "%d", 0);
}
void CGContainerInfo::LeaveWorld() {
    FrameScript_SignalEvent(SCRIPT_BAG_CLOSED, "%d", 0);
}
void CGContainerInfo::OpenContainer() {
    FrameScript_SignalEvent(SCRIPT_BAG_OPEN, "%d", 0);
}
void CGContainerInfo::UpdateContainers() { UpdateContents(); }
void CGContainerInfo::UpdateContents() {
    FrameScript_SignalEvent(SCRIPT_BAG_UPDATE, "%d", 0);
}
void CGContainerInfo::UpdateCooldowns() {
    FrameScript_SignalEvent(SCRIPT_BAG_UPDATE_COOLDOWN);
}
void CGContainerInfo::UpdateItem() { UpdateContents(); }

void ContainerRegisterScriptFunctions() {
    for (const ScriptFunction& function : kContainerFunctions)
        FrameScript_RegisterFunction(function.name, function.function);
}
void ContainerUnregisterScriptFunctions() {
    for (const ScriptFunction& function : kContainerFunctions)
        FrameScript_UnregisterFunction(function.name);
}

void InvUpdateHandler() { CGContainerInfo::UpdateContents(); }
void UpdateContainerContents() { CGContainerInfo::UpdateContents(); }
void UpdateInvContents() { CGContainerInfo::UpdateContents(); }
