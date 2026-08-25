#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CinematicCameraRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CinematicCameraRec();
    ~CinematicCameraRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_155();
int _E1_155();
int _E5_154();
int _E4_154();
int _E8_154();
int _E7_154();
