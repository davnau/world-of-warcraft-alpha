#pragma once

#include "WoW/Source/Game/gametime.h"
#include "ENGINE/Source/Base/cdatastore.h"

using NETMESSAGE = unsigned;
using GameTimeForcedChangeCallback = void (__fastcall *)(unsigned, unsigned);

extern CGameTime g_clientGameTime;
extern GameTimeForcedChangeCallback s_forcedChangeCallbacks[4];

int ClientGameTimeTickHandler(float* elapsed);
void ClientInitializeGameTime();
int CCommand_ShowLocalGameTime();
int CCommand_ShowServerGameTime();
int __fastcall CCommand_GameTime(const char* command, const char* arguments);
int __fastcall CCommand_LocalTime(const char* command, const char* arguments);
void UpdateTime();
int __fastcall CCommand_SpawnTime(const char* command, const char* arguments);
int __fastcall CCommand_GameSpeed(const char* command, const char* arguments);
int __fastcall ReceiveNewGameSpeed(NETMESSAGE messageID, int context, unsigned flags, CDataStore* message);
int __fastcall ReceiveNewTimeSpeed(NETMESSAGE messageID, int context, unsigned flags, CDataStore* message);
int __fastcall ReceiveGameTimeUpdate(NETMESSAGE messageID, int context, unsigned flags, CDataStore* message);
int __fastcall ReceiveServerTime(NETMESSAGE messageID, int context, unsigned flags, CDataStore* message);
int __fastcall ReceiveNewGameTime(NETMESSAGE messageID, int context, unsigned flags, CDataStore* message);
void ClientDestroyGameTime();
void __fastcall SetGameTimeForcedChangeCallback(GameTimeForcedChangeCallback addOrRemove,
                                                  GameTimeForcedChangeCallback callback);

int _E153_2();
void _E150_2();
int _E152_1();
void _E151_0();
int _E159_1();
CGameTime* _E156_0();
int _E158_1();
void _E157_0();
