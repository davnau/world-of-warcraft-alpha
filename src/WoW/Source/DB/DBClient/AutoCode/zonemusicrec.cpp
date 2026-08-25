#include "WoW/Source/DB/DBClient/AutoCode/zonemusicrec.h"

#include <limits>

const char* ZoneMusicRec::GetFilename() {
    return "DBFilesClient\\ZoneMusic.dbc";
}

ZoneMusicRec::ZoneMusicRec() = default;
ZoneMusicRec::~ZoneMusicRec() = default;

unsigned char ZoneMusicRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_204() { return 0; }
int _E1_204() { return 2139095040; }
int _E5_203() { return 0; }
int _E4_203() { return 0; }
int _E8_203() { return 0; }
int _E7_203() { return 0; }
