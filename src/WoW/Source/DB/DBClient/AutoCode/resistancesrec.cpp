#include "WoW/Source/DB/DBClient/AutoCode/resistancesrec.h"

#include <limits>

const char* ResistancesRec::GetFilename() {
    return "DBFilesClient\\Resistances.dbc";
}

ResistancesRec::ResistancesRec() = default;
ResistancesRec::~ResistancesRec() = default;

unsigned char ResistancesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_212() { return 0; }
int _E1_212() { return 2139095040; }
int _E5_211() { return 0; }
int _E4_211() { return 0; }
int _E8_211() { return 0; }
int _E7_211() { return 0; }
