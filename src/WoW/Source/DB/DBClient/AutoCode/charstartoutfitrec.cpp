#include "WoW/Source/DB/DBClient/AutoCode/charstartoutfitrec.h"

#include <limits>

const char* CharStartOutfitRec::GetFilename() {
    return "DBFilesClient\\CharStartOutfit.dbc";
}

CharStartOutfitRec::CharStartOutfitRec() = default;
CharStartOutfitRec::~CharStartOutfitRec() = default;

unsigned char CharStartOutfitRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_152() { return 0; }
int _E1_152() { return 2139095040; }
int _E5_151() { return 0; }
int _E4_151() { return 0; }
int _E8_151() { return 0; }
int _E7_151() { return 0; }
