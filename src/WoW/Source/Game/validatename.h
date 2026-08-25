#pragma once

#include <cstdint>

enum WOW_LOCALE : int {
    LOCALE_en_US = 0,
    LOCALE_ko_KR = 1,
    LOCALE_ja_JP = 2,
    LOCALE_zh_CN = 3,
    LOCALE_zh_TW = 4,
    LOCALE_es_ES = 5,
    LOCALE_fr_FR = 6,
    LOCALE_de_DE = 7,
    NUM_LOCALES = 8,
    DEFAULT_LANGUAGE = 0,
};

enum CHARSET : int {
    CHARSET_UNKNOWN = 0,
    CHARSET_LATIN1 = 1,
    CHARSET_KOREAN = 2,
};

enum VALIDATE_NAME_RESULT : int {
    NAME_NO_NAME = 0,
    NAME_TOO_SHORT = 1,
    NAME_TOO_LONG = 2,
    NAME_STARTS_WITH_GRAVE = 3,
    NAME_TWO_GRAVES = 4,
    NAME_INVALID_CHARACTER = 5,
    NAME_MIXED_LANGUAGES = 6,
    NAME_PROFANE = 7,
    NAME_RESERVED = 8,
    NAME_FAILURE = 9,
    NAME_SUCCESS = 10,
    NUM_NAME_RESULTS = 11,
    NAME_LEADING_GRAVE = NAME_STARTS_WITH_GRAVE,
    NAME_MULTIPLE_GRAVES = NAME_TWO_GRAVES,
    NAME_MIXED_CHARSET = NAME_MIXED_LANGUAGES,
    NAME_INVALID_UTF8 = NAME_FAILURE,
};

void ValidateNameInitialize();
void ValidateNameDestroy();
VALIDATE_NAME_RESULT ValidateCharacterName(WOW_LOCALE locale, const char* name);
VALIDATE_NAME_RESULT ValidatePetName(WOW_LOCALE locale, const char* name);
VALIDATE_NAME_RESULT ValidateGuildName(WOW_LOCALE locale, const char* name);

// Compatibility overloads used by the decompiler-shaped call sites.
VALIDATE_NAME_RESULT ValidateCharacterName(char* name, const char* context);
VALIDATE_NAME_RESULT ValidatePetName(char* name, const char* context);
VALIDATE_NAME_RESULT ValidateGuildName(char* name, const char* context);

int ValidateName(const char* name,
                 const std::uint16_t* validChars,
                 unsigned int* nameLength,
                 CHARSET* charset);
int ValidateName(WOW_LOCALE locale, const char* name,
                 const std::uint16_t* validChars,
                 unsigned int* nameLength, CHARSET* charset);

int GetCharSet(std::uint16_t ch);
unsigned char IsAlpha(WOW_LOCALE locale, int ch);
int IsLatin1(std::uint16_t ch);
bool IsAlphaLatin1(std::uint16_t ch);
int IsKorean(std::uint16_t ch);
unsigned char IsAlphaKorean(std::uint16_t ch);

int _E151_2();
int _E148_3();
int _E150_3();
void _E149_3();
