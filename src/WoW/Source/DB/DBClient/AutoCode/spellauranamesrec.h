#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellAuraNamesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellAuraNamesRec();
    ~SpellAuraNamesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_182();
int _E1_182();
int _E5_181();
int _E4_181();
int _E8_181();
int _E7_181();
