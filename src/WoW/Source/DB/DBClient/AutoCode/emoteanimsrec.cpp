#include "WoW/Source/DB/DBClient/AutoCode/emoteanimsrec.h"

#include <limits>

const char* EmoteAnimsRec::GetFilename() {
    return "DBFilesClient\\EmoteAnims.dbc";
}

EmoteAnimsRec::EmoteAnimsRec() = default;
EmoteAnimsRec::~EmoteAnimsRec() = default;

unsigned char EmoteAnimsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_227() { return 0; }
int _E1_227() { return 2139095040; }
int _E5_226() { return 0; }
int _E4_226() { return 0; }
int _E8_226() { return 0; }
int _E7_226() { return 0; }
