#include "WoW/Source/DB/DBClient/AutoCode/spelliconrec.h"

#include <limits>

const char* SpellIconRec::GetFilename() {
    return "DBFilesClient\\SpellIcon.dbc";
}

SpellIconRec::SpellIconRec() = default;
SpellIconRec::~SpellIconRec() = default;

unsigned char SpellIconRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_169() { return 0; }
int _E1_169() { return 2139095040; }
int _E5_168() { return 0; }
int _E4_168() { return 0; }
int _E8_168() { return 0; }
int _E7_168() { return 0; }
