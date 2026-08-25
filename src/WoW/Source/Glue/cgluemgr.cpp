#include "WoW/Source/Glue/cgluemgr.h"

#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Base/status.h"
#include "ENGINE/Source/Frame/frame.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "ENGINE/Source/Services/asyncfileread.h"
#include "ENGINE/Source/Services/texture.h"
#include "Storm/h/storm.h"
#include "WoW/Source/Glue/charcreateinfo.h"
#include "WoW/Source/Glue/charselectinfo.h"
#include "WoW/Source/Glue/gluescriptevents.h"
#include "WoW/Source/Net/netinternal.h"
#include "WoW/Source/SoundInterface/soundinterface.h"
#include "WoW/Source/SoundInterface/soundinterfacegluemusic.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"
#include "WoW/Source/client.h"
#include "WoW/Source/clientruntime.h"
#include "WoW/Source/loadingscreen.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <limits>
#include <utility>

float INFINITY_85 = 0.0F;
float TWO_PI_85 = 0.0F;
float OO_TWO_PI_85 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_16 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_16 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_16 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_16 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_16 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_16 = 0.0F;
float SECONDS_PER_HEARTBEAT_16 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_16 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_16 = 0.0F;

bool CGlueMgr::m_initialized = false;
bool CGlueMgr::m_suspended = false;
bool CGlueMgr::m_reconnect = false;
bool CGlueMgr::m_reload = false;
bool CGlueMgr::m_disconnectPending = false;
GLUE_IDLE_STATE CGlueMgr::m_idleState = IDLE_NONE;
std::string CGlueMgr::m_currentScreen;
std::string CGlueMgr::m_accountName;
const CHARACTER_INFO* CGlueMgr::m_characterInfo = nullptr;
void* CGlueMgr::m_cursorModel = nullptr;
unsigned CGlueMgr::m_estimatedWaitTime = 0;
std::array<unsigned, 2> CGlueMgr::m_queuePosition{};
std::array<unsigned, 2> CGlueMgr::m_queueTime{};
int CGlueMgr::m_region = 0;
int CGlueMgr::m_locale = LOCALE_en_US;

namespace {
const char* g_scriptCommand = nullptr;
std::uint64_t s_loginGUID = 0;
CHARACTER_INFO s_enterWorldCharacterInfo{};
}

void CGlueMgr::Initialize()
{
    if (m_initialized)
        return;
    m_initialized = true;
    Resume();
}

void CGlueMgr::Resume()
{
    m_suspended = false;
    m_reconnect = false;
    m_disconnectPending = false;
    m_idleState = IDLE_NONE;
    m_characterInfo = nullptr;
    if (!CSimpleTop::s_instance) new CSimpleTop;
    InitCursor();
    FrameScript_Flush();
    FrameScript_LoadTextTables("Interface\\GlueXML\\GlueStrings.lua");
    LoadScriptFunctions();
    CCharSelectInfo::Initialize();
    CCharCreateInfo::Initialize();
    CStatus status;
    if (!FrameXML_CreateFrames("Interface\\GlueXML\\GlueXML.toc", &status) || !status.IsEmpty())
        status.Display();
    // The recovered glue art was authored for the fixed 1024x768 DDC canvas.
    // Keep that canvas centered while the world-facing CSimpleTop remains
    // aspect-aware for widescreen rendering.
    if (CSimpleFrame* glueParent = SimpleFrameRegistryGetEntry(nullptr, "GlueParent")) {
        glueParent->ClearAllPoints();
        glueParent->SetPoint(FRAMEPOINT_CENTER, CSimpleTop::s_instance,
                             FRAMEPOINT_CENTER, 0.0f, 0.0f);
        glueParent->SetWidth(0.8f);
        glueParent->SetHeight(0.6f);
        std::fprintf(stderr, "GLUE_CANVAS rect=(%.6f,%.6f)-(%.6f,%.6f)\n",
                     glueParent->m_rect.l, glueParent->m_rect.t,
                     glueParent->m_rect.r, glueParent->m_rect.b);
    }
    RegisterConsoleCommands();
}

void CGlueMgr::Suspend()
{
    if (m_suspended)
        return;
    m_suspended = true;
    CCharSelectInfo::Shutdown();
    CCharCreateInfo::Shutdown();
    FrameXML_DestroyFrames();
    DestroyCursor();
    UnloadScriptFunctions();
    UnregisterConsoleCommands();
}

void CGlueMgr::Shutdown()
{
    if (!m_initialized)
        return;
    Suspend();
    m_initialized = false;
}

void CGlueMgr::InitCursor()
{
    // The model handle is opaque until the model subsystem is restored.  A
    // non-null sentinel retains the recovered ownership/cleanup transition.
    m_cursorModel = reinterpret_cast<void*>(static_cast<std::uintptr_t>(1));
}

void CGlueMgr::DestroyCursor()
{
    m_cursorModel = nullptr;
}

void CGlueMgr::UpdateWaitQueue(unsigned position)
{
    if (position != m_queuePosition[0] || m_queueTime[0] == 0) {
        m_queuePosition[1] = m_queuePosition[0];
        m_queueTime[1] = m_queueTime[0];
        m_queuePosition[0] = position;
    }
    m_queueTime[0] = OsGetAsyncTimeMs();
    if (m_queuePosition[0] < m_queuePosition[1] && m_queueTime[1] != 0) {
        const unsigned elapsed = m_queueTime[0] - m_queueTime[1];
        const unsigned distance = m_queuePosition[1] - m_queuePosition[0];
        m_estimatedWaitTime = distance ? (position * elapsed) / distance : 0;
    }
}

void CGlueMgr::DefaultServerLogin()
{
    if (m_idleState != IDLE_NONE)
        return;
    const char* accountName = ClientServices_GetAccountName();
    m_accountName = accountName ? accountName : "";
    m_idleState = IDLE_ACCOUNT_LOGIN;
    m_disconnectPending = false;
    FrameScript_SignalEvent(3, "%s", "CANCEL");
    ClientServices_Connect();
    GlueScriptEventsSetConnected(true);
}

void CGlueMgr::ChangeRealm(const GlueRealmInfo* info, int valid)
{
    if (!valid || !info)
        return;
    if (GlueScriptEventsIsDisconnectPending())
        m_reconnect = true;
    m_idleState = IDLE_ACCOUNT_LOGIN;
    GlueScriptEventsSetConnected(true);
}

void CGlueMgr::CreateCharacter(const CHARACTER_CREATE_INFO* info)
{
    if (!info)
        return;
    m_idleState = IDLE_CREATE_CHARACTER;
    FrameScript_SignalEvent(3, "%s", "CANCEL");
    ClientServices_CharacterCreate(info);
}

void CGlueMgr::DeleteCharacter(std::uint64_t guid)
{
    if (guid != 0) {
        m_idleState = IDLE_DELETE_CHARACTER;
        FrameScript_SignalEvent(3, "%s", "CANCEL");
        ClientServices_CharacterDelete(guid);
    }
}

void CGlueMgr::EnterWorld()
{
    const CHARINFO* selected = CCharSelectInfo::GetSelectedCharacterInfo();
    if (selected) {
        // The glue FrameXML tree and CCharSelectInfo are destroyed by Suspend
        // before ClientInitializeGame creates the world.  Preserve the record
        // exactly as the recovered transition does instead of retaining a
        // pointer into the soon-to-be-destroyed character list.
        s_enterWorldCharacterInfo = selected->m_characterInfo;
        m_characterInfo = &s_enterWorldCharacterInfo;
    } else {
        m_characterInfo = nullptr;
    }
    if (!m_characterInfo || !ClientServices_IsConnected())
        return;

    SRegSaveValue("WoW\\Client", "LastCharacter", 0, CCharSelectInfo::m_selectionIndex);
    SRegSaveString("WoW\\Client", "LastAccount", 0, m_accountName.c_str());
    SRegSaveString("WoW\\Client", "LastRealm", 0,
                   ClientServices_GetSelectedRealmAddress());
    ModelCacheFlush();
    TextureCacheFlush();
    TextureGxCacheFlush();
    EnableLoadingScreen();
    m_idleState = IDLE_ENTER_WORLD;
}

void CGlueMgr::StatusDialogClick()
{
    switch (m_idleState) {
    case IDLE_NONE:
        break;
    case IDLE_ACCOUNT_LOGIN:
    case IDLE_CREATE_CHARACTER:
    case IDLE_DELETE_CHARACTER:
    case IDLE_ENTER_WORLD:
        m_idleState = IDLE_NONE;
        break;
    case IDLE_CHARACTER_LIST:
        m_idleState = IDLE_NONE;
        SetScreen(m_currentScreen.c_str());
        break;
    case IDLE_WORLD_LOGIN:
        m_idleState = IDLE_NONE;
        GetCharacterList();
        break;
    default:
        break;
    }
}

void CGlueMgr::SetScreen(const char* screen)
{
    if (screen) FrameScript_SignalEvent(0, "%s", screen);
}

void CGlueMgr::UpdateCurrentScreen(const char* screen)
{
    m_currentScreen = screen ? screen : "";
}

void CGlueMgr::QuitGame(HEVENTCONTEXT context)
{
    EventPostClose(context);
}

void CGlueMgr::GetCharacterList()
{
    if (m_idleState != IDLE_WORLD_LOGIN) {
        m_idleState = IDLE_CHARACTER_LIST;
        FrameScript_SignalEvent(3, "%s", "CANCEL");
        ClientServices_GetCharacterList();
    }
}

void CGlueMgr::GetRealmList()
{
    m_idleState = IDLE_REALM_LIST;
}

int CGlueMgr::Idle()
{
    if (!m_initialized)
        return 0;
    if (m_idleState != IDLE_NONE) {
        WOWCS_OPS op = COP_NONE;
        const char* message = "";
        int result = 0;
        int errorCode = 0;
        const int complete = ClientServices_PollStatus(&op, &message, &result, &errorCode);

        if (m_idleState == IDLE_ACCOUNT_LOGIN) {
            FrameScript_SignalEvent(4, "%s", message ? message : "");
            if (!complete)
                return 1;
            if (result) {
                if (op == COP_CONNECT) {
                    ClientServices_AccountLogin(m_accountName.c_str(), "", m_region,
                        static_cast<WOW_LOCALE>(m_locale));
                    return 1;
                }
                if (op == COP_AUTHENTICATE || op == COP_WAIT_QUEUE) {
                    m_idleState = IDLE_NONE;
                    FrameScript_SignalEvent(5);
                    CCharSelectInfo::SelectCharacter(-1);
                    SetScreen("charselect");
                    return 1;
                }
            }
            FrameScript_SignalEvent(3, "%s%s", "OKAY", message ? message : "");
            m_idleState = IDLE_NONE;
            SetScreen("login");
            return 1;
        }

        if (m_idleState == IDLE_CHARACTER_LIST) {
            if (complete) {
                std::fprintf(stderr,
                             "CHARSELECT_IDLE complete op=%d result=%d error=%d connected=%d message=%s\n",
                             static_cast<int>(op), result, errorCode,
                             ClientServices_IsConnected(), message ? message : "");
            }
            FrameScript_SignalEvent(4, "%s", message ? message : "");
            if (!complete)
                return 1;
            if (op == COP_GET_CHARACTERS && result) {
                m_idleState = IDLE_NONE;
                FrameScript_SignalEvent(5);
                CCharSelectInfo::UpdateCharacterList();
                return 1;
            }
            m_idleState = IDLE_NONE;
            FrameScript_SignalEvent(3, "%s%s", "OKAY", message ? message : "");
            return 1;
        }

        if (m_idleState == IDLE_CREATE_CHARACTER) {
            FrameScript_SignalEvent(4, "%s", message ? message : "");
            if (!complete)
                return 1;
            if (op == COP_CREATE_CHARACTER && result) {
                m_idleState = IDLE_NONE;
                FrameScript_SignalEvent(5);
                FrameScript_SignalEvent(10);
                SetScreen("charselect");
                return 1;
            }
            m_idleState = IDLE_NONE;
            FrameScript_SignalEvent(3, "%s%s", "OKAY", message ? message : "");
            return 1;
        }

        if (m_idleState == IDLE_DELETE_CHARACTER) {
            FrameScript_SignalEvent(4, "%s", message ? message : "");
            if (!complete)
                return 1;
            if (op == COP_DELETE_CHARACTER && result) {
                CCharSelectInfo::SelectCharacter(0);
                GetCharacterList();
                return 1;
            }
            m_idleState = IDLE_NONE;
            FrameScript_SignalEvent(3, "%s%s", "OKAY", message ? message : "");
            return 1;
        }

        if (m_idleState == IDLE_ENTER_WORLD) {
            if (!DrawingLoadingScreen())
                return 1;
            if (!m_suspended) {
                if (!m_characterInfo) {
                    m_idleState = IDLE_NONE;
                    return 1;
                }
                s_loginGUID = m_characterInfo->guid;
                const unsigned mapID = m_characterInfo->mapID;
                const NTempest::C3Vector position = m_characterInfo->position;
                Suspend();
                ClientInitializeGame(0, static_cast<int>(mapID), position);
                return 1;
            }
            if (!AsyncFileReadIsReading()) {
                m_idleState = IDLE_NONE;
                SndInterfaceSetGlueMusic(nullptr);
                ClientServices_CharacterLogin(s_loginGUID, 0, {});
                return 1;
            }
        }
    }
    if (m_reload) {
        m_reload = false;
        Suspend();
        Resume();
    }
    return 1;
}

int CGlueMgr::NetDisconnectHandler()
{
    if (m_disconnectPending) {
        m_disconnectPending = false;
        GlueScriptEventsSetConnected(false);
        if (m_reconnect) {
            m_reconnect = false;
            DefaultServerLogin();
        }
        return 1;
    }
    GlueScriptEventsSetConnected(false);
    m_idleState = IDLE_NONE;
    return 1;
}

void CGlueMgr::RegisterConsoleCommands(const char* previous)
{
    if (!g_scriptCommand)
        g_scriptCommand = ConsoleCommandRegister(previous, "script",
                                                   CLIENT_COMMAND_CATEGORY_DEBUG,
                                                   CCommand_Script);
}

void CGlueMgr::UnregisterConsoleCommands(const char* handle)
{
    if (handle)
        ConsoleCommandUnregister(handle);
    else if (g_scriptCommand)
        ConsoleCommandUnregister(g_scriptCommand);
    g_scriptCommand = nullptr;
}

void LoadScriptFunctions()
{
    GlueScriptEventsRegisterFunctions();
    CharSelectRegisterScriptFunctions();
    CharCreateRegisterScriptFunctions();
    SoundRegisterScriptFunctions();
}

void UnloadScriptFunctions()
{
    GlueScriptEventsUnregisterFunctions();
    CharSelectUnregisterScriptFunctions();
    CharCreateUnregisterScriptFunctions();
    SoundUnregisterScriptFunctions();
}

int __fastcall CCommand_Script(const char*, const char*)
{
    return 1;
}

int SysMsgPrintf(SYSMSG_TYPE severity, const char* format, ...)
{
    if (!format)
        return 0;
    char buffer[1024]{};
    std::va_list args;
    va_start(args, format);
    const int written = std::vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    ::SysMsgAdd(buffer, severity, 1U);
    return written;
}

int _E2_85() { return _E1_85(); }
int _E1_85() { INFINITY_85 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_85() { _E4_85(); return 0; }
void _E4_85() { TWO_PI_85 = 3.1415927F + 3.1415927F; }
int _E8_85() { _E7_85(); return 0; }
void _E7_85() { OO_TWO_PI_85 = 1.0F / TWO_PI_85; }

#define GLUEMGR_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
GLUEMGR_CONST_PAIR(_E14_8, _E13_3, MAX_DUEL_DISTANCE_SQUARED_16, 100.0F)
GLUEMGR_CONST_PAIR(_E17_6, _E16_10, MAX_TRADE_DISTANCE_SQUARED_16, 123.45679F)
GLUEMGR_CONST_PAIR(_E20_24, _E19_11, MAX_SHOP_DISTANCE_SQUARED_16, 30.864195F)
GLUEMGR_CONST_PAIR(_E23_26, _E22_17, MAX_INSPECT_DISTANCE_SQUARED_16, 30.864195F)
GLUEMGR_CONST_PAIR(_E26_22, _E25_15, MAX_LOOT_DISTANCE_SQUARED_16, 25.0F)
GLUEMGR_CONST_PAIR(_E29_21, _E28_9, MAX_BIND_DISTANCE_SQUARED_16, 100.0F)
GLUEMGR_CONST_PAIR(_E32_19, _E31_6, MAX_SITCHAIR_DISTANCE_SQUARED_16, 0.25F)
GLUEMGR_CONST_PAIR(_E35_19, _E34_7, MAX_SITCHAIRUSE_DISTANCE_SQUARED_16, 9.0F)
GLUEMGR_CONST_PAIR(_E38_19, _E37_6, MAX_SHARE_DISTANCE_SQUARED_16, 10000.0F)
GLUEMGR_CONST_PAIR(_E41_17, _E40_7, SECONDS_PER_HEARTBEAT_16, 5.0F)
GLUEMGR_CONST_PAIR(_E44_19, _E43_6, SPELL_FOCUS_DISTANCE_SQUARED_16, 2500.0F)
GLUEMGR_CONST_PAIR(_E47_21, _E46_4, MAX_OBJ_INTEREST_RADIUS_SQUARED_16, 10000.0F)
#undef GLUEMGR_CONST_PAIR

int _E83_0()
{
    _E80_1();
    return _E82_1();
}

void _E80_1()
{
    if (++WowConnectionInitializer::count == 1)
        WowConnectionInitializer::Initialize();
}

int _E82_1() { return std::atexit(_E81_0); }

void _E81_0()
{
    if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0)
        WowConnectionInitializer::Destroy();
}
