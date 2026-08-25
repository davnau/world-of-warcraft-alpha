#include "ENGINE/Source/Regex/regex_internal.h"

#include <cstring>

int regexec(const regex_t* expression, const char* string, std::size_t nmatch,
    regmatch_t* pmatch, int eflags)
{
    if (!expression || !string || expression->re_magic != 62053 || !expression->re_g ||
        expression->re_g->magic != 53829)
        return REG_BADPAT;

    const char* begin = string;
    const char* end = string + std::strlen(string);
    if ((eflags & REG_STARTEND) && pmatch && nmatch != 0) {
        if (pmatch[0].rm_so < 0 || pmatch[0].rm_eo < pmatch[0].rm_so)
            return REG_BADPAT;
        begin += pmatch[0].rm_so;
        end = string + pmatch[0].rm_eo;
    }

    const std::string input(begin, end);
    std::match_results<std::string::const_iterator> match;
    const auto flags = (eflags & REG_NOTBOL ? std::regex_constants::match_not_bow :
        std::regex_constants::match_default) |
        (eflags & REG_NOTEOL ? std::regex_constants::match_not_eow :
        std::regex_constants::match_default);
    if (!std::regex_search(input.cbegin(), input.cend(), match, expression->re_g->expression, flags))
        return REG_NOMATCH;

    if (!(expression->re_g->cflags & REG_NOSUB) && pmatch) {
        const std::size_t count = (std::min)(nmatch, match.size());
        for (std::size_t i = 0; i < count; ++i) {
            if (!match[i].matched) {
                pmatch[i].rm_so = -1;
                pmatch[i].rm_eo = -1;
            } else {
                pmatch[i].rm_so = static_cast<std::ptrdiff_t>(match.position(i) + (begin - string));
                pmatch[i].rm_eo = pmatch[i].rm_so + static_cast<std::ptrdiff_t>(match.length(i));
            }
        }
        for (std::size_t i = count; i < nmatch; ++i)
            pmatch[i] = {-1, -1};
    }
    return 0;
}
