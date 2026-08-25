#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class LanguagesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    LanguagesRec();
    ~LanguagesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_195();
int _E1_195();
int _E5_194();
int _E4_194();
int _E8_194();
int _E7_194();
