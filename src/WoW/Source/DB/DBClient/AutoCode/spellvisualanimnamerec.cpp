#include "WoW/Source/DB/DBClient/AutoCode/spellvisualanimnamerec.h"

#include <limits>

const char* SpellVisualAnimNameRec::GetFilename() {
    return "DBFilesClient\\SpellVisualAnimName.dbc";
}

SpellVisualAnimNameRec::SpellVisualAnimNameRec() = default;
SpellVisualAnimNameRec::~SpellVisualAnimNameRec() = default;

unsigned char SpellVisualAnimNameRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_185() { return 0; }
int _E1_185() { return 2139095040; }
int _E5_184() { return 0; }
int _E4_184() { return 0; }
int _E8_184() { return 0; }
int _E7_184() { return 0; }
