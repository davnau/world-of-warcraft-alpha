#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class UnitBloodRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    UnitBloodRec();
    ~UnitBloodRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_223();
int _E1_223();
int _E5_222();
int _E4_222();
int _E8_222();
int _E7_222();
