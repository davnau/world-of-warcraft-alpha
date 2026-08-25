#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellChainEffectsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellChainEffectsRec();
    ~SpellChainEffectsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_237();
int _E1_237();
int _E5_236();
int _E4_236();
int _E8_236();
int _E7_236();
