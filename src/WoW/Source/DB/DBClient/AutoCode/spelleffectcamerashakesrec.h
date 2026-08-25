#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SpellEffectCameraShakesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SpellEffectCameraShakesRec();
    ~SpellEffectCameraShakesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_205();
int _E1_205();
int _E5_204();
int _E4_204();
int _E8_204();
int _E7_204();
