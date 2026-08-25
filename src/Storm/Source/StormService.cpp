// Reconstructed from D:\build\buildWoW\Storm\Source\StormService.cpp
// (RVA 0x25ABC0-0x25AC50; compiler initialization thunks omitted).

#include "Storm/h/storm.h"

#include <cstdarg>
#include <cstdio>

int ISMemGetAllocator(void** destination)
{
    if (!destination)
        return 0;
    *destination = nullptr;
    return 1;
}

int ISMemGenerateReport(char* destination)
{
    return SMemDumpStateEx(destination);
}

int ISMemMarkAllHeaps(char* marker)
{
    return SMemMarkAllHeapsEx(marker);
}

int ISStrI64ToString(I64ToStringRequest* request)
{
    if (!request || !request->destination || request->destinationSize == 0)
        return 0;
    const int written = _snprintf_s(
        request->destination,
        request->destinationSize,
        _TRUNCATE,
        "%I64d",
        request->value);
    return written < 0 ? 0 : 1;
}

int StormCallService(int selector, ...)
{
    va_list arguments;
    va_start(arguments, selector);
    int result = 0;
    switch (selector) {
    case 1: result = ISMemGetAllocator(va_arg(arguments, void**)); break;
    case 2: result = ISMemGenerateReport(va_arg(arguments, char*)); break;
    case 3: result = ISStrI64ToString(va_arg(arguments, I64ToStringRequest*)); break;
    case 4: result = ISMemMarkAllHeaps(va_arg(arguments, char*)); break;
    default: break;
    }
    va_end(arguments);
    return result;
}
