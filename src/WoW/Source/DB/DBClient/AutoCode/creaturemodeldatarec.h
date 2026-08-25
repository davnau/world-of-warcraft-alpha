#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CreatureModelDataRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CreatureModelDataRec();
    ~CreatureModelDataRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_149();
int _E1_149();
int _E5_148();
int _E4_148();
int _E8_148();
int _E7_148();
