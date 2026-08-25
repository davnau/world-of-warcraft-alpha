#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class PageTextMaterialRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    PageTextMaterialRec();
    ~PageTextMaterialRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_209();
int _E1_209();
int _E5_208();
int _E4_208();
int _E8_208();
int _E7_208();
