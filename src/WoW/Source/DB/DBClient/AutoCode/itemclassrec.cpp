#include "WoW/Source/DB/DBClient/AutoCode/itemclassrec.h"

#include <limits>

const char* ItemClassRec::GetFilename() {
    return "DBFilesClient\\ItemClass.dbc";
}

ItemClassRec::ItemClassRec() = default;
ItemClassRec::~ItemClassRec() = default;

unsigned char ItemClassRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_220() { return 0; }
int _E1_220() { return 2139095040; }
int _E5_219() { return 0; }
int _E4_219() { return 0; }
int _E8_219() { return 0; }
int _E7_219() { return 0; }
