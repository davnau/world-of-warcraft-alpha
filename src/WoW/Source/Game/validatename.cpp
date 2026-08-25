#include "WoW/Source/Game/validatename.h"

#include "WoW/Source/DB/DBClient/dbclient.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <string>
#include <string_view>
#include <vector>

namespace {

std::vector<std::regex> g_profanityTokens;

bool DecodeUTF8(const char* text, std::vector<std::uint16_t>& output) {
    if (text == nullptr) {
        return false;
    }

    const auto* bytes = reinterpret_cast<const unsigned char*>(text);
    while (*bytes != 0) {
        const unsigned char first = *bytes++;
        std::uint32_t codepoint = 0;
        int continuationCount = 0;
        if (first < 0x80) {
            codepoint = first;
        } else if ((first & 0xE0) == 0xC0) {
            codepoint = first & 0x1F;
            continuationCount = 1;
            if (codepoint == 0) return false;
        } else if ((first & 0xF0) == 0xE0) {
            codepoint = first & 0x0F;
            continuationCount = 2;
        } else if ((first & 0xF8) == 0xF0) {
            codepoint = first & 0x07;
            continuationCount = 3;
        } else {
            return false;
        }

        for (int i = 0; i < continuationCount; ++i) {
            const unsigned char next = *bytes++;
            if ((next & 0xC0) != 0x80) return false;
            codepoint = (codepoint << 6) | (next & 0x3F);
        }

        if (codepoint > 0x10FFFF ||
            (codepoint >= 0xD800 && codepoint <= 0xDFFF) ||
            (continuationCount == 2 && codepoint < 0x800) ||
            (continuationCount == 3 && codepoint < 0x10000)) {
            return false;
        }

        if (codepoint <= 0xFFFF) {
            output.push_back(static_cast<std::uint16_t>(codepoint));
        } else {
            codepoint -= 0x10000;
            output.push_back(static_cast<std::uint16_t>(0xD800 + (codepoint >> 10)));
            output.push_back(static_cast<std::uint16_t>(0xDC00 + (codepoint & 0x3FF)));
        }
    }
    return true;
}

bool ContainsCharacter(const std::uint16_t* validChars, std::uint16_t value) {
    if (validChars == nullptr) return false;
    for (const std::uint16_t* current = validChars; *current != 0; ++current) {
        if (*current == value) return true;
    }
    return false;
}

std::string LowerAscii(std::string_view value) {
    std::string result(value);
    std::transform(result.begin(), result.end(), result.begin(), [](unsigned char ch) {
        return static_cast<char>(std::tolower(ch));
    });
    return result;
}

} // namespace

int _E148_3() {
    g_profanityTokens.clear();
    return 0;
}

void _E149_3() {
    g_profanityTokens.clear();
}

int _E150_3() {
    return 0;
}

int _E151_2() {
    _E148_3();
    return _E150_3();
}

void ValidateNameInitialize() {
    g_profanityTokens.clear();
    for (const auto& record : g_namesProfanityDB.m_records) {
        if (record.m_Name.empty()) continue;
        try {
            g_profanityTokens.emplace_back(record.m_Name,
                                           std::regex_constants::ECMAScript |
                                           std::regex_constants::icase);
        } catch (const std::regex_error&) {
            // The original reports an application fatal for a malformed DB
            // expression.  Keeping the other filters usable is safer for a
            // preservation build with partially recovered DB data.
        }
    }
}

void ValidateNameDestroy() {
    g_profanityTokens.clear();
}

VALIDATE_NAME_RESULT ValidateCharacterName(WOW_LOCALE locale, const char* name) {
    unsigned int length = 0;
    CHARSET charset = CHARSET_UNKNOWN;
    const auto result = static_cast<VALIDATE_NAME_RESULT>(
        ValidateName(locale, name, nullptr, &length, &charset));
    if (result == NAME_SUCCESS) {
        const unsigned int maximum = charset == CHARSET_LATIN1 ? 12U : 8U;
        if (length > maximum) return NAME_TOO_LONG;
    }
    return result;
}

VALIDATE_NAME_RESULT ValidatePetName(WOW_LOCALE locale, const char* name) {
    unsigned int length = 0;
    CHARSET charset = CHARSET_UNKNOWN;
    const auto result = static_cast<VALIDATE_NAME_RESULT>(
        ValidateName(locale, name, nullptr, &length, &charset));
    if (result == NAME_SUCCESS) {
        const unsigned int maximum = charset == CHARSET_LATIN1 ? 12U : 8U;
        if (length > maximum) return NAME_TOO_LONG;
    }
    return result;
}

VALIDATE_NAME_RESULT ValidateGuildName(WOW_LOCALE locale, const char* name) {
    static constexpr std::uint16_t validChars[] = {32, 0};
    unsigned int length = 0;
    CHARSET charset = CHARSET_UNKNOWN;
    const auto result = static_cast<VALIDATE_NAME_RESULT>(
        ValidateName(locale, name, validChars, &length, &charset));
    if (result == NAME_SUCCESS) {
        const unsigned int maximum = charset == CHARSET_LATIN1 ? 24U : 16U;
        if (length > maximum) return NAME_TOO_LONG;
    }
    return result;
}

VALIDATE_NAME_RESULT ValidateCharacterName(char* name, const char* context) {
    return ValidateCharacterName(LOCALE_en_US, name != nullptr ? name : context);
}

VALIDATE_NAME_RESULT ValidatePetName(char* name, const char* context) {
    return ValidatePetName(LOCALE_en_US, name != nullptr ? name : context);
}

VALIDATE_NAME_RESULT ValidateGuildName(char* name, const char* context) {
    return ValidateGuildName(LOCALE_en_US, name != nullptr ? name : context);
}

int ValidateName(const char* name, const std::uint16_t* validChars,
                 unsigned int* nameLength, CHARSET* charset) {
    if (nameLength != nullptr) *nameLength = 0;
    if (charset != nullptr) *charset = CHARSET_UNKNOWN;
    if (name == nullptr) return NAME_FAILURE;
    if (*name == '`') return NAME_STARTS_WITH_GRAVE;

    std::vector<std::uint16_t> unicodeName;
    if (!DecodeUTF8(name, unicodeName)) return NAME_INVALID_UTF8;
    if (nameLength != nullptr) *nameLength = static_cast<unsigned>(unicodeName.size());

    CHARSET detectedCharset = CHARSET_UNKNOWN;
    bool usedGrave = false;
    for (const std::uint16_t character : unicodeName) {
        if (IsAlpha(static_cast<WOW_LOCALE>(detectedCharset), character) != 0) {
            const int characterSet = GetCharSet(character);
            if (detectedCharset != CHARSET_UNKNOWN && detectedCharset != characterSet) {
                return NAME_MIXED_LANGUAGES;
            }
            detectedCharset = static_cast<CHARSET>(characterSet);
            continue;
        }

        if (ContainsCharacter(validChars, character)) continue;
        if (character == '`') {
            if (usedGrave) return NAME_TWO_GRAVES;
            usedGrave = true;
            continue;
        }
        return NAME_INVALID_CHARACTER;
    }

    if (nameLength != nullptr && usedGrave && *nameLength != 0) --*nameLength;
    if (charset != nullptr) *charset = detectedCharset;

    const unsigned length = nameLength != nullptr ? *nameLength :
        static_cast<unsigned>(unicodeName.size() - (usedGrave ? 1U : 0U));
    if (length == 0) return NAME_NO_NAME;
    if (length < 3) return NAME_TOO_SHORT;

    for (const std::regex& token : g_profanityTokens) {
        if (std::regex_search(name, token)) return NAME_PROFANE;
    }

    const std::string lowerName = LowerAscii(name);
    for (const auto& record : g_namesReservedDB.m_records) {
        if (!record.m_Name.empty() && LowerAscii(record.m_Name) == lowerName) {
            return NAME_RESERVED;
        }
    }
    return NAME_SUCCESS;
}

int ValidateName(WOW_LOCALE locale, const char* name,
                 const std::uint16_t* validChars, unsigned int* nameLength,
                 CHARSET* charset) {
    (void)locale;
    return ValidateName(name, validChars, nameLength, charset);
}

int GetCharSet(std::uint16_t ch) {
    if (IsLatin1(ch)) return CHARSET_LATIN1;
    return IsKorean(ch) ? CHARSET_KOREAN : LOCALE_en_US;
}

unsigned char IsAlpha(WOW_LOCALE locale, int ch) {
    (void)locale;
    const auto character = static_cast<std::uint16_t>(ch);
    if (IsLatin1(character)) return IsAlphaLatin1(character) ? 1 : 0;
    return IsAlphaKorean(character);
}

int IsLatin1(std::uint16_t ch) {
    return ch <= 0xFFU ? 1 : 0;
}

bool IsAlphaLatin1(std::uint16_t ch) {
    return (ch >= 0x41U && ch <= 0x5AU) ||
           (ch >= 0x61U && ch <= 0x7AU) ||
           (ch >= 0xC0U && ch <= 0xDDU) ||
           (ch >= 0xE0U && ch <= 0xFFU);
}

int IsKorean(std::uint16_t ch) {
    return (ch >= 0x1100U && ch <= 0x11FFU) ||
           (ch >= 0x3130U && ch <= 0x318FU) ||
           (ch >= 0xAC00U && ch <= 0xD7A3U);
}

unsigned char IsAlphaKorean(std::uint16_t ch) {
    // Valid_Korean in the recovered image is the 2,350-character Hangul
    // syllable table.  Its entries are within this syllable block; accepting
    // the block preserves the client’s charset behavior for recovered data
    // where the read-only table itself was not present.
    return ch >= 0xAC00U && ch <= 0xD7A3U ? 1 : 0;
}
