#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#ifdef INFINITY
#undef INFINITY
#endif

#include "WoW/Source/clientruntime.h"

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

struct TSStackArray_C3Vector;

extern float INFINITY;
extern float TWO_PI;
extern float OO_TWO_PI;
extern int clientGameInitialized;
extern unsigned s_cacheUpdateTimerHandle;
extern std::uint32_t s_newZoneID;
extern NTempest::C3Vector s_newPosition;
extern float s_newFacing;

void StormStaticEntryPoint();
int _E2(); int _E1(); int _E5(); void _E4(); int _E8(); void _E7();

unsigned ClientSetTimer(EventHandler handler, void* param);
unsigned ClientSetTimer(EventGuidHandler handler, std::uint64_t param, void* param2);
void ClientKillTimer(EventHandler handler, const char* handlerName);
void ClientPostClose();
void ClientInitializeGame(int continentID, int mapID, NTempest::C3Vector position);
int ReceiveObjectRotation(unsigned time, CDataStore* message);
int __fastcall ClientChatHandler(void* context, unsigned messageID,
                                 unsigned timestamp, CDataStore* message);
int ChannelNotifyHandler(CDataStore* context, unsigned timestamp, CDataStore* message);
int ClientTextEmoteHandler(CDataStore* context, unsigned timestamp, CDataStore* message);
int ClientChannelListHandler(CDataStore* context, unsigned timestamp, CDataStore* message);
int DebugAIStateHandler(const void* eventData, void* param, CDataStore* message);
int MovementFallLoggingHandler(unsigned time, CDataStore* message);
int MovementLoggingHandler(unsigned time, CDataStore* message);
int LookupResultsHandler(unsigned timestamp, CDataStore* message);
int ReceiveObjectPosition(unsigned time, CDataStore* message);
int PlayedTimeHandler(unsigned eventData, CDataStore* message);
void FormatTime(char* buffer, unsigned capacity, int seconds);
int TransferPendingHandler(unsigned eventData, CDataStore* message);
int TransferAbortedHandler(unsigned eventData, CDataStore* message);
void MovementInit();
int __fastcall NewWorldHandler(int messageID, int expectedMessageID, unsigned timestamp, CDataStore* message);
int LoadNewWorld(int eventData);
int NotifyHandler(unsigned timestamp, CDataStore* message);
int ClientIdle(const void* eventData, void* param);
int ClientFocus(int eventData);
void ClientDestroyGame(int resumeUI, int setScreen, const char* loginError);
unsigned __fastcall Bot_QueryAreaId(float x, float y);
int Bot_GeneratePath(void* path);
int Bot_GetWanderPoint(float radius, const NTempest::C3Vector* center, float minDistance, NTempest::C3Vector* result);
void BotClientSetAccount();
void BotClientAddKnownSpell();
void BotClientLoseTarget();
int __stdcall WinMain(HINSTANCE, HINSTANCE, LPSTR, int);
unsigned char __fastcall InitializeGlobal();
int InitializeHandlerPlayer();
unsigned char __stdcall ErrorFileLogCallback(const char* newValue, void* arg);
unsigned CacheUpdateInitialize();
int CacheUpdateHandler();
void WowClientInit();
const char* ClientRegisterConsoleCommands(const char* previous);
unsigned char __stdcall ErrorDisplayCallback(const char* newValue, void* arg);
unsigned char __stdcall ErrorDisplayMinLevelCallback(const char* newValue, void* arg);
void DisplayErrorLevelStatus();
unsigned char __stdcall ErrorDisplayMaxLevelCallback(const char* newValue, void* arg);
unsigned char __stdcall ErrorDisplayFilterCallback(const char* newValue, void* arg);
void PrintFilterMask();
unsigned char __stdcall SetFilterMask(const char* newValue, void* arg);
unsigned char __stdcall DebugTargetInfoCallback(const char* newValue, void* arg);
unsigned char __stdcall DebugShowGUIDsCallback(const char* newValue, void* arg);
unsigned char __stdcall GammaCallback(const char* newValue, void* arg);
unsigned char __stdcall DesktopGammaCallback(const char* newValue, void* arg);
unsigned char __stdcall ProfanityFilterCallback(const char* newValue, void* arg);
int CCommand_ReloadUI();
int __fastcall CCommand_ToggleLighting(const char* args, const char* commandLine);
int __fastcall CCommand_ToggleFog(const char* args, const char* commandLine);
int CCommand_ToggleDepthTesting(const char* command);
int CCommand_ToggleDepthSetting(const char* command);
int CCommand_ToggleCulling(const char* command);
int CCommand_ToggleDblBuffer(const char* command);
int CCommand_SetResolutionXY();
int CCommand_SetResolutionMode();
int CCommand_SetColorDepth();
int CCommand_SetAPI();
int CCommand_Bug(const char* args);
int PollNet();
int DestroyHandlerPlayer();
void CacheUpdateShutdown();
void WowClientDestroy();
void ClientUnregisterConsoleCommands(const char* firstHandle);
int SetPaths();
int OpenArchives();
int ProcessCommandLine();
int DestroyGlobal();
int ShutdownFileAccess();
void WowLogHeader();
void LogZoneInfo();
int ClientIsValidPointer(const void* pointer);
void LogObjectInfo(const void* object);
void SendErrorLog(const char* text);
void LaunchWoWError(const char* text);

int _E94(); void _E93(); int _E97(); void _E96(); int _E100(); void _E99();
int _E103(); void _E102(); int _E106(); void _E105(); int _E207(); void _E204();
int _E206(); void _E205(); int _E212(); void _E209(); int _E211(); void _E210();
int _E20(); void _E19(); int _E23(); void _E22(); int _E26(); void _E25();
int _E29(); void _E28(); int _E32(); void _E31(); int _E35(); void _E34();
int _E38(); void _E37(); int _E41(); void _E40(); int _E44(); void _E43();
int _E47(); void _E46(); int _E50(); void _E49(); int _E53(); void _E52();
int _E90(); void _E87(); int _E89(); void _E88();
