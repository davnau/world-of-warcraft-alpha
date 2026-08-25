#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ChrRacesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ChrRacesRec();
    ~ChrRacesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_153();
int _E1_153();
int _E5_152();
int _E4_152();
int _E8_152();
int _E7_152();
