#pragma once
#include <cstdint>
#include <cstdio>
struct ContextData;
void OsCallInitialize(const char* outputFile);void __cdecl OsCallDestroy();void OsCallGlobalEnable(int);void OsCallEnable(int);ContextData* OsCallInitializeContext(const char* name);void OsCallDestroyContext(ContextData*);void OsCallSetContext(ContextData*);void OsCallResetContext(ContextData*);void __cdecl OsCallBeginTurn();void* __fastcall OsCallEndTurn();void __cdecl OsCallCompleteTurn();void OsCallDump(const char* path);void OsCallDumpContextData(FILE*,ContextData*);void OsCallDumpProfileData();int __cdecl OsCallEnter(std::uintptr_t funcAddr,std::uintptr_t retAddr);void* __cdecl OsCallExit();void __fastcall OsCallData(unsigned int);void __stdcall OsCallData(unsigned int,unsigned int tag);
