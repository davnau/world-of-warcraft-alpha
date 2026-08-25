#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TerrainTypeSoundsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    TerrainTypeSoundsRec();
    ~TerrainTypeSoundsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_187();
int _E1_187();
int _E5_186();
int _E4_186();
int _E8_186();
int _E7_186();
