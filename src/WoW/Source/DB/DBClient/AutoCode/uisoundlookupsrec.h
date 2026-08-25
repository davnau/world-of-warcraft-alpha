#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class UISoundLookupsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    UISoundLookupsRec();
    ~UISoundLookupsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_194();
int _E1_194();
int _E5_193();
int _E4_193();
int _E8_193();
int _E7_193();
