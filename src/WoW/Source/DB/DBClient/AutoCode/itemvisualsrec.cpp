#include "WoW/Source/DB/DBClient/AutoCode/itemvisualsrec.h"

#include <limits>

const char* ItemVisualsRec::GetFilename() {
    return "DBFilesClient\\ItemVisuals.dbc";
}

ItemVisualsRec::ItemVisualsRec() = default;
ItemVisualsRec::~ItemVisualsRec() = default;

unsigned char ItemVisualsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_248() { return 0; }
int _E1_248() { return 2139095040; }
int _E5_247() { return 0; }
int _E4_247() { return 0; }
int _E8_247() { return 0; }
int _E7_247() { return 0; }
