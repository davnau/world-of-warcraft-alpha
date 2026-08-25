#include "ENGINE/Source/Os/W32/debugging.h"
#include <cstdio>
BOOL __fastcall OsBeep(DWORD frequency,DWORD duration){return Beep(frequency,duration);}
void OsOutputDebugStringV(const char* format,std::va_list args){if(!format)return;char text[4096]{};vsnprintf_s(text,sizeof(text),_TRUNCATE,format,args);OutputDebugStringA(text);}
void OsOutputDebugString(const char* format,...){std::va_list args;va_start(args,format);OsOutputDebugStringV(format,args);va_end(args);}
