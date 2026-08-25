#include "WoW/Source/DB/DBClient/AutoCode/vocaluisoundsrec.h"

#include <limits>

const char* VocalUISoundsRec::GetFilename() {
    return "DBFilesClient\\VocalUISounds.dbc";
}

VocalUISoundsRec::VocalUISoundsRec() = default;
VocalUISoundsRec::~VocalUISoundsRec() = default;

unsigned char VocalUISoundsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_199() { return 0; }
int _E1_199() { return 2139095040; }
int _E5_198() { return 0; }
int _E4_198() { return 0; }
int _E8_198() { return 0; }
int _E7_198() { return 0; }
