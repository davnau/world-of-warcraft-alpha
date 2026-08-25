#include "WoW/Source/DB/DBClient/AutoCode/itemsubclassrec.h"

#include <limits>

const char* ItemSubClassRec::GetFilename() {
    return "DBFilesClient\\ItemSubClass.dbc";
}

ItemSubClassRec::ItemSubClassRec() = default;
ItemSubClassRec::~ItemSubClassRec() = default;

unsigned char ItemSubClassRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_165() { return 0; }
int _E1_165() { return 2139095040; }
int _E5_164() { return 0; }
int _E4_164() { return 0; }
int _E8_164() { return 0; }
int _E7_164() { return 0; }
