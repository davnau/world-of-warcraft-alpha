#include "WoW/Source/clientgmcommands.h"

#include "ENGINE/Source/Base/cdatastore.h"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <cstring>
#include <limits>

float INFINITY_1 = 0.0F;
float TWO_PI_1 = 0.0F;
float OO_TWO_PI_1 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_1 = 0.0F;

namespace {
const char* g_handles[7]{};

int SendSimple(std::uint32_t opcode, const char* text = nullptr) {
    CDataStore message;
    message.Put(opcode);
    if (text) message.PutString(text);
    ClientServices_Send(&message);
    return 1;
}

int SendBool(std::uint32_t opcode, bool value) {
    CDataStore message;
    message.Put(opcode)->Put<std::uint8_t>(value ? 1 : 0);
    ClientServices_Send(&message);
    return 1;
}

std::uint64_t SelectedTarget() {
    std::uint64_t guid = 0;
    ClientRuntime_GetSelectedTarget(&guid);
    return guid;
}
}

void InstallGMCommands(const char* previous) {
    const char* cursor = previous;
    cursor = g_handles[0] = ConsoleCommandRegister(cursor, "ghost", CLIENT_COMMAND_CATEGORY_GM, CCommand_Ghost);
    cursor = g_handles[1] = ConsoleCommandRegister(cursor, "invis", CLIENT_COMMAND_CATEGORY_GM, CCommand_Invis);
    cursor = g_handles[2] = ConsoleCommandRegister(cursor, "bindplayer", CLIENT_COMMAND_CATEGORY_GM, CCommand_BindPlayer);
    cursor = g_handles[3] = ConsoleCommandRegister(cursor, "summon", CLIENT_COMMAND_CATEGORY_GM, CCommand_Summon);
    cursor = g_handles[4] = ConsoleCommandRegister(cursor, "showlabel", CLIENT_COMMAND_CATEGORY_GM, CCommand_ShowLabel);
    cursor = g_handles[5] = ConsoleCommandRegister(cursor, "setsecurity", CLIENT_COMMAND_CATEGORY_GM, CCommand_SetSecurity);
    g_handles[6] = ConsoleCommandRegister(cursor, "nuke", CLIENT_COMMAND_CATEGORY_GM, CCommand_Nuke);
}

int __fastcall CCommand_Ghost(const char*, const char* commandLine) {
    if (commandLine && SStrCmpI(commandLine, "off", 0x7FFFFFFFu) == 0)
        return SendBool(471, false) + (ClientRuntime_SetGhosting(0), 0);
    const bool explicitTarget = commandLine && *commandLine;
    if (explicitTarget) {
        SendBool(471, true);
        ClientRuntime_SetGhosting(1);
    } else {
        SendBool(471, true);
        ClientRuntime_SetGhosting(1);
    }
    return 1;
}

int __fastcall CCommand_Invis(const char*, const char* commandLine) {
    return SendBool(471, !(commandLine && SStrCmpI(commandLine, "off", 0x7FFFFFFFu) == 0));
}

int __fastcall CCommand_BindPlayer(const char*, const char* commandLine) {
    CDataStore message;
    message.Put<std::uint32_t>(473);
    if (commandLine && *commandLine) {
        message.Put<std::uint8_t>(1)->PutString(commandLine);
    } else {
        message.Put<std::uint8_t>(0)->Put(SelectedTarget());
    }
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_Summon(const char*, const char* commandLine) {
    return SendSimple(474, commandLine ? commandLine : "");
}

int __fastcall CCommand_ShowLabel(const char*, const char* commandLine) {
    CDataStore message;
    message.Put<std::uint32_t>(480)->Put(SStrToInt(commandLine));
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_SetSecurity(const char*, const char* commandLine) {
    char name[49]{};
    const char* cursor = commandLine ? commandLine : "";
    while (*cursor && !std::isspace(static_cast<unsigned char>(*cursor))) {
        const auto used = std::strlen(name);
        if (used + 1 >= sizeof(name)) break;
        name[used] = *cursor++;
        name[used + 1] = 0;
    }
    while (*cursor && std::isspace(static_cast<unsigned char>(*cursor))) ++cursor;
    ConsolePrintf("Setting '%s' to security group %d", name, SStrToInt(cursor));
    CDataStore message;
    message.Put<std::uint32_t>(490)->PutString(name)->Put(static_cast<std::uint32_t>(SStrToInt(cursor)));
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_Nuke(const char*, const char* commandLine) {
    return SendSimple(491, commandLine ? commandLine : "");
}

void UninstallGMCommands(const char*) {
    for (auto& handle : g_handles) {
        ConsoleCommandUnregister(handle);
        handle = nullptr;
    }
}

int _E1_1() { INFINITY_1 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2_1() { return _E1_1(); }
void _E4_1() { TWO_PI_1 = 6.28318530717958647692F; }
int _E5_1() { _E4_1(); return 0; }
void _E7_1() { OO_TWO_PI_1 = 1.0F / TWO_PI_1; }
int _E8_1() { _E7_1(); return 0; }

void _E19_1() { MAX_DUEL_DISTANCE_SQUARED_1 = 100.0F; }
int _E20_1() { _E19_1(); return 0; }
void _E22_1() {}
int _E23_1() { _E22_1(); return 0; }
void _E25_1() {}
int _E26_1() { _E25_1(); return 0; }
void _E28_1() {}
int _E29_1() { _E28_1(); return 0; }
void _E31_1() {}
int _E32_1() { _E31_1(); return 0; }
void _E34_1() {}
int _E35_1() { _E34_1(); return 0; }
void _E37_1() {}
int _E38_1() { _E37_1(); return 0; }
void _E40_1() {}
int _E41_1() { _E40_1(); return 0; }
void _E43_1() {}
int _E44_1() { _E43_1(); return 0; }
void _E46_1() {}
int _E47_1() { _E46_1(); return 0; }
void _E49_1() {}
int _E50_1() { _E49_1(); return 0; }
void _E52_1() {}
int _E53_1() { _E52_1(); return 0; }
void _E87_1() {}
int _E90_1() { _E87_1(); return 0; }
void _E88_1() {}
int _E89_1() { _E88_1(); return 0; }
