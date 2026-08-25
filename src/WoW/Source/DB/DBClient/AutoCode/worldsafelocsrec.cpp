#include "WoW/Source/DB/DBClient/AutoCode/worldsafelocsrec.h"

#include <limits>

const char* WorldSafeLocsRec::GetFilename() {
    return "DBFilesClient\\WorldSafeLocs.dbc";
}

WorldSafeLocsRec::WorldSafeLocsRec() = default;
WorldSafeLocsRec::~WorldSafeLocsRec() = default;

unsigned char WorldSafeLocsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_243() { return 0; }
int _E1_243() { return 2139095040; }
int _E5_242() { return 0; }
int _E4_242() { return 0; }
int _E8_242() { return 0; }
int _E7_242() { return 0; }
