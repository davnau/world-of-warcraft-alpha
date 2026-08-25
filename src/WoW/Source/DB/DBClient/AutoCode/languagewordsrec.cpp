#include "WoW/Source/DB/DBClient/AutoCode/languagewordsrec.h"

#include <limits>

const char* LanguageWordsRec::GetFilename() {
    return "DBFilesClient\\LanguageWords.dbc";
}

LanguageWordsRec::LanguageWordsRec() = default;
LanguageWordsRec::~LanguageWordsRec() = default;

unsigned char LanguageWordsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_196() { return 0; }
int _E1_196() { return 2139095040; }
int _E5_195() { return 0; }
int _E4_195() { return 0; }
int _E8_195() { return 0; }
int _E7_195() { return 0; }
