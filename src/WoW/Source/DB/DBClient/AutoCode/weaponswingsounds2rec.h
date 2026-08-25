#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class WeaponSwingSounds2Rec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    WeaponSwingSounds2Rec();
    ~WeaponSwingSounds2Rec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_193();
int _E1_193();
int _E5_192();
int _E4_192();
int _E8_192();
int _E7_192();
