#include "WoW/Source/DB/DBClient/AutoCode/characterfacialhairstylesrec.h"

#include <limits>

const char* CharacterFacialHairStylesRec::GetFilename() {
    return "DBFilesClient\\CharacterFacialHairStyles.dbc";
}

CharacterFacialHairStylesRec::CharacterFacialHairStylesRec() = default;
CharacterFacialHairStylesRec::~CharacterFacialHairStylesRec() = default;

unsigned char CharacterFacialHairStylesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_202() { return 0; }
int _E1_202() { return 2139095040; }
int _E5_201() { return 0; }
int _E4_201() { return 0; }
int _E8_201() { return 0; }
int _E7_201() { return 0; }
