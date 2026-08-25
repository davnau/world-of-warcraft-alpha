#pragma once

#include "ENGINE/Source/Regex/regex.h"

#include <regex>
#include <string>

struct re_guts {
    int magic = 0;
    int cflags = 0;
    std::regex expression;
    std::string pattern;
};

int RegexTranslateError(const std::regex_error& error);
std::regex_constants::syntax_option_type RegexSyntaxOptions(int cflags);

