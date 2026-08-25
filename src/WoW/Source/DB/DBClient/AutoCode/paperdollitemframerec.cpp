#include "WoW/Source/DB/DBClient/AutoCode/paperdollitemframerec.h"

#include <limits>

const char* PaperDollItemFrameRec::GetFilename() {
    return "DBFilesClient\\PaperDollItemFrame.dbc";
}

PaperDollItemFrameRec::PaperDollItemFrameRec() = default;
PaperDollItemFrameRec::~PaperDollItemFrameRec() = default;

unsigned char PaperDollItemFrameRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_163() { return 0; }
int _E1_163() { return 2139095040; }
int _E5_162() { return 0; }
int _E4_162() { return 0; }
int _E8_162() { return 0; }
int _E7_162() { return 0; }
