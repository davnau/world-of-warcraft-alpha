#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellEffectNamesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellEffectNamesRec();
    ~SpellEffectNamesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_181();
int _E1_181();
int _E5_180();
int _E4_180();
int _E8_180();
int _E7_180();
