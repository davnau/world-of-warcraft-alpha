#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharTextureVariationsV2Rec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharTextureVariationsV2Rec();
    ~CharTextureVariationsV2Rec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_238();
int _E1_238();
int _E5_237();
int _E4_237();
int _E8_237();
int _E7_237();
