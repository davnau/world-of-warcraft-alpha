#include "WoW/Source/DB/DBClient/AutoCode/skilllineabilityrec.h"

#include <limits>

const char* SkillLineAbilityRec::GetFilename() {
    return "DBFilesClient\\SkillLineAbility.dbc";
}

SkillLineAbilityRec::SkillLineAbilityRec() = default;
SkillLineAbilityRec::~SkillLineAbilityRec() = default;

unsigned char SkillLineAbilityRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_167() { return 0; }
int _E1_167() { return 2139095040; }
int _E5_166() { return 0; }
int _E4_166() { return 0; }
int _E8_166() { return 0; }
int _E7_166() { return 0; }
