#include "WoW/Source/DB/DBClient/AutoCode/charbaseinforec.h"

#include <limits>

const char* CharBaseInfoRec::GetFilename() {
    return "DBFilesClient\\CharBaseInfo.dbc";
}

CharBaseInfoRec::CharBaseInfoRec() = default;
CharBaseInfoRec::~CharBaseInfoRec() = default;

unsigned char CharBaseInfoRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_175() { return 0; }
int _E1_175() { return 2139095040; }
int _E5_174() { return 0; }
int _E4_174() { return 0; }
int _E8_174() { return 0; }
int _E7_174() { return 0; }
