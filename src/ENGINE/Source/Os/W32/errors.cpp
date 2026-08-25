#include "ENGINE/Source/Os/W32/errors.h"
#include <windows.h>
char* __fastcall OsGetLastErrorStr(unsigned long error){char*text=nullptr;FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,nullptr,error,0,reinterpret_cast<char*>(&text),0,nullptr);return text;}
char* OsGetLastErrorStr(){return OsGetLastErrorStr(GetLastError());}
void OsFreeLastErrorStr(void* text){if(text)LocalFree(text);}
