#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class VocalUISoundsRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    VocalUISoundsRec();
    ~VocalUISoundsRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_199();
int _E1_199();
int _E5_198();
int _E4_198();
int _E8_198();
int _E7_198();
