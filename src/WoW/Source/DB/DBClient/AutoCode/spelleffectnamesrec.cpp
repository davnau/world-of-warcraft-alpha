#include "WoW/Source/DB/DBClient/AutoCode/spelleffectnamesrec.h"

#include <limits>

const char* SpellEffectNamesRec::GetFilename() {
    return "DBFilesClient\\SpellEffectNames.dbc";
}

SpellEffectNamesRec::SpellEffectNamesRec() = default;
SpellEffectNamesRec::~SpellEffectNamesRec() = default;

unsigned char SpellEffectNamesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_181() { return 0; }
int _E1_181() { return 2139095040; }
int _E5_180() { return 0; }
int _E4_180() { return 0; }
int _E8_180() { return 0; }
int _E7_180() { return 0; }
