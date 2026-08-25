#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class GameObjectDisplayInfoRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    GameObjectDisplayInfoRec();
    ~GameObjectDisplayInfoRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_208();
int _E1_208();
int _E5_207();
int _E4_207();
int _E8_207();
int _E7_207();
