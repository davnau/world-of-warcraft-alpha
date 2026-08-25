#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ItemDisplayInfoRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ItemDisplayInfoRec();
    ~ItemDisplayInfoRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_160();
int _E1_160();
int _E5_159();
int _E4_159();
int _E8_159();
int _E7_159();
