#include "WoW/Source/DB/DBClient/AutoCode/spellcasttimesrec.h"

#include <limits>

const char* SpellCastTimesRec::GetFilename() {
    return "DBFilesClient\\SpellCastTimes.dbc";
}

SpellCastTimesRec::SpellCastTimesRec() = default;
SpellCastTimesRec::~SpellCastTimesRec() = default;

unsigned char SpellCastTimesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_179() { return 0; }
int _E1_179() { return 2139095040; }
int _E5_178() { return 0; }
int _E4_178() { return 0; }
int _E8_178() { return 0; }
int _E7_178() { return 0; }
