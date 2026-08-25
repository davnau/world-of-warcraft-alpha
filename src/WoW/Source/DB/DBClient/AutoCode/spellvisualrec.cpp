#include "WoW/Source/DB/DBClient/AutoCode/spellvisualrec.h"

#include <limits>

const char* SpellVisualRec::GetFilename() {
    return "DBFilesClient\\SpellVisual.dbc";
}

SpellVisualRec::SpellVisualRec() = default;
SpellVisualRec::~SpellVisualRec() = default;

unsigned char SpellVisualRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_172() { return 0; }
int _E1_172() { return 2139095040; }
int _E5_171() { return 0; }
int _E4_171() { return 0; }
int _E8_171() { return 0; }
int _E7_171() { return 0; }
