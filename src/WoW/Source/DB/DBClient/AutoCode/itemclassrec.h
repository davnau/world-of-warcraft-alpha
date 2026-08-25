#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class ItemClassRec final : public ClientDBRecord {
public:
    int m_flags = 0;
    static const char* GetFilename();
    ItemClassRec();
    ~ItemClassRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_220();
int _E1_220();
int _E5_219();
int _E4_219();
int _E8_219();
int _E7_219();
