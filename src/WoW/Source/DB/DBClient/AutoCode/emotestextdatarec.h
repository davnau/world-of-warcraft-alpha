#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class EmotesTextDataRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    EmotesTextDataRec();
    ~EmotesTextDataRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_230();
int _E1_230();
int _E5_229();
int _E4_229();
int _E8_229();
int _E7_229();
