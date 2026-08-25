#include "Storm/h/storm.h"

#include <cstddef>
#include <cstring>

namespace {

[[noreturn]] void SStrNullArgument() {
    // The preserved paths report an application-fatal error and then terminate.
    // SErr's reporting state is recovered separately; preserve the non-returning contract here.
    ExitProcess(1U);
}

} // namespace

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x002391F0
const char* __fastcall SStrChr(const char* string, char ch) {
    if (string == nullptr) {
        SStrNullArgument();
    }
    for (; *string != '\0'; ++string) {
        if (*string == ch) {
            return string;
        }
    }
    return nullptr;
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x00239240
char* __fastcall SStrChr(char* string, char ch) {
    return const_cast<char*>(SStrChr(static_cast<const char*>(string), ch));
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x00239290
const char* __fastcall SStrChrR(const char* string, char ch) {
    if (string == nullptr) {
        SStrNullArgument();
    }
    const char* result = nullptr;
    for (; *string != '\0'; ++string) {
        if (*string == ch) {
            result = string;
        }
    }
    return result;
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x002392E0
char* __fastcall SStrChrR(char* string, char ch) {
    return const_cast<char*>(SStrChrR(static_cast<const char*>(string), ch));
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x00239330
int __stdcall SStrCmp(const char* string1, const char* string2, std::size_t maxchars) {
    if (string1 == nullptr || string2 == nullptr) {
        SStrNullArgument();
    }
    return std::strncmp(string1, string2, maxchars);
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x002393A0
int __stdcall SStrCmpI(const char* string1, const char* string2, std::size_t maxchars) {
    if (string1 == nullptr || string2 == nullptr) {
        SStrNullArgument();
    }
    return _strnicmp(string1, string2, maxchars);
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x0023A7C0
void __stdcall SStrUpper(char* string) {
    _strupr(string);
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x0023A7E0
void __stdcall SStrLower(char* string) {
    _strlwr(string);
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x00239680
unsigned long __stdcall SStrLen(const char* string) {
    if (string == nullptr) {
        SStrNullArgument();
    }
    const char* cursor = string;
    while (*cursor != '\0') {
        ++cursor;
    }
    return static_cast<unsigned long>(cursor - string);
}

// PDB source: D:\build\buildWoW\Storm\Source\SStr.cpp
// RVA: 0x00239740
unsigned long __stdcall SStrLen(const wchar_t* string) {
    if (string == nullptr) {
        SStrNullArgument();
    }
    const wchar_t* cursor = string;
    while (*cursor != L'\0') {
        ++cursor;
    }
    return static_cast<unsigned long>(cursor - string);
}
