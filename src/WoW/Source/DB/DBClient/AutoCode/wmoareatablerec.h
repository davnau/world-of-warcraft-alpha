#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class WMOAreaTableRec final : public ClientDBRecord {
public:
    int m_WMOGroupID = 0;
    int m_WMOID = 0;
    int m_NameSetID = 0;
    const char* m_Name = "";
    static const char* GetFilename();
    WMOAreaTableRec();
    ~WMOAreaTableRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_234();
int _E1_234();
int _E5_233();
int _E4_233();
int _E8_233();
int _E7_233();
