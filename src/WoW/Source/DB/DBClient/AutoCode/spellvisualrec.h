#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellVisualRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellVisualRec();
    ~SpellVisualRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_172();
int _E1_172();
int _E5_171();
int _E4_171();
int _E8_171();
int _E7_171();
