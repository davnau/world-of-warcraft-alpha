#include "WoW/Source/DB/DBClient/AutoCode/weaponimpactsoundsrec.h"

#include <limits>

const char* WeaponImpactSoundsRec::GetFilename() {
    return "DBFilesClient\\WeaponImpactSounds.dbc";
}

WeaponImpactSoundsRec::WeaponImpactSoundsRec() = default;
WeaponImpactSoundsRec::~WeaponImpactSoundsRec() = default;

unsigned char WeaponImpactSoundsRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_178() { return 0; }
int _E1_178() { return 2139095040; }
int _E5_177() { return 0; }
int _E4_177() { return 0; }
int _E8_177() { return 0; }
int _E7_177() { return 0; }
