#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellFocusObjectRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellFocusObjectRec();
    ~SpellFocusObjectRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_231();
int _E1_231();
int _E5_230();
int _E4_230();
int _E8_230();
int _E7_230();
