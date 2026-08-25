#include "WoW/Source/DB/DBClient/AutoCode/worldmapcontinentrec.h"

#include <limits>

const char* WorldMapContinentRec::GetFilename() {
    return "DBFilesClient\\WorldMapContinent.dbc";
}

WorldMapContinentRec::WorldMapContinentRec() = default;
WorldMapContinentRec::~WorldMapContinentRec() = default;

unsigned char WorldMapContinentRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_241() { return 0; }
int _E1_241() { return 2139095040; }
int _E5_240() { return 0; }
int _E4_240() { return 0; }
int _E8_240() { return 0; }
int _E7_240() { return 0; }
