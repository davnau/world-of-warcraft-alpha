#pragma once
#include "WoW/Source/DB/DBClient/dbrecord.h"
class AreaMIDIAmbiencesRec : public ClientDBRecord {
public:
    static const char* GetFilename();
    AreaMIDIAmbiencesRec();
    ~AreaMIDIAmbiencesRec();
    unsigned char Read(SFile*, const char*);
};
int _E2_222();
int _E1_222();
int _E5_221();
int _E4_221();
int _E8_221();
int _E7_221();
