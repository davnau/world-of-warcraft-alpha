#pragma once

#include <cstdint>

void SndInterfaceZoneIntroIdler();
void SndInterfaceZoneIntroInitialize(const char* previous = nullptr);
int CCommand_ZoneIntroReset();
void SndInterfaceZoneIntroDestroy();
void SndInterfaceRegisterNewZoneIntro(int priority, int soundId);
void SndInterfaceZoneIntroStop();

extern float INFINITY_77;
extern float TWO_PI_77;
extern float OO_TWO_PI_77;
extern float MAX_DUEL_DISTANCE_SQUARED_9;
extern float MAX_TRADE_DISTANCE_SQUARED_9;
extern float MAX_SHOP_DISTANCE_SQUARED_9;
extern float MAX_INSPECT_DISTANCE_SQUARED_9;
extern float MAX_LOOT_DISTANCE_SQUARED_9;
extern float MAX_BIND_DISTANCE_SQUARED_9;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_9;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_9;
extern float MAX_SHARE_DISTANCE_SQUARED_9;
extern float SECONDS_PER_HEARTBEAT_9;
extern float SPELL_FOCUS_DISTANCE_SQUARED_9;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_9;

int _E2_77(); int _E1_77(); int _E5_77(); void _E4_77(); int _E8_77(); void _E7_77();
int _E21_12(); void _E20_16(); int _E24_15(); void _E23_19();
int _E27_12(); void _E26_15(); int _E30_10(); void _E29_14();
int _E33_8(); void _E32_12(); int _E36_7(); void _E35_13();
int _E39_9(); void _E38_13(); int _E42_8(); void _E41_11();
int _E45_7(); void _E44_13(); int _E48_12(); void _E47_15();
int _E51_6(); void _E50_16(); int _E54_11(); void _E53_14();

