#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ItemVisualEffectsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ItemVisualEffectsRec();
    ~ItemVisualEffectsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_249();
int _E1_249();
int _E5_248();
int _E4_248();
int _E8_248();
int _E7_248();
