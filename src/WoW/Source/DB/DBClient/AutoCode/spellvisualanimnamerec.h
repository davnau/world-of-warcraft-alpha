#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellVisualAnimNameRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellVisualAnimNameRec();
    ~SpellVisualAnimNameRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_185();
int _E1_185();
int _E5_184();
int _E4_184();
int _E8_184();
int _E7_184();
