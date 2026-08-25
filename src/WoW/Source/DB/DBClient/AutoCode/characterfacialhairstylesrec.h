#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharacterFacialHairStylesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharacterFacialHairStylesRec();
    ~CharacterFacialHairStylesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_202();
int _E1_202();
int _E5_201();
int _E4_201();
int _E8_201();
int _E7_201();
