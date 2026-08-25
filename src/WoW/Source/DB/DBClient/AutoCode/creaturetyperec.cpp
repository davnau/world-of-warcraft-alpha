#include "WoW/Source/DB/DBClient/AutoCode/creaturetyperec.h"

#include <limits>

const char* CreatureTypeRec::GetFilename() {
    return "DBFilesClient\\CreatureType.dbc";
}

CreatureTypeRec::CreatureTypeRec() = default;
CreatureTypeRec::~CreatureTypeRec() = default;

unsigned char CreatureTypeRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_151() { return 0; }
int _E1_151() { return 2139095040; }
int _E5_150() { return 0; }
int _E4_150() { return 0; }
int _E8_150() { return 0; }
int _E7_150() { return 0; }
