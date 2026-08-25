#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ChrProficiencyRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ChrProficiencyRec();
    ~ChrProficiencyRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_176();
int _E1_176();
int _E5_175();
int _E4_175();
int _E8_175();
int _E7_175();
