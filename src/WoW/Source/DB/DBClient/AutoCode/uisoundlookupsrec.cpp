#include "WoW/Source/DB/DBClient/AutoCode/uisoundlookupsrec.h"

#include <limits>

const char* UISoundLookupsRec::GetFilename() {
    return "DBFilesClient\\UISoundLookups.dbc";
}

UISoundLookupsRec::UISoundLookupsRec() = default;
UISoundLookupsRec::~UISoundLookupsRec() = default;

unsigned char UISoundLookupsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_194() { return 0; }
int _E1_194() { return 2139095040; }
int _E5_193() { return 0; }
int _E4_193() { return 0; }
int _E8_193() { return 0; }
int _E7_193() { return 0; }
