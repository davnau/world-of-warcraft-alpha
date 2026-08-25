#include "WoW/Source/DB/DBClient/AutoCode/spelleffectcamerashakesrec.h"

#include <limits>

const char* SpellEffectCameraShakesRec::GetFilename() {
    return "DBFilesClient\\SpellEffectCameraShakes.dbc";
}

SpellEffectCameraShakesRec::SpellEffectCameraShakesRec() = default;
SpellEffectCameraShakesRec::~SpellEffectCameraShakesRec() = default;

unsigned char SpellEffectCameraShakesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_205() { return 0; }
int _E1_205() { return 2139095040; }
int _E5_204() { return 0; }
int _E4_204() { return 0; }
int _E8_204() { return 0; }
int _E7_204() { return 0; }
