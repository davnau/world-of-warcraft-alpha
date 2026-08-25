#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TaxiNodesRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_ContinentID = 0;
    float m_X = 0.0F;
    float m_Y = 0.0F;
    float m_Z = 0.0F;

    static const char* GetFilename();
    TaxiNodesRec();
    ~TaxiNodesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_213();
int _E1_213();
int _E5_212();
int _E4_212();
int _E8_212();
int _E7_212();
