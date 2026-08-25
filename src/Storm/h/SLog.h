#pragma once

#include "Storm/h/sapibase.h"

#include <cstdarg>

int __stdcall SLogCreate(const char* filename, unsigned int flags, HSLOG__** log);
void __stdcall SLogClose(HSLOG__* log);
void __stdcall SLogDestroy();
void __stdcall SLogDump(HSLOG__* log, const void* data, unsigned int bytes);
void __stdcall SLogFlush(HSLOG__* log);
void __stdcall SLogFlushAll();
void __stdcall SLogGetDefaultDirectory(char* directoryName, unsigned int directoryNameSize);
void __stdcall SLogInitialize();
HSLOG__* __stdcall SLogIsInitialized();
void SLogPend(HSLOG__* log, const char* format, ...);
int __stdcall SLogSetAbsIndent(HSLOG__* log, int indent);
void __stdcall SLogSetDefaultDirectory(const char* directoryName);
int __stdcall SLogSetIndent(HSLOG__* log, int deltaIndent);
void __stdcall SLogSetTimestamp(HSLOG__* log, int timestamp);
void __stdcall SLogVWrite(HSLOG__* log, const char* format, va_list arguments);
void SLogWrite(HSLOG__* log, const char* format, ...);
