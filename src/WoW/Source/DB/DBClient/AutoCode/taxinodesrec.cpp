#include "WoW/Source/DB/DBClient/AutoCode/taxinodesrec.h"

#include <limits>

const char* TaxiNodesRec::GetFilename() {
    return "DBFilesClient\\TaxiNodes.dbc";
}

TaxiNodesRec::TaxiNodesRec() = default;
TaxiNodesRec::~TaxiNodesRec() = default;

unsigned char TaxiNodesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_213() { return 0; }
int _E1_213() { return 2139095040; }
int _E5_212() { return 0; }
int _E4_212() { return 0; }
int _E8_212() { return 0; }
int _E7_212() { return 0; }
