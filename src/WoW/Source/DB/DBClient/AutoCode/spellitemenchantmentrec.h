#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellItemEnchantmentRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellItemEnchantmentRec();
    ~SpellItemEnchantmentRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_226();
int _E1_226();
int _E5_225();
int _E4_225();
int _E8_225();
int _E7_225();
