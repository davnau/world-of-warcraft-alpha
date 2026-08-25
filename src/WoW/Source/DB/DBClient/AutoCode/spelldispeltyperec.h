#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellDispelTypeRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellDispelTypeRec();
    ~SpellDispelTypeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_183();
int _E1_183();
int _E5_182();
int _E4_182();
int _E8_182();
int _E7_182();
