#include "WoW/Source/DB/DBClient/AutoCode/groundeffectdoodadrec.h"

#include <limits>

const char* GroundEffectDoodadRec::GetFilename() {
    return "DBFilesClient\\GroundEffectDoodad.dbc";
}

GroundEffectDoodadRec::GroundEffectDoodadRec() = default;
GroundEffectDoodadRec::~GroundEffectDoodadRec() = default;

unsigned char GroundEffectDoodadRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_144() { return 0; }
int _E1_144() { return 2139095040; }
int _E5_143() { return 0; }
int _E4_143() { return 0; }
int _E8_143() { return 0; }
int _E7_143() { return 0; }
