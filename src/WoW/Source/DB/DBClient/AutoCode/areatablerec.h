#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class AreaTableRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    AreaTableRec();
    ~AreaTableRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_189();
int _E1_189();
int _E5_188();
int _E4_188();
int _E8_188();
int _E7_188();
