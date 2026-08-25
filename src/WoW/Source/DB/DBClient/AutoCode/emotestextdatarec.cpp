#include "WoW/Source/DB/DBClient/AutoCode/emotestextdatarec.h"

#include <limits>

const char* EmotesTextDataRec::GetFilename() {
    return "DBFilesClient\\EmotesTextData.dbc";
}

EmotesTextDataRec::EmotesTextDataRec() = default;
EmotesTextDataRec::~EmotesTextDataRec() = default;

unsigned char EmotesTextDataRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_230() { return 0; }
int _E1_230() { return 2139095040; }
int _E5_229() { return 0; }
int _E4_229() { return 0; }
int _E8_229() { return 0; }
int _E7_229() { return 0; }
