#include "WoW/Source/DB/DBClient/AutoCode/itemvisualeffectsrec.h"

#include <limits>

const char* ItemVisualEffectsRec::GetFilename() {
    return "DBFilesClient\\ItemVisualEffects.dbc";
}

ItemVisualEffectsRec::ItemVisualEffectsRec() = default;
ItemVisualEffectsRec::~ItemVisualEffectsRec() = default;

unsigned char ItemVisualEffectsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_249() { return 0; }
int _E1_249() { return 2139095040; }
int _E5_248() { return 0; }
int _E4_248() { return 0; }
int _E8_248() { return 0; }
int _E7_248() { return 0; }
