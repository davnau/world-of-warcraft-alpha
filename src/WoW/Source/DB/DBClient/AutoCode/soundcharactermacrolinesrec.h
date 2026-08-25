#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SoundCharacterMacroLinesRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_Category = 0;
    int m_Sex = 0;
    int m_Race = 0;
    int m_SoundID = 0;
    static const char* GetFilename();
    SoundCharacterMacroLinesRec();
    ~SoundCharacterMacroLinesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_242();
int _E1_242();
int _E5_241();
int _E4_241();
int _E8_241();
int _E7_241();
