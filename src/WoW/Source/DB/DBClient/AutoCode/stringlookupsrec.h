#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class StringLookupsRec final : public ClientDBRecord {
public:
    const char* m_String = nullptr;
    static const char* GetFilename();
    StringLookupsRec();
    ~StringLookupsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_207();
int _E1_207();
int _E5_206();
int _E4_206();
int _E8_206();
int _E7_206();
