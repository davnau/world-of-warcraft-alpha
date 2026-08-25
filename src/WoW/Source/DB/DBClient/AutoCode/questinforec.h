#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class QuestInfoRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    QuestInfoRec();
    ~QuestInfoRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_245();
int _E1_245();
int _E5_244();
int _E4_244();
int _E8_244();
int _E7_244();
