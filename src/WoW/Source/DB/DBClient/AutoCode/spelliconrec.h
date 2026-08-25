#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellIconRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellIconRec();
    ~SpellIconRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_169();
int _E1_169();
int _E5_168();
int _E4_168();
int _E8_168();
int _E7_168();
