#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class NPCSoundsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    NPCSoundsRec();
    ~NPCSoundsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_221();
int _E1_221();
int _E5_220();
int _E4_220();
int _E8_220();
int _E7_220();
