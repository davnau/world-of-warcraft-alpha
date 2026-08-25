#include "WoW/Source/DB/DBClient/AutoCode/tabardemblemtexturesrec.h"

#include <limits>

const char* TabardEmblemTexturesRec::GetFilename() {
    return "DBFilesClient\\TabardEmblemTextures.dbc";
}

TabardEmblemTexturesRec::TabardEmblemTexturesRec() = default;
TabardEmblemTexturesRec::~TabardEmblemTexturesRec() = default;

unsigned char TabardEmblemTexturesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_162() { return 0; }
int _E1_162() { return 2139095040; }
int _E5_161() { return 0; }
int _E4_161() { return 0; }
int _E8_161() { return 0; }
int _E7_161() { return 0; }
