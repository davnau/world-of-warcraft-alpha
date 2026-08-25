#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharVariationsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharVariationsRec();
    ~CharVariationsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_164();
int _E1_164();
int _E5_163();
int _E4_163();
int _E8_163();
int _E7_163();
