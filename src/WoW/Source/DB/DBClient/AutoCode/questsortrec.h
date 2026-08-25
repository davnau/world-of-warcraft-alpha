#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class QuestSortRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    QuestSortRec();
    ~QuestSortRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_244();
int _E1_244();
int _E5_243();
int _E4_243();
int _E8_243();
int _E7_243();
