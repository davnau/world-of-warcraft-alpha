#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class FactionTemplateRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    FactionTemplateRec();
    ~FactionTemplateRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_159();
int _E1_159();
int _E5_158();
int _E4_158();
int _E8_158();
int _E7_158();
