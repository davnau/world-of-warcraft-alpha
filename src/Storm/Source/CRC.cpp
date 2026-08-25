#include "Storm/h/storm.h"

#include <array>
#include <cstddef>

namespace {

constexpr unsigned long CrcTableEntry(unsigned long value) {
    for (unsigned long bit = 0; bit < 8; ++bit) {
        value = (value & 1U) != 0 ? (value >> 1U) ^ 0xEDB88320U : value >> 1U;
    }
    return value;
}

constexpr std::array<unsigned long, 256> MakeCrcTable() {
    std::array<unsigned long, 256> table{};
    for (unsigned long index = 0; index < table.size(); ++index) {
        table[index] = CrcTableEntry(index);
    }
    return table;
}

constexpr auto crctable = MakeCrcTable();

} // namespace

// PDB source: D:\build\buildWoW\Storm\Source\CRC.CPP
// RVA: 0x0025AC80
// Original decorated signature: ?CrcBuffer@@YIKPBXKPAKK@Z
unsigned long __fastcall CrcBuffer(const void* buffer, unsigned long len, unsigned long* pcrc, unsigned long stage) {
    unsigned long result = pcrc != nullptr ? *pcrc : 0;
    if ((stage & 1U) != 0) {
        result = ~0UL;
    }

    if ((stage & 2U) != 0 && len != 0) {
        const auto* bytes = static_cast<const unsigned char*>(buffer);
        do {
            result = crctable[(static_cast<unsigned char>(*bytes++) ^ static_cast<unsigned char>(result))] ^ (result >> 8U);
        } while (--len != 0);
    }

    if ((stage & 4U) != 0) {
        result = ~result;
    }
    if (pcrc != nullptr) {
        *pcrc = result;
    }
    return result;
}

// PDB source: D:\build\buildWoW\Storm\Source\CRC.CPP
// RVA: 0x0025ACF0
unsigned long __stdcall SCrcBuffer(const void* buffer, unsigned long len, unsigned long* pcrc, unsigned long stage) {
    return CrcBuffer(buffer, len, pcrc, stage);
}
