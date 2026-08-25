#include "WoW/Source/DB/DBClient/AutoCode/creaturesounddatarec.h"

#include <limits>

const char* CreatureSoundDataRec::GetFilename() {
    return "DBFilesClient\\CreatureSoundData.dbc";
}

CreatureSoundDataRec::CreatureSoundDataRec() = default;
CreatureSoundDataRec::~CreatureSoundDataRec() = default;

unsigned char CreatureSoundDataRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_150() { return 0; }
int _E1_150() { return 2139095040; }
int _E5_149() { return 0; }
int _E4_149() { return 0; }
int _E8_149() { return 0; }
int _E7_149() { return 0; }
