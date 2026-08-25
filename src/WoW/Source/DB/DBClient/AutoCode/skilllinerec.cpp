#include "WoW/Source/DB/DBClient/AutoCode/skilllinerec.h"

#include <limits>

const char* SkillLineRec::GetFilename() {
    return "DBFilesClient\\SkillLine.dbc";
}

SkillLineRec::SkillLineRec() = default;
SkillLineRec::~SkillLineRec() = default;

unsigned char SkillLineRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_166() { return 0; }
int _E1_166() { return 2139095040; }
int _E5_165() { return 0; }
int _E4_165() { return 0; }
int _E8_165() { return 0; }
int _E7_165() { return 0; }
