#include "WoW/Source/clientruntime.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <string>
#include <vector>

namespace {
struct CommandEntry {
    std::string name;
    ClientCommandCategory category;
    ClientCommandFunction function;
    std::uint64_t serial;
};

std::mutex g_commandMutex;
std::vector<CommandEntry> g_commands;
std::uint64_t g_nextSerial = 1;
std::vector<std::uint8_t> g_lastPacket;
std::uint32_t g_lastOpcode = 0;
std::uint64_t g_selectedTarget = 0;
int g_ghosting = 0;

const char* HandleFor(const CommandEntry& entry) {
    return reinterpret_cast<const char*>(entry.serial);
}
}

const char* ConsoleCommandRegister(const char*, const char* name,
                                   ClientCommandCategory category,
                                   ClientCommandFunction function) {
    if (!name) return nullptr;
    std::lock_guard lock(g_commandMutex);
    g_commands.push_back({name, category, function, g_nextSerial++});
    return HandleFor(g_commands.back());
}

void ConsoleCommandUnregister(const char* handle) {
    if (!handle) return;
    const auto serial = reinterpret_cast<std::uintptr_t>(handle);
    std::lock_guard lock(g_commandMutex);
    g_commands.erase(std::remove_if(g_commands.begin(), g_commands.end(),
                                    [serial](const CommandEntry& entry) {
                                        return entry.serial == serial;
                                    }),
                     g_commands.end());
}

void ConsoleCommandUnregisterAll() {
    std::lock_guard lock(g_commandMutex);
    g_commands.clear();
}

int ConsoleCommandExecute(const char* name, const char* commandLine) {
    if (!name) return 0;
    ClientCommandFunction function = nullptr;
    {
        std::lock_guard lock(g_commandMutex);
        for (auto it = g_commands.rbegin(); it != g_commands.rend(); ++it) {
            if (SStrCmpI(it->name.c_str(), name, 0x7FFFFFFFu) == 0) {
                function = it->function;
                break;
            }
        }
    }
    return function ? function(commandLine, commandLine) : 0;
}

void ClientServices_Send(const CDataStore* message) {
    g_lastPacket.clear();
    g_lastOpcode = 0;
    if (!message) return;
    void* data = nullptr;
    unsigned size = 0;
    unsigned alloc = 0;
    message->GetBufferParams(&data, &size, &alloc);
    if (!data || !size) return;
    g_lastPacket.assign(static_cast<const std::uint8_t*>(data),
                        static_cast<const std::uint8_t*>(data) + size);
    if (size >= sizeof(g_lastOpcode))
        std::memcpy(&g_lastOpcode, g_lastPacket.data(), sizeof(g_lastOpcode));
}

const std::uint8_t* ClientServices_LastPacket(std::size_t* size) {
    if (size) *size = g_lastPacket.size();
    return g_lastPacket.empty() ? nullptr : g_lastPacket.data();
}

std::uint32_t ClientServices_LastOpcode() { return g_lastOpcode; }
void ClientServices_ClearLastPacket() { g_lastPacket.clear(); g_lastOpcode = 0; }

int ClientRuntime_GetSelectedTarget(std::uint64_t* guid) {
    if (guid) *guid = g_selectedTarget;
    return g_selectedTarget != 0;
}

void ClientRuntime_SetSelectedTarget(std::uint64_t guid) { g_selectedTarget = guid; }
void ClientRuntime_SetGhosting(int enabled) { g_ghosting = enabled != 0; }
int ClientRuntime_IsGhosting() { return g_ghosting; }

int SStrToInt(const char* text) {
    if (!text) return 0;
    char* end = nullptr;
    const long value = std::strtol(text, &end, 0);
    return static_cast<int>(value);
}

void ConsolePrintf(const char* format, ...) {
    if (!format) return;
    va_list args;
    va_start(args, format);
    std::vfprintf(stdout, format, args);
    std::fputc('\n', stdout);
    va_end(args);
}
