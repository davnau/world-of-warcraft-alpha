#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ItemSubClassRec final : public ClientDBRecord {
public:
    int m_classID = -1;
    int m_subClassID = -1;
    int m_flags = 0;
    int m_weaponParrySeq = 0;
    int m_weaponReadySeq = 0;
    int m_weaponAttackSeq = 0;
    static const char* GetFilename();
    ItemSubClassRec();
    ~ItemSubClassRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_165();
int _E1_165();
int _E5_164();
int _E4_164();
int _E8_164();
int _E7_164();
