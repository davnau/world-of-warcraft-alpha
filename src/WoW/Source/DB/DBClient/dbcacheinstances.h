#pragma once

#include "WoW/Source/DB/DBClient/dbcache.h"

using NETMESSAGE = unsigned;

void DBCache_Initialize();
void LoadDBCaches(unsigned flags = 0);
void DBCache_Destroy();
void DBCache_RegisterHandlers(NETMESSAGE message);
int __stdcall ReceiveCreature(unsigned flags, CDataStore* message);
int __stdcall ReceiveGameObject(unsigned flags, CDataStore* message);
int __stdcall ReceiveSingleItem(unsigned flags, CDataStore* message);
int __stdcall ReceiveMultipleItems(unsigned flags, CDataStore* message);
int __stdcall ReceiveNPCText(unsigned flags, CDataStore* message);
int __stdcall ReceiveName(unsigned flags, CDataStore* message);
int __stdcall ReceiveGuildInfo(unsigned flags, CDataStore* message);
int __stdcall ReceiveQuest(unsigned flags, CDataStore* message);
int __stdcall ReceivePageText(unsigned flags, CDataStore* message);
int __stdcall ReceivePetName(unsigned flags, CDataStore* message);
int __stdcall ReceivePetition(unsigned flags, CDataStore* message);
void DBCache_ClearHandlers(NETMESSAGE message);
int _E60_57();
int _E59_20();
int _E65_15();
int _E64_45();
int _E70_45();
int _E69_41();
int _E75_37();
int _E74_13();
int _E80_15();
int _E79_11();
int _E85_2();
int _E84_2();
int _E90_5();
int _E89_5();
int _E95_1();
int _E94_3();
int _E100_3();
int _E99_2();
int _E105_3();
int _E104_3();
