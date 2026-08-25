#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class EmotesTextRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    EmotesTextRec();
    ~EmotesTextRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_229();
int _E1_229();
int _E5_228();
int _E4_228();
int _E8_228();
int _E7_228();
