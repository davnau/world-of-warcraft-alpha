#pragma once

#include <cstdint>

struct Currency {
    int copper{};
    int silver{};
    int gold{};
};

int CurrencyMultiplier(unsigned int denomination);
char* CurrencyAbbreviation(int denomination);
void CurrencyBreakdown(int coins, int* breakdown);
int CurrencyTotal(const int* coins);
int CurrencyTotal(const Currency* coins);
void LootInitialize();
void LootDestroy();

extern int s_currencyMultiplier[3];
extern bool s_lootInitialized;

int _E2_299();
int _E1_299();
void _E4_296();
void _E7_296();
void _E13_12();
void _E16_24();
void _E19_51();
void _E22_59();
void _E25_60();
void _E28_98();
void _E31_95();
void _E34_72();
void _E37_67();
void _E40_68();
void _E43_66();
void _E46_64();
int _E5_296();
int _E8_296();
int _E14_17();
int _E17_19();
int _E20_68();
int _E23_73();
int _E26_69();
int _E29_69();
int _E32_61();
int _E35_42();
int _E38_41();
int _E41_37();
int _E44_39();
int _E47_40();
