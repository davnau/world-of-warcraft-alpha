#include "WoW/Source/DB/DBClient/AutoCode/helmetgeosetvisdatarec.h"

#include <limits>

const char* HelmetGeosetVisDataRec::GetFilename() {
    return "DBFilesClient\\HelmetGeosetVisData.dbc";
}

HelmetGeosetVisDataRec::HelmetGeosetVisDataRec() = default;
HelmetGeosetVisDataRec::~HelmetGeosetVisDataRec() = default;

unsigned char HelmetGeosetVisDataRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_206() { return 0; }
int _E1_206() { return 2139095040; }
int _E5_205() { return 0; }
int _E4_205() { return 0; }
int _E8_205() { return 0; }
int _E7_205() { return 0; }
