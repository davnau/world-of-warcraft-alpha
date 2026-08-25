#include "WoW/Source/DB/DBClient/AutoCode/factionrec.h"

#include <limits>

const char* FactionRec::GetFilename() {
    return "DBFilesClient\\Faction.dbc";
}

FactionRec::FactionRec() = default;
FactionRec::~FactionRec() = default;

unsigned char FactionRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_157() { return 0; }
int _E1_157() { return 2139095040; }
int _E5_156() { return 0; }
int _E4_156() { return 0; }
int _E8_156() { return 0; }
int _E7_156() { return 0; }
