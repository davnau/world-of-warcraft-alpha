#include "WoW/Source/DB/DBClient/AutoCode/itemdisplayinforec.h"

#include <limits>

const char* ItemDisplayInfoRec::GetFilename() {
    return "DBFilesClient\\ItemDisplayInfo.dbc";
}

ItemDisplayInfoRec::ItemDisplayInfoRec() = default;
ItemDisplayInfoRec::~ItemDisplayInfoRec() = default;

unsigned char ItemDisplayInfoRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_160() { return 0; }
int _E1_160() { return 2139095040; }
int _E5_159() { return 0; }
int _E4_159() { return 0; }
int _E8_159() { return 0; }
int _E7_159() { return 0; }
