#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>

#include <cstddef>
#include <cstdint>

// Opaque handle tags recovered from the Storm PDB.  Keeping the tags distinct
// prevents accidental interchange while preserving the original pointer ABI.
struct HSHEAP__;
struct HLOCKEDHEAP__;
struct HSLOG__;
struct HLOCKEDLOG__;
struct HSRGN__;
struct HLOCKEDRGN__;

struct SArchive;
class SEvent;
struct ZipFileFCB;
struct z_stream_s;

enum SFILE_TYPE : std::int32_t {
    SFILE_PLAIN = 0,
    SFILE_COMPRESSED = 1,
    SFILE_PAQ = 2,
    SFILE_OLD_SFILE = 3,
    SFILE_ZIP_FILE = 4,
};

enum SARCHIVE_TYPE : std::int32_t { SARCHIVE_MPQ = 0, SARCHIVE_ZIP = 1 };

struct SArchive {
    SARCHIVE_TYPE m_type;
    void* m_archive;
};

static_assert(sizeof(SArchive) == 8, "SArchive layout must remain PDB-compatible on x86.");

struct MD5 {
    std::uint32_t val[4];
};

static_assert(sizeof(MD5) == 16, "MD5 layout must remain PDB-compatible.");

struct SCritSect {
    std::uint8_t m_opaqueData[24];
    SCritSect();
    ~SCritSect();
    void Enter();
    void Leave();
    int TryEnter();
};

static_assert(sizeof(SCritSect) == 24, "SCritSect layout must remain PDB-compatible.");
