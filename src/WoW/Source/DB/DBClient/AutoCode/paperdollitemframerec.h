#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class PaperDollItemFrameRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    PaperDollItemFrameRec();
    ~PaperDollItemFrameRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_163();
int _E1_163();
int _E5_162();
int _E4_162();
int _E8_162();
int _E7_162();
