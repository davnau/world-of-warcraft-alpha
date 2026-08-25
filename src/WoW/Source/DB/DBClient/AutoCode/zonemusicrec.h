#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ZoneMusicRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ZoneMusicRec();
    ~ZoneMusicRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_204();
int _E1_204();
int _E5_203();
int _E4_203();
int _E8_203();
int _E7_203();
