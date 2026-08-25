#include "WoW/Source/DB/DBClient/AutoCode/wmoareatablerec.h"

#include <limits>

const char* WMOAreaTableRec::GetFilename() {
    return "DBFilesClient\\WMOAreaTable.dbc";
}

WMOAreaTableRec::WMOAreaTableRec() = default;
WMOAreaTableRec::~WMOAreaTableRec() = default;

unsigned char WMOAreaTableRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_234() { return 0; }
int _E1_234() { return 2139095040; }
int _E5_233() { return 0; }
int _E4_233() { return 0; }
int _E8_233() { return 0; }
int _E7_233() { return 0; }
