#include "WoW/Source/DB/DBClient/AutoCode/footstepterrainlookuprec.h"

#include <limits>

const char* FootstepTerrainLookupRec::GetFilename() {
    return "DBFilesClient\\FootstepTerrainLookup.dbc";
}

FootstepTerrainLookupRec::FootstepTerrainLookupRec() = default;
FootstepTerrainLookupRec::~FootstepTerrainLookupRec() = default;

unsigned char FootstepTerrainLookupRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_211() { return 0; }
int _E1_211() { return 2139095040; }
int _E5_210() { return 0; }
int _E4_210() { return 0; }
int _E8_210() { return 0; }
int _E7_210() { return 0; }
