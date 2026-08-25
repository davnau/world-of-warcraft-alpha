#include "WoW/Source/DB/DBClient/AutoCode/spellchaineffectsrec.h"

#include <limits>

const char* SpellChainEffectsRec::GetFilename() {
    return "DBFilesClient\\SpellChainEffects.dbc";
}

SpellChainEffectsRec::SpellChainEffectsRec() = default;
SpellChainEffectsRec::~SpellChainEffectsRec() = default;

unsigned char SpellChainEffectsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_237() { return 0; }
int _E1_237() { return 2139095040; }
int _E5_236() { return 0; }
int _E4_236() { return 0; }
int _E8_236() { return 0; }
int _E7_236() { return 0; }
