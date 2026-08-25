#include "WoW/Source/DB/DBClient/AutoCode/chartexturevariationsv2rec.h"

#include <limits>

const char* CharTextureVariationsV2Rec::GetFilename() {
    return "DBFilesClient\\CharTextureVariationsV2.dbc";
}

CharTextureVariationsV2Rec::CharTextureVariationsV2Rec() = default;
CharTextureVariationsV2Rec::~CharTextureVariationsV2Rec() = default;

unsigned char CharTextureVariationsV2Rec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_238() { return 0; }
int _E1_238() { return 2139095040; }
int _E5_237() { return 0; }
int _E4_237() { return 0; }
int _E8_237() { return 0; }
int _E7_237() { return 0; }
