#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CreatureDisplayInfoExtraRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CreatureDisplayInfoExtraRec();
    ~CreatureDisplayInfoExtraRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_147();
int _E1_147();
int _E5_146();
int _E4_146();
int _E8_146();
int _E7_146();
