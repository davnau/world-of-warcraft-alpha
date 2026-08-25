#include "WoW/Source/DB/DBClient/AutoCode/spellauranamesrec.h"

#include <limits>

const char* SpellAuraNamesRec::GetFilename() {
    return "DBFilesClient\\SpellAuraNames.dbc";
}

SpellAuraNamesRec::SpellAuraNamesRec() = default;
SpellAuraNamesRec::~SpellAuraNamesRec() = default;

unsigned char SpellAuraNamesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_182() { return 0; }
int _E1_182() { return 2139095040; }
int _E5_181() { return 0; }
int _E4_181() { return 0; }
int _E8_181() { return 0; }
int _E7_181() { return 0; }
