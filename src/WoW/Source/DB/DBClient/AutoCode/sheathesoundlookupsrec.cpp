#include "WoW/Source/DB/DBClient/AutoCode/sheathesoundlookupsrec.h"

#include <limits>

const char* SheatheSoundLookupsRec::GetFilename() {
    return "DBFilesClient\\SheatheSoundLookups.dbc";
}

SheatheSoundLookupsRec::SheatheSoundLookupsRec() = default;
SheatheSoundLookupsRec::~SheatheSoundLookupsRec() = default;

unsigned char SheatheSoundLookupsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_214() { return 0; }
int _E1_214() { return 2139095040; }
int _E5_213() { return 0; }
int _E4_213() { return 0; }
int _E8_213() { return 0; }
int _E7_213() { return 0; }
