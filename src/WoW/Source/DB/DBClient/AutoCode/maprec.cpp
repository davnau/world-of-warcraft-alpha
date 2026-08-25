#include "WoW/Source/DB/DBClient/AutoCode/maprec.h"

#include <limits>

const char* MapRec::GetFilename() {
    return "DBFilesClient\\Map.dbc";
}

MapRec::MapRec() = default;
MapRec::~MapRec() = default;

unsigned char MapRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_200() { return 0; }
int _E1_200() { return 2139095040; }
int _E5_199() { return 0; }
int _E4_199() { return 0; }
int _E8_199() { return 0; }
int _E7_199() { return 0; }
