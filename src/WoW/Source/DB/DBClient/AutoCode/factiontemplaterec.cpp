#include "WoW/Source/DB/DBClient/AutoCode/factiontemplaterec.h"

#include <limits>

const char* FactionTemplateRec::GetFilename() {
    return "DBFilesClient\\FactionTemplate.dbc";
}

FactionTemplateRec::FactionTemplateRec() = default;
FactionTemplateRec::~FactionTemplateRec() = default;

unsigned char FactionTemplateRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_159() { return 0; }
int _E1_159() { return 2139095040; }
int _E5_158() { return 0; }
int _E4_158() { return 0; }
int _E8_158() { return 0; }
int _E7_158() { return 0; }
