#include "Storm/h/LogMachineState.h"

#include <dbghelp.h>
#include <tlhelp32.h>

#include <array>
#include <algorithm>
#include <cstring>
#include <mutex>

namespace {
HMODULE s_dbgHelp = nullptr;
std::recursive_mutex s_symbolLock;

using MiniDumpWriteDumpProc = BOOL (WINAPI*)(HANDLE, DWORD, HANDLE, MINIDUMP_TYPE,
    const MINIDUMP_EXCEPTION_INFORMATION*, const MINIDUMP_USER_STREAM_INFORMATION*, const MINIDUMP_CALLBACK_INFORMATION*);

MiniDumpWriteDumpProc MiniDumpProc() {
    std::lock_guard lock(s_symbolLock);
    if (!s_dbgHelp) s_dbgHelp = LoadLibraryA("dbghelp.dll");
    return s_dbgHelp ? reinterpret_cast<MiniDumpWriteDumpProc>(GetProcAddress(s_dbgHelp, "MiniDumpWriteDump")) : nullptr;
}

} // namespace

int __fastcall CheckMachineStateSymbolHelper() { return MiniDumpProc() ? 1 : 0; }
void LoadMachineStateSymbols() { (void)MiniDumpProc(); }
void UnloadMachineStateSymbols() {
    std::lock_guard lock(s_symbolLock);
    if (s_dbgHelp) { FreeLibrary(s_dbgHelp); s_dbgHelp = nullptr; }
}

void __fastcall LogComputerInfoHeader(unsigned int, MACHINE_LOG_PROC logLineProc,
    void* parameter, char* headerTitleLine, SYSTEMTIME* suppliedTime) {
    if (!logLineProc) return;
    SYSTEMTIME localTime{};
    if (!suppliedTime) { GetLocalTime(&localTime); suppliedTime = &localTime; }
    char userName[256] = {};
    DWORD userChars = sizeof(userName);
    GetUserNameA(userName, &userChars);
    char computerName[256] = {};
    DWORD computerChars = sizeof(computerName);
    GetComputerNameA(computerName, &computerChars);
    MEMORYSTATUSEX memory{sizeof(memory)};
    GlobalMemoryStatusEx(&memory);
    logLineProc(parameter, "%s", headerTitleLine ? headerTitleLine : "Machine State");
    logLineProc(parameter, "Date: %04u-%02u-%02u %02u:%02u:%02u", suppliedTime->wYear,
        suppliedTime->wMonth, suppliedTime->wDay, suppliedTime->wHour, suppliedTime->wMinute, suppliedTime->wSecond);
    logLineProc(parameter, "User: %s", userName);
    logLineProc(parameter, "Computer: %s", computerName);
    logLineProc(parameter, "Processors: %u", [] { SYSTEM_INFO info{}; GetSystemInfo(&info); return info.dwNumberOfProcessors; }());
    logLineProc(parameter, "Physical memory: %llu / %llu", memory.ullAvailPhys, memory.ullTotalPhys);
}

void __fastcall LogMachineState(unsigned int options, MACHINE_LOG_PROC logLineProc,
    void* parameter, unsigned int stackFramesToSkip, CONTEXT* context) {
    if (!logLineProc) return;
    char title[] = "---------------- Machine State ----------------";
    LogComputerInfoHeader(options, logLineProc, parameter, title, nullptr);
    logLineProc(parameter, "Process ID: %u  Thread ID: %u", GetCurrentProcessId(), GetCurrentThreadId());
#if defined(_M_IX86)
    if (context) {
        logLineProc(parameter, "EIP=%08X ESP=%08X EBP=%08X EAX=%08X EBX=%08X ECX=%08X EDX=%08X",
            context->Eip, context->Esp, context->Ebp, context->Eax, context->Ebx, context->Ecx, context->Edx);
    }
#else
    (void)context;
#endif
    std::array<void*, 64> frames{};
    const USHORT captured = CaptureStackBackTrace(stackFramesToSkip + 1,
        static_cast<DWORD>(frames.size()), frames.data(), nullptr);
    for (USHORT index = 0; index < captured; ++index) logLineProc(parameter, "#%02u %p", index, frames[index]);
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, GetCurrentProcessId());
    if (snapshot != INVALID_HANDLE_VALUE) {
        MODULEENTRY32 module{sizeof(module)};
        if (Module32First(snapshot, &module)) {
            do {
                logLineProc(parameter, "Module %p %8u %s", module.modBaseAddr, module.modBaseSize, module.szExePath);
            } while (Module32Next(snapshot, &module));
        }
        CloseHandle(snapshot);
    }
    logLineProc(parameter, "-------------------------------------------------");
}

BOOL __fastcall LogMiniDumpIsAvailable() { return MiniDumpProc() != nullptr; }

int __fastcall LogMiniDump(HANDLE logFile, EXCEPTION_POINTERS* exceptionPointers,
    unsigned int userStringCount, char** userStrings) {
    MiniDumpWriteDumpProc writeDump = MiniDumpProc();
    if (!writeDump || !logFile || logFile == INVALID_HANDLE_VALUE) return 0;
    MINIDUMP_EXCEPTION_INFORMATION exceptionInfo{};
    exceptionInfo.ThreadId = GetCurrentThreadId();
    exceptionInfo.ExceptionPointers = exceptionPointers;
    exceptionInfo.ClientPointers = FALSE;
    std::array<MINIDUMP_USER_STREAM, 16> streams{};
    const unsigned int count = std::min<unsigned int>(userStringCount, static_cast<unsigned int>(streams.size()));
    unsigned int used = 0;
    for (unsigned int index = 0; index < count; ++index) {
        if (!userStrings || !userStrings[index]) continue;
        streams[used].Type = static_cast<MINIDUMP_STREAM_TYPE>(CommentStreamA + used);
        streams[used].BufferSize = static_cast<ULONG>(std::strlen(userStrings[index]) + 1);
        streams[used].Buffer = userStrings[index];
        ++used;
    }
    MINIDUMP_USER_STREAM_INFORMATION streamInfo{used, streams.data()};
    return writeDump(GetCurrentProcess(), GetCurrentProcessId(), logFile, MiniDumpWithDataSegs,
        exceptionPointers ? &exceptionInfo : nullptr, used ? &streamInfo : nullptr, nullptr) ? 1 : 0;
}
