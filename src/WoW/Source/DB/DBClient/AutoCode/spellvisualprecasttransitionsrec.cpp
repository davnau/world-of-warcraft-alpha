#include "WoW/Source/DB/DBClient/AutoCode/spellvisualprecasttransitionsrec.h"

#include <limits>

const char* SpellVisualPrecastTransitionsRec::GetFilename() {
    return "DBFilesClient\\SpellVisualPrecastTransitions.dbc";
}

SpellVisualPrecastTransitionsRec::SpellVisualPrecastTransitionsRec() = default;
SpellVisualPrecastTransitionsRec::~SpellVisualPrecastTransitionsRec() = default;

unsigned char SpellVisualPrecastTransitionsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_232() { return 0; }
int _E1_232() { return 2139095040; }
int _E5_231() { return 0; }
int _E4_231() { return 0; }
int _E8_231() { return 0; }
int _E7_231() { return 0; }
