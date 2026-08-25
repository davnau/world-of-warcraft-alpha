#include "WoW/Source/DB/DBClient/AutoCode/spellrangerec.h"

#include <limits>

const char* SpellRangeRec::GetFilename() {
    return "DBFilesClient\\SpellRange.dbc";
}

SpellRangeRec::SpellRangeRec() = default;
SpellRangeRec::~SpellRangeRec() = default;

unsigned char SpellRangeRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_170() { return 0; }
int _E1_170() { return 2139095040; }
int _E5_169() { return 0; }
int _E4_169() { return 0; }
int _E8_169() { return 0; }
int _E7_169() { return 0; }
