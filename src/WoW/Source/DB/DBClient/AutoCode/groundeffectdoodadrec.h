#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class GroundEffectDoodadRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    GroundEffectDoodadRec();
    ~GroundEffectDoodadRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_144();
int _E1_144();
int _E5_143();
int _E4_143();
int _E8_143();
int _E7_143();
