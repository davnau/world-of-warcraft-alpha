#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class VideoHardwareRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    VideoHardwareRec();
    ~VideoHardwareRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_337();
int _E1_337();
int _E5_312();
int _E4_312();
int _E8_302();
int _E7_312();
