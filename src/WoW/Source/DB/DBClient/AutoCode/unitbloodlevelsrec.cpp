#include "WoW/Source/DB/DBClient/AutoCode/unitbloodlevelsrec.h"

#include <limits>

const char* UnitBloodLevelsRec::GetFilename() {
    return "DBFilesClient\\UnitBloodLevels.dbc";
}

UnitBloodLevelsRec::UnitBloodLevelsRec() = default;
UnitBloodLevelsRec::~UnitBloodLevelsRec() = default;

unsigned char UnitBloodLevelsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_224() { return 0; }
int _E1_224() { return 2139095040; }
int _E5_223() { return 0; }
int _E4_223() { return 0; }
int _E8_223() { return 0; }
int _E7_223() { return 0; }
