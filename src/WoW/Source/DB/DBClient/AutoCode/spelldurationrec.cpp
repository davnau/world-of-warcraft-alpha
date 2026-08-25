#include "WoW/Source/DB/DBClient/AutoCode/spelldurationrec.h"

#include <limits>

const char* SpellDurationRec::GetFilename() {
    return "DBFilesClient\\SpellDuration.dbc";
}

SpellDurationRec::SpellDurationRec() = default;
SpellDurationRec::~SpellDurationRec() = default;

unsigned char SpellDurationRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_180() { return 0; }
int _E1_180() { return 2139095040; }
int _E5_179() { return 0; }
int _E4_179() { return 0; }
int _E8_179() { return 0; }
int _E7_179() { return 0; }
