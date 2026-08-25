#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SheatheSoundLookupsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SheatheSoundLookupsRec();
    ~SheatheSoundLookupsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_214();
int _E1_214();
int _E5_213();
int _E4_213();
int _E8_213();
int _E7_213();
