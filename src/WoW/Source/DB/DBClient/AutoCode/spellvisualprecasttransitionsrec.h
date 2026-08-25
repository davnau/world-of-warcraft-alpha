#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellVisualPrecastTransitionsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellVisualPrecastTransitionsRec();
    ~SpellVisualPrecastTransitionsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_232();
int _E1_232();
int _E5_231();
int _E4_231();
int _E8_231();
int _E7_231();
