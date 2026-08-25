#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellCastTimesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellCastTimesRec();
    ~SpellCastTimesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_179();
int _E1_179();
int _E5_178();
int _E4_178();
int _E8_178();
int _E7_178();
