// Reconstructed from D:\build\buildWoW\Storm\Source\SOption.cpp
// (RVA 0x23BCF0-0x23C070; compiler initialization thunks intentionally omitted).

#include "Storm/h/storm.h"

namespace {
unsigned int s_alloccount = 0;
unsigned int s_freecount = 0;

bool IsPowerOfTwo(unsigned int value)
{
    return value != 0 && (value & (value - 1)) == 0;
}

bool ReadDword(unsigned int* value, unsigned int* valueLength, unsigned int source)
{
    if (*valueLength < sizeof(source))
        return false;
    *value = source;
    *valueLength = sizeof(source);
    return true;
}

} // namespace

StormOptions g_opt{0, 0, 0, 0, 0, 0, 1, 0};

void IncrementAllocCount() { ++s_alloccount; }
void IncrementFreeCount() { ++s_freecount; }

int __stdcall StormGetOption(int optname, unsigned int optval, unsigned int* optlen)
{
    if (!optval || !optlen) {
        SErrSetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    auto* value = reinterpret_cast<unsigned int*>(optval);
    bool result = false;
    switch (optname) {
    case 1: result = ReadDword(value, optlen, g_opt.serrleaksilentwarning); break;
    case 2: result = ReadDword(value, optlen, g_opt.wavechunksize); break;
    case 3: result = ReadDword(value, optlen, g_opt.smemleaksilentwarning); break;
    case 4: result = ReadDword(value, optlen, g_opt.alignstreamingwavedata); break;
    case 5: result = ReadDword(value, optlen, g_opt.echotooutputdebugstring); break;
    case 6:
        if (*optlen >= 8) { value[0] = s_alloccount; value[1] = s_freecount; *optlen = 8; result = true; }
        break;
    case 7: result = ReadDword(value, optlen, g_opt.serrsuppresslogs); break;
    case 9: result = ReadDword(value, optlen, g_opt.crcenabled); break;
    case 10: result = ReadDword(value, optlen, g_opt.orderedprintfenabled); break;
    default: break;
    }
    SErrSetLastError(result ? ERROR_SUCCESS : ERROR_INVALID_PARAMETER);
    return result ? 1 : 0;
}

int __stdcall StormSetOption(int optname, int* optval, unsigned int optlen)
{
    if (!optval) { SErrSetLastError(ERROR_INVALID_PARAMETER); return 0; }
    const unsigned int value = static_cast<unsigned int>(*optval);
    bool result = optlen == sizeof(value);
    if (result) {
        switch (optname) {
        case 1: g_opt.serrleaksilentwarning = value; break;
        case 2: if (!IsPowerOfTwo(value)) result = false; else g_opt.wavechunksize = value; break;
        case 3: g_opt.smemleaksilentwarning = value; break;
        case 4: g_opt.alignstreamingwavedata = value; break;
        case 5: g_opt.echotooutputdebugstring = value; break;
        case 7: g_opt.serrsuppresslogs = value; break;
        case 9: g_opt.crcenabled = value != 0; break;
        case 10: g_opt.orderedprintfenabled = value != 0; break;
        default: result = false; break;
        }
    }
    if (optname == 6) {
        result = optlen == 8;
        if (result) { s_alloccount = static_cast<unsigned int>(optval[0]); s_freecount = static_cast<unsigned int>(optval[1]); }
    }
    SErrSetLastError(result ? ERROR_SUCCESS : ERROR_INVALID_PARAMETER);
    return result ? 1 : 0;
}
