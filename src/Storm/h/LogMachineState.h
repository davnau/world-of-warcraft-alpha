#pragma once

#include "Storm/h/sapibase.h"

using MACHINE_LOG_PROC = void (__cdecl*)(void* parameter, const char* format, ...);

int __fastcall CheckMachineStateSymbolHelper();
void LoadMachineStateSymbols();
void UnloadMachineStateSymbols();
void __fastcall LogComputerInfoHeader(unsigned int options, MACHINE_LOG_PROC logLineProc,
    void* logLineProcParameter, char* headerTitleLine, SYSTEMTIME* time);
void __fastcall LogMachineState(unsigned int options, MACHINE_LOG_PROC logLineProc,
    void* logLineProcParameter, unsigned int stackFramesToSkip, CONTEXT* context);
int __fastcall LogMiniDump(HANDLE logFile, EXCEPTION_POINTERS* exceptionPointers,
    unsigned int userStringCount, char** userStrings);
BOOL __fastcall LogMiniDumpIsAvailable();
