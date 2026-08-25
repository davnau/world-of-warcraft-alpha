#pragma once

#include "WoW/Source/clientruntime.h"

#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

extern float INFINITY_0;
extern float TWO_PI_0;
extern float OO_TWO_PI_0;
extern float MAX_DUEL_DISTANCE_SQUARED_0;
extern float OOSMOLTILE_SIZE_0;

struct CMemCmdItem {
    std::uint32_t allocated = 0;
    std::uint32_t committed = 0;
    std::uint32_t reserved = 0;
    char name[256]{};
    static int __cdecl Compare(const char* left, const char* right);
};

struct CMemDump {
    std::uint64_t allocated = 0;
    std::uint64_t committed = 0;
    std::uint64_t reserved = 0;
    std::uint32_t count = 0;
    CMemCmdItem* items = nullptr;
};

void InstallGameConsoleCommands(const char* previous);
void UninstallGameConsoleCommands(const char* firstHandle);

int __fastcall CCommand_DBLookup(const char* args, const char* commandLine);
int CCommand_DrawLog(const char* command);
int CCommand_AnimLog(const char* command);
int CCommand_TogglePlayer();
int CCommand_ToggleAnimBlending();
int CCommand_ShowBounds();
int CCommand_Loc();
int __fastcall CCommand_TerminalVelocity(const char* args, const char* commandLine);
int CCommand_DLoc();
int CCommand_TargetLoc();
int CCommand_Facing();
int CCommand_DFacing();
int __fastcall CCommand_Speed(const char* args, const char* commandLine);
int __fastcall CCommand_WalkSpeed(const char* args, const char* commandLine);
int __fastcall CCommand_SwimSpeed(const char* args, const char* commandLine);
int __fastcall CCommand_TurnSpeed(const char* args, const char* commandLine);
int __fastcall CCommand_Money(const char* args, char* commandLine);
int __fastcall CCommand_WorldTeleport(const char* args, char* commandLine);
int __fastcall CCommand_Teleport(const char* args, char* commandLine);
int __fastcall CCommand_CreateItem(const char* args, const char* commandLine);
int __fastcall CCommand_CreateGameObject(const char* args, const char* commandLine);
int __fastcall CCommand_CreateMonster(const char* args, char* commandLine);
int __fastcall CCommand_CreatePet(const char* args, char* commandLine);
int CCommand_DestroyMonster();
int CCommand_AttackPlayer();
int __fastcall CCommand_TargetAttack(const char* args, const char* commandLine);
int CCommand_Save();
int CCommand_BindPoint();
int __fastcall CCommand_Beastmaster(const char* args, const char* commandLine);
int __fastcall CCommand_SendEvent(const char* args, const char* commandLine);
int CCommand_Recharge();
int __fastcall CCommand_Level(const char* args, const char* commandLine);
int __fastcall CCommand_PetLevel(const char* args, const char* commandLine);
int __fastcall CCommand_ClearQuest(const char* args, const char* commandLine);
int __fastcall CCommand_FlagQuest(const char* args, const char* commandLine);
int __fastcall CCommand_FlagQuestFinish(const char* args, const char* commandLine);
int __fastcall CCommand_QuestCommand(const char* args, char* commandLine);
void __fastcall QueryQuest(std::uint64_t* questID, int questGiver);
void __fastcall AcceptQuest(std::uint64_t* questID, int questGiver);
void __fastcall CompleteQuest(std::uint64_t* questID, int questGiver);
void __fastcall QuestLogRemoveQuest(std::uint8_t questLogIndex);
int CCommand_TaxiClearAllNodes();
int CCommand_TaxiEnableAllNodes();
int __fastcall CCommand_ChangeCellZone(const char* args, const char* commandLine);
int CCommand_Played();
int __fastcall CCommand_LootMethod(const char* args, const std::uint8_t* commandLine);
int CCommand_CameraTarget();
int __fastcall CCommand_Reclaim(const char* args, char* commandLine);
int __fastcall CCommand_BuySpell(const char* args, char* commandLine);
int __fastcall CCommand_SellItem(const char* args, char* commandLine);
int __fastcall CCommand_BuyItem(const char* args, char* commandLine);
int __fastcall CCommand_BuyItemInSlot(const char* args, char* commandLine);
int __fastcall CCommand_Mem(const char* args, const std::uint8_t* commandLine);
void CmdMemOutput(const char* filter, CMemDump* dump, const char* line);
std::uint32_t CmdMemParseNum(char** text);
void DebugPrintMemDump(CMemDump* dump);
void __fastcall FilePrintMemDump(const char* dumpFile, const char* outputFile);

long double __fastcall ClientCommandSqrt(float value);
int CDataStoreFetchWrite(CDataStore* store, unsigned position, unsigned bytes,
                         const char* fileName, int lineNumber);

int _E2_0(); int _E1_0(); int _E5_0(); void _E4_0();
int _E8_0(); void _E7_0(); int _E94_0(); void _E93_0();
int _E97_0(); void _E96_0(); int _E100_0(); void _E99_0();
int _E103_0(); void _E102_0(); int _E106_0(); void _E105_0();
int _E20_0(); void _E19_0(); int _E23_0(); void _E22_0();
int _E26_0(); void _E25_0(); int _E29_0(); void _E28_0();
int _E32_0(); void _E31_0(); int _E35_0(); void _E34_0();
int _E38_0(); void _E37_0(); int _E41_0(); void _E40_0();
int _E44_0(); void _E43_0(); int _E47_0(); void _E46_0();
int _E50_0(); void _E49_0(); int _E53_0(); void _E52_0();
int _E90_0(); void _E87_0(); int _E89_0(); void _E88_0();
