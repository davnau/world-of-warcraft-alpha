#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class LanguageWordsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    LanguageWordsRec();
    ~LanguageWordsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_196();
int _E1_196();
int _E5_195();
int _E4_195();
int _E8_195();
int _E7_195();
