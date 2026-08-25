#pragma once

#include <cstddef>

struct re_guts;

struct regex_t {
    int re_magic = 0;
    unsigned re_nsub = 0;
    const char* re_endp = nullptr;
    re_guts* re_g = nullptr;
};

struct regmatch_t {
    std::ptrdiff_t rm_so;
    std::ptrdiff_t rm_eo;
};

enum {
    REG_BASIC = 0x0000,
    REG_EXTENDED = 0x0001,
    REG_ICASE = 0x0002,
    REG_NOSUB = 0x0004,
    REG_NEWLINE = 0x0008,
    REG_NOSPEC = 0x0010,
    REG_PEND = 0x0020,
    REG_EXPECT = 0x0040,
    REG_QUOTE = 0x0080,
    REG_NOTBOL = 0x0001,
    REG_NOTEOL = 0x0002,
    REG_STARTEND = 0x0004,
};

enum {
    REG_NOMATCH = 1,
    REG_BADPAT = 2,
    REG_ECOLLATE = 3,
    REG_ECTYPE = 4,
    REG_EESCAPE = 5,
    REG_ESUBREG = 6,
    REG_EBRACK = 7,
    REG_EPAREN = 8,
    REG_EBRACE = 9,
    REG_BADBR = 10,
    REG_ERANGE = 11,
    REG_ESPACE = 12,
    REG_BADRPT = 13,
};

int regcomp(regex_t* expression, const char* pattern, int cflags);
int regexec(const regex_t* expression, const char* string, std::size_t nmatch,
    regmatch_t* pmatch, int eflags);
void regfree(regex_t* expression);

