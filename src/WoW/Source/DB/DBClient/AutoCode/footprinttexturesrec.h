#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class FootprintTexturesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    FootprintTexturesRec();
    ~FootprintTexturesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_201();
int _E1_201();
int _E5_200();
int _E4_200();
int _E8_200();
int _E7_200();
