#include "WoW/Source/DB/DBClient/AutoCode/lockrec.h"

#include <limits>

const char* LockRec::GetFilename() {
    return "DBFilesClient\\Lock.dbc";
}

LockRec::LockRec() = default;
LockRec::~LockRec() = default;

unsigned char LockRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_197() { return 0; }
int _E1_197() { return 2139095040; }
int _E5_196() { return 0; }
int _E4_196() { return 0; }
int _E8_196() { return 0; }
int _E7_196() { return 0; }
