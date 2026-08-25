#pragma once

#include "WoW/Source/DB/DBClient/dbrecord.h"

class BankBagSlotPricesRec final : public ClientDBRecord {
public:
    static const char* GetFilename();
    BankBagSlotPricesRec();
    ~BankBagSlotPricesRec() override;
    unsigned char Read(SFile* file, const char* stringBuffer);
};

int _E2_235();
int _E1_235();
int _E5_234();
int _E4_234();
int _E8_234();
int _E7_234();
