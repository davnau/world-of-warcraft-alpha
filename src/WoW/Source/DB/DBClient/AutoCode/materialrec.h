#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class MaterialRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    MaterialRec();
    ~MaterialRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_177();
int _E1_177();
int _E5_176();
int _E4_176();
int _E8_176();
int _E7_176();
