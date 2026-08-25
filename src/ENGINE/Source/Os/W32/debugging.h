#pragma once
#include <cstdarg>
#include <windows.h>
BOOL __fastcall OsBeep(DWORD frequency, DWORD duration);
void OsOutputDebugString(const char* format, ...);
void OsOutputDebugStringV(const char* format, std::va_list args);
