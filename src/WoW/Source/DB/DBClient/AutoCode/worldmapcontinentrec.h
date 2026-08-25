#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class WorldMapContinentRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    WorldMapContinentRec();
    ~WorldMapContinentRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_241();
int _E1_241();
int _E5_240();
int _E4_240();
int _E8_240();
int _E7_240();
