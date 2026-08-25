#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CreatureTypeRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CreatureTypeRec();
    ~CreatureTypeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_151();
int _E1_151();
int _E5_150();
int _E4_150();
int _E8_150();
int _E7_150();
