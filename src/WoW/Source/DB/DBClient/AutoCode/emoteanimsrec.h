#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class EmoteAnimsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    EmoteAnimsRec();
    ~EmoteAnimsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_227();
int _E1_227();
int _E5_226();
int _E4_226();
int _E8_226();
int _E7_226();
