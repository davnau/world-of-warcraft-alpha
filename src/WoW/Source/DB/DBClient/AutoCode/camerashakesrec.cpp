#include "WoW/Source/DB/DBClient/AutoCode/camerashakesrec.h"

#include <limits>

const char* CameraShakesRec::GetFilename() {
    return "DBFilesClient\\CameraShakes.dbc";
}

CameraShakesRec::CameraShakesRec() = default;
CameraShakesRec::~CameraShakesRec() = default;

unsigned char CameraShakesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_145() { return 0; }
int _E1_145() { return 2139095040; }
int _E5_144() { return 0; }
int _E4_144() { return 0; }
int _E8_144() { return 0; }
int _E7_144() { return 0; }
