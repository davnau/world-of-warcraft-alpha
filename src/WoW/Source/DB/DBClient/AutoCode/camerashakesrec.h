#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CameraShakesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CameraShakesRec();
    ~CameraShakesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_145();
int _E1_145();
int _E5_144();
int _E4_144();
int _E8_144();
int _E7_144();
