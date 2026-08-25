#include "WoW/Source/DB/DBClient/AutoCode/areatablerec.h"

#include <limits>

const char* AreaTableRec::GetFilename() {
    return "DBFilesClient\\AreaTable.dbc";
}

AreaTableRec::AreaTableRec() = default;
AreaTableRec::~AreaTableRec() = default;

unsigned char AreaTableRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_189() { return 0; }
int _E1_189() { return 2139095040; }
int _E5_188() { return 0; }
int _E4_188() { return 0; }
int _E8_188() { return 0; }
int _E7_188() { return 0; }
