#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CreatureDisplayInfoRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CreatureDisplayInfoRec();
    ~CreatureDisplayInfoRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_146();
int _E1_146();
int _E5_145();
int _E4_145();
int _E8_145();
int _E7_145();
