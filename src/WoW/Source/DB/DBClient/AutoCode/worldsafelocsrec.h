#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class WorldSafeLocsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    WorldSafeLocsRec();
    ~WorldSafeLocsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_243();
int _E1_243();
int _E5_242();
int _E4_242();
int _E8_242();
int _E7_242();
