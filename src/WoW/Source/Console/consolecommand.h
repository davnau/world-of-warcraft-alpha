#pragma once

#include "WoW/Source/clientruntime.h"

#include <cstdint>

struct CONSOLECOMMAND {
    const char* name = nullptr;
    ClientCommandCategory category = CLIENT_COMMAND_CATEGORY_DEBUG;
    ClientCommandFunction function = nullptr;
    const char* help = nullptr;
};

int __fastcall ConsoleCommand_RunExec(const char* args, const char* commandLine);
int __fastcall CreateWTFFilePath(char* path, unsigned capacity);
CONSOLECOMMAND* __fastcall ParseCommand(const char** command, std::uint32_t* flags, const char** arguments);
unsigned __fastcall ConsoleCommandHistoryDepth();
char* __fastcall ConsoleCommandHistory(char direction);
int __fastcall ConsoleCommandComplete(const char** previous, const char** text, int direction);
void ConsoleCommandWriteHelp(const char* command);
int __fastcall ConsoleCommand_Help(const char* args, char* commandLine);
void ConsoleCommandRegisterDefault(ClientCommandFunction function);
void ConsoleCommandInitialize();
int ConsoleCommand_Quit();
int ConsoleCommand_Ver(const char* command);
int ConsoleCommand_CreateExec(const char* command, const char* fileName);
int ValidateFileName(const char* fileName);
int __fastcall ConsoleCommand_AppendExec(const char* args, const char* fileName);
int ConsoleCommand_CloseExec();
int __fastcall ConsoleCommand_TypeExec(const char* args, const char* fileName);
int ConsoleCommand_DirWtf();
void ConsoleCommandDestroy();

extern float INFINITY_296;
extern float TWO_PI_291;
extern float OO_TWO_PI_292;
int _E2_296(); int _E1_296(); int _E5_293(); void _E4_293(); int _E8_293(); void _E7_293();
int _E16_23(); void _E13_11(); int _E15_13(); void _E14_16();

