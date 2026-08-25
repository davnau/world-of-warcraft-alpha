#include "Storm/h/SComp.h"
#include "Storm/h/SZip.h"

#include <algorithm>
#include <array>
#include <cstring>
#include <unordered_map>
#include <vector>

namespace {

unsigned int ReverseBits(unsigned int value, unsigned int count) {
    unsigned int result = 0;
    while (count--) { result = (result << 1) | (value & 1U); value >>= 1; }
    return result;
}

class BitWriter {
public:
    void Write(unsigned int value, unsigned int count) {
        for (unsigned int bit = 0; bit < count; ++bit) {
            m_rack |= ((value >> bit) & 1U) << m_bits;
            if (++m_bits == 8) { m_data.push_back(static_cast<unsigned char>(m_rack)); m_rack = 0; m_bits = 0; }
        }
    }
    std::vector<unsigned char> Finish() {
        if (m_bits) m_data.push_back(static_cast<unsigned char>(m_rack));
        return std::move(m_data);
    }
private:
    std::vector<unsigned char> m_data;
    unsigned int m_rack = 0;
    unsigned int m_bits = 0;
};

struct FixedCodes {
    std::array<unsigned int, 288> code{};
    std::array<unsigned int, 288> length{};
    FixedCodes() {
        std::fill(length.begin(), length.end(), 8U);
        std::fill(length.begin() + 144, length.begin() + 256, 9U);
        std::fill(length.begin() + 256, length.begin() + 280, 7U);
        std::fill(length.begin() + 280, length.end(), 8U);
        std::array<unsigned int, 16> count{}, next{};
        for (unsigned int bits : length) ++count[bits];
        unsigned int value = 0;
        for (unsigned int bits = 1; bits <= 15; ++bits) { value = (value + count[bits - 1]) << 1; next[bits] = value; }
        for (unsigned int symbol = 0; symbol < code.size(); ++symbol)
            code[symbol] = ReverseBits(next[length[symbol]]++, length[symbol]);
    }
};

void EmitFixed(BitWriter& bits, unsigned int symbol) {
    static const FixedCodes codes;
    bits.Write(codes.code[symbol], codes.length[symbol]);
}

std::vector<unsigned char> DeflateFixed(const unsigned char* source, unsigned int size) {
    static constexpr unsigned int lengthBase[] = {
        3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,
        131,163,195,227,258
    };
    static constexpr unsigned int lengthExtra[] = {
        0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
    };
    static constexpr unsigned int distanceBase[] = {
        1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,
        1537,2049,3073,4097,6145,8193,12289,16385,24577
    };
    static constexpr unsigned int distanceExtra[] = {
        0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
    };
    BitWriter bits;
    bits.Write(1U, 1); // final block
    bits.Write(1U, 2); // fixed Huffman
    std::unordered_map<unsigned int, unsigned int> latest;
    auto keyAt = [&](unsigned int position) {
        return static_cast<unsigned int>(source[position]) |
            (static_cast<unsigned int>(source[position + 1]) << 8) |
            (static_cast<unsigned int>(source[position + 2]) << 16);
    };
    unsigned int position = 0;
    while (position < size) {
        unsigned int matchLength = 0, matchDistance = 0;
        if (position + 2 < size) {
            const unsigned int key = keyAt(position);
            const auto found = latest.find(key);
            if (found != latest.end() && position - found->second <= 32768U) {
                const unsigned int limit = std::min(258U, size - position);
                while (matchLength < limit && source[found->second + matchLength] == source[position + matchLength]) ++matchLength;
                if (matchLength >= 3) matchDistance = position - found->second;
                else matchLength = 0;
            }
        }
        if (matchLength < 3) {
            EmitFixed(bits, source[position]);
            if (position + 2 < size) latest[keyAt(position)] = position;
            ++position;
            continue;
        }
        unsigned int lengthIndex = 0;
        while (lengthIndex + 1 < std::size(lengthBase) && lengthBase[lengthIndex + 1] <= matchLength) ++lengthIndex;
        EmitFixed(bits, 257U + lengthIndex);
        bits.Write(matchLength - lengthBase[lengthIndex], lengthExtra[lengthIndex]);
        unsigned int distanceIndex = 0;
        while (distanceIndex + 1 < std::size(distanceBase) && distanceBase[distanceIndex + 1] <= matchDistance) ++distanceIndex;
        bits.Write(ReverseBits(distanceIndex, 5), 5);
        bits.Write(matchDistance - distanceBase[distanceIndex], distanceExtra[distanceIndex]);
        for (unsigned int index = 0; index < matchLength; ++index)
            if (position + index + 2 < size) latest[keyAt(position + index)] = position + index;
        position += matchLength;
    }
    EmitFixed(bits, 256U);
    return bits.Finish();
}

unsigned int Adler32(const unsigned char* data, unsigned int bytes) {
    constexpr unsigned int modulus = 65521U;
    unsigned int a = 1, b = 0;
    for (unsigned int index = 0; index < bytes; ++index) { a = (a + data[index]) % modulus; b = (b + a) % modulus; }
    return (b << 16) | a;
}

std::vector<unsigned char> ZlibCompress(const unsigned char* source, unsigned int sourceBytes) {
    std::vector<unsigned char> raw = DeflateFixed(source, sourceBytes);
    std::vector<unsigned char> result;
    result.reserve(raw.size() + 6);
    result.push_back(0x78U);
    result.push_back(0x01U);
    result.insert(result.end(), raw.begin(), raw.end());
    const unsigned int checksum = Adler32(source, sourceBytes);
    result.push_back(static_cast<unsigned char>(checksum >> 24));
    result.push_back(static_cast<unsigned char>(checksum >> 16));
    result.push_back(static_cast<unsigned char>(checksum >> 8));
    result.push_back(static_cast<unsigned char>(checksum));
    return result;
}

} // namespace

int __stdcall SCompCompress(void* destination, unsigned int* destinationBytes, const void* source,
    unsigned int sourceBytes, unsigned int compressionTypes, unsigned int, unsigned int) {
    if (!destination || !destinationBytes || (!source && sourceBytes) || *destinationBytes < sourceBytes) return 0;
    if ((compressionTypes & ~(SCOMP_HUFFMAN | SCOMP_ZLIB | SCOMP_PKWARE | SCOMP_BZIP2 |
        SCOMP_SPARSE | SCOMP_ADPCM_MONO | SCOMP_ADPCM_STEREO)) != 0) return 0;
    if (compressionTypes & SCOMP_ZLIB) {
        const auto compressed = ZlibCompress(static_cast<const unsigned char*>(source), sourceBytes);
        if (compressed.size() + 1 < sourceBytes && compressed.size() + 1 <= *destinationBytes) {
            auto* output = static_cast<unsigned char*>(destination);
            output[0] = SCOMP_ZLIB;
            std::memcpy(output + 1, compressed.data(), compressed.size());
            *destinationBytes = static_cast<unsigned int>(compressed.size() + 1);
            return 1;
        }
    }
    if (destination != source && sourceBytes) std::memmove(destination, source, sourceBytes);
    *destinationBytes = sourceBytes;
    return 1;
}

int __stdcall SCompDecompress(void* destination, unsigned int* destinationBytes, const void* source,
    unsigned int sourceBytes) {
    return SCompDecompress2(destination, destinationBytes, source, sourceBytes, nullptr);
}

int __stdcall SCompDecompress2(void* destination, unsigned int* destinationBytes, const void* source,
    unsigned int sourceBytes, const char*) {
    if (!destination || !destinationBytes || !source) return 0;
    if (*destinationBytes == sourceBytes) {
        if (destination != source) std::memmove(destination, source, sourceBytes);
        return 1;
    }
    if (sourceBytes < 7) return 0;
    const auto* input = static_cast<const unsigned char*>(source);
    if (input[0] != SCOMP_ZLIB) return 0;
    const unsigned char* zlib = input + 1;
    const unsigned int zlibBytes = sourceBytes - 1;
    if ((zlib[0] & 0x0FU) != 8U || ((zlib[0] << 8) + zlib[1]) % 31U != 0 || (zlib[1] & 0x20U)) return 0;
    unsigned int actual = *destinationBytes;
    if (!StormInflateRaw(zlib + 2, zlibBytes - 6, destination, &actual)) return 0;
    const unsigned int storedChecksum = (static_cast<unsigned int>(zlib[zlibBytes - 4]) << 24) |
        (static_cast<unsigned int>(zlib[zlibBytes - 3]) << 16) |
        (static_cast<unsigned int>(zlib[zlibBytes - 2]) << 8) | zlib[zlibBytes - 1];
    if (Adler32(static_cast<const unsigned char*>(destination), actual) != storedChecksum) return 0;
    *destinationBytes = actual;
    return 1;
}

int __stdcall SCompDestroy() { return 1; }
