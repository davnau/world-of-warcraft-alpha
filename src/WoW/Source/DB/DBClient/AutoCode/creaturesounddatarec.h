#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CreatureSoundDataRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CreatureSoundDataRec();
    ~CreatureSoundDataRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_150();
int _E1_150();
int _E5_149();
int _E4_149();
int _E8_149();
int _E7_149();
