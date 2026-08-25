#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharHairGeosetsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharHairGeosetsRec();
    ~CharHairGeosetsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_239();
int _E1_239();
int _E5_238();
int _E4_238();
int _E8_238();
int _E7_238();
