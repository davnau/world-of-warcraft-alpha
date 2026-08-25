#include "WoW/Source/DB/DBClient/AutoCode/areapoirec.h"

#include <limits>

const char* AreaPOIRec::GetFilename() {
    return "DBFilesClient\\AreaPOI.dbc";
}

AreaPOIRec::AreaPOIRec() = default;
AreaPOIRec::~AreaPOIRec() = default;

unsigned char AreaPOIRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_188() { return 0; }
int _E1_188() { return 2139095040; }
int _E5_187() { return 0; }
int _E4_187() { return 0; }
int _E8_187() { return 0; }
int _E7_187() { return 0; }
