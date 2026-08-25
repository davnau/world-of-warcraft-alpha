#include "WoW/Source/DB/DBClient/AutoCode/taxipathrec.h"

#include <limits>

const char* TaxiPathRec::GetFilename() {
    return "DBFilesClient\\TaxiPath.dbc";
}

TaxiPathRec::TaxiPathRec() = default;
TaxiPathRec::~TaxiPathRec() = default;

unsigned char TaxiPathRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_215() { return 0; }
int _E1_215() { return 2139095040; }
int _E5_214() { return 0; }
int _E4_214() { return 0; }
int _E8_214() { return 0; }
int _E7_214() { return 0; }
