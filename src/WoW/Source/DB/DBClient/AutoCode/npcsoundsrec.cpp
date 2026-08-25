#include "WoW/Source/DB/DBClient/AutoCode/npcsoundsrec.h"

#include <limits>

const char* NPCSoundsRec::GetFilename() {
    return "DBFilesClient\\NPCSounds.dbc";
}

NPCSoundsRec::NPCSoundsRec() = default;
NPCSoundsRec::~NPCSoundsRec() = default;

unsigned char NPCSoundsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_221() { return 0; }
int _E1_221() { return 2139095040; }
int _E5_220() { return 0; }
int _E4_220() { return 0; }
int _E8_220() { return 0; }
int _E7_220() { return 0; }
