#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TaxiPathRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_FromTaxiNode = 0;
    int m_ToTaxiNode = 0;
    int m_Cost = 0;

    static const char* GetFilename();
    TaxiPathRec();
    ~TaxiPathRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_215();
int _E1_215();
int _E5_214();
int _E4_214();
int _E8_214();
int _E7_214();
