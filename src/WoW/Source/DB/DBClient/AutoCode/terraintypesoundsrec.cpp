#include "WoW/Source/DB/DBClient/AutoCode/terraintypesoundsrec.h"

#include <limits>

const char* TerrainTypeSoundsRec::GetFilename() {
    return "DBFilesClient\\TerrainTypeSounds.dbc";
}

TerrainTypeSoundsRec::TerrainTypeSoundsRec() = default;
TerrainTypeSoundsRec::~TerrainTypeSoundsRec() = default;

unsigned char TerrainTypeSoundsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_187() { return 0; }
int _E1_187() { return 2139095040; }
int _E5_186() { return 0; }
int _E4_186() { return 0; }
int _E8_186() { return 0; }
int _E7_186() { return 0; }
