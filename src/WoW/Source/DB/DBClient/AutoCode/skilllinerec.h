#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SkillLineRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SkillLineRec();
    ~SkillLineRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_166();
int _E1_166();
int _E5_165();
int _E4_165();
int _E8_165();
int _E7_165();
