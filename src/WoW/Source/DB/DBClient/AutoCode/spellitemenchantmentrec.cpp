#include "WoW/Source/DB/DBClient/AutoCode/spellitemenchantmentrec.h"

#include <limits>

const char* SpellItemEnchantmentRec::GetFilename() {
    return "DBFilesClient\\SpellItemEnchantment.dbc";
}

SpellItemEnchantmentRec::SpellItemEnchantmentRec() = default;
SpellItemEnchantmentRec::~SpellItemEnchantmentRec() = default;

unsigned char SpellItemEnchantmentRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_226() { return 0; }
int _E1_226() { return 2139095040; }
int _E5_225() { return 0; }
int _E4_225() { return 0; }
int _E8_225() { return 0; }
int _E7_225() { return 0; }
