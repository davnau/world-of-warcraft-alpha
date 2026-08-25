#include "WoW/Source/DB/DBClient/AutoCode/taxipathnoderec.h"

#include <limits>

const char* TaxiPathNodeRec::GetFilename() {
    return "DBFilesClient\\TaxiPathNode.dbc";
}

TaxiPathNodeRec::TaxiPathNodeRec() = default;
TaxiPathNodeRec::~TaxiPathNodeRec() = default;

unsigned char TaxiPathNodeRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_216() { return 0; }
int _E1_216() { return 2139095040; }
int _E5_215() { return 0; }
int _E4_215() { return 0; }
int _E8_215() { return 0; }
int _E7_215() { return 0; }
