// Reconstructed from D:\build\buildWoW\Storm\Source\SRtl.cpp
// (StormRtlDestroy RVA 0x234880; StormRtlInitialize RVA 0x2348A0).

#include "Storm/h/storm.h"

void StormRtlDestroy()
{
    // Order is evidence-backed: detach error reporting before destroying memory,
    // then terminate error and logging state.
    SErrSetLogCallback(nullptr);
    SMemDestroy();
    SErrDestroy();
    SLogDestroy();
}

void StormRtlInitialize()
{
    SLogInitialize();
    SMemInitialize();
}
