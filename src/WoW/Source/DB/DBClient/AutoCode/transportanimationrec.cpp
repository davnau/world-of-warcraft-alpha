#include "WoW/Source/DB/DBClient/AutoCode/transportanimationrec.h"

#include <limits>

const char* TransportAnimationRec::GetFilename() {
    return "DBFilesClient\\TransportAnimation.dbc";
}

TransportAnimationRec::TransportAnimationRec() = default;
TransportAnimationRec::~TransportAnimationRec() = default;

unsigned char TransportAnimationRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_236() { return 0; }
int _E1_236() { return 2139095040; }
int _E5_235() { return 0; }
int _E4_235() { return 0; }
int _E8_235() { return 0; }
int _E7_235() { return 0; }
