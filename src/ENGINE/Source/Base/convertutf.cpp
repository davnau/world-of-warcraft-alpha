#include "ENGINE/Source/Base/convertutf.h"

#include <cstddef>

namespace {
constexpr unsigned kReplacement = 0xFFFD;
unsigned Decode(const std::uint8_t*& p, const std::uint8_t* end) {
    if (p == end) return 0;
    unsigned c = *p++;
    if (c < 0x80) return c;
    unsigned need = 0, value = 0, minimum = 0;
    if ((c & 0xE0) == 0xC0) { need = 1; value = c & 0x1F; minimum = 0x80; }
    else if ((c & 0xF0) == 0xE0) { need = 2; value = c & 0x0F; minimum = 0x800; }
    else if ((c & 0xF8) == 0xF0) { need = 3; value = c & 7; minimum = 0x10000; }
    else return kReplacement;
    if (static_cast<std::size_t>(end - p) < need) { p = end; return kReplacement; }
    for (unsigned i = 0; i < need; ++i) {
        if ((*p & 0xC0) != 0x80) return kReplacement;
        value = (value << 6) | (*p++ & 0x3F);
    }
    return value < minimum || value > 0x10FFFF || (value >= 0xD800 && value <= 0xDFFF) ? kReplacement : value;
}
unsigned Utf8Bytes(unsigned cp) { return cp < 0x80 ? 1 : cp < 0x800 ? 2 : cp < 0x10000 ? 3 : 4; }
}

char* sputu8(unsigned cp, char* out) {
    if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) cp = kReplacement;
    if (cp < 0x80) *out++ = static_cast<char>(cp);
    else if (cp < 0x800) { *out++ = static_cast<char>(0xC0 | (cp >> 6)); *out++ = static_cast<char>(0x80 | (cp & 63)); }
    else if (cp < 0x10000) { *out++ = static_cast<char>(0xE0 | (cp >> 12)); *out++ = static_cast<char>(0x80 | ((cp >> 6) & 63)); *out++ = static_cast<char>(0x80 | (cp & 63)); }
    else { *out++ = static_cast<char>(0xF0 | (cp >> 18)); *out++ = static_cast<char>(0x80 | ((cp >> 12) & 63)); *out++ = static_cast<char>(0x80 | ((cp >> 6) & 63)); *out++ = static_cast<char>(0x80 | (cp & 63)); }
    return out;
}

unsigned sgetu8(const char** strptr, int* chars) {
    if (!strptr || !*strptr || !chars || *chars <= 0) return 0;
    const auto* p = reinterpret_cast<const std::uint8_t*>(*strptr);
    const auto* end = p + *chars;
    unsigned cp = Decode(p, end);
    int used = static_cast<int>(p - reinterpret_cast<const std::uint8_t*>(*strptr));
    *strptr += used; *chars -= used;
    return cp;
}

int ConvertUTF16toUTF8Length(const std::uint16_t* src, unsigned max, unsigned* consumed) {
    if (!src) return 0;
    unsigned i = 0, bytes = 0;
    while (i < max && src[i]) {
        unsigned cp = src[i++];
        if (cp >= 0xD800 && cp <= 0xDBFF && i < max && src[i] >= 0xDC00 && src[i] <= 0xDFFF)
            cp = 0x10000 + ((cp - 0xD800) << 10) + (src[i++] - 0xDC00);
        else if (cp >= 0xD800 && cp <= 0xDFFF) cp = kReplacement;
        bytes += Utf8Bytes(cp);
    }
    if (consumed) *consumed = i;
    return static_cast<int>(bytes);
}

int ConvertUTF16toUTF8(char* dst, unsigned cap, const std::uint16_t* src, unsigned max, unsigned* written, unsigned* consumed) {
    if (!dst || !cap || !src) return 0;
    unsigned i = 0, n = 0;
    while (i < max && src[i]) {
        unsigned cp = src[i++];
        if (cp >= 0xD800 && cp <= 0xDBFF && i < max && src[i] >= 0xDC00 && src[i] <= 0xDFFF) cp = 0x10000 + ((cp - 0xD800) << 10) + (src[i++] - 0xDC00);
        else if (cp >= 0xD800 && cp <= 0xDFFF) cp = kReplacement;
        unsigned need = Utf8Bytes(cp); if (n + need >= cap) { --i; break; }
        n = static_cast<unsigned>(sputu8(cp, dst + n) - dst);
    }
    dst[n] = 0; if (written) *written = n; if (consumed) *consumed = i; return 1;
}

int ConvertUTF8toUTF16Length(const std::uint8_t* src, unsigned max, unsigned* consumed) {
    if (!src) return 0; const auto* p = src; const auto* end = src + max; unsigned n = 0;
    while (p < end && *p) { unsigned cp = Decode(p, end); n += cp >= 0x10000 ? 2 : 1; }
    if (consumed) *consumed = static_cast<unsigned>(p - src); return static_cast<int>(n);
}

int ConvertUTF8toUTF16(std::uint16_t* dst, unsigned cap, const std::uint8_t* src, unsigned max, unsigned* written, unsigned* consumed) {
    if (!dst || !cap || !src) return 0; const auto* p = src; const auto* end = src + max; unsigned n = 0;
    while (p < end && *p) { const auto* before = p; unsigned cp = Decode(p, end); unsigned need = cp >= 0x10000 ? 2 : 1; if (n + need >= cap) { p = before; break; }
        if (need == 2) { cp -= 0x10000; dst[n++] = static_cast<std::uint16_t>(0xD800 + (cp >> 10)); dst[n++] = static_cast<std::uint16_t>(0xDC00 + (cp & 1023)); }
        else dst[n++] = static_cast<std::uint16_t>(cp);
    }
    dst[n] = 0; if (written) *written = n; if (consumed) *consumed = static_cast<unsigned>(p - src); return 1;
}
