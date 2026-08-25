#include "WoW/Source/DB/DBClient/AutoCode/spellrec.h"

#include <limits>

const char* SpellRec::GetFilename() {
    return "DBFilesClient\\Spell.dbc";
}

SpellRec::SpellRec() = default;
SpellRec::~SpellRec() = default;

unsigned char SpellRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_168() { return 0; }
int _E1_168() { return 2139095040; }
int _E5_167() { return 0; }
int _E4_167() { return 0; }
int _E8_167() { return 0; }
int _E7_167() { return 0; }
