#include "WoW/Source/DB/DBClient/AutoCode/stringlookupsrec.h"

#include <limits>

const char* StringLookupsRec::GetFilename() {
    return "DBFilesClient\\StringLookups.dbc";
}

StringLookupsRec::StringLookupsRec() = default;
StringLookupsRec::~StringLookupsRec() = default;

unsigned char StringLookupsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_207() { return 0; }
int _E1_207() { return 2139095040; }
int _E5_206() { return 0; }
int _E4_206() { return 0; }
int _E8_206() { return 0; }
int _E7_206() { return 0; }
