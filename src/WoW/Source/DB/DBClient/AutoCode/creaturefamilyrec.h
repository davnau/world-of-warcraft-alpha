#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CreatureFamilyRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CreatureFamilyRec();
    ~CreatureFamilyRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_148();
int _E1_148();
int _E5_147();
int _E4_147();
int _E8_147();
int _E7_147();
