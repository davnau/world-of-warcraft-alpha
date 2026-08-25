#include "WoW/Source/DB/DBClient/AutoCode/charactercreatecamerasrec.h"

#include <limits>

const char* CharacterCreateCamerasRec::GetFilename() {
    return "DBFilesClient\\CharacterCreateCameras.dbc";
}

CharacterCreateCamerasRec::CharacterCreateCamerasRec() = default;
CharacterCreateCamerasRec::~CharacterCreateCamerasRec() = default;

unsigned char CharacterCreateCamerasRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_203() { return 0; }
int _E1_203() { return 2139095040; }
int _E5_202() { return 0; }
int _E4_202() { return 0; }
int _E8_202() { return 0; }
int _E7_202() { return 0; }
