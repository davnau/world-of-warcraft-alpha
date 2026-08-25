#include "Storm/h/storm.h"

#include <cstdarg>
#include <cstdio>

// PDB source: D:\build\buildWoW\Storm\Source\W32\SSystem.cpp
// RVA: 0x00257F10
void SOutputDebugString(const char* format, ...) {
    char buffer[256];
    va_list arguments;
    va_start(arguments, format);
    _vsnprintf(buffer, sizeof(buffer), format, arguments);
    va_end(arguments);
    OutputDebugStringA(buffer);
}
