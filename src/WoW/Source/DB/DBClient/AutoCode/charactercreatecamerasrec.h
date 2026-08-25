#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CharacterCreateCamerasRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CharacterCreateCamerasRec();
    ~CharacterCreateCamerasRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_203();
int _E1_203();
int _E5_202();
int _E4_202();
int _E8_202();
int _E7_202();
