#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellShapeshiftFormRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellShapeshiftFormRec();
    ~SpellShapeshiftFormRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_233();
int _E1_233();
int _E5_232();
int _E4_232();
int _E8_232();
int _E7_232();
