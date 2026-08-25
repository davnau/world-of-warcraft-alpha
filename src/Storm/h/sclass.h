#pragma once
#include "Storm/h/storm.h"

struct RECTF { float left; float bottom; float right; float top; };
struct RGN_SOURCE { RECTF rect; void* param; int sequence; unsigned int flags; };
struct RGN_FOUNDPARAM { void* param; int sequence; };

struct RGN {
    std::uint8_t m_hashObject[24];
    TSGrowableArray<RGN_SOURCE> source;
    TSGrowableArray<RECTF> combined;
    TSGrowableArray<RGN_FOUNDPARAM> foundparams;
    RECTF foundparamsrect;
    int sequence;
    int dirty;
};

#if defined(_M_IX86)
static_assert(sizeof(RECTF) == 16, "RECTF PDB layout mismatch.");
static_assert(sizeof(RGN_SOURCE) == 28, "RGN_SOURCE PDB layout mismatch.");
static_assert(sizeof(RGN) == 108, "RGN PDB layout mismatch.");
#endif
