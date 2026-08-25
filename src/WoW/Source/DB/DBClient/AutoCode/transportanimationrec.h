#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TransportAnimationRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    TransportAnimationRec();
    ~TransportAnimationRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_236();
int _E1_236();
int _E5_235();
int _E4_235();
int _E8_235();
int _E7_235();
