#include "WoW/Source/DB/DBClient/AutoCode/chrracesrec.h"

#include <limits>

const char* ChrRacesRec::GetFilename() {
    return "DBFilesClient\\ChrRaces.dbc";
}

ChrRacesRec::ChrRacesRec() = default;
ChrRacesRec::~ChrRacesRec() = default;

unsigned char ChrRacesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_153() { return 0; }
int _E1_153() { return 2139095040; }
int _E5_152() { return 0; }
int _E4_152() { return 0; }
int _E8_152() { return 0; }
int _E7_152() { return 0; }
