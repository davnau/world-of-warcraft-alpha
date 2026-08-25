#include "WoW/Source/DB/DBClient/AutoCode/groundeffecttexturerec.h"

#include <limits>

const char* GroundEffectTextureRec::GetFilename() {
    return "DBFilesClient\\GroundEffectTexture.dbc";
}

GroundEffectTextureRec::GroundEffectTextureRec() = default;
GroundEffectTextureRec::~GroundEffectTextureRec() = default;

unsigned char GroundEffectTextureRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_143() { return 0; }
int _E1_143() { return 2139095040; }
int _E5_142() { return 0; }
int _E4_142() { return 0; }
int _E8_142() { return 0; }
int _E7_142() { return 0; }
