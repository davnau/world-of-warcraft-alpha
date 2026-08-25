#include "Storm/h/storm.h"

#include <cstring>

namespace {

constexpr std::uint32_t RotateLeft(std::uint32_t value, unsigned int count) {
    return (value << count) | (value >> (32U - count));
}

constexpr std::uint32_t LoadBigEndian(const std::uint8_t* value) {
    return (static_cast<std::uint32_t>(value[0]) << 24U) |
           (static_cast<std::uint32_t>(value[1]) << 16U) |
           (static_cast<std::uint32_t>(value[2]) << 8U) |
           static_cast<std::uint32_t>(value[3]);
}

void SaveBigEndian(std::uint32_t value, std::uint8_t* destination) {
    destination[0] = static_cast<std::uint8_t>(value >> 24U);
    destination[1] = static_cast<std::uint8_t>(value >> 16U);
    destination[2] = static_cast<std::uint8_t>(value >> 8U);
    destination[3] = static_cast<std::uint8_t>(value);
}

} // namespace

// PDB source: D:\build\buildWoW\Storm\Source\Sha1.cpp
// RVA: 0x0025DC40
void __fastcall Sha1::Pump(std::uint32_t* hash, const std::uint8_t* data) {
    std::uint32_t words[80];
    for (unsigned int index = 0; index < 16; ++index) {
        words[index] = LoadBigEndian(data + index * 4U);
    }
    for (unsigned int index = 16; index < 80; ++index) {
        words[index] = RotateLeft(words[index - 3] ^ words[index - 8] ^ words[index - 14] ^ words[index - 16], 1U);
    }

    std::uint32_t a = hash[0];
    std::uint32_t b = hash[1];
    std::uint32_t c = hash[2];
    std::uint32_t d = hash[3];
    std::uint32_t e = hash[4];

    for (unsigned int index = 0; index < 80; ++index) {
        std::uint32_t function;
        std::uint32_t constant;
        if (index < 20) {
            function = (b & c) | (~b & d);
            constant = 0x5A827999U;
        } else if (index < 40) {
            function = b ^ c ^ d;
            constant = 0x6ED9EBA1U;
        } else if (index < 60) {
            function = (b & c) | (b & d) | (c & d);
            constant = 0x8F1BBCDCU;
        } else {
            function = b ^ c ^ d;
            constant = 0xCA62C1D6U;
        }
        const std::uint32_t next = RotateLeft(a, 5U) + function + e + constant + words[index];
        e = d;
        d = c;
        c = RotateLeft(b, 30U);
        b = a;
        a = next;
    }

    hash[0] += a;
    hash[1] += b;
    hash[2] += c;
    hash[3] += d;
    hash[4] += e;
}

// PDB source: D:\build\buildWoW\Storm\Source\Sha1.cpp
// RVA: 0x0025E110
void Sha1::Initialize() {
    m_size = 0;
    m_hash[0] = 0x67452301U;
    m_hash[1] = 0xEFCDAB89U;
    m_hash[2] = 0x98BADCFEU;
    m_hash[3] = 0x10325476U;
    m_hash[4] = 0xC3D2E1F0U;
}

// PDB source: D:\build\buildWoW\Storm\Source\Sha1.cpp
// RVA: 0x0025E150
void Sha1::Append(const void* data, unsigned long size) {
    const auto* bytes = static_cast<const std::uint8_t*>(data);
    unsigned long used = static_cast<unsigned long>((m_size >> 3U) & DATA_MASK);
    m_size += static_cast<std::uint64_t>(size) * 8U;

    if (used != 0U) {
        const unsigned long available = DATA_SIZE - used;
        if (size < available) {
            std::memcpy(m_data + used, bytes, size);
            return;
        }
        std::memcpy(m_data + used, bytes, available);
        Pump(m_hash, m_data);
        bytes += available;
        size -= available;
    }

    while (size >= DATA_SIZE) {
        Pump(m_hash, bytes);
        bytes += DATA_SIZE;
        size -= DATA_SIZE;
    }
    if (size != 0U) {
        std::memcpy(m_data, bytes, size);
    }
}

// PDB source: D:\build\buildWoW\Storm\Source\Sha1.cpp
// RVA: 0x0025E200
void Sha1::Finalize(std::uint8_t* hash) {
    const std::uint64_t originalSize = m_size;
    const std::uint8_t padding[] = {0x80U};
    const std::uint8_t zeroes[DATA_SIZE]{};
    const unsigned long used = static_cast<unsigned long>((originalSize >> 3U) & DATA_MASK);
    Append(padding, 1U);
    if (used >= 56U) {
        Append(zeroes, DATA_SIZE - used - 1U);
        Append(zeroes, 56U);
    } else {
        Append(zeroes, 56U - used - 1U);
    }

    std::uint8_t size[8];
    for (unsigned int index = 0; index < 8; ++index) {
        size[7U - index] = static_cast<std::uint8_t>(originalSize >> (index * 8U));
    }
    Append(size, 8U);
    for (unsigned int index = 0; index < 5; ++index) {
        SaveBigEndian(m_hash[index], hash + index * 4U);
    }
}
