#include "WoW/Source/DB/DBClient/AutoCode/languagesrec.h"

#include <limits>

const char* LanguagesRec::GetFilename() {
    return "DBFilesClient\\Languages.dbc";
}

LanguagesRec::LanguagesRec() = default;
LanguagesRec::~LanguagesRec() = default;

unsigned char LanguagesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_195() { return 0; }
int _E1_195() { return 2139095040; }
int _E5_194() { return 0; }
int _E4_194() { return 0; }
int _E8_194() { return 0; }
int _E7_194() { return 0; }
