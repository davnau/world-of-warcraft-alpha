#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

enum SNDROOMTYPE : unsigned { SNDROOMTYPE_PSYCHOTIC = 0, SNDROOMTYPE_LAST = 7 };

void SndDebugInitialize(const char* previous = nullptr);
int __fastcall PingSound(const char* arguments, const char* value);
int __fastcall RoomType(const char* arguments, const char* value);
int __fastcall DebugTickHandler(const void* eventData, void* parameter);
char SndDebugTick();
void SndDebugShutdown(const char* previous = nullptr);
void SndDebugDungeonTransition(const char* indoors, unsigned continent, unsigned mapId);
void SndDebugRegisterContinent(unsigned continent);

extern int s_pingSound;
extern int s_pingFrequency;
extern unsigned s_lastPingTime;
extern NTempest::C3Vector s_pingPosition;

int _E2_75(); int _E1_75(); int _E62_4(); void _E59_5(); int _E61_2(); void _E60_7();
int _E5_75(); void _E4_75(); int _E8_75(); void _E7_75(); int _E21_10(); void _E20_14();
int _E24_13(); void _E23_17(); int _E27_10(); void _E26_13(); int _E30_8(); void _E29_12();
int _E33_6(); void _E32_10(); int _E36_5(); void _E35_11(); int _E39_7(); void _E38_11();
int _E42_6(); void _E41_9(); int _E45_5(); void _E44_11(); int _E48_10(); void _E47_13();
int _E51_4(); void _E50_14(); int _E54_9(); void _E53_12();
