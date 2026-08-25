#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellRadiusRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellRadiusRec();
    ~SpellRadiusRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_171();
int _E1_171();
int _E5_170();
int _E4_170();
int _E8_170();
int _E7_170();
