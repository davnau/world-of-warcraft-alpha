#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class GroundEffectTextureRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    GroundEffectTextureRec();
    ~GroundEffectTextureRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_143();
int _E1_143();
int _E5_142();
int _E4_142();
int _E8_142();
int _E7_142();
