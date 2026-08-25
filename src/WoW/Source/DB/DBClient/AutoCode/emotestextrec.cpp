#include "WoW/Source/DB/DBClient/AutoCode/emotestextrec.h"

#include <limits>

const char* EmotesTextRec::GetFilename() {
    return "DBFilesClient\\EmotesText.dbc";
}

EmotesTextRec::EmotesTextRec() = default;
EmotesTextRec::~EmotesTextRec() = default;

unsigned char EmotesTextRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_229() { return 0; }
int _E1_229() { return 2139095040; }
int _E5_228() { return 0; }
int _E4_228() { return 0; }
int _E8_228() { return 0; }
int _E7_228() { return 0; }
