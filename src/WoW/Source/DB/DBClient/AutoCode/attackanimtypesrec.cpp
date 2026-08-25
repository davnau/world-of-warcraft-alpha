#include "WoW/Source/DB/DBClient/AutoCode/attackanimtypesrec.h"

#include <limits>

const char* AttackAnimTypesRec::GetFilename() {
    return "DBFilesClient\\AttackAnimTypes.dbc";
}

AttackAnimTypesRec::AttackAnimTypesRec() = default;
AttackAnimTypesRec::~AttackAnimTypesRec() = default;

unsigned char AttackAnimTypesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_191() { return 0; }
int _E1_191() { return 2139095040; }
int _E5_190() { return 0; }
int _E4_190() { return 0; }
int _E8_190() { return 0; }
int _E7_190() { return 0; }
