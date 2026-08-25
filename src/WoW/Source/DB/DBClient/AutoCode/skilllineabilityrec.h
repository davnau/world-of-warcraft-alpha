#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SkillLineAbilityRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SkillLineAbilityRec();
    ~SkillLineAbilityRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
    int m_ID = 0;
    int m_skillLineID = 0;
    int m_spellID = 0;
    int m_acquireMethod = 0;
};

int _E2_167();
int _E1_167();
int _E5_166();
int _E4_166();
int _E8_166();
int _E7_166();
