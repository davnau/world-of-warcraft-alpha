#pragma once

#include "ENGINE/Source/Base/cdatastore.h"

#include <cstddef>
#include <cstdint>

using ClientCommandFunction = int (__fastcall *)(const char* args, const char* commandLine);

enum ClientCommandCategory : int {
    CLIENT_COMMAND_CATEGORY_DEBUG = 0,
    CLIENT_COMMAND_CATEGORY_GAME = 1,
    CLIENT_COMMAND_CATEGORY_COMBAT = 2,
    CLIENT_COMMAND_CATEGORY_GRAPHICS = 3,
    CLIENT_COMMAND_CATEGORY_GM = 4,
};

const char* ConsoleCommandRegister(const char* previous, const char* name,
                                   ClientCommandCategory category,
                                   ClientCommandFunction function);
void ConsoleCommandUnregister(const char* handle);
void ConsoleCommandUnregisterAll();
int ConsoleCommandExecute(const char* name, const char* commandLine = nullptr);

void ClientServices_Send(const CDataStore* message);
const std::uint8_t* ClientServices_LastPacket(std::size_t* size);
std::uint32_t ClientServices_LastOpcode();
void ClientServices_ClearLastPacket();

int ClientRuntime_GetSelectedTarget(std::uint64_t* guid);
void ClientRuntime_SetSelectedTarget(std::uint64_t guid);
void ClientRuntime_SetGhosting(int enabled);
int ClientRuntime_IsGhosting();

int __stdcall SStrCmpI(const char* left, const char* right, std::size_t maxChars);
int SStrToInt(const char* text);
void ConsolePrintf(const char* format, ...);
