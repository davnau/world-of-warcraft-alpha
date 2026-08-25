#include "WoW/Source/Game/GameClient/gametime_c.h"

#include "WoW/Source/Console/consolecommand.h"
#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

CGameTime g_clientGameTime;
GameTimeForcedChangeCallback s_forcedChangeCallbacks[4]{};

namespace {
int s_connectionInitializerCount = 0;

int ParseTimeMinutes(const char* text) {
    if (text == nullptr) return 0;
    const int hour = std::clamp(SStrToInt(text), 0, 23);
    const char* separator = std::strchr(text, ' ');
    if (separator == nullptr) separator = std::strchr(text, ':');
    const int minute = std::clamp(separator != nullptr ? SStrToInt(separator + 1) : 0, 0, 59);
    return hour * 60 + minute;
}

int __fastcall ConsoleGameTimeWrapper(const char*, const char* arguments) {
    return CCommand_GameTime(nullptr, arguments);
}

int __fastcall ConsoleLocalTimeWrapper(const char*, const char* arguments) {
    return CCommand_LocalTime(nullptr, arguments);
}

int __fastcall ConsoleGameSpeedWrapper(const char*, const char* arguments) {
    return CCommand_GameSpeed(nullptr, arguments);
}

int __fastcall ConsoleSpawnTimeWrapper(const char*, const char* arguments) {
    return CCommand_SpawnTime(nullptr, arguments);
}

int __fastcall ConsoleShowLocalWrapper(const char*, const char*) {
    return CCommand_ShowLocalGameTime();
}

int __fastcall ConsoleShowServerWrapper(const char*, const char*) {
    return CCommand_ShowServerGameTime();
}
} // namespace

void _E150_2() { ++s_connectionInitializerCount; }
void _E151_0() { if (s_connectionInitializerCount > 0) --s_connectionInitializerCount; }
int _E152_1() { return 0; }
int _E153_2() { _E150_2(); return _E152_1(); }
CGameTime* _E156_0() { return &g_clientGameTime; }
void _E157_0() { g_clientGameTime.Destroy(); }
int _E158_1() { return 0; }
int _E159_1() { _E156_0(); return _E158_1(); }

int ClientGameTimeTickHandler(float* elapsed) {
    if (elapsed == nullptr) return 0;
    g_clientGameTime.GameTimeUpdate(*elapsed);
    return 1;
}

void ClientInitializeGameTime() {
    ConsoleCommandRegister(nullptr, "gametime", CLIENT_COMMAND_CATEGORY_DEBUG, ConsoleGameTimeWrapper);
    ConsoleCommandRegister(nullptr, "localtime", CLIENT_COMMAND_CATEGORY_DEBUG, ConsoleLocalTimeWrapper);
    ConsoleCommandRegister(nullptr, "gamespeed", CLIENT_COMMAND_CATEGORY_DEBUG, ConsoleGameSpeedWrapper);
    ConsoleCommandRegister(nullptr, "spawntime", CLIENT_COMMAND_CATEGORY_DEBUG, ConsoleSpawnTimeWrapper);
    ConsoleCommandRegister(nullptr, "dtime", CLIENT_COMMAND_CATEGORY_DEBUG, ConsoleShowLocalWrapper);
    ConsoleCommandRegister(nullptr, "time", CLIENT_COMMAND_CATEGORY_DEBUG, ConsoleShowServerWrapper);
}

int CCommand_ShowLocalGameTime() {
    char buffer[128]{};
    std::snprintf(buffer, sizeof(buffer), "Current game time is %02d:%02d",
                  g_clientGameTime.WowTime.m_hour, g_clientGameTime.WowTime.m_minute);
    ConsolePrintf("%s", buffer);
    return 1;
}

int CCommand_ShowServerGameTime() {
    CDataStore message;
    message.Put<std::uint32_t>(72);
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_GameTime(const char*, const char* arguments) {
    ::WowTime newTime = g_clientGameTime.WowTime;
    newTime.SetHourAndMinutes(ParseTimeMinutes(arguments));
    std::uint32_t encoded = 0;
    ::WowTime::WowEncodeTime(encoded, newTime);
    CDataStore message;
    message.Put<std::uint32_t>(68)->Put(encoded);
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_LocalTime(const char*, const char* arguments) {
    ::WowTime newTime = g_clientGameTime.WowTime;
    newTime.SetHourAndMinutes(ParseTimeMinutes(arguments));
    g_clientGameTime.SetTimeDateBias(0, 0, 0);
    const int difference = newTime.GetHourAndMinutes() - g_clientGameTime.WowTime.GetHourAndMinutes();
    g_clientGameTime.SetTimeDateBias(difference, 0, 1);
    UpdateTime();
    return 1;
}

void UpdateTime() {
    for (GameTimeForcedChangeCallback callback : s_forcedChangeCallbacks) {
        if (callback != nullptr) callback(g_clientGameTime.WowTime.m_hour, g_clientGameTime.WowTime.m_minute);
    }
    char timeString[128]{};
    char buffer[256]{};
    ::WowTime::WowGetTimeString(&g_clientGameTime.WowTime, timeString, sizeof(timeString));
    std::snprintf(buffer, sizeof(buffer), "Time set to %s", timeString);
    ConsolePrintf("%s", buffer);
}

int __fastcall CCommand_SpawnTime(const char*, const char* arguments) {
    CDataStore message;
    message.Put<std::uint32_t>(49)->Put<std::uint32_t>(ParseTimeMinutes(arguments));
    ClientServices_Send(&message);
    return 1;
}

int __fastcall CCommand_GameSpeed(const char*, const char* arguments) {
    const float speed = arguments != nullptr ? std::strtof(arguments, nullptr) : 0.0F;
    CDataStore message;
    message.Put<std::uint32_t>(70)->Put(speed);
    ClientServices_Send(&message);
    return 1;
}

int __fastcall ReceiveNewGameSpeed(NETMESSAGE, int, unsigned, CDataStore* message) {
    float speed = 0.0F;
    if (message == nullptr || !message->Get(&speed)->IsRead()) return 0;
    const double previous = g_clientGameTime.GameTimeSetMinutesPerSecond(speed);
    char buffer[256]{};
    std::snprintf(buffer, sizeof(buffer), "Gamespeed set from %.03f to %.03f", previous, speed);
    ConsolePrintf("%s", buffer);
    return 1;
}

int __fastcall ReceiveNewTimeSpeed(NETMESSAGE, int, unsigned, CDataStore* message) {
    std::uint32_t encoded = 0;
    float speed = 0.0F;
    if (message == nullptr || !message->Get(&encoded)->Get(&speed)->IsRead()) return 0;
    ::WowTime time;
    ::WowTime::WowDecodeTime(encoded, &time);
    g_clientGameTime.GameTimeSetTime(&time);
    UpdateTime();
    g_clientGameTime.GameTimeSetMinutesPerSecond(speed);
    return 1;
}

int __fastcall ReceiveGameTimeUpdate(NETMESSAGE, int, unsigned, CDataStore* message) {
    std::uint32_t encoded = 0;
    if (message == nullptr || !message->Get(&encoded)->IsRead()) return 0;
    ::WowTime time;
    ::WowTime::WowDecodeTime(encoded, &time);
    g_clientGameTime.GameTimeSync(&time, 0);
    return 1;
}

int __fastcall ReceiveServerTime(NETMESSAGE, int, unsigned, CDataStore* message) {
    std::uint32_t encoded = 0;
    if (message == nullptr || !message->Get(&encoded)->IsRead()) return 0;
    ::WowTime time;
    ::WowTime::WowDecodeTime(encoded, &time);
    char timeString[128]{};
    ::WowTime::WowGetTimeString(&time, timeString, sizeof(timeString));
    ConsolePrintf("Server game time is %s", timeString);
    return 1;
}

int __fastcall ReceiveNewGameTime(NETMESSAGE, int, unsigned, CDataStore* message) {
    std::uint32_t encoded = 0;
    if (message == nullptr || !message->Get(&encoded)->IsRead()) return 0;
    ::WowTime time;
    ::WowTime::WowDecodeTime(encoded, &time);
    g_clientGameTime.GameTimeSetTime(&time);
    UpdateTime();
    return 1;
}

void ClientDestroyGameTime() {
    ConsoleCommandUnregisterAll();
    g_clientGameTime.Destroy();
}

void __fastcall SetGameTimeForcedChangeCallback(GameTimeForcedChangeCallback addOrRemove,
                                                  GameTimeForcedChangeCallback callback) {
    for (auto& slot : s_forcedChangeCallbacks) {
        if (slot == nullptr && addOrRemove != nullptr) {
            slot = callback;
            return;
        }
        if (slot == callback && addOrRemove == nullptr) {
            slot = nullptr;
            return;
        }
    }
}
