#include "WoW/Source/DB/DBClient/AutoCode/creaturedisplayinforec.h"

#include <limits>

const char* CreatureDisplayInfoRec::GetFilename() {
    return "DBFilesClient\\CreatureDisplayInfo.dbc";
}

CreatureDisplayInfoRec::CreatureDisplayInfoRec() = default;
CreatureDisplayInfoRec::~CreatureDisplayInfoRec() = default;

unsigned char CreatureDisplayInfoRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_146() { return 0; }
int _E1_146() { return 2139095040; }
int _E5_145() { return 0; }
int _E4_145() { return 0; }
int _E8_145() { return 0; }
int _E7_145() { return 0; }
