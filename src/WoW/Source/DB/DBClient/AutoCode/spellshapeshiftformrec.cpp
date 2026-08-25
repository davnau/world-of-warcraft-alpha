#include "WoW/Source/DB/DBClient/AutoCode/spellshapeshiftformrec.h"

#include <limits>

const char* SpellShapeshiftFormRec::GetFilename() {
    return "DBFilesClient\\SpellShapeshiftForm.dbc";
}

SpellShapeshiftFormRec::SpellShapeshiftFormRec() = default;
SpellShapeshiftFormRec::~SpellShapeshiftFormRec() = default;

unsigned char SpellShapeshiftFormRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_233() { return 0; }
int _E1_233() { return 2139095040; }
int _E5_232() { return 0; }
int _E4_232() { return 0; }
int _E8_232() { return 0; }
int _E7_232() { return 0; }
