#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class SoundSamplePreferencesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    SoundSamplePreferencesRec();
    ~SoundSamplePreferencesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_218();
int _E1_218();
int _E5_217();
int _E4_217();
int _E8_217();
int _E7_217();
