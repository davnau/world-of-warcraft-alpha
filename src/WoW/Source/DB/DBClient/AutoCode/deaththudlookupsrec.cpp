#include "WoW/Source/DB/DBClient/AutoCode/deaththudlookupsrec.h"

#include <limits>

const char* DeathThudLookupsRec::GetFilename() {
    return "DBFilesClient\\DeathThudLookups.dbc";
}

DeathThudLookupsRec::DeathThudLookupsRec() = default;
DeathThudLookupsRec::~DeathThudLookupsRec() = default;

unsigned char DeathThudLookupsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_219() { return 0; }
int _E1_219() { return 2139095040; }
int _E5_218() { return 0; }
int _E4_218() { return 0; }
int _E8_218() { return 0; }
int _E7_218() { return 0; }
