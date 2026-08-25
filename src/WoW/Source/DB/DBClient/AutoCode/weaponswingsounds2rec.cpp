#include "WoW/Source/DB/DBClient/AutoCode/weaponswingsounds2rec.h"

#include <limits>

const char* WeaponSwingSounds2Rec::GetFilename() {
    return "DBFilesClient\\WeaponSwingSounds2.dbc";
}

WeaponSwingSounds2Rec::WeaponSwingSounds2Rec() = default;
WeaponSwingSounds2Rec::~WeaponSwingSounds2Rec() = default;

unsigned char WeaponSwingSounds2Rec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_193() { return 0; }
int _E1_193() { return 2139095040; }
int _E5_192() { return 0; }
int _E4_192() { return 0; }
int _E8_192() { return 0; }
int _E7_192() { return 0; }
