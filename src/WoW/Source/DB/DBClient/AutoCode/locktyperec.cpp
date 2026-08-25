#include "WoW/Source/DB/DBClient/AutoCode/locktyperec.h"

#include <limits>

const char* LockTypeRec::GetFilename() {
    return "DBFilesClient\\LockType.dbc";
}

LockTypeRec::LockTypeRec() = default;
LockTypeRec::~LockTypeRec() = default;

unsigned char LockTypeRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_198() { return 0; }
int _E1_198() { return 2139095040; }
int _E5_197() { return 0; }
int _E4_197() { return 0; }
int _E8_197() { return 0; }
int _E7_197() { return 0; }
