#include "WoW/Source/DB/DBClient/AutoCode/chrclassesrec.h"

#include <limits>

const char* ChrClassesRec::GetFilename() {
    return "DBFilesClient\\ChrClasses.dbc";
}

ChrClassesRec::ChrClassesRec() = default;
ChrClassesRec::~ChrClassesRec() = default;

unsigned char ChrClassesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_154() { return 0; }
int _E1_154() { return 2139095040; }
int _E5_153() { return 0; }
int _E4_153() { return 0; }
int _E8_153() { return 0; }
int _E7_153() { return 0; }
