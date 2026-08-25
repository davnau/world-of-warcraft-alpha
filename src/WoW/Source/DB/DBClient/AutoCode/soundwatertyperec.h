#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SoundWaterTypeRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_soundType = 0;
    int m_soundSubtype = 0;
    int m_SoundID = 0;
    static const char* GetFilename();
    SoundWaterTypeRec();
    ~SoundWaterTypeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_225();
int _E1_225();
int _E5_224();
int _E4_224();
int _E8_224();
int _E7_224();
