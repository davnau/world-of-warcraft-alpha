#include "WoW/Source/DB/DBClient/AutoCode/cinematiccamerarec.h"

#include <limits>

const char* CinematicCameraRec::GetFilename() {
    return "DBFilesClient\\CinematicCamera.dbc";
}

CinematicCameraRec::CinematicCameraRec() = default;
CinematicCameraRec::~CinematicCameraRec() = default;

unsigned char CinematicCameraRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_155() { return 0; }
int _E1_155() { return 2139095040; }
int _E5_154() { return 0; }
int _E4_154() { return 0; }
int _E8_154() { return 0; }
int _E7_154() { return 0; }
