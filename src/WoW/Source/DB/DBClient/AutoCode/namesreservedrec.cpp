#include "WoW/Source/DB/DBClient/AutoCode/namesreservedrec.h"

#include <limits>

const char* NamesReservedRec::GetFilename() {
    return "DBFilesClient\\NamesReserved.dbc";
}

NamesReservedRec::NamesReservedRec() = default;
NamesReservedRec::~NamesReservedRec() = default;

unsigned char NamesReservedRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_247() { return 0; }
int _E1_247() { return 2139095040; }
int _E5_246() { return 0; }
int _E4_246() { return 0; }
int _E8_246() { return 0; }
int _E7_246() { return 0; }
