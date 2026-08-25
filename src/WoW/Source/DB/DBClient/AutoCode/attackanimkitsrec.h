#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class AttackAnimKitsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    AttackAnimKitsRec();
    ~AttackAnimKitsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_190();
int _E1_190();
int _E5_189();
int _E4_189();
int _E8_189();
int _E7_189();
