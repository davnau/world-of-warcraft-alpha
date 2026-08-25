#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class WeaponImpactSoundsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    WeaponImpactSoundsRec();
    ~WeaponImpactSoundsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_178();
int _E1_178();
int _E5_177();
int _E4_177();
int _E8_177();
int _E7_177();
