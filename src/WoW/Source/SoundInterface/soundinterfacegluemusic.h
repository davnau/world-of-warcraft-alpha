#pragma once

extern float INFINITY_73;
extern float TWO_PI_73;
extern float OO_TWO_PI_73;
extern float MAX_DUEL_DISTANCE_SQUARED_5;
extern float MAX_TRADE_DISTANCE_SQUARED_5;
extern float MAX_SHOP_DISTANCE_SQUARED_5;
extern float MAX_INSPECT_DISTANCE_SQUARED_5;
extern float MAX_LOOT_DISTANCE_SQUARED_5;
extern float MAX_BIND_DISTANCE_SQUARED_5;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_5;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_5;
extern float MAX_SHARE_DISTANCE_SQUARED_5;
extern float SECONDS_PER_HEARTBEAT_5;
extern float SPELL_FOCUS_DISTANCE_SQUARED_5;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_5;

void InitializeGlueMusic();
void ShutdownGlueMusic();
void SndInterfaceSetGlueMusic(const char* fileName);
void SndInterfaceStopGlueMusic(float fadeTime);

extern bool s_glueMusicInitialized;
extern const char* s_musicFile;

int _E2_73(); int _E1_73(); int _E5_73(); void _E4_73(); int _E8_73(); void _E7_73();
int _E21_8(); void _E20_12(); int _E24_11(); void _E23_15(); int _E27_8(); void _E26_11();
int _E30_6(); void _E29_10(); int _E33_4(); void _E32_8(); int _E36_3(); void _E35_9();
int _E39_5(); void _E38_9(); int _E42_4(); void _E41_7(); int _E45_3(); void _E44_9();
int _E48_8(); void _E47_11(); int _E51_2(); void _E50_12(); int _E54_7(); void _E53_10();
