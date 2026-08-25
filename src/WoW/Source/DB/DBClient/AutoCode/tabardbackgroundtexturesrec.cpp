#include "WoW/Source/DB/DBClient/AutoCode/tabardbackgroundtexturesrec.h"

#include <limits>

const char* TabardBackgroundTexturesRec::GetFilename() {
    return "DBFilesClient\\TabardBackgroundTextures.dbc";
}

TabardBackgroundTexturesRec::TabardBackgroundTexturesRec() = default;
TabardBackgroundTexturesRec::~TabardBackgroundTexturesRec() = default;

unsigned char TabardBackgroundTexturesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_161() { return 0; }
int _E1_161() { return 2139095040; }
int _E5_160() { return 0; }
int _E4_160() { return 0; }
int _E8_160() { return 0; }
int _E7_160() { return 0; }
