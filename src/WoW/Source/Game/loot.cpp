#include "WoW/Source/Game/loot.h"

int s_currencyMultiplier[3] = {1, 100, 10000};
bool s_lootInitialized = false;

double TWO_PI_294 = 0.0;
double OO_TWO_PI_295 = 0.0;
double MAX_DUEL_DISTANCE_SQUARED_100 = 0.0;
double MAX_TRADE_DISTANCE_SQUARED_100 = 0.0;
double MAX_SHOP_DISTANCE_SQUARED_100 = 0.0;
double MAX_INSPECT_DISTANCE_SQUARED_100 = 0.0;
double MAX_LOOT_DISTANCE_SQUARED_100 = 0.0;
double MAX_BIND_DISTANCE_SQUARED_100 = 0.0;
double MAX_SITCHAIR_DISTANCE_SQUARED_100 = 0.0;
double MAX_SITCHAIRUSE_DISTANCE_SQUARED_100 = 0.0;
double MAX_SHARE_DISTANCE_SQUARED_100 = 0.0;
double SECONDS_PER_HEARTBEAT_100 = 0.0;
double SPELL_FOCUS_DISTANCE_SQUARED_99 = 0.0;
double MAX_OBJ_INTEREST_RADIUS_SQUARED_100 = 0.0;

int CurrencyMultiplier(unsigned int denomination) {
    if (!s_lootInitialized) {
        // The recovered client reports an assertion here and then continues.
        // The table is initialized at static defaults so release behavior is
        // still deterministic when a caller arrives early.
    }
    if (denomination > 2) {
        return s_currencyMultiplier[2];
    }
    return s_currencyMultiplier[denomination];
}

char* CurrencyAbbreviation(int denomination) {
    static char copper[] = "Copper";
    static char silver[] = "Silver";
    static char gold[] = "Gold";
    static char error[] = "Error";
    switch (denomination) {
    case 0: return copper;
    case 1: return silver;
    case 2: return gold;
    default: return error;
    }
}

void CurrencyBreakdown(int coins, int* breakdown) {
    if (breakdown == nullptr) {
        return;
    }
    for (int denomination = 2; denomination >= 0; --denomination) {
        const int multiplier = CurrencyMultiplier(static_cast<unsigned>(denomination));
        if (coins < multiplier) {
            breakdown[denomination] = 0;
        } else {
            breakdown[denomination] = coins / multiplier;
            coins %= multiplier;
        }
    }
}

int CurrencyTotal(const int* coins) {
    if (coins == nullptr) {
        return 0;
    }
    int total = 0;
    for (int denomination = 2; denomination >= 0; --denomination) {
        total += coins[denomination] * CurrencyMultiplier(static_cast<unsigned>(denomination));
    }
    return total;
}

int CurrencyTotal(const Currency* coins) {
    return coins == nullptr ? 0 : CurrencyTotal(&coins->copper);
}

void LootInitialize() {
    s_currencyMultiplier[0] = 1;
    s_currencyMultiplier[1] = 100 * s_currencyMultiplier[0];
    s_currencyMultiplier[2] = 100 * s_currencyMultiplier[1];
    s_lootInitialized = true;
}

void LootDestroy() {
    s_lootInitialized = false;
}

int _E1_299() { return 2139095040; }
int _E2_299() { return _E1_299(); }
void _E4_296() { TWO_PI_294 = 3.1415927 + 3.1415927; }
int _E5_296() { _E4_296(); return 0; }
void _E7_296() { OO_TWO_PI_295 = 1.0 / TWO_PI_294; }
int _E8_296() { _E7_296(); return 0; }
void _E13_12() { MAX_DUEL_DISTANCE_SQUARED_100 = 10.0 * 10.0; }
int _E14_17() { _E13_12(); return 0; }
void _E16_24() { MAX_TRADE_DISTANCE_SQUARED_100 = 11.111111 * 11.111111; }
int _E17_19() { _E16_24(); return 0; }
void _E19_51() { MAX_SHOP_DISTANCE_SQUARED_100 = 5.5555553 * 5.5555553; }
int _E20_68() { _E19_51(); return 0; }
void _E22_59() { MAX_INSPECT_DISTANCE_SQUARED_100 = 5.5555553 * 5.5555553; }
int _E23_73() { _E22_59(); return 0; }
void _E25_60() { MAX_LOOT_DISTANCE_SQUARED_100 = 5.0 * 5.0; }
int _E26_69() { _E25_60(); return 0; }
void _E28_98() { MAX_BIND_DISTANCE_SQUARED_100 = 10.0 * 10.0; }
int _E29_69() { _E28_98(); return 0; }
void _E31_95() { MAX_SITCHAIR_DISTANCE_SQUARED_100 = 0.5 * 0.5; }
int _E32_61() { _E31_95(); return 0; }
void _E34_72() { MAX_SITCHAIRUSE_DISTANCE_SQUARED_100 = 3.0 * 3.0; }
int _E35_42() { _E34_72(); return 0; }
void _E37_67() { MAX_SHARE_DISTANCE_SQUARED_100 = 100.0 * 100.0; }
int _E38_41() { _E37_67(); return 0; }
void _E40_68() { SECONDS_PER_HEARTBEAT_100 = 5.0; }
int _E41_37() { _E40_68(); return 0; }
void _E43_66() { SPELL_FOCUS_DISTANCE_SQUARED_99 = 50.0 * 50.0; }
int _E44_39() { _E43_66(); return 0; }
void _E46_64() { MAX_OBJ_INTEREST_RADIUS_SQUARED_100 = 100.0 * 100.0; }
int _E47_40() { _E46_64(); return 0; }
