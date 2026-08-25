// Reconstructed from the PDB-mapped functions in
// D:\build\buildWoW\Storm\Source\soprintf.cpp (RVA 0x24C420-0x24C990).
// The recovered Hex-Rays listing is retained in WoWClient_recovered as evidence.

#include "Storm/h/storm.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

char* __cdecl vsnoprintf(char* out, int outSize, const char* format, char* argumentList)
{
    if (!out || outSize <= 0)
        return out;
    // On the preserved x86 ABI va_list is the address of the argument area.
    va_list arguments = reinterpret_cast<va_list>(argumentList);
    const int written = _vsnprintf_s(out, static_cast<size_t>(outSize), _TRUNCATE, format, arguments);
    // The original source declared char*, but the recovered epilogue returns the
    // output character count through EAX; preserve that ABI quirk exactly.
    const int count = written < 0 ? static_cast<int>(std::strlen(out)) : written;
    return reinterpret_cast<char*>(static_cast<uintptr_t>(count));
}

int __fastcall ParseFormatSpecifier(const char** size, unsigned long* argumentSize, int* orderingPtr)
{
    const char* cursor = *size;
    int order = 0;
    while (*cursor >= '0' && *cursor <= '9') order = order * 10 + *cursor++ - '0';
    if (*cursor == '$') { *orderingPtr = order > 0 ? order - 1 : 0; ++cursor; }
    while (std::strchr("#0- +", *cursor)) ++cursor;
    while ((*cursor >= '0' && *cursor <= '9') || *cursor == '*') ++cursor;
    if (*cursor == '.') { ++cursor; while ((*cursor >= '0' && *cursor <= '9') || *cursor == '*') ++cursor; }
    *argumentSize = 0;
    if (cursor[0] == 'l' && cursor[1] == 'l') { *argumentSize = 2; cursor += 2; }
    else if (cursor[0] == 'I' && cursor[1] == '6' && cursor[2] == '4') { *argumentSize = 2; cursor += 3; }
    else if (*cursor == 'l' || *cursor == 'h' || *cursor == 'L') ++cursor;
    const char conversion = *cursor;
    if (conversion != '\0') ++cursor;
    if (std::strchr("eEfFgG", conversion)) *argumentSize = 3;
    else if (std::strchr("sSpP", conversion)) *argumentSize = 1;
    else if (conversion == '\0' || conversion == '%') *argumentSize = 4;
    *size = cursor;
    return *argumentSize != 4;
}

void RemoveOrderingFromFormatSpecifier(char* format)
{
    char* dollarSign = std::strchr(format, '$');
    if (!dollarSign) return;
    char* firstDigit = dollarSign;
    while (firstDigit > format && firstDigit[-1] >= '0' && firstDigit[-1] <= '9') --firstDigit;
    std::memmove(firstDigit, dollarSign + 1, std::strlen(dollarSign + 1) + 1);
}

void FixUpLongLongFormatSpecifier(char* format)
{
    char* longLong = std::strstr(format, "ll");
    if (!longLong) return;
    std::memmove(longLong + 3, longLong + 2, std::strlen(longLong + 2) + 1);
    std::memcpy(longLong, "I64", 3);
}

char* __cdecl vsoprintf(char* out, const char* format, char* argumentList) { return vsnoprintf(out, 0x100000, format, argumentList); }

char* snoprintf(char* out, int outSize, const char* format, ...)
{
    va_list argumentList;
    va_start(argumentList, format);
    char* result = vsnoprintf(out, outSize, format, argumentList);
    va_end(argumentList);
    return result;
}

char* soprintf(char* out, const char* format, ...)
{
    va_list argumentList;
    va_start(argumentList, format);
    char* result = vsnoprintf(out, 0x100000, format, argumentList);
    va_end(argumentList);
    return result;
}
