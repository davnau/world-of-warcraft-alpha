#include "WoW/Source/DB/DBClient/AutoCode/creaturemodeldatarec.h"

#include <limits>

const char* CreatureModelDataRec::GetFilename() {
    return "DBFilesClient\\CreatureModelData.dbc";
}

CreatureModelDataRec::CreatureModelDataRec() = default;
CreatureModelDataRec::~CreatureModelDataRec() = default;

unsigned char CreatureModelDataRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_149() { return 0; }
int _E1_149() { return 2139095040; }
int _E5_148() { return 0; }
int _E4_148() { return 0; }
int _E8_148() { return 0; }
int _E7_148() { return 0; }
