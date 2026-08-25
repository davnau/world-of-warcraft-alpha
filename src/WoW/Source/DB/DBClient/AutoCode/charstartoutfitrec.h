#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharStartOutfitRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharStartOutfitRec();
    ~CharStartOutfitRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_152();
int _E1_152();
int _E5_151();
int _E4_151();
int _E8_151();
int _E7_151();
