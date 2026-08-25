#include "WoW/Source/DB/DBClient/AutoCode/areatriggerrec.h"

#include <limits>

const char* AreaTriggerRec::GetFilename() {
    return "DBFilesClient\\AreaTrigger.dbc";
}

AreaTriggerRec::AreaTriggerRec() = default;
AreaTriggerRec::~AreaTriggerRec() = default;

unsigned char AreaTriggerRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_184() { return 0; }
int _E1_184() { return 2139095040; }
int _E5_183() { return 0; }
int _E4_183() { return 0; }
int _E8_183() { return 0; }
int _E7_183() { return 0; }
