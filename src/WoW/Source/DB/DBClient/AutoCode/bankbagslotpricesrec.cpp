#include "WoW/Source/DB/DBClient/AutoCode/bankbagslotpricesrec.h"

#include <limits>

const char* BankBagSlotPricesRec::GetFilename() {
    return "DBFilesClient\\BankBagSlotPrices.dbc";
}

BankBagSlotPricesRec::BankBagSlotPricesRec() = default;
BankBagSlotPricesRec::~BankBagSlotPricesRec() = default;

unsigned char BankBagSlotPricesRec::Read(SFile* file, const char* stringBuffer) {
    return ReadRecord(file, stringBuffer);
}

int _E2_235() { return 0; }
int _E1_235() { return 2139095040; }
int _E5_234() { return 0; }
int _E4_234() { return 0; }
int _E8_234() { return 0; }
int _E7_234() { return 0; }
