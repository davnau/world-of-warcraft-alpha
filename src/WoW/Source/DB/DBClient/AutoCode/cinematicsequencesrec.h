#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class CinematicSequencesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    CinematicSequencesRec();
    ~CinematicSequencesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_156();
int _E1_156();
int _E5_155();
int _E4_155();
int _E8_155();
int _E7_155();
