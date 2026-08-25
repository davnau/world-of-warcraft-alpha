#include "WoW/Source/DB/DBClient/AutoCode/itemgroupsoundsrec.h"

#include <limits>

const char* ItemGroupSoundsRec::GetFilename() {
    return "DBFilesClient\\ItemGroupSounds.dbc";
}

ItemGroupSoundsRec::ItemGroupSoundsRec() = default;
ItemGroupSoundsRec::~ItemGroupSoundsRec() = default;

unsigned char ItemGroupSoundsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_210() { return 0; }
int _E1_210() { return 2139095040; }
int _E5_209() { return 0; }
int _E4_209() { return 0; }
int _E8_209() { return 0; }
int _E7_209() { return 0; }
