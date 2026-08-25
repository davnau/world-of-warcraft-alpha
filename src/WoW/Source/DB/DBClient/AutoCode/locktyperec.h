#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class LockTypeRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    LockTypeRec();
    ~LockTypeRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_198();
int _E1_198();
int _E5_197();
int _E4_197();
int _E8_197();
int _E7_197();
