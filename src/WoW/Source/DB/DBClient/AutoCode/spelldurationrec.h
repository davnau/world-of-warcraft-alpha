#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellDurationRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellDurationRec();
    ~SpellDurationRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_180();
int _E1_180();
int _E5_179();
int _E4_179();
int _E8_179();
int _E7_179();
