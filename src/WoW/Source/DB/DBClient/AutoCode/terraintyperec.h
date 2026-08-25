#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TerrainTypeRec final : public ClientDBRecord {
public:
    unsigned m_SoundID = 0;
    static const char* GetFilename();
    TerrainTypeRec();
    ~TerrainTypeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_186();
int _E1_186();
int _E5_185();
int _E4_185();
int _E8_185();
int _E7_185();
