#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class FactionRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    FactionRec();
    ~FactionRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_157();
int _E1_157();
int _E5_156();
int _E4_156();
int _E8_156();
int _E7_156();
