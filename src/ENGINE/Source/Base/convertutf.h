#pragma once

#include <cstdint>

int ConvertUTF16toUTF8Length(const std::uint16_t* src, unsigned srcMaxChars, unsigned* srcChars = nullptr);
int ConvertUTF16toUTF8(char* dst, unsigned dstMaxChars, const std::uint16_t* src, unsigned srcMaxChars,
                       unsigned* dstChars = nullptr, unsigned* srcChars = nullptr);
int ConvertUTF8toUTF16Length(const std::uint8_t* src, unsigned srcMaxChars, unsigned* srcChars = nullptr);
int ConvertUTF8toUTF16(std::uint16_t* dst, unsigned dstMaxChars, const std::uint8_t* src, unsigned srcMaxChars,
                       unsigned* dstChars = nullptr, unsigned* srcChars = nullptr);
unsigned sgetu8(const char** strptr, int* chars);
char* sputu8(unsigned codepoint, char* strptr);
