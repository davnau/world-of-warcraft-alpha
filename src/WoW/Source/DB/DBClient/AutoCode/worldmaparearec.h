#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class WorldMapAreaRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    WorldMapAreaRec();
    ~WorldMapAreaRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_240();
int _E1_240();
int _E5_239();
int _E4_239();
int _E8_239();
int _E7_239();
