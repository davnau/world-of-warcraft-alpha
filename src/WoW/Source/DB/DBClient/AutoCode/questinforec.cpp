#include "WoW/Source/DB/DBClient/AutoCode/questinforec.h"

#include <limits>

const char* QuestInfoRec::GetFilename() {
    return "DBFilesClient\\QuestInfo.dbc";
}

QuestInfoRec::QuestInfoRec() = default;
QuestInfoRec::~QuestInfoRec() = default;

unsigned char QuestInfoRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_245() { return 0; }
int _E1_245() { return 2139095040; }
int _E5_244() { return 0; }
int _E4_244() { return 0; }
int _E8_244() { return 0; }
int _E7_244() { return 0; }
