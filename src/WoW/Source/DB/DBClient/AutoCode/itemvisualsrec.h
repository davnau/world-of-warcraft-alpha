#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ItemVisualsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    ItemVisualsRec();
    ~ItemVisualsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_248();
int _E1_248();
int _E5_247();
int _E4_247();
int _E8_247();
int _E7_247();
