#include "WoW/Source/DB/DBClient/AutoCode/attackanimkitsrec.h"

#include <limits>

const char* AttackAnimKitsRec::GetFilename() {
    return "DBFilesClient\\AttackAnimKits.dbc";
}

AttackAnimKitsRec::AttackAnimKitsRec() = default;
AttackAnimKitsRec::~AttackAnimKitsRec() = default;

unsigned char AttackAnimKitsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_190() { return 0; }
int _E1_190() { return 2139095040; }
int _E5_189() { return 0; }
int _E4_189() { return 0; }
int _E8_189() { return 0; }
int _E7_189() { return 0; }
