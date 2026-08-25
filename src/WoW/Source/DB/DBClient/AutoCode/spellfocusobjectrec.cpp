#include "WoW/Source/DB/DBClient/AutoCode/spellfocusobjectrec.h"

#include <limits>

const char* SpellFocusObjectRec::GetFilename() {
    return "DBFilesClient\\SpellFocusObject.dbc";
}

SpellFocusObjectRec::SpellFocusObjectRec() = default;
SpellFocusObjectRec::~SpellFocusObjectRec() = default;

unsigned char SpellFocusObjectRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_231() { return 0; }
int _E1_231() { return 2139095040; }
int _E5_230() { return 0; }
int _E4_230() { return 0; }
int _E8_230() { return 0; }
int _E7_230() { return 0; }
