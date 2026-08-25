#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellRangeRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellRangeRec();
    ~SpellRangeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_170();
int _E1_170();
int _E5_169();
int _E4_169();
int _E8_169();
int _E7_169();
