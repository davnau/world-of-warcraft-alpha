#include "WoW/Source/DB/DBClient/AutoCode/factiongrouprec.h"

#include <limits>

const char* FactionGroupRec::GetFilename() {
    return "DBFilesClient\\FactionGroup.dbc";
}

FactionGroupRec::FactionGroupRec() = default;
FactionGroupRec::~FactionGroupRec() = default;

unsigned char FactionGroupRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_158() { return 0; }
int _E1_158() { return 2139095040; }
int _E5_157() { return 0; }
int _E4_157() { return 0; }
int _E8_157() { return 0; }
int _E7_157() { return 0; }
