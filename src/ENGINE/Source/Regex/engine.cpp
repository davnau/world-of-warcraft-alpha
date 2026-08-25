#include "ENGINE/Source/Regex/regex_internal.h"

int RegexTranslateError(const std::regex_error& error)
{
    using code = std::regex_constants::error_type;
    switch (error.code()) {
    case code::error_collate: return REG_ECOLLATE;
    case code::error_ctype: return REG_ECTYPE;
    case code::error_escape: return REG_EESCAPE;
    case code::error_backref: return REG_ESUBREG;
    case code::error_brack: return REG_EBRACK;
    case code::error_paren: return REG_EPAREN;
    case code::error_brace: return REG_EBRACE;
    case code::error_badbrace: return REG_BADBR;
    case code::error_range: return REG_ERANGE;
    case code::error_space: return REG_ESPACE;
    case code::error_badrepeat: return REG_BADRPT;
    default: return REG_BADPAT;
    }
}

std::regex_constants::syntax_option_type RegexSyntaxOptions(int cflags)
{
    auto options = (cflags & REG_EXTENDED)
        ? std::regex_constants::extended
        : std::regex_constants::basic;
    if (cflags & REG_ICASE)
        options |= std::regex_constants::icase;
    return options;
}

