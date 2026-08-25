#include "WoW/Source/DB/DBClient/AutoCode/questsortrec.h"

#include <limits>

const char* QuestSortRec::GetFilename() {
    return "DBFilesClient\\QuestSort.dbc";
}

QuestSortRec::QuestSortRec() = default;
QuestSortRec::~QuestSortRec() = default;

unsigned char QuestSortRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_244() { return 0; }
int _E1_244() { return 2139095040; }
int _E5_243() { return 0; }
int _E4_243() { return 0; }
int _E8_243() { return 0; }
int _E7_243() { return 0; }
