#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

struct lua_State;

// The recovered glue code exposes realm records to Lua.  These fields are the
// stable data used by the handlers in this translation unit; the remaining
// service-side fields are owned by the ClientServices implementation.
struct GlueRealmInfo {
    std::string name;
    std::string address;
    std::uint32_t players = 0;
};

void GlueScriptEventsInitialize();
void GlueScriptEventsRegisterFunctions();
void GlueScriptEventsUnregisterFunctions();

// State hooks used by the typed client-services bridge while the original Lua
// VM and glue manager are restored.  The exported Script_* entry points keep
// the recovered names and lua_State signatures.
void GlueScriptEventsSetBuildInfo(const char* product, const char* version,
                                  const char* build, const char* date,
                                  const char* assertions);
void GlueScriptEventsSetRealmList(const GlueRealmInfo* realms, std::size_t count,
                                  const char* selectedAddress = nullptr);
void GlueScriptEventsSetConnected(bool connected);
const char* GlueScriptEventsGetCurrentScreen();
const char* GlueScriptEventsGetLastURL();
const char* GlueScriptEventsGetLastAccountName();
bool GlueScriptEventsIsDisconnectPending();

int Script_GetBuildInfo(lua_State* state);
int Script_SetCurrentScreen(lua_State* state);
int Script_QuitGame(lua_State* state);
int Script_PlayGlueMusic(lua_State* state);
int Script_LaunchURL(lua_State* state);
int Script_LaunchAccountCreate(lua_State* state);
int Script_GetLastAccountName(lua_State* state);
int Script_DefaultServerLogin(lua_State* state);
int Script_StatusDialogClick(lua_State* state);
int Script_GetServerName(lua_State* state);
int Script_DisconnectFromServer(lua_State* state);
int Script_IsConnectedToServer(lua_State* state);
int Script_GetRealmList(lua_State* state);
int Script_GetNumRealms(lua_State* state);
int Script_GetRealmInfo(lua_State* state);
int Script_ChangeRealm(lua_State* state);
int Script_EnterWorld(lua_State* state);

extern float INFINITY_86;
extern float TWO_PI_86;
extern float OO_TWO_PI_86;
extern float MAX_DUEL_DISTANCE_SQUARED_17;
extern float MAX_TRADE_DISTANCE_SQUARED_17;
extern float MAX_SHOP_DISTANCE_SQUARED_17;
extern float MAX_INSPECT_DISTANCE_SQUARED_17;
extern float MAX_LOOT_DISTANCE_SQUARED_17;
extern float MAX_BIND_DISTANCE_SQUARED_17;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_17;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_17;
extern float MAX_SHARE_DISTANCE_SQUARED_17;
extern float SECONDS_PER_HEARTBEAT_17;
extern float SPELL_FOCUS_DISTANCE_SQUARED_17;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_17;

int _E2_86(); int _E1_86(); int _E5_86(); void _E4_86(); int _E8_86(); void _E7_86();
int _E14_9(); void _E13_4(); int _E17_7(); void _E16_11();
int _E20_25(); void _E19_12(); int _E23_27(); void _E22_18();
int _E26_23(); void _E25_16(); int _E29_22(); void _E28_10();
int _E32_20(); void _E31_7(); int _E35_20(); void _E34_8();
int _E38_20(); void _E37_7(); int _E41_18(); void _E40_8();
int _E44_20(); void _E43_7(); int _E47_22(); void _E46_5();
int _E83_1(); void _E80_2(); int _E82_2(); void _E81_1();

