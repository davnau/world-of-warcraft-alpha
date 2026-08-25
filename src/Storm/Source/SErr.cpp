#include "Storm/h/storm.h"

namespace {

DWORD s_lasterror = 0;
void* s_logCallback = nullptr;

} // namespace

// PDB source: D:\build\buildWoW\Storm\Source\SErr.cpp
// RVA: 0x00237E80
DWORD __stdcall SErrGetLastError() {
    return s_lasterror;
}

// PDB source: D:\build\buildWoW\Storm\Source\SErr.cpp
// RVA: 0x002385A0
void __stdcall SErrSetLastError(DWORD errorcode) {
    s_lasterror = errorcode;
    SetLastError(errorcode);
}

void SErrSetLogCallback(void* callback) {
    s_logCallback = callback;
}

void SErrDestroy() {
    s_logCallback = nullptr;
    s_lasterror = 0;
}
