#include "WoW/Source/DB/DBClient/AutoCode/namesprofanityrec.h"

#include <limits>

const char* NamesProfanityRec::GetFilename() {
    return "DBFilesClient\\NamesProfanity.dbc";
}

NamesProfanityRec::NamesProfanityRec() = default;
NamesProfanityRec::~NamesProfanityRec() = default;

unsigned char NamesProfanityRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_246() { return 0; }
int _E1_246() { return 2139095040; }
int _E5_245() { return 0; }
int _E4_245() { return 0; }
int _E8_245() { return 0; }
int _E7_245() { return 0; }
