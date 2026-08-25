#pragma once

#include <cstdint>

void SoundInterfaceInitializeWorldMIDICVars(const char* previous = nullptr);
unsigned char AmbienceVolumeHandler(const char* newValue, void* userArg);
unsigned char EnableAmbienceHandler(const char* newValue, void* userArg);
void SoundInterfaceInitializeWorldMIDI(const char* previous = nullptr);
void SoundInterfaceShutdownWorldMIDI();
void SndInterfaceSetMIDIArea(int normalArea, int underwaterArea);
void StartAmbience();
void SndInterfaceClearMIDI();
void SndInterfaceMIDIAmbienceChanged();
void SndInterfaceMIDIUnderwaterChanged();
void SndInterfaceMIDISetPaused(unsigned char paused);

extern float s_midiVolume;
extern bool s_midiPaused;
extern bool s_midiUnderwater;

extern float INFINITY_80;
extern float TWO_PI_80;
extern float OO_TWO_PI_80;
extern float MAX_DUEL_DISTANCE_SQUARED_12;
extern float MAX_TRADE_DISTANCE_SQUARED_12;
extern float MAX_SHOP_DISTANCE_SQUARED_12;
extern float MAX_INSPECT_DISTANCE_SQUARED_12;
extern float MAX_LOOT_DISTANCE_SQUARED_12;
extern float MAX_BIND_DISTANCE_SQUARED_12;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_12;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_12;
extern float MAX_SHARE_DISTANCE_SQUARED_12;
extern float SECONDS_PER_HEARTBEAT_12;
extern float SPELL_FOCUS_DISTANCE_SQUARED_12;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_12;

int _E2_80(); int _E1_80(); int _E5_80(); void _E4_80(); int _E8_80(); void _E7_80();
int _E21_15(); void _E20_19(); int _E24_18(); void _E23_22();
int _E27_15(); void _E26_18(); int _E30_13(); void _E29_17();
int _E33_11(); void _E32_15(); int _E36_10(); void _E35_16();
int _E39_12(); void _E38_16(); int _E42_11(); void _E41_14();
int _E45_10(); void _E44_16(); int _E48_15(); void _E47_18();
int _E51_9(); void _E50_19(); int _E54_14(); void _E53_17();

