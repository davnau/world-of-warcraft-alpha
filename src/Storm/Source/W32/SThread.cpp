#include "Storm/h/SThread.h"

#include <memory>

namespace {
struct ProcessCompletion {
    HANDLE process;
    void (__fastcall* callback)(void*);
    void* data;
};

DWORD WINAPI ProcessCompletionThread(void* parameter) {
    std::unique_ptr<ProcessCompletion> completion(static_cast<ProcessCompletion*>(parameter));
    WaitForSingleObject(completion->process, INFINITE);
    CloseHandle(completion->process);
    if (completion->callback) completion->callback(completion->data);
    return 0;
}
} // namespace

void __fastcall SCreateProcess(char* applicationName, char* commandLine,
    void (__fastcall* callbackWhenProcessCompletes)(void*), void* callbackData) {
    STARTUPINFOA startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessA(applicationName, commandLine, nullptr, nullptr, FALSE, 0, nullptr, nullptr, &startup, &process)) return;
    CloseHandle(process.hThread);
    if (!callbackWhenProcessCompletes) {
        CloseHandle(process.hProcess);
        return;
    }
    auto completion = std::make_unique<ProcessCompletion>();
    completion->process = process.hProcess;
    completion->callback = callbackWhenProcessCompletes;
    completion->data = callbackData;
    HANDLE waiter = CreateThread(nullptr, 0, ProcessCompletionThread, completion.get(), 0, nullptr);
    if (waiter) { completion.release(); CloseHandle(waiter); }
    else CloseHandle(process.hProcess);
}

DWORD __stdcall SGetCurrentThreadId() { return GetCurrentThreadId(); }
int __fastcall SGetCurrentThreadPriority() { return GetThreadPriority(GetCurrentThread()); }
void SSetCurrentThreadPriority(int priority) { SetThreadPriority(GetCurrentThread(), priority); }

void* __fastcall SCreateThread(unsigned int (__stdcall* startAddress)(void*), void* parameter,
    unsigned int creationFlags, unsigned int* threadId, char*) {
    if (!startAddress) return nullptr;
    DWORD id = 0;
    HANDLE thread = CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(startAddress),
        parameter, creationFlags, &id);
    if (threadId) *threadId = id;
    return thread;
}
