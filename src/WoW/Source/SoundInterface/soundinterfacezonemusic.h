#pragma once

#include <cstdint>

void InitializeZoneMusic();
int ZoneMusicIdle(unsigned now);
std::uint32_t GetNextPlayTime();
void PlayMusic();
void ShutdownZoneMusic(int eventId = 0);
void SndInterfaceRegisterNewZone(int zoneId);
void SndInterfacePauseZoneMusic(int paused);
int SndInterfaceIsZoneMusicPaused();
void SndInterfaceSetZoneMusicIntervals(unsigned minimumMs, unsigned maximumMs);

extern float INFINITY_76;
extern float TWO_PI_76;
extern float OO_TWO_PI_76;
extern float MAX_DUEL_DISTANCE_SQUARED_8;
extern float MAX_TRADE_DISTANCE_SQUARED_8;
extern float MAX_SHOP_DISTANCE_SQUARED_8;
extern float MAX_INSPECT_DISTANCE_SQUARED_8;
extern float MAX_LOOT_DISTANCE_SQUARED_8;
extern float MAX_BIND_DISTANCE_SQUARED_8;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_8;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_8;
extern float MAX_SHARE_DISTANCE_SQUARED_8;
extern float SECONDS_PER_HEARTBEAT_8;
extern float SPELL_FOCUS_DISTANCE_SQUARED_8;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_8;

int _E2_76(); int _E1_76(); int _E5_76(); void _E4_76(); int _E8_76(); void _E7_76();
int _E21_11(); void _E20_15(); int _E24_14(); void _E23_18();
int _E27_11(); void _E26_14(); int _E30_9(); void _E29_13();
int _E33_7(); void _E32_11(); int _E36_6(); void _E35_12();
int _E39_8(); void _E38_12(); int _E42_7(); void _E41_10();
int _E45_6(); void _E44_12(); int _E48_11(); void _E47_14();
int _E51_5(); void _E50_15(); int _E54_10(); void _E53_13();
int _E62_5(); void _E59_6(); int _E61_3(); void _E60_8();

