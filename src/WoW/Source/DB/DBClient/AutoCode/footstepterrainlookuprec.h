#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class FootstepTerrainLookupRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    FootstepTerrainLookupRec();
    ~FootstepTerrainLookupRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_211();
int _E1_211();
int _E5_210();
int _E4_210();
int _E8_210();
int _E7_210();
