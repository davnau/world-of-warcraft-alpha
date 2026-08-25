#include "WoW/Source/DB/DBClient/AutoCode/unitbloodrec.h"

#include <limits>

const char* UnitBloodRec::GetFilename() {
    return "DBFilesClient\\UnitBlood.dbc";
}

UnitBloodRec::UnitBloodRec() = default;
UnitBloodRec::~UnitBloodRec() = default;

unsigned char UnitBloodRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_223() { return 0; }
int _E1_223() { return 2139095040; }
int _E5_222() { return 0; }
int _E4_222() { return 0; }
int _E8_222() { return 0; }
int _E7_222() { return 0; }
