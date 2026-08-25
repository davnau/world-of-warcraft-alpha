#include "Storm/h/storm.h"

#include <cstdint>
#include <limits>

// PDB source: D:\build\buildWoW\Storm\Source\SUnicode.cpp
// RVA: 0x00243790
int __stdcall SUniConvertUTF16to8Len(const std::uint16_t* src, unsigned long srcMaxChars, unsigned long* srcChars) {
    if (src == nullptr || srcMaxChars == 0U) {
        if (srcChars != nullptr) {
            *srcChars = 0;
        }
        return -1;
    }

    const unsigned long limit = srcMaxChars == 0x7FFFFFFFU
        ? std::numeric_limits<unsigned long>::max()
        : srcMaxChars;
    unsigned long consumed = 0;
    int result = 0;

    while (consumed < limit) {
        std::uint32_t codepoint = src[consumed++];
        if (codepoint >= 0xD800U && codepoint <= 0xDBFFU && consumed < limit) {
            const std::uint32_t low = src[consumed];
            if (low >= 0xDC00U && low <= 0xDFFFU) {
                codepoint = 0x10000U + ((codepoint - 0xD800U) << 10U) + (low - 0xDC00U);
                ++consumed;
            }
        }

        if (codepoint < 0x80U) {
            ++result;
            if (codepoint == 0U) {
                if (srcChars != nullptr) {
                    *srcChars = consumed;
                }
                return result;
            }
        } else if (codepoint < 0x800U) {
            result += 2;
        } else if (codepoint < 0x10000U) {
            result += 3;
        } else if (codepoint < 0x200000U) {
            result += 4;
        } else if (codepoint < 0x4000000U) {
            result += 5;
        } else if (codepoint <= 0x7FFFFFFFU) {
            result += 6;
        } else {
            break;
        }
    }

    if (srcChars != nullptr) {
        *srcChars = consumed;
    }
    return -1;
}

// PDB source: D:\build\buildWoW\Storm\Source\SUnicode.cpp
// RVA: 0x00243890
int __stdcall SUniConvertUTF16to8(
    char* dst,
    unsigned long dstMaxChars,
    const std::uint16_t* src,
    unsigned long srcMaxChars,
    unsigned long* dstChars,
    unsigned long* srcChars) {
    const unsigned long sourceLimit = srcMaxChars == 0x7FFFFFFFU
        ? std::numeric_limits<unsigned long>::max()
        : srcMaxChars;
    unsigned long sourceUsed = 0;
    unsigned long destinationUsed = 0;
    int result = -1;

    while (sourceUsed < sourceLimit) {
        std::uint32_t codepoint = src[sourceUsed];
        unsigned long sourceWidth = 1;
        if (codepoint >= 0xD800U && codepoint <= 0xDBFFU) {
            if (sourceUsed + 1U >= sourceLimit) {
                result = -1;
                break;
            }
            const std::uint32_t low = src[sourceUsed + 1U];
            if (low >= 0xDC00U && low <= 0xDFFFU) {
                codepoint = 0x10000U + ((codepoint - 0xD800U) << 10U) + (low - 0xDC00U);
                sourceWidth = 2;
            }
        }

        unsigned long destinationWidth;
        if (codepoint == 0U) {
            destinationWidth = 1;
        } else if (codepoint < 0x80U) {
            destinationWidth = 1;
        } else if (codepoint < 0x800U) {
            destinationWidth = 2;
        } else if (codepoint < 0x10000U) {
            destinationWidth = 3;
        } else if (codepoint < 0x200000U) {
            destinationWidth = 4;
        } else if (codepoint < 0x4000000U) {
            destinationWidth = 5;
        } else if (codepoint <= 0x7FFFFFFFU) {
            destinationWidth = 6;
        } else {
            codepoint = 0xFFFDU;
            destinationWidth = 3;
        }

        if (destinationUsed + destinationWidth > dstMaxChars) {
            result = static_cast<int>(destinationWidth);
            break;
        }

        if (codepoint == 0U) {
            dst[destinationUsed++] = '\0';
            sourceUsed += sourceWidth;
            result = 0;
            break;
        }

        for (unsigned long index = destinationWidth; index > 1U; --index) {
            dst[destinationUsed + index - 1U] = static_cast<char>(0x80U | (codepoint & 0x3FU));
            codepoint >>= 6U;
        }
        constexpr unsigned char firstByteMark[] = {0x00U, 0x00U, 0xC0U, 0xE0U, 0xF0U, 0xF8U, 0xFCU};
        dst[destinationUsed] = static_cast<char>(firstByteMark[destinationWidth] | codepoint);
        destinationUsed += destinationWidth;
        sourceUsed += sourceWidth;
    }

    if (srcChars != nullptr) {
        *srcChars = sourceUsed;
    }
    if (dstChars != nullptr) {
        *dstChars = destinationUsed;
    }
    return result;
}

// PDB source: D:\build\buildWoW\Storm\Source\SUnicode.cpp
// RVA: 0x00243A60
int __stdcall SUniConvertUTF8to16Len(const char* src, unsigned long srcMaxChars, unsigned long* srcChars) {
    if (src == nullptr || srcMaxChars == 0U) {
        if (srcChars != nullptr) {
            *srcChars = 0;
        }
        return -1;
    }

    const unsigned long limit = srcMaxChars == 0x7FFFFFFFU
        ? std::numeric_limits<unsigned long>::max()
        : srcMaxChars;
    unsigned long consumed = 0;
    int result = 0;

    while (consumed < limit) {
        const std::uint8_t first = static_cast<std::uint8_t>(src[consumed]);
        unsigned long width = 1;
        if (first >= 0xC0U && first <= 0xDFU) {
            width = 2;
        } else if (first >= 0xE0U && first <= 0xEFU) {
            width = 3;
        } else if (first >= 0xF0U && first <= 0xF7U) {
            width = 4;
        } else if (first >= 0xF8U && first <= 0xFBU) {
            width = 5;
        } else if (first >= 0xFCU) {
            width = 6;
        }

        if (width > limit - consumed) {
            result = -1 - static_cast<int>(width);
            break;
        }

        std::uint32_t codepoint = first;
        for (unsigned long index = 1; index < width; ++index) {
            codepoint = (codepoint << 6U) + static_cast<std::uint8_t>(src[consumed + index]);
        }
        constexpr std::uint32_t offsets[] = {0U, 0U, 0x3080U, 0xE2080U, 0x3C82080U, 0xFA082080U, 0x82082080U};
        codepoint -= offsets[width];
        consumed += width;

        if (codepoint > 0xFFFFU) {
            result += codepoint <= 0x10FFFFU ? 2 : 1;
        } else {
            ++result;
            if (codepoint == 0U) {
                break;
            }
        }
    }

    if (srcChars != nullptr) {
        *srcChars = consumed;
    }
    return result;
}
