#include "WoW/Source/client.h"

#include "WoW/Source/clientcommands.h"
#include "WoW/Source/clientgmcommands.h"
#include "WoW/Source/loadingscreen.h"
#include "WoW/Source/Console/consoleclient.h"
#include "WoW/Source/Console/consolecommand.h"
#include "WoW/Source/Console/consolevar.h"
#include "WoW/Source/DB/DBClient/dbcacheinstances.h"
#include "WoW/Source/DB/DBClient/dbclient.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/Glue/gluescriptevents.h"
#include "WoW/Source/Game/validatename.h"
#include "WoW/Source/Magic/spellvisuals.h"
#include "WoW/Source/Object/objectclient/zonedebug.h"
#include "WoW/Source/Object/objectclient/player_c.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/ObjectAlloc/objectalloc.h"
#include "WoW/Source/SoundInterface/soundinterface.h"
#include "WoW/Source/UIUtil/camera.h"
#include "WoW/Source/UIUtil/inputcontrol.h"
#include "WoW/Source/UIUtil/smartscreenrects.h"
#include "WoW/Source/UIUtil/scriptevents.h"
#include "WoW/Source/UI/gameui.h"
#include "WoW/Source/UI/chatframe.h"
#include "WoW/Source/WorldClient/World.h"

#include "ENGINE/Source/Base/base.h"
#include "ENGINE/Source/Base/cmdline.h"
#include "ENGINE/Source/Base/status.h"
#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Os/W32/osime.h"
#include "ENGINE/Source/Os/W32/osfile.h"
#include "ENGINE/Source/Os/W32/path.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Scrn/scrn.h"
#include "ENGINE/Source/Services/asyncfileread.h"
#include "ENGINE/Source/Services/sysmessage.h"
#include "ENGINE/Source/Services/texture.h"

#include "Storm/h/storm.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

void CharCustomizationInitialize();
struct LoginData;
using CLIENTMESSAGEHANDLER = int (__fastcall *)(void*, unsigned, unsigned, CDataStore*);
void ClientServices_Initialize(const LoginData* loginData);
void ClientServices_Cleanup();
void ClientServices_PollEventQueue();
void ClientServices_CharacterSetInGame(int state);
void ClientServices_Send(CDataStore* message);
void ClientServices_SetMessageHandler(unsigned msgId, CLIENTMESSAGEHANDLER handler,
                                      void* param);
void ZoneDebugInitialize();
void CameraInitialize();

#ifdef INFINITY
#undef INFINITY
#endif

float INFINITY = 0.0F;
float TWO_PI = 0.0F;
float OO_TWO_PI = 0.0F;
int clientGameInitialized = EVENT_ID_CAPTURECHANGED;
unsigned s_cacheUpdateTimerHandle = 0;
std::uint32_t s_newZoneID = 0;
NTempest::C3Vector s_newPosition{};
float s_newFacing = 0.0F;

namespace {
const char* g_clientCommandHandles[14]{};
bool g_globalInitialized = false;
bool g_handlerInitialized = false;
bool g_movementLogging = false;
bool g_movementFallLogging = false;
bool g_debugTargetInfo = false;
bool g_debugShowGuid = false;
int g_profanityFilter = 1;
int g_gxMaster[8]{};
std::uint32_t g_filterMask = 0;
int g_errorMin = SYSMSG_INFO;
int g_errorMax = SYSMSG_FATAL;
SArchive* g_archives[8]{};
const char* const g_archiveNames[8] = {
    "Data\\model.MPQ",
    "Data\\texture.MPQ",
    "Data\\sound.MPQ",
    "Data\\misc.MPQ",
    "Data\\interface.MPQ",
    "Data\\fonts.MPQ",
    "Data\\speech.MPQ",
    "Data\\dbc.MPQ",
};

std::uint32_t ReadU32(CDataStore* message) {
    std::uint32_t value = 0;
    if (message) message->Get(&value);
    return value;
}

template <typename T>
bool ReadValue(CDataStore* message, T& value) {
    return message && message->Get(&value) && !message->Failed();
}

int RegisterClientCommand(std::size_t index, const char* previous, const char* name) {
    g_clientCommandHandles[index] = ConsoleCommandRegister(previous, name, CLIENT_COMMAND_CATEGORY_GRAPHICS, nullptr);
    return 1;
}

int __fastcall InitializeHandlerPlayerEvent(const void*, void*) {
    return InitializeHandlerPlayer();
}

int __fastcall DestroyHandlerPlayerEvent(const void*, void*) {
    return DestroyHandlerPlayer();
}

int __fastcall ClientIdleEvent(const void* data, void* param) {
    return ClientIdle(data, param);
}

int __fastcall PollNetEvent(const void*, void*) {
    return PollNet();
}

int __fastcall NewWorldMessageHandler(void*, NETMESSAGE msgId, unsigned timestamp,
                                      CDataStore* message) {
    return NewWorldHandler(static_cast<int>(msgId), 0x3E, timestamp, message);
}

int __fastcall TransferPendingMessageHandler(void*, NETMESSAGE, unsigned timestamp,
                                             CDataStore* message) {
    return TransferPendingHandler(timestamp, message);
}

int __fastcall TransferAbortedMessageHandler(void*, NETMESSAGE, unsigned timestamp,
                                             CDataStore* message) {
    return TransferAbortedHandler(timestamp, message);
}

int __fastcall LoadNewWorldTimer(const void*, void*) {
    return LoadNewWorld(0);
}
}

void StormStaticEntryPoint() { StormRtlInitialize(); }
int _E1() { INFINITY = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2() { return _E1(); }
void _E4() { TWO_PI = 6.28318530717958647692F; }
int _E5() { _E4(); return 0; }
void _E7() { OO_TWO_PI = 1.0F / TWO_PI; }
int _E8() { _E7(); return 0; }

unsigned ClientSetTimer(EventHandler handler, void* param) { return EventSetTimer(0u, handler, param); }
unsigned ClientSetTimer(EventGuidHandler handler, std::uint64_t param, void* param2) { return EventSetTimer(0u, handler, param, param2); }
void ClientKillTimer(EventHandler handler, const char* handlerName) { EventKillTimer(reinterpret_cast<unsigned>(handlerName), handler, handlerName); }
void ClientPostClose() { EventPostClose(); }

void ClientInitializeGame(int, int mapID, NTempest::C3Vector position) {
    CWorld::Initialize();
    PlayerClientInitialize();
    ClntObjMgrInitializeShared();
    ClntObjMgr* objectManager = ClntObjMgrCreate(0, nullptr);
    ClntObjMgrSetCurrent(objectManager);
    ClntObjMgrSetNet(nullptr);
    ClntObjMgrInitialize();
    CGGameUI::InitializeGame();
    // Startup registers these before a realm connection exists.  The original
    // connection lifecycle reattaches cache handlers when entering the world;
    // do the same here so item/name query responses reach their caches.
    DBCache_RegisterHandlers(0);
    InstallGameConsoleCommands(nullptr);
    ClientServices_SetMessageHandler(0x3E, NewWorldMessageHandler, nullptr);
    ClientServices_SetMessageHandler(0x3F, TransferPendingMessageHandler, nullptr);
    ClientServices_SetMessageHandler(0x40, TransferAbortedMessageHandler, nullptr);
    ClientServices_SetMessageHandler(0x96, ClientChatHandler, nullptr);
    if (mapID < 0) mapID = 0;
    s_newZoneID = static_cast<std::uint32_t>(mapID);
    ClntObjMgrSetMapID(s_newZoneID);
    s_newPosition = position;
    if (!CWorld::LoadMap(mapID, position, 1))
        std::fprintf(stderr, "CLIENT_WORLD_LOAD_FAILED map=%d\n", mapID);
    LoadingScreenRegisterWorldLoaded();
    clientGameInitialized = EVENT_ID_CHAR;
}

int ReceiveObjectRotation(unsigned, CDataStore* message) {
    float facing = 0.0F, pitch = 0.0F;
    ReadValue(message, facing); ReadValue(message, pitch);
    ConsolePrintf("Facing: %f, pitch: %f", facing * 57.29578F, pitch * 57.29578F);
    return 1;
}

int __fastcall ClientChatHandler(void*, unsigned, unsigned, CDataStore* message) {
    return CGChat::ChatHandler(message);
}
int ChannelNotifyHandler(CDataStore*, unsigned, CDataStore*) { return 1; }
int ClientTextEmoteHandler(CDataStore* context, unsigned, CDataStore*) { return context != nullptr; }
int ClientChannelListHandler(CDataStore*, unsigned, CDataStore*) { return 1; }
int DebugAIStateHandler(const void*, void*, CDataStore* message) {
    std::uint64_t unit = 0; std::uint32_t lineCount = 0;
    if (message) { message->Get(&unit); message->Get(&lineCount); }
    char line[128]{};
    for (std::uint32_t i = 0; i < lineCount && message; ++i) { message->GetString(line, sizeof(line)); ConsolePrintf("%s", line); }
    return 1;
}

int MovementFallLoggingHandler(unsigned, CDataStore*) { g_movementFallLogging = !g_movementFallLogging; return 1; }
int MovementLoggingHandler(unsigned, CDataStore*) { g_movementLogging = !g_movementLogging; return 1; }

int LookupResultsHandler(unsigned, CDataStore* message) {
    const auto count = ReadU32(message);
    if (!count) { ConsolePrintf("No results"); return 1; }
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t id = 0; char name[65]{}, type[65]{}, description[65]{};
        ReadValue(message, id); if (message) { message->GetString(name, sizeof(name)); message->GetString(type, sizeof(type)); message->GetString(description, sizeof(description)); }
        ConsolePrintf("[%04u] \"%s\" \"%s\" %s", id, name, type, description);
    }
    return 1;
}

int ReceiveObjectPosition(unsigned, CDataStore* message) {
    NTempest::C3Vector position{};
    ReadValue(message, position.x); ReadValue(message, position.y); ReadValue(message, position.z);
    ConsolePrintf("%f, %f, %f", position.x, position.y, position.z);
    return 1;
}

void FormatTime(char* buffer, unsigned capacity, int seconds) {
    if (!buffer || capacity == 0) return;
    const int days = seconds / 86400;
    const int hours = seconds % 86400 / 3600;
    const int minutes = seconds % 3600 / 60;
    const int secs = seconds % 60;
    std::snprintf(buffer, capacity, "%dd %dh %dm %ds", days, hours, minutes, secs);
}

int PlayedTimeHandler(unsigned, CDataStore* message) {
    int total = 0, level = 0; ReadValue(message, total); ReadValue(message, level);
    char text[256]{}; ConsolePrintf("Time played"); FormatTime(text, sizeof(text), total); ConsolePrintf("Total: %s", text); FormatTime(text, sizeof(text), level); ConsolePrintf("Level: %s", text); return 1;
}

int TransferPendingHandler(unsigned, CDataStore*) { EnableLoadingScreen(); return 1; }
int TransferAbortedHandler(unsigned, CDataStore*) { DisableLoadingScreen(); return 1; }
void MovementInit() { g_movementLogging = false; g_movementFallLogging = false; }

int __fastcall NewWorldHandler(int messageID, int expectedMessageID, unsigned, CDataStore* message) {
    if (messageID != expectedMessageID) return 0;
    std::uint8_t mapID = 0;
    ReadValue(message, mapID);
    s_newZoneID = mapID;
    ReadValue(message, s_newPosition.x); ReadValue(message, s_newPosition.y); ReadValue(message, s_newPosition.z); ReadValue(message, s_newFacing);
    if (!message || !message->IsRead()) { if (message) message->Reset(); return 1; }
    EnableLoadingScreen();
    std::fprintf(stderr, "ENTERWORLD_NEW_WORLD map=%u pos=%.3f,%.3f,%.3f facing=%.3f\n",
                 s_newZoneID, s_newPosition.x, s_newPosition.y, s_newPosition.z,
                 s_newFacing);
    EventSetTimer(0U, LoadNewWorldTimer, nullptr);
    return 1;
}

int LoadNewWorld(int) {
    ClientServices_CharacterSetInGame(0);
    CDataStore message;
    message.Put(217u);
    message.Finalize();
    ClientServices_Send(&message);
    LoadingScreenRegisterWorldLoaded();
    std::fprintf(stderr, "ENTERWORLD_WORLDPORT_ACK map=%u\n", s_newZoneID);
    return 1;
}

int NotifyHandler(unsigned, CDataStore* message) { char text[256]{}; if (message) message->GetString(text, sizeof(text)); ConsolePrintf("%s", text); return 1; }
int ClientIdle(const void*, void*) { return CGlueMgr::Idle(); }
int ClientFocus(int) { return 1; }

void ClientDestroyGame(int, int, const char*) {
    if (clientGameInitialized == EVENT_ID_CAPTURECHANGED) return;
    CWorld::Destroy();
    UninstallGameConsoleCommands(nullptr);
    clientGameInitialized = EVENT_ID_CAPTURECHANGED;
    DisableLoadingScreen();
}

unsigned __fastcall Bot_QueryAreaId(float, float) { return 0; }
int Bot_GeneratePath(void*) { return 0; }
int Bot_GetWanderPoint(float, const NTempest::C3Vector*, float, NTempest::C3Vector*) { return 0; }
void BotClientSetAccount() {}
void BotClientAddKnownSpell() {}
void BotClientLoseTarget() {}

int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int) {
    // Prevent modern Windows DPI virtualization from stretching the D3D11
    // backbuffer after the renderer has already selected a widescreen size.
    SetProcessDPIAware();
    StormStaticEntryPoint();
    _E2(); _E5(); _E8();
    if (InitializeGlobal()) {
        EventDoMessageLoop();
        DestroyGlobal();
    }
    StormRtlDestroy();
    return 0;
}

unsigned char __fastcall InitializeGlobal() {
    if (g_globalInitialized) return 1;
    ProcessCommandLine();
    if (!SetPaths() || !OpenArchives()) return 0;
    ConsoleCommandInitialize();
    CVar::Initialize("Config.wtf");
    ConsoleDeviceInitialize(1);
    BaseInitializeGlobal();
    EventInitialize(0, 1);
    OsIMEInitialize();
    EventCreateContextEx(InitializeHandlerPlayerEvent, ClientIdleEvent,
                         DestroyHandlerPlayerEvent, 10, 0);
    g_globalInitialized = true;
    return 1;
}

int InitializeHandlerPlayer() {
    if (!g_globalInitialized) return 0;
    BaseInitializeContext();
    ScrnInitialize(1);
    ConsoleScreenInitialize();
    AsyncFileReadInitialize();
    TextureInitialize();
    ModelInitialize();
    InitializeProgressBar();
    CacheUpdateInitialize();
    WowClientInit();
    g_handlerInitialized = true;
    return 1;
}

unsigned char __stdcall ErrorFileLogCallback(const char*, void*) { SysMsgEnableFileLog("logs"); return 1; }
unsigned CacheUpdateInitialize() { s_cacheUpdateTimerHandle = EventSetTimer(250u, nullptr, nullptr); return s_cacheUpdateTimerHandle; }
int CacheUpdateHandler() { ModelCacheUpdate(); TextureCacheUpdate(nullptr, 0); return 1; }

void WowClientInit() {
    ObjectAllocInitialize(nullptr);
    SysMsgInitialize();
    ClientDBInitialize();
    ClientRegisterConsoleCommands(nullptr);
    SndInterfaceInitialize();
    FrameScript_Initialize();
    FrameXML_RegisterDefault();
    CGGameUI::RegisterFrameFactories();
    GlueScriptEventsInitialize();
    ScriptEventsInitialize();
    CharCustomizationInitialize();
    ClientServices_Initialize(nullptr);
    ZoneDebugInitialize();
    DBCache_Initialize();
    DBCache_RegisterHandlers(0);
    CGlueMgr::Initialize();
    CGlueMgr::SetScreen("login");
    InputControlInitialize();
    CameraInitialize();
    SpellVisualsInitialize();
    ValidateNameInitialize();
    SmartScreenRectInitialize();
    InstallGameConsoleCommands(nullptr);
    EventRegisterEx(EVENT_ID_POLL, PollNetEvent);
}

const char* ClientRegisterConsoleCommands(const char* previous) {
    const char* cursor = previous;
    const char* names[] = {"handler", "light", "fog", "depthtest", "depthset", "culling", "dblbuffer", "resxy", "resmode", "bitdepth", "3dapi", "bug", "suggestion", "note"};
    for (std::size_t i = 0; i < 14; ++i) { cursor = ConsoleCommandRegister(cursor, names[i], CLIENT_COMMAND_CATEGORY_GRAPHICS, nullptr); g_clientCommandHandles[i] = cursor; }
    return cursor;
}

unsigned char __stdcall ErrorDisplayCallback(const char* value, void*) { const int enabled = SStrToInt(value); SysMsgEnable(enabled); return 1; }
unsigned char __stdcall ErrorDisplayMinLevelCallback(const char* value, void*) { const int level=SStrToInt(value); if(level<0||level>=SYSMSG_NUMTYPES)return 0; g_errorMin=level; SysMsgSetMinDisplayLevel(static_cast<SYSMSG_TYPE>(level)); DisplayErrorLevelStatus(); return 1; }
void DisplayErrorLevelStatus() { ConsolePrintf("Displaying error levels %d through %d", g_errorMin, g_errorMax); }
unsigned char __stdcall ErrorDisplayMaxLevelCallback(const char* value, void*) { const int level=SStrToInt(value); if(level<0||level>=SYSMSG_NUMTYPES)return 0; g_errorMax=level; SysMsgSetMaxDisplayLevel(static_cast<SYSMSG_TYPE>(level)); DisplayErrorLevelStatus(); return 1; }
unsigned char __stdcall ErrorDisplayFilterCallback(const char* value, void*) { return SetFilterMask(value, nullptr); }
void PrintFilterMask() { ConsolePrintf("Filter mask: 0x%08X", g_filterMask); }
unsigned char __stdcall SetFilterMask(const char* value, void*) { g_filterMask=static_cast<std::uint32_t>(SStrToInt(value)); SysMsgSetFilter(g_filterMask); return 1; }
unsigned char __stdcall DebugTargetInfoCallback(const char* value, void*) { g_debugTargetInfo=SStrToInt(value)!=0; return 1; }
unsigned char __stdcall DebugShowGUIDsCallback(const char* value, void*) { g_debugShowGuid=SStrToInt(value)!=0; return 1; }
unsigned char __stdcall GammaCallback(const char*, void*) { return 1; }
unsigned char __stdcall DesktopGammaCallback(const char*, void*) { return 1; }
unsigned char __stdcall ProfanityFilterCallback(const char* value, void*) { g_profanityFilter=SStrToInt(value); return 1; }

int CCommand_ReloadUI() { return 1; }
int __fastcall CCommand_ToggleLighting(const char*, const char* value) { g_gxMaster[0]=SStrToInt(value)==0; return 1; }
int __fastcall CCommand_ToggleFog(const char*, const char* value) { g_gxMaster[1]=SStrToInt(value)!=0; return 1; }
int CCommand_ToggleDepthTesting(const char*) { g_gxMaster[2]=!g_gxMaster[2]; return 1; }
int CCommand_ToggleDepthSetting(const char*) { g_gxMaster[3]=!g_gxMaster[3]; return 1; }
int CCommand_ToggleCulling(const char*) { g_gxMaster[4]=!g_gxMaster[4]; return 1; }
int CCommand_ToggleDblBuffer(const char*) { g_gxMaster[6]=!g_gxMaster[6]; return 1; }
int CCommand_SetResolutionXY() { return 1; }
int CCommand_SetResolutionMode() { return 1; }
int CCommand_SetColorDepth() { return 1; }
int CCommand_SetAPI() { return 1; }
int CCommand_Bug(const char*) { return 1; }
int PollNet() { ClientServices_PollEventQueue(); return 1; }

int DestroyHandlerPlayer() { ClientDestroyGame(0, 0, nullptr); WowClientDestroy(); return 1; }
void CacheUpdateShutdown() { if (s_cacheUpdateTimerHandle) { EventKillTimer(s_cacheUpdateTimerHandle); s_cacheUpdateTimerHandle=0; } }
void WowClientDestroy() {
    EventUnregisterEx(EVENT_ID_POLL, PollNetEvent);
    SmartScreenRectShutdown();
    ValidateNameDestroy();
    SpellVisualsShutdown();
    CGlueMgr::Shutdown();
    DBCache_Destroy();
    ClientServices_Cleanup();
    GlueScriptEventsUnregisterFunctions();
    FrameXML_ClearFactories();
    FrameScript_Destroy();
    SysMsgShutdown();
    ObjectAllocDestroy(nullptr);
    if (g_handlerInitialized) {
        ConsoleScreenDestroy();
        TextureDestroy();
        AsyncFileReadDestroy();
        BaseDestroyContext();
        g_handlerInitialized = false;
    }
    ClientUnregisterConsoleCommands(nullptr);
    ConsoleDeviceDestroy();
    CVar::Destroy();
    ConsoleCommandDestroy();
    OsIMEDestroy();
}
void ClientUnregisterConsoleCommands(const char*) { for(auto& handle:g_clientCommandHandles){ConsoleCommandUnregister(handle);handle=nullptr;} UninstallGameConsoleCommands(nullptr); }
int SetPaths() {
    SFile::DisableSFileCheckDisk();
    SFile::EnableDirectAccess(3U);
    char basePath[MAX_PATH]{};
    OsGetExePath(basePath, std::size(basePath));
    if (!basePath[0]) return 0;
    if (!SFile::SetBasePath(basePath) || !SFile::SetDataPath("Data\\")) return 0;
    return OsSetCurrentDirectory(basePath) ? 1 : 0;
}
int OpenArchives() {
    int result = 0;
    for (std::size_t index = 0; index < std::size(g_archives); ++index) {
        result = SFile::OpenArchive(g_archiveNames[index], 0, 0, &g_archives[index]);
        if (!result) g_archives[index] = nullptr;
    }
    return result;
}
int ProcessCommandLine() { return CmdLineProcess(); }
int DestroyGlobal() { if(!g_globalInitialized)return 0; std::fprintf(stderr, "CLIENT_DESTROY_GLOBAL\n"); CacheUpdateShutdown(); if(g_handlerInitialized)DestroyHandlerPlayer(); EventDestroy(); BaseDestroyGlobal(); g_globalInitialized=false; return 1; }
int ShutdownFileAccess() {
    int result = 1;
    for (SArchive*& archive : g_archives) {
        if (archive) {
            result = SFile::CloseArchive(archive);
            archive = nullptr;
        }
    }
    return result;
}
void WowLogHeader() {}
void LogZoneInfo() {}
int ClientIsValidPointer(const void* pointer) { return pointer != nullptr; }
void LogObjectInfo(const void*) {}
void SendErrorLog(const char* text) { if(text) std::fputs(text, stderr); }
void LaunchWoWError(const char* text) { SendErrorLog(text); }

void _E93() {}
int _E94() { _E93(); return 0; }
void _E96() {}
int _E97() { _E96(); return 0; }
void _E99() {}
int _E100() { _E99(); return 0; }
void _E102() {}
int _E103() { _E102(); return 0; }
void _E105() {}
int _E106() { _E105(); return 0; }
void _E204() {}
int _E207() { _E204(); return 0; }
void _E205() {}
int _E206() { _E205(); return 0; }
void _E209() {}
int _E212() { _E209(); return 0; }
void _E210() {}
int _E211() { _E210(); return 0; }
void _E19() { MAX_DUEL_DISTANCE_SQUARED_0 = 100.0F; }
int _E20() { _E19(); return 0; }
void _E22() {}
int _E23() { _E22(); return 0; }
void _E25() {}
int _E26() { _E25(); return 0; }
void _E28() {}
int _E29() { _E28(); return 0; }
void _E31() {}
int _E32() { _E31(); return 0; }
void _E34() {}
int _E35() { _E34(); return 0; }
void _E37() {}
int _E38() { _E37(); return 0; }
void _E40() {}
int _E41() { _E40(); return 0; }
void _E43() {}
int _E44() { _E43(); return 0; }
void _E46() {}
int _E47() { _E46(); return 0; }
void _E49() {}
int _E50() { _E49(); return 0; }
void _E52() {}
int _E53() { _E52(); return 0; }
void _E87() {}
int _E90() { _E87(); return 0; }
void _E88() {}
int _E89() { _E88(); return 0; }
