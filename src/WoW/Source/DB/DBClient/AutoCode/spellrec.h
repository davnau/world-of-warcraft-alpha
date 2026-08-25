#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellRec();
    ~SpellRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_168();
int _E1_168();
int _E5_167();
int _E4_167();
int _E8_167();
int _E7_167();
