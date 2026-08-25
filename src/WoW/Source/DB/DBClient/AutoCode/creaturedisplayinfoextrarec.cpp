#include "WoW/Source/DB/DBClient/AutoCode/creaturedisplayinfoextrarec.h"

#include <limits>

const char* CreatureDisplayInfoExtraRec::GetFilename() {
    return "DBFilesClient\\CreatureDisplayInfoExtra.dbc";
}

CreatureDisplayInfoExtraRec::CreatureDisplayInfoExtraRec() = default;
CreatureDisplayInfoExtraRec::~CreatureDisplayInfoExtraRec() = default;

unsigned char CreatureDisplayInfoExtraRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_147() { return 0; }
int _E1_147() { return 2139095040; }
int _E5_146() { return 0; }
int _E4_146() { return 0; }
int _E8_146() { return 0; }
int _E7_146() { return 0; }
