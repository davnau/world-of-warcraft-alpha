#include "WoW/Source/DB/DBClient/AutoCode/charvariationsrec.h"

#include <limits>

const char* CharVariationsRec::GetFilename() {
    return "DBFilesClient\\CharVariations.dbc";
}

CharVariationsRec::CharVariationsRec() = default;
CharVariationsRec::~CharVariationsRec() = default;

unsigned char CharVariationsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_164() { return 0; }
int _E1_164() { return 2139095040; }
int _E5_163() { return 0; }
int _E4_163() { return 0; }
int _E8_163() { return 0; }
int _E7_163() { return 0; }
