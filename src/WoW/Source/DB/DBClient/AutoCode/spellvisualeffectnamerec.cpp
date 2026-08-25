#include "WoW/Source/DB/DBClient/AutoCode/spellvisualeffectnamerec.h"

#include <limits>

const char* SpellVisualEffectNameRec::GetFilename() {
    return "DBFilesClient\\SpellVisualEffectName.dbc";
}

SpellVisualEffectNameRec::SpellVisualEffectNameRec() = default;
SpellVisualEffectNameRec::~SpellVisualEffectNameRec() = default;

unsigned char SpellVisualEffectNameRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_173() { return 0; }
int _E1_173() { return 2139095040; }
int _E5_172() { return 0; }
int _E4_172() { return 0; }
int _E8_172() { return 0; }
int _E7_172() { return 0; }
