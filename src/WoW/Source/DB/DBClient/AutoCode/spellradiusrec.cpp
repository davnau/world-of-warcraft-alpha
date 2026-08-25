#include "WoW/Source/DB/DBClient/AutoCode/spellradiusrec.h"

#include <limits>

const char* SpellRadiusRec::GetFilename() {
    return "DBFilesClient\\SpellRadius.dbc";
}

SpellRadiusRec::SpellRadiusRec() = default;
SpellRadiusRec::~SpellRadiusRec() = default;

unsigned char SpellRadiusRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_171() { return 0; }
int _E1_171() { return 2139095040; }
int _E5_170() { return 0; }
int _E4_170() { return 0; }
int _E8_170() { return 0; }
int _E7_170() { return 0; }
