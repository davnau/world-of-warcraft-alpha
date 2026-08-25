#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TaxiPathNodeRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_PathID = 0;
    int m_NodeIndex = 0;
    int m_ContinentID = 0;
    float m_X = 0.0F;
    float m_Y = 0.0F;
    float m_Z = 0.0F;

    static const char* GetFilename();
    TaxiPathNodeRec();
    ~TaxiPathNodeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_216();
int _E1_216();
int _E5_215();
int _E4_215();
int _E8_215();
int _E7_215();
