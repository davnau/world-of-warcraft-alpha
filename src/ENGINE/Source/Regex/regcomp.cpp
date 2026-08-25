#include "ENGINE/Source/Regex/regex_internal.h"

#include <cstring>

int regcomp(regex_t* expression, const char* pattern, int cflags)
{
    if (!expression || !pattern)
        return REG_BADPAT;

    regfree(expression);
    try {
        auto* guts = new re_guts;
        guts->magic = 53829;
        guts->cflags = cflags;
        guts->pattern = pattern;
        guts->expression = std::regex(guts->pattern, RegexSyntaxOptions(cflags));
        expression->re_magic = 62053;
        expression->re_nsub = static_cast<unsigned>(guts->expression.mark_count());
        expression->re_endp = pattern + std::strlen(pattern);
        expression->re_g = guts;
        return 0;
    } catch (const std::regex_error& error) {
        expression->re_magic = 0;
        expression->re_nsub = 0;
        expression->re_endp = nullptr;
        expression->re_g = nullptr;
        return RegexTranslateError(error);
    } catch (const std::bad_alloc&) {
        expression->re_magic = 0;
        expression->re_nsub = 0;
        expression->re_endp = nullptr;
        expression->re_g = nullptr;
        return REG_ESPACE;
    }
}

