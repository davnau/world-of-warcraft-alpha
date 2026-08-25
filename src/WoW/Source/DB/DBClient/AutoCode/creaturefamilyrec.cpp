#include "WoW/Source/DB/DBClient/AutoCode/creaturefamilyrec.h"

#include <limits>

const char* CreatureFamilyRec::GetFilename() {
    return "DBFilesClient\\CreatureFamily.dbc";
}

CreatureFamilyRec::CreatureFamilyRec() = default;
CreatureFamilyRec::~CreatureFamilyRec() = default;

unsigned char CreatureFamilyRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_148() { return 0; }
int _E1_148() { return 2139095040; }
int _E5_147() { return 0; }
int _E4_147() { return 0; }
int _E8_147() { return 0; }
int _E7_147() { return 0; }
