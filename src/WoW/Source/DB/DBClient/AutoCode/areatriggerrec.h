#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class AreaTriggerRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    AreaTriggerRec();
    ~AreaTriggerRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_184();
int _E1_184();
int _E5_183();
int _E4_183();
int _E8_183();
int _E7_183();
