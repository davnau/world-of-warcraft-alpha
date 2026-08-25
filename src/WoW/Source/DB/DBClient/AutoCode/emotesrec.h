#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class EmotesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    EmotesRec();
    ~EmotesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_228();
int _E1_228();
int _E5_227();
int _E4_227();
int _E8_227();
int _E7_227();
