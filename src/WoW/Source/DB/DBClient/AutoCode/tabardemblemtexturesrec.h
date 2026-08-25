#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class TabardEmblemTexturesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    TabardEmblemTexturesRec();
    ~TabardEmblemTexturesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_162();
int _E1_162();
int _E5_161();
int _E4_161();
int _E8_161();
int _E7_161();
