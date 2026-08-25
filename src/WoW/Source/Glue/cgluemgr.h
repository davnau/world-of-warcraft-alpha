#pragma once

#include "ENGINE/Source/Services/sysmessage.h"

#include <array>
#include <cstdint>
#include <string>

struct GlueRealmInfo;
struct CHARACTER_CREATE_INFO;
struct CHARACTER_INFO;
struct HEVENTCONTEXT__;
using HEVENTCONTEXT = HEVENTCONTEXT__*;

enum GLUE_IDLE_STATE : std::int32_t {
    IDLE_NONE = 0,
    IDLE_ACCOUNT_LOGIN = 1,
    IDLE_CHARACTER_LIST = 2,
    IDLE_REALM_LIST = 3,
    IDLE_CREATE_CHARACTER = 4,
    IDLE_DELETE_CHARACTER = 5,
    IDLE_ENTER_WORLD = 6,
    IDLE_WORLD_LOGIN = 7,
};

class CGlueMgr {
public:
    static void Initialize();
    static void Resume();
    static void Suspend();
    static void Shutdown();
    static void InitCursor();
    static void DestroyCursor();
    static void UpdateWaitQueue(unsigned position);
    static void DefaultServerLogin();
    static void ChangeRealm(const GlueRealmInfo* info, int valid);
    static void CreateCharacter(const CHARACTER_CREATE_INFO* info);
    static void DeleteCharacter(std::uint64_t guid);
    static void EnterWorld();
    static void StatusDialogClick();
    static void SetScreen(const char* screen);
    static void UpdateCurrentScreen(const char* screen);
    static void QuitGame(HEVENTCONTEXT context = nullptr);
    static void GetCharacterList();
    static void GetRealmList();
    static int Idle();
    static int NetDisconnectHandler();
    static void RegisterConsoleCommands(const char* previous = nullptr);
    static void UnregisterConsoleCommands(const char* handle = nullptr);

    static bool m_initialized;
    static bool m_suspended;
    static bool m_reconnect;
    static bool m_reload;
    static bool m_disconnectPending;
    static GLUE_IDLE_STATE m_idleState;
    static std::string m_currentScreen;
    static std::string m_accountName;
    static const CHARACTER_INFO* m_characterInfo;
    static void* m_cursorModel;
    static unsigned m_estimatedWaitTime;
    static std::array<unsigned, 2> m_queuePosition;
    static std::array<unsigned, 2> m_queueTime;
    static int m_region;
    static int m_locale;
};

void LoadScriptFunctions();
void UnloadScriptFunctions();
int __fastcall CCommand_Script(const char* arguments, const char* commandLine);
int SysMsgPrintf(SYSMSG_TYPE severity, const char* format, ...);

extern float INFINITY_85;
extern float TWO_PI_85;
extern float OO_TWO_PI_85;
extern float MAX_DUEL_DISTANCE_SQUARED_16;
extern float MAX_TRADE_DISTANCE_SQUARED_16;
extern float MAX_SHOP_DISTANCE_SQUARED_16;
extern float MAX_INSPECT_DISTANCE_SQUARED_16;
extern float MAX_LOOT_DISTANCE_SQUARED_16;
extern float MAX_BIND_DISTANCE_SQUARED_16;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_16;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_16;
extern float MAX_SHARE_DISTANCE_SQUARED_16;
extern float SECONDS_PER_HEARTBEAT_16;
extern float SPELL_FOCUS_DISTANCE_SQUARED_16;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_16;

int _E2_85(); int _E1_85(); int _E5_85(); void _E4_85(); int _E8_85(); void _E7_85();
int _E14_8(); void _E13_3(); int _E17_6(); void _E16_10();
int _E20_24(); void _E19_11(); int _E23_26(); void _E22_17();
int _E26_22(); void _E25_15(); int _E29_21(); void _E28_9();
int _E32_19(); void _E31_6(); int _E35_19(); void _E34_7();
int _E38_19(); void _E37_6(); int _E41_17(); void _E40_7();
int _E44_19(); void _E43_6(); int _E47_21(); void _E46_4();
int _E83_0(); void _E80_1(); int _E82_1(); void _E81_0();
