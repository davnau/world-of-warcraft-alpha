#include "WoW/Source/DB/DBClient/AutoCode/videohardwarerec.h"

#include <limits>

const char* VideoHardwareRec::GetFilename() {
    return "DBFilesClient\\VideoHardware.dbc";
}

VideoHardwareRec::VideoHardwareRec() = default;
VideoHardwareRec::~VideoHardwareRec() = default;

unsigned char VideoHardwareRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_337() { return 0; }
int _E1_337() { return 2139095040; }
int _E5_312() { return 0; }
int _E4_312() { return 0; }
int _E8_302() { return 0; }
int _E7_312() { return 0; }
