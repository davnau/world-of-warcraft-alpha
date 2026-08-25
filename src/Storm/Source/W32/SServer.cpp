#include "Storm/h/storm.h"

namespace {

using TryEnterCriticalSectionProc = BOOL(WINAPI*)(LPCRITICAL_SECTION);

HMODULE s_kernel32lib = nullptr;
TryEnterCriticalSectionProc s_tryEnterPtr = nullptr;

} // namespace

// PDB source: D:\build\buildWoW\Storm\Source\W32\SServer.cpp
// RVA: 0x0024C9F0
void __fastcall SServerInitialize() {
    if (s_tryEnterPtr != nullptr) {
        return;
    }
    s_kernel32lib = LoadLibraryA("kernel32.dll");
    if (s_kernel32lib != nullptr) {
        s_tryEnterPtr = reinterpret_cast<TryEnterCriticalSectionProc>(
            GetProcAddress(s_kernel32lib, "TryEnterCriticalSection"));
    }
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SServer.cpp
// RVA: 0x0024CA20
void __fastcall SServerDestroy() {
    if (s_kernel32lib != nullptr) {
        FreeLibrary(s_kernel32lib);
    }
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SServer.cpp
// RVA: 0x0024CA40
int __fastcall STryEnterCriticalSection(void* opaqueData) {
    if (s_tryEnterPtr == nullptr) {
        ExitProcess(1U);
    }
    return s_tryEnterPtr(static_cast<LPCRITICAL_SECTION>(opaqueData));
}
