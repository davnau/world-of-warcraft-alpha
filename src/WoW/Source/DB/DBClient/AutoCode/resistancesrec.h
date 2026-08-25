#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ResistancesRec final : public ClientDBRecord {
public:
    int m_ID = 0;
    int m_Flags = 0;
    static const char* GetFilename();
    ResistancesRec();
    ~ResistancesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_212();
int _E1_212();
int _E5_211();
int _E4_211();
int _E8_211();
int _E7_211();
