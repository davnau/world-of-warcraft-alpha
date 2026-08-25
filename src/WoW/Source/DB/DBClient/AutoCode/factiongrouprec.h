#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class FactionGroupRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    FactionGroupRec();
    ~FactionGroupRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_158();
int _E1_158();
int _E5_157();
int _E4_157();
int _E8_157();
int _E7_157();
