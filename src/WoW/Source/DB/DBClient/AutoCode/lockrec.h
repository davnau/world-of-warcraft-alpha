#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class LockRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    LockRec();
    ~LockRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_197();
int _E1_197();
int _E5_196();
int _E4_196();
int _E8_196();
int _E7_196();
