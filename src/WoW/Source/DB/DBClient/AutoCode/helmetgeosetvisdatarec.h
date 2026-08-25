#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class HelmetGeosetVisDataRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    HelmetGeosetVisDataRec();
    ~HelmetGeosetVisDataRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_206();
int _E1_206();
int _E5_205();
int _E4_205();
int _E8_205();
int _E7_205();
