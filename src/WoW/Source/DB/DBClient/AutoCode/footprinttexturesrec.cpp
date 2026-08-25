#include "WoW/Source/DB/DBClient/AutoCode/footprinttexturesrec.h"

#include <limits>

const char* FootprintTexturesRec::GetFilename() {
    return "DBFilesClient\\FootprintTextures.dbc";
}

FootprintTexturesRec::FootprintTexturesRec() = default;
FootprintTexturesRec::~FootprintTexturesRec() = default;

unsigned char FootprintTexturesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_201() { return 0; }
int _E1_201() { return 2139095040; }
int _E5_200() { return 0; }
int _E4_200() { return 0; }
int _E8_200() { return 0; }
int _E7_200() { return 0; }
