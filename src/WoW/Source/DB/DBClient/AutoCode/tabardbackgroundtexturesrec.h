#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TabardBackgroundTexturesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    TabardBackgroundTexturesRec();
    ~TabardBackgroundTexturesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_161();
int _E1_161();
int _E5_160();
int _E4_160();
int _E8_160();
int _E7_160();
