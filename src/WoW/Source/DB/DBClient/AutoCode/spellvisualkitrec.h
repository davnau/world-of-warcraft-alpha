#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellVisualKitRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellVisualKitRec();
    ~SpellVisualKitRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_174();
int _E1_174();
int _E5_173();
int _E4_173();
int _E8_173();
int _E7_173();
