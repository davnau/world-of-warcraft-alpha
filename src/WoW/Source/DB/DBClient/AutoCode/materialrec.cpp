#include "WoW/Source/DB/DBClient/AutoCode/materialrec.h"

#include <limits>

const char* MaterialRec::GetFilename() {
    return "DBFilesClient\\Material.dbc";
}

MaterialRec::MaterialRec() = default;
MaterialRec::~MaterialRec() = default;

unsigned char MaterialRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_177() { return 0; }
int _E1_177() { return 2139095040; }
int _E5_176() { return 0; }
int _E4_176() { return 0; }
int _E8_176() { return 0; }
int _E7_176() { return 0; }
