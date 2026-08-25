#pragma once

int __fastcall WowLogInitialize();
void __cdecl WowLogDestroy();
void WLog();
void __stdcall WVLog(const char* fmt, char* arglist);
void __stdcall WLogDumpHex(unsigned char* data, unsigned char* len);

int _E2_372();
int _E1_372();

