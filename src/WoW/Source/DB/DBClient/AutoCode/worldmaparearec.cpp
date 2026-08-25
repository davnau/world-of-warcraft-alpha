#include "WoW/Source/DB/DBClient/AutoCode/worldmaparearec.h"

#include <limits>

const char* WorldMapAreaRec::GetFilename() {
    return "DBFilesClient\\WorldMapArea.dbc";
}

WorldMapAreaRec::WorldMapAreaRec() = default;
WorldMapAreaRec::~WorldMapAreaRec() = default;

unsigned char WorldMapAreaRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_240() { return 0; }
int _E1_240() { return 2139095040; }
int _E5_239() { return 0; }
int _E4_239() { return 0; }
int _E8_239() { return 0; }
int _E7_239() { return 0; }
