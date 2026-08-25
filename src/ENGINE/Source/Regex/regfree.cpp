#include "ENGINE/Source/Regex/regex_internal.h"

void regfree(regex_t* expression)
{
    if (!expression)
        return;
    delete expression->re_g;
    expression->re_magic = 0;
    expression->re_nsub = 0;
    expression->re_endp = nullptr;
    expression->re_g = nullptr;
}

