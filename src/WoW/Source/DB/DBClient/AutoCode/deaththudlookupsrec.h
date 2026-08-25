#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class DeathThudLookupsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    DeathThudLookupsRec();
    ~DeathThudLookupsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_219();
int _E1_219();
int _E5_218();
int _E4_218();
int _E8_218();
int _E7_218();
