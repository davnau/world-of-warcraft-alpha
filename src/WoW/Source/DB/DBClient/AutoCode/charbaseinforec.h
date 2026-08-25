#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharBaseInfoRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharBaseInfoRec();
    ~CharBaseInfoRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_175();
int _E1_175();
int _E5_174();
int _E4_174();
int _E8_174();
int _E7_174();
