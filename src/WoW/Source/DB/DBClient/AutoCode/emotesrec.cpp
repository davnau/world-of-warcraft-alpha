#include "WoW/Source/DB/DBClient/AutoCode/emotesrec.h"

#include <limits>

const char* EmotesRec::GetFilename() {
    return "DBFilesClient\\Emotes.dbc";
}

EmotesRec::EmotesRec() = default;
EmotesRec::~EmotesRec() = default;

unsigned char EmotesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_228() { return 0; }
int _E1_228() { return 2139095040; }
int _E5_227() { return 0; }
int _E4_227() { return 0; }
int _E8_227() { return 0; }
int _E7_227() { return 0; }
