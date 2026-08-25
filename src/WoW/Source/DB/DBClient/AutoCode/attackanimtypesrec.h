#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class AttackAnimTypesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    AttackAnimTypesRec();
    ~AttackAnimTypesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_191();
int _E1_191();
int _E5_190();
int _E4_190();
int _E8_190();
int _E7_190();
