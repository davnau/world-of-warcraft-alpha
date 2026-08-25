#include "WoW/Source/DB/DBClient/AutoCode/chrproficiencyrec.h"

#include <limits>

const char* ChrProficiencyRec::GetFilename() {
    return "DBFilesClient\\ChrProficiency.dbc";
}

ChrProficiencyRec::ChrProficiencyRec() = default;
ChrProficiencyRec::~ChrProficiencyRec() = default;

unsigned char ChrProficiencyRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_176() { return 0; }
int _E1_176() { return 2139095040; }
int _E5_175() { return 0; }
int _E4_175() { return 0; }
int _E8_175() { return 0; }
int _E7_175() { return 0; }
