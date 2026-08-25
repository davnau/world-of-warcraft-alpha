#include "WoW/Source/DB/DBClient/AutoCode/terraintyperec.h"

#include <limits>

const char* TerrainTypeRec::GetFilename() {
    return "DBFilesClient\\TerrainType.dbc";
}

TerrainTypeRec::TerrainTypeRec() = default;
TerrainTypeRec::~TerrainTypeRec() = default;

unsigned char TerrainTypeRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_186() { return 0; }
int _E1_186() { return 2139095040; }
int _E5_185() { return 0; }
int _E4_185() { return 0; }
int _E8_185() { return 0; }
int _E7_185() { return 0; }
