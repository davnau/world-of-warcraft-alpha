#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ItemGroupSoundsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ItemGroupSoundsRec();
    ~ItemGroupSoundsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_210();
int _E1_210();
int _E5_209();
int _E4_209();
int _E8_209();
int _E7_209();
