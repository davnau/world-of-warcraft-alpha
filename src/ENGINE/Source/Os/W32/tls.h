#pragma once
#include <windows.h>
DWORD __fastcall OsTlsAlloc();void __fastcall OsTlsFree(DWORD index);LPVOID __fastcall OsTlsGetValue(DWORD index);BOOL __fastcall OsTlsSetValue(DWORD index,void* value);
