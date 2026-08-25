#include "WoW/Source/DB/DBClient/AutoCode/charhairgeosetsrec.h"

#include <limits>

const char* CharHairGeosetsRec::GetFilename() {
    return "DBFilesClient\\CharHairGeosets.dbc";
}

CharHairGeosetsRec::CharHairGeosetsRec() = default;
CharHairGeosetsRec::~CharHairGeosetsRec() = default;

unsigned char CharHairGeosetsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_239() { return 0; }
int _E1_239() { return 2139095040; }
int _E5_238() { return 0; }
int _E4_238() { return 0; }
int _E8_238() { return 0; }
int _E7_238() { return 0; }
