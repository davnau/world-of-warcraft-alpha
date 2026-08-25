#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class MapRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    MapRec();
    ~MapRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_200();
int _E1_200();
int _E5_199();
int _E4_199();
int _E8_199();
int _E7_199();
