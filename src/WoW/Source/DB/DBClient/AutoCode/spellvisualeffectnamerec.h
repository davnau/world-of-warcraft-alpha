#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellVisualEffectNameRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellVisualEffectNameRec();
    ~SpellVisualEffectNameRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);

    int m_ID = 0;
    const char* m_fileName = nullptr;
    int m_specialID = -1;
    int m_specialAttachPoint = 0;
    float m_areaEffectSize = 0.0F;
    int m_VisualEffectNameFlags = 0;
};

int _E2_173();
int _E1_173();
int _E5_172();
int _E4_172();
int _E8_172();
int _E7_172();
