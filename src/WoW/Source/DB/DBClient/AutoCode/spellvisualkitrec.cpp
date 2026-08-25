#include "WoW/Source/DB/DBClient/AutoCode/spellvisualkitrec.h"

#include <limits>

const char* SpellVisualKitRec::GetFilename() {
    return "DBFilesClient\\SpellVisualKit.dbc";
}

SpellVisualKitRec::SpellVisualKitRec() = default;
SpellVisualKitRec::~SpellVisualKitRec() = default;

unsigned char SpellVisualKitRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_174() { return 0; }
int _E1_174() { return 2139095040; }
int _E5_173() { return 0; }
int _E4_173() { return 0; }
int _E8_173() { return 0; }
int _E7_173() { return 0; }
