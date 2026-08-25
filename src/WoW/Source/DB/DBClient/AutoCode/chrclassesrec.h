#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ChrClassesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ChrClassesRec();
    ~ChrClassesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_154();
int _E1_154();
int _E5_153();
int _E4_153();
int _E8_153();
int _E7_153();
