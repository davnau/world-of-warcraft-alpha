#include "WoW/Source/DB/DBClient/AutoCode/gameobjectdisplayinforec.h"

#include <limits>

const char* GameObjectDisplayInfoRec::GetFilename() {
    return "DBFilesClient\\GameObjectDisplayInfo.dbc";
}

GameObjectDisplayInfoRec::GameObjectDisplayInfoRec() = default;
GameObjectDisplayInfoRec::~GameObjectDisplayInfoRec() = default;

unsigned char GameObjectDisplayInfoRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_208() { return 0; }
int _E1_208() { return 2139095040; }
int _E5_207() { return 0; }
int _E4_207() { return 0; }
int _E8_207() { return 0; }
int _E7_207() { return 0; }
