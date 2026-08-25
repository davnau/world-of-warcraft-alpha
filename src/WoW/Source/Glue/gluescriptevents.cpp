#include "WoW/Source/Glue/gluescriptevents.h"
#include "WoW/Source/Glue/cgluemgr.h"

#include "ENGINE/Source/Frame/frame.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Os/W32/ossystem.h"
#include "WoW/Source/Net/netinternal.h"
#include "WoW/Source/SoundInterface/soundinterfacegluemusic.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <array>
#include <cstdlib>
#include <limits>
#include <string>
#include <utility>
#include <vector>

float INFINITY_86 = 0.0F;
float TWO_PI_86 = 0.0F;
float OO_TWO_PI_86 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_17 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_17 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_17 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_17 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_17 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_17 = 0.0F;
float SECONDS_PER_HEARTBEAT_17 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_17 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_17 = 0.0F;

namespace {
struct GlueScriptState {
    std::array<const char*, 11> events{};
    std::array<std::string, 5> buildInfo{
        "WoW", "Alpha", "5.3", "3368", "Dec 11 2003"};
    std::vector<GlueRealmInfo> realms;
    std::string selectedAddress;
    std::string currentScreen;
    std::string lastURL;
    std::string lastAccountName;
    bool connected = false;
    bool disconnectPending = false;
    bool initialized = false;
    bool quitRequested = false;
    bool statusDialogClicked = false;
    bool realmListRequested = false;
    bool enteredWorld = false;
};

GlueScriptState g_state;

constexpr std::array<const char*, 11> kGlueEvents = {
    "SET_GLUE_SCREEN", "START_GLUE_MUSIC", "DISCONNECTED_FROM_SERVER",
    "OPEN_STATUS_DIALOG", "UPDATE_STATUS_DIALOG", "CLOSE_STATUS_DIALOG",
    "CHARACTER_LIST_UPDATE", "UPDATE_SELECTED_CHARACTER", "OPEN_REALM_LIST",
    "UPDATE_SELECTED_REALM", "SELECT_LAST_CHARACTER"};

constexpr std::array<const char*, 17> kScriptNames = {
    "GetBuildInfo", "SetCurrentScreen", "QuitGame", "PlayGlueMusic",
    "LaunchURL", "LaunchAccountCreate", "GetLastAccountName",
    "DefaultServerLogin", "StatusDialogClick", "GetServerName",
    "DisconnectFromServer", "IsConnectedToServer", "GetRealmList",
    "GetNumRealms", "GetRealmInfo", "ChangeRealm", "EnterWorld"};

template <typename Function>
int InvokeScript(lua_State* state, const char* name, Function sideEffect)
{
    sideEffect();
    return FrameScript_Invoke(state, name);
}

const GlueRealmInfo* FindRealm(std::size_t index)
{
    return index < g_state.realms.size() ? &g_state.realms[index] : nullptr;
}
}

void GlueScriptEventsInitialize()
{
    g_state.events = kGlueEvents;
    FrameScript_CreateEvents(kGlueEvents.data(), static_cast<unsigned>(kGlueEvents.size()));
    g_state.initialized = true;
}

void GlueScriptEventsRegisterFunctions()
{
    GlueScriptEventsInitialize();
    FrameScript_RegisterFunction("GetBuildInfo", Script_GetBuildInfo);
    FrameScript_RegisterFunction("SetCurrentScreen", Script_SetCurrentScreen);
    FrameScript_RegisterFunction("QuitGame", Script_QuitGame);
    FrameScript_RegisterFunction("PlayGlueMusic", Script_PlayGlueMusic);
    FrameScript_RegisterFunction("LaunchURL", Script_LaunchURL);
    FrameScript_RegisterFunction("LaunchAccountCreate", Script_LaunchAccountCreate);
    FrameScript_RegisterFunction("GetLastAccountName", Script_GetLastAccountName);
    FrameScript_RegisterFunction("DefaultServerLogin", Script_DefaultServerLogin);
    FrameScript_RegisterFunction("StatusDialogClick", Script_StatusDialogClick);
    FrameScript_RegisterFunction("GetServerName", Script_GetServerName);
    FrameScript_RegisterFunction("DisconnectFromServer", Script_DisconnectFromServer);
    FrameScript_RegisterFunction("IsConnectedToServer", Script_IsConnectedToServer);
    FrameScript_RegisterFunction("GetRealmList", Script_GetRealmList);
    FrameScript_RegisterFunction("GetNumRealms", Script_GetNumRealms);
    FrameScript_RegisterFunction("GetRealmInfo", Script_GetRealmInfo);
    FrameScript_RegisterFunction("ChangeRealm", Script_ChangeRealm);
    FrameScript_RegisterFunction("EnterWorld", Script_EnterWorld);
}

void GlueScriptEventsUnregisterFunctions()
{
    for (const char* name : kScriptNames) FrameScript_UnregisterFunction(name);
    g_state.initialized = false;
}

void GlueScriptEventsSetBuildInfo(const char* product, const char* version,
                                  const char* build, const char* date,
                                  const char* assertions)
{
    const std::array<const char*, 5> values = {product, version, build, date, assertions};
    for (std::size_t i = 0; i < values.size(); ++i)
        g_state.buildInfo[i] = values[i] ? values[i] : "";
}

void GlueScriptEventsSetRealmList(const GlueRealmInfo* realms, std::size_t count,
                                  const char* selectedAddress)
{
    g_state.realms.clear();
    if (realms && count)
        g_state.realms.assign(realms, realms + count);
    g_state.selectedAddress = selectedAddress ? selectedAddress : "";
}

void GlueScriptEventsSetConnected(bool connected)
{
    g_state.connected = connected;
    if (connected)
        g_state.disconnectPending = false;
}

const char* GlueScriptEventsGetCurrentScreen() { return g_state.currentScreen.c_str(); }
const char* GlueScriptEventsGetLastURL() { return g_state.lastURL.c_str(); }
const char* GlueScriptEventsGetLastAccountName() { return g_state.lastAccountName.c_str(); }
bool GlueScriptEventsIsDisconnectPending() { return g_state.disconnectPending; }

int Script_GetBuildInfo(lua_State* state)
{
    for (const std::string& value : g_state.buildInfo) lua_pushstring(state, value.c_str());
    return static_cast<int>(g_state.buildInfo.size());
}

int Script_SetCurrentScreen(lua_State* state)
{
    const char* screen = luaL_checkstring(state, 1);
    g_state.currentScreen = screen ? screen : "";
    CGlueMgr::UpdateCurrentScreen(g_state.currentScreen.c_str());
    return 0;
}

int Script_QuitGame(lua_State* state)
{
    (void)state;
    g_state.quitRequested = true;
    CGlueMgr::QuitGame();
    return 0;
}

int Script_PlayGlueMusic(lua_State*) { return 0; }

int Script_LaunchURL(lua_State* state)
{
    const char* url = luaL_checkstring(state, 1);
    g_state.lastURL = url ? url : "";
    if (!g_state.lastURL.empty()) OsLaunchURL(g_state.lastURL.c_str());
    return 0;
}

int Script_LaunchAccountCreate(lua_State* state)
{
    (void)state;
    const char* url = FrameScript_GetText(
        "ACCOUNT_CREATE_URL", -1, GENDER_NOT_APPLICABLE);
    g_state.lastURL = url ? url : "";
    if (!g_state.lastURL.empty()) OsLaunchURL(g_state.lastURL.c_str());
    return 0;
}

int Script_GetLastAccountName(lua_State* state)
{
    lua_pushstring(state, g_state.lastAccountName.c_str());
    return 1;
}

int Script_DefaultServerLogin(lua_State* state)
{
    if (!lua_isstring(state, 1) || !lua_isstring(state, 2))
        return luaL_error(state, "Usage: DefaultServerLogin(\"account\", \"password\")");
    const char* account = lua_tostring(state, 1);
    const char* password = lua_tostring(state, 2);
    if (account) {
        g_state.lastAccountName = account;
    }
    ClientServices_SetLoginCredentials(account, password);
    g_state.connected = true;
    CGlueMgr::DefaultServerLogin();
    return 0;
}

int Script_StatusDialogClick(lua_State* state)
{
    (void)state;
    g_state.statusDialogClicked = true;
    CGlueMgr::StatusDialogClick();
    return 0;
}

int Script_GetServerName(lua_State* state)
{
    lua_pushstring(state, g_state.selectedAddress.c_str());
    return 1;
}

int Script_DisconnectFromServer(lua_State* state)
{
    (void)state;
    if (ClientServices_IsConnected()) {
        g_state.disconnectPending = true;
        ClientServices_Disconnect();
    }
    g_state.connected = false;
    return 0;
}

int Script_IsConnectedToServer(lua_State* state)
{
    const bool connected = ClientServices_IsConnected() != 0;
    std::fprintf(stderr, "CHARSELECT_CONNECTED value=%d screen=%s\n",
        connected ? 1 : 0, g_state.currentScreen.c_str());
    if (connected)
        lua_pushnumber(state, 1.0);
    else
        lua_pushnil(state);
    return 1;
}

int Script_GetRealmList(lua_State* state)
{
    (void)state;
    g_state.realmListRequested = true;
    CGlueMgr::GetRealmList();
    return 0;
}

int Script_GetNumRealms(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(g_state.realms.size()));
    return 1;
}

int Script_GetRealmInfo(lua_State* state)
{
    const int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    const GlueRealmInfo* realm = index >= 0 ? FindRealm(static_cast<std::size_t>(index)) : nullptr;
    if (!realm) return 0;
    lua_pushstring(state, realm->name.c_str());
    lua_pushnumber(state, realm->players);
    return 2;
}

int Script_ChangeRealm(lua_State* state)
{
    const int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    const GlueRealmInfo* realm = index >= 0 ? FindRealm(static_cast<std::size_t>(index)) : nullptr;
    if (realm) {
        g_state.selectedAddress = realm->address;
        CGlueMgr::ChangeRealm(realm, 1);
    }
    return 0;
}

int Script_EnterWorld(lua_State* state)
{
    (void)state;
    g_state.enteredWorld = true;
    CGlueMgr::EnterWorld();
    return 0;
}

int _E2_86() { return _E1_86(); }
int _E1_86() { INFINITY_86 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_86() { _E4_86(); return 0; }
void _E4_86() { TWO_PI_86 = 3.1415927F + 3.1415927F; }
int _E8_86() { _E7_86(); return 0; }
void _E7_86() { OO_TWO_PI_86 = 1.0F / TWO_PI_86; }

#define GLUE_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
GLUE_CONST_PAIR(_E14_9, _E13_4, MAX_DUEL_DISTANCE_SQUARED_17, 100.0F)
GLUE_CONST_PAIR(_E17_7, _E16_11, MAX_TRADE_DISTANCE_SQUARED_17, 123.45679F)
GLUE_CONST_PAIR(_E20_25, _E19_12, MAX_SHOP_DISTANCE_SQUARED_17, 30.864195F)
GLUE_CONST_PAIR(_E23_27, _E22_18, MAX_INSPECT_DISTANCE_SQUARED_17, 30.864195F)
GLUE_CONST_PAIR(_E26_23, _E25_16, MAX_LOOT_DISTANCE_SQUARED_17, 25.0F)
GLUE_CONST_PAIR(_E29_22, _E28_10, MAX_BIND_DISTANCE_SQUARED_17, 100.0F)
GLUE_CONST_PAIR(_E32_20, _E31_7, MAX_SITCHAIR_DISTANCE_SQUARED_17, 0.25F)
GLUE_CONST_PAIR(_E35_20, _E34_8, MAX_SITCHAIRUSE_DISTANCE_SQUARED_17, 9.0F)
GLUE_CONST_PAIR(_E38_20, _E37_7, MAX_SHARE_DISTANCE_SQUARED_17, 10000.0F)
GLUE_CONST_PAIR(_E41_18, _E40_8, SECONDS_PER_HEARTBEAT_17, 5.0F)
GLUE_CONST_PAIR(_E44_20, _E43_7, SPELL_FOCUS_DISTANCE_SQUARED_17, 2500.0F)
GLUE_CONST_PAIR(_E47_22, _E46_5, MAX_OBJ_INTEREST_RADIUS_SQUARED_17, 10000.0F)
#undef GLUE_CONST_PAIR

int _E83_1()
{
    _E80_2();
    return _E82_2();
}

void _E80_2()
{
    if (++WowConnectionInitializer::count == 1)
        WowConnectionInitializer::Initialize();
}

int _E82_2() { return std::atexit(_E81_1); }

void _E81_1()
{
    if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0)
        WowConnectionInitializer::Destroy();
}
