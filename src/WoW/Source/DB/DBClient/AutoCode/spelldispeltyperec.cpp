#include "WoW/Source/DB/DBClient/AutoCode/spelldispeltyperec.h"

#include <limits>

const char* SpellDispelTypeRec::GetFilename() {
    return "DBFilesClient\\SpellDispelType.dbc";
}

SpellDispelTypeRec::SpellDispelTypeRec() = default;
SpellDispelTypeRec::~SpellDispelTypeRec() = default;

unsigned char SpellDispelTypeRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_183() { return 0; }
int _E1_183() { return 2139095040; }
int _E5_182() { return 0; }
int _E4_182() { return 0; }
int _E8_182() { return 0; }
int _E7_182() { return 0; }
