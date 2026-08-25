#include "WoW/Source/clientcommands.h"
#include "WoW/Source/clientgmcommands.h"

#include "ENGINE/Source/Tempest/cmath.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <vector>

float INFINITY_0 = 0.0F;
float TWO_PI_0 = 0.0F;
float OO_TWO_PI_0 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_0 = 0.0F;
float OOSMOLTILE_SIZE_0 = 0.0F;

namespace {
const char* g_commandHandles[52]{};
float g_terminalVelocity = 60.0F;
float g_speed = 7.0F;
float g_walkSpeed = 2.5F;
float g_swimSpeed = 4.7F;
float g_turnSpeed = 3.14F;
bool g_drawLog = false;
bool g_animLog = false;
bool g_playerVisible = true;
bool g_boundsVisible = false;
bool g_animBlending = false;
std::uint64_t g_lastTarget = 0;
std::uint64_t g_questCommandId = 0;

std::vector<std::string> Tokens(const char* input, const char* separators = " \t\r\n\",") {
    std::vector<std::string> result;
    if (!input) return result;
    const char* cursor = input;
    while (*cursor) {
        while (*cursor && std::strchr(separators, *cursor)) ++cursor;
        if (!*cursor) break;
        const char* begin = cursor;
        while (*cursor && !std::strchr(separators, *cursor)) ++cursor;
        result.emplace_back(begin, cursor);
    }
    return result;
}

std::int64_t ToSigned(const char* text) {
    return text ? std::strtoll(text, nullptr, 0) : 0;
}
std::uint64_t ToUnsigned64(const std::string& text) {
    return std::strtoull(text.c_str(), nullptr, 16);
}
float ToFloat(const std::string& text) { return std::strtof(text.c_str(), nullptr); }

template <typename T>
int SendValues(std::uint32_t opcode, const T& value) {
    CDataStore message;
    message.Put(opcode)->Put(value);
    ClientServices_Send(&message);
    return 1;
}

int SendOpcode(std::uint32_t opcode) {
    CDataStore message;
    message.Put(opcode);
    ClientServices_Send(&message);
    return 1;
}

std::uint64_t TargetGuid() {
    std::uint64_t guid = 0;
    ClientRuntime_GetSelectedTarget(&guid);
    return guid ? guid : g_lastTarget;
}

void SendQuest(std::uint32_t opcode, std::uint64_t questID, int questGiver) {
    CDataStore message;
    message.Put(opcode)->Put(questID)->Put(questGiver);
    ClientServices_Send(&message);
}

int SendGuidOpcode(std::uint32_t opcode, std::uint64_t guid) {
    CDataStore message;
    message.Put(opcode)->Put(guid);
    ClientServices_Send(&message);
    return 1;
}

const char* Register(const char* cursor, std::size_t index, const char* name,
                     ClientCommandCategory category, ClientCommandFunction function = nullptr) {
    return g_commandHandles[index] = ConsoleCommandRegister(cursor, name, category, function);
}
}

int CMemCmdItem::Compare(const char* left, const char* right) {
    if (!left || !right) return left == right ? 0 : (left ? 1 : -1);
    return SStrCmpI(left + 12, right + 12, 0x7FFFFFFFu);
}

void InstallGameConsoleCommands(const char* previous) {
    const char* cursor = previous;
    cursor = Register(cursor, 0, "loc", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 1, "dloc", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 2, "facing", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 3, "dfacing", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 4, "showbounds", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 5, "tloc", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 6, "terminalvelocity", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_TerminalVelocity);
    cursor = Register(cursor, 7, "ci", CLIENT_COMMAND_CATEGORY_GAME, CCommand_CreateItem);
    cursor = Register(cursor, 8, "cm", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 9, "cgo", CLIENT_COMMAND_CATEGORY_GAME, CCommand_CreateGameObject);
    cursor = Register(cursor, 10, "pet", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 11, "dm", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 12, "attackme", CLIENT_COMMAND_CATEGORY_COMBAT);
    cursor = Register(cursor, 13, "targetattack", CLIENT_COMMAND_CATEGORY_COMBAT, CCommand_TargetAttack);
    cursor = Register(cursor, 14, "speed", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_Speed);
    cursor = Register(cursor, 15, "walkspeed", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_WalkSpeed);
    cursor = Register(cursor, 16, "swimspeed", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_SwimSpeed);
    cursor = Register(cursor, 17, "turnspeed", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_TurnSpeed);
    cursor = Register(cursor, 18, "port", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 19, "worldport", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 20, "money", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 21, "save", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 22, "deathbind", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 23, "db", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_DBLookup);
    cursor = Register(cursor, 24, "drawlog", CLIENT_COMMAND_CATEGORY_DEBUG, nullptr);
    cursor = Register(cursor, 25, "animlog", CLIENT_COMMAND_CATEGORY_DEBUG, nullptr);
    cursor = Register(cursor, 26, "showplayer", CLIENT_COMMAND_CATEGORY_GRAPHICS);
    cursor = Register(cursor, 27, "motionblend", CLIENT_COMMAND_CATEGORY_GRAPHICS);
    cursor = Register(cursor, 28, "beastmaster", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_Beastmaster);
    cursor = Register(cursor, 29, "sendevent", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_SendEvent);
    cursor = Register(cursor, 30, "recharge", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 31, "mem", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 32, "level", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_Level);
    cursor = Register(cursor, 33, "petlevel", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_PetLevel);
    cursor = Register(cursor, 34, "clearquest", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_ClearQuest);
    cursor = Register(cursor, 35, "flagquest", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_FlagQuest);
    cursor = Register(cursor, 36, "taxiclearallnodes", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 37, "taxienableallnodes", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 38, "changecellzone", CLIENT_COMMAND_CATEGORY_DEBUG, CCommand_ChangeCellZone);
    cursor = Register(cursor, 39, "played", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 40, "lootmethod", CLIENT_COMMAND_CATEGORY_GAME);
    cursor = Register(cursor, 41, "finishquest", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 42, "cameratarget", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 43, "questquery", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 44, "questaccept", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 45, "questcomplete", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 46, "questcancel", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 47, "reclaim", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 48, "buyspell", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 49, "sellitem", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 50, "buyitem", CLIENT_COMMAND_CATEGORY_DEBUG);
    cursor = Register(cursor, 51, "buyiteminslot", CLIENT_COMMAND_CATEGORY_DEBUG);
    InstallGMCommands(cursor);
}

int __fastcall CCommand_DBLookup(const char*, const char* commandLine) {
    CDataStore message;
    message.Put(2)->PutString(commandLine ? commandLine : "");
    ClientServices_Send(&message);
    return 1;
}

int CCommand_DrawLog(const char*) { g_drawLog = !g_drawLog; ConsolePrintf("Model render log: %s", g_drawLog ? "on" : "off"); return 1; }
int CCommand_AnimLog(const char*) { g_animLog = !g_animLog; ConsolePrintf("Model animation log: %s", g_animLog ? "on" : "off"); return 1; }
int CCommand_TogglePlayer() { g_playerVisible = !g_playerVisible; return 1; }
int CCommand_ToggleAnimBlending() { g_animBlending = !g_animBlending; return 1; }
int CCommand_ShowBounds() { g_boundsVisible = !g_boundsVisible; return 1; }

int CCommand_Loc() { return SendValues<std::uint64_t>(4, TargetGuid()); }

int __fastcall CCommand_TerminalVelocity(const char*, const char* commandLine) {
    if (commandLine && *commandLine) g_terminalVelocity = std::clamp(std::strtof(commandLine, nullptr), 1.0F, 60.0F);
    ConsolePrintf("Terminal velocity: %f", g_terminalVelocity);
    return 1;
}

int CCommand_DLoc() { ConsolePrintf("0.000000, 0.000000, 0.000000"); return 1; }

int CCommand_TargetLoc() {
    g_lastTarget = TargetGuid();
    if (!g_lastTarget) { ConsolePrintf("No unit target"); return 1; }
    SendGuidOpcode(4, g_lastTarget);
    SendGuidOpcode(6, g_lastTarget);
    return 1;
}

int CCommand_Facing() { return SendGuidOpcode(6, TargetGuid()); }
int CCommand_DFacing() { ConsolePrintf("0.000000 degrees"); return 1; }

int __fastcall CCommand_Speed(const char*, const char* commandLine) { if (commandLine) g_speed = std::strtof(commandLine, nullptr); return 1; }
int __fastcall CCommand_WalkSpeed(const char*, const char* commandLine) { if (commandLine) g_walkSpeed = std::strtof(commandLine, nullptr); return 1; }
int __fastcall CCommand_SwimSpeed(const char*, const char* commandLine) { if (commandLine) g_swimSpeed = std::strtof(commandLine, nullptr); return 1; }
int __fastcall CCommand_TurnSpeed(const char*, const char* commandLine) { if (commandLine) g_turnSpeed = std::strtof(commandLine, nullptr); return 1; }

int __fastcall CCommand_Money(const char*, char* commandLine) {
    auto tokens = Tokens(commandLine, "\t\r\n\" ");
    if (tokens.empty()) { ConsolePrintf("Usage: money <copper>"); return 0; }
    return SendValues<std::uint32_t>(36, static_cast<std::uint32_t>(std::strtoul(tokens[0].c_str(), nullptr, 0)));
}

int __fastcall CCommand_WorldTeleport(const char*, char* commandLine) {
    auto tokens = Tokens(commandLine);
    if (tokens.empty()) { ConsolePrintf("Usage: worldport <map> [x y z facing]"); return 0; }
    const int map = static_cast<int>(ToSigned(tokens[0].c_str()));
    float x = tokens.size() > 1 ? ToFloat(tokens[1]) : 0.0F;
    float y = tokens.size() > 2 ? ToFloat(tokens[2]) : 0.0F;
    float z = tokens.size() > 3 ? ToFloat(tokens[3]) : 0.0F;
    float facing = tokens.size() > 4 ? ToFloat(tokens[4]) * 0.017453292F : 0.0F;
    CDataStore message;
    message.Put(8)->Put<std::uint32_t>(0)->Put(map)->Put(x)->Put(y)->Put(z)->Put(facing);
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_Teleport(const char*, char* commandLine) {
    auto tokens = Tokens(commandLine);
    if (tokens.empty()) return 0;
    if (!std::isdigit(static_cast<unsigned char>(tokens[0][0])) && tokens[0][0] != '-') {
        CDataStore message;
        message.Put(9)->PutString(commandLine ? commandLine : "");
        ClientServices_Send(&message);
        return 1;
    }
    if (tokens.size() < 2) return 0;
    const float x = ToFloat(tokens[0]);
    const float y = ToFloat(tokens[1]);
    const float z = tokens.size() > 2 ? ToFloat(tokens[2]) : 0.0F;
    const float facing = tokens.size() > 3 ? ToFloat(tokens[3]) * 0.017453292F : 0.0F;
    CDataStore message;
    message.Put(198)->Put(x)->Put(y)->Put(z)->Put(facing);
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_CreateItem(const char*, const char* commandLine) { return SendValues<int>(19, static_cast<int>(ToSigned(commandLine))); }
int __fastcall CCommand_CreateGameObject(const char*, const char* commandLine) { return SendValues<int>(20, static_cast<int>(ToSigned(commandLine))); }
int __fastcall CCommand_CreateMonster(const char*, char* commandLine) { auto t=Tokens(commandLine," \t"); return SendValues<int>(17,t.empty()?0:static_cast<int>(ToSigned(t[0].c_str()))); }
int __fastcall CCommand_CreatePet(const char*, char* commandLine) { auto t=Tokens(commandLine," \t"); return SendValues<int>(17,t.empty()?0:-static_cast<int>(ToSigned(t[0].c_str()))); }
int CCommand_DestroyMonster() { return SendGuidOpcode(18, TargetGuid()); }
int CCommand_AttackPlayer() { return SendGuidOpcode(21, TargetGuid()); }

int __fastcall CCommand_TargetAttack(const char*, const char* commandLine) {
    if (!commandLine || !*commandLine) return 1;
    const auto victim = static_cast<std::uint64_t>(std::strtoull(commandLine, nullptr, 0));
    const auto attacker = TargetGuid();
    if (!attacker) { ConsolePrintf("No active player"); return 1; }
    CDataStore message;
    message.Put(22)->Put(attacker)->Put(victim);
    ClientServices_Send(&message);
    return 1;
}

int CCommand_Save() { return SendOpcode(326); }
int CCommand_BindPoint() { return SendOpcode(327); }
int __fastcall CCommand_Beastmaster(const char*, const char* commandLine) { return SendValues<std::uint8_t>(33, !(commandLine && SStrCmpI(commandLine, "off", 0x7FFFFFFFu) == 0)); }
int __fastcall CCommand_SendEvent(const char*, const char* commandLine) { return SendValues<int>(45, static_cast<int>(ToSigned(commandLine))); }
int CCommand_Recharge() { return SendOpcode(15); }

int __fastcall CCommand_Level(const char*, const char* commandLine) { const int level=static_cast<int>(ToSigned(commandLine)); return (level>0&&level<=100)?SendValues<int>(37,level):1; }
int __fastcall CCommand_PetLevel(const char*, const char* commandLine) { const int level=static_cast<int>(ToSigned(commandLine)); return (level>0&&level<=100)?SendValues<int>(38,level):1; }
int __fastcall CCommand_ClearQuest(const char*, const char* commandLine) { const int id=static_cast<int>(ToSigned(commandLine)); return id>=0?SendValues<int>(44,id):1; }
int __fastcall CCommand_FlagQuest(const char*, const char* commandLine) { const int id=static_cast<int>(ToSigned(commandLine)); return id>=1?SendValues<int>(42,id):1; }
int __fastcall CCommand_FlagQuestFinish(const char*, const char* commandLine) { const int id=static_cast<int>(ToSigned(commandLine)); return id>=1?SendValues<int>(43,id):1; }

int __fastcall CCommand_QuestCommand(const char* command, char* commandLine) {
    const auto tokens = Tokens(commandLine, " \t");
    if (tokens.size() < 2) return 0;
    g_questCommandId = ToUnsigned64(tokens[0]);
    const int giver = static_cast<int>(ToSigned(tokens[1].c_str()));
    if (SStrCmpI(command, "questquery", 0x7FFFFFFFu) == 0) QueryQuest(&g_questCommandId, giver);
    else if (SStrCmpI(command, "questaccept", 0x7FFFFFFFu) == 0) AcceptQuest(&g_questCommandId, giver);
    else if (SStrCmpI(command, "questcomplete", 0x7FFFFFFFu) == 0) CompleteQuest(&g_questCommandId, giver);
    else if (SStrCmpI(command, "questcancel", 0x7FFFFFFFu) == 0) QuestLogRemoveQuest(0);
    return 1;
}

void __fastcall QueryQuest(std::uint64_t* questID, int questGiver) { SendQuest(386, questID ? *questID : g_questCommandId, questGiver); }
void __fastcall AcceptQuest(std::uint64_t* questID, int questGiver) { SendQuest(389, questID ? *questID : g_questCommandId, questGiver); }
void __fastcall CompleteQuest(std::uint64_t* questID, int questGiver) { SendQuest(390, questID ? *questID : g_questCommandId, questGiver); }
void __fastcall QuestLogRemoveQuest(std::uint8_t questLogIndex) { SendValues<std::uint8_t>(400, questLogIndex); }

int CCommand_TaxiClearAllNodes() { return SendOpcode(408); }
int CCommand_TaxiEnableAllNodes() { return SendOpcode(409); }
int __fastcall CCommand_ChangeCellZone(const char*, const char* commandLine) { return SendValues<int>(12, static_cast<int>(ToSigned(commandLine))); }
int CCommand_Played() { return SendOpcode(445); }

int __fastcall CCommand_LootMethod(const char*, const std::uint8_t* commandLine) {
    const char mode = commandLine && *commandLine ? static_cast<char>(*commandLine) : 0;
    CDataStore message;
    message.Put(122);
    if (mode == 'F' || mode == 'f') message.Put<std::uint32_t>(0)->Put<std::uint64_t>(0);
    else if (mode == 'R' || mode == 'r') message.Put<std::uint32_t>(1)->Put<std::uint64_t>(0);
    else if (mode == 'M' || mode == 'm') message.Put<std::uint32_t>(2)->Put<std::uint64_t>(TargetGuid());
    else { ConsolePrintf("Valid loot methods: F, R, M"); return 1; }
    ClientServices_Send(&message);
    return 1;
}

int CCommand_CameraTarget() { return 1; }
int __fastcall CCommand_Reclaim(const char*, char* commandLine) { auto t=Tokens(commandLine," \t"); return t.empty()?0:SendValues<std::uint64_t>(451,ToUnsigned64(t[0])); }

int __fastcall CCommand_BuySpell(const char*, char* commandLine) {
    auto t=Tokens(commandLine," \t"); if(t.size()<2)return 0;
    CDataStore message; message.Put(420)->Put(ToUnsigned64(t[0]))->Put(static_cast<int>(ToSigned(t[1].c_str()))); ClientServices_Send(&message); return 1;
}
int __fastcall CCommand_SellItem(const char*, char* commandLine) {
    auto t=Tokens(commandLine," \t"); if(t.size()<2)return 0;
    CDataStore message; message.Put(368)->Put(ToUnsigned64(t[0]))->Put(ToUnsigned64(t[1]))->Put<std::uint8_t>(t.size()>2?static_cast<std::uint8_t>(ToSigned(t[2].c_str())):0); ClientServices_Send(&message); return 1;
}
int __fastcall CCommand_BuyItem(const char*, char* commandLine) {
    auto t=Tokens(commandLine," \t"); if(t.size()<3)return 0;
    CDataStore message; message.Put(370)->Put(ToUnsigned64(t[0]))->Put(static_cast<std::uint32_t>(ToSigned(t[1].c_str())))->Put(static_cast<std::uint32_t>(ToSigned(t[2].c_str())))->Put<std::uint8_t>(t.size()>3?static_cast<std::uint8_t>(ToSigned(t[3].c_str())!=0):0); ClientServices_Send(&message); return 1;
}
int __fastcall CCommand_BuyItemInSlot(const char*, char* commandLine) {
    auto t=Tokens(commandLine," \t"); if(t.size()<4)return 0;
    CDataStore message; message.Put(371)->Put(ToUnsigned64(t[0]))->Put(static_cast<std::uint32_t>(ToSigned(t[1].c_str())))->Put(ToUnsigned64(t[2]))->Put<std::uint8_t>(t.size()>4?static_cast<std::uint8_t>(ToSigned(t[4].c_str())):0)->Put(static_cast<std::uint32_t>(ToSigned(t[3].c_str()))); ClientServices_Send(&message); return 1;
}

std::uint32_t CmdMemParseNum(char** text) {
    if (!text || !*text) return 0;
    char* cursor = *text;
    while (*cursor && !std::isdigit(static_cast<unsigned char>(*cursor))) ++cursor;
    std::uint32_t value = 0;
    while (*cursor && std::isdigit(static_cast<unsigned char>(*cursor))) value = value * 10 + static_cast<unsigned>(*cursor++ - '0');
    *text = cursor;
    return value;
}

void CmdMemOutput(const char* filter, CMemDump* dump, const char* line) {
    if (!dump || !line || (filter && *filter && !std::strstr(line, filter))) return;
    auto* items = static_cast<CMemCmdItem*>(std::realloc(dump->items, sizeof(CMemCmdItem) * (dump->count + 1)));
    if (!items) return;
    dump->items = items;
    auto& item = dump->items[dump->count++];
    const auto tokens = Tokens(line, " \t");
    item.allocated = tokens.size() > 0 ? static_cast<std::uint32_t>(ToSigned(tokens[0].c_str())) : 0;
    item.committed = tokens.size() > 1 ? static_cast<std::uint32_t>(ToSigned(tokens[1].c_str())) : 0;
    item.reserved = tokens.size() > 2 ? static_cast<std::uint32_t>(ToSigned(tokens[2].c_str())) : 0;
    std::strncpy(item.name, tokens.size() > 3 ? tokens[3].c_str() : line, sizeof(item.name) - 1);
    dump->allocated += item.allocated; dump->committed += item.committed; dump->reserved += item.reserved;
}

void DebugPrintMemDump(CMemDump* dump) {
    if (!dump) return;
    const auto count = dump->count ? dump->count : 1;
    ConsolePrintf("*** MEMORY DUMP BEGIN ***");
    ConsolePrintf("   *** sums: %I64uk/%I64uk/%I64uk ***", dump->allocated, dump->committed, dump->reserved);
    ConsolePrintf("   *** averages: %uk/%uk/%uk ***", static_cast<unsigned>(dump->allocated/count), static_cast<unsigned>(dump->committed/count), static_cast<unsigned>(dump->reserved/count));
    for (std::uint32_t i=0;i<dump->count;++i) ConsolePrintf("%s", dump->items[i].name);
    ConsolePrintf("*** MEMORY DUMP END ***");
}

void __fastcall FilePrintMemDump(const char*, const char* outputFile) {
    if (!outputFile) return;
    if (FILE* file = std::fopen(outputFile, "wt")) { std::fputs("*** MEMORY DUMP BEGIN ***\n*** MEMORY DUMP END ***\n", file); std::fclose(file); }
}

int __fastcall CCommand_Mem(const char*, const std::uint8_t* commandLine) {
    CMemDump dump;
    if (commandLine && *commandLine) FilePrintMemDump(reinterpret_cast<const char*>(&dump), reinterpret_cast<const char*>(commandLine));
    else DebugPrintMemDump(&dump);
    std::free(dump.items);
    return 1;
}

long double __fastcall ClientCommandSqrt(float value) { return value < 0.0F ? 0.0L : std::sqrt(value); }

int CDataStoreFetchWrite(CDataStore* store, unsigned position, unsigned bytes, const char* fileName, int lineNumber) {
    return store ? store->FetchWrite(position, bytes, fileName, lineNumber) : 0;
}

void UninstallGameConsoleCommands(const char*) { for (auto& handle : g_commandHandles) { ConsoleCommandUnregister(handle); handle=nullptr; } UninstallGMCommands(nullptr); }

int _E1_0() { INFINITY_0 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E2_0() { return _E1_0(); }
void _E4_0() { TWO_PI_0 = 6.28318530717958647692F; }
int _E5_0() { _E4_0(); return 0; }
void _E7_0() { OO_TWO_PI_0 = 1.0F / TWO_PI_0; }
int _E8_0() { _E7_0(); return 0; }
void _E93_0() {}
int _E94_0() { _E93_0(); return 0; }
void _E96_0() {}
int _E97_0() { _E96_0(); return 0; }
void _E99_0() {}
int _E100_0() { _E99_0(); return 0; }
void _E102_0() {}
int _E103_0() { _E102_0(); return 0; }
void _E105_0() { OOSMOLTILE_SIZE_0 = 1.0F / 4.1666665F; }
int _E106_0() { _E105_0(); return 0; }
void _E19_0() { MAX_DUEL_DISTANCE_SQUARED_0 = 100.0F; }
int _E20_0() { _E19_0(); return 0; }
void _E22_0() {}
int _E23_0() { _E22_0(); return 0; }
void _E25_0() {}
int _E26_0() { _E25_0(); return 0; }
void _E28_0() {}
int _E29_0() { _E28_0(); return 0; }
void _E31_0() {}
int _E32_0() { _E31_0(); return 0; }
void _E34_0() {}
int _E35_0() { _E34_0(); return 0; }
void _E37_0() {}
int _E38_0() { _E37_0(); return 0; }
void _E40_0() {}
int _E41_0() { _E40_0(); return 0; }
void _E43_0() {}
int _E44_0() { _E43_0(); return 0; }
void _E46_0() {}
int _E47_0() { _E46_0(); return 0; }
void _E49_0() {}
int _E50_0() { _E49_0(); return 0; }
void _E52_0() {}
int _E53_0() { _E52_0(); return 0; }
void _E87_0() {}
int _E90_0() { _E87_0(); return 0; }
void _E88_0() {}
int _E89_0() { _E88_0(); return 0; }
