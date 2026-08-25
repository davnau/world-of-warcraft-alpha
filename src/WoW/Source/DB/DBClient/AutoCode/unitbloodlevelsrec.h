#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class UnitBloodLevelsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    UnitBloodLevelsRec();
    ~UnitBloodLevelsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_224();
int _E1_224();
int _E5_223();
int _E4_223();
int _E8_223();
int _E7_223();
