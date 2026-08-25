#pragma once

#include "WoW/Source/clientruntime.h"

#include <cstdint>

extern float INFINITY_1;
extern float TWO_PI_1;
extern float OO_TWO_PI_1;
extern float MAX_DUEL_DISTANCE_SQUARED_1;

void InstallGMCommands(const char* previous);
void UninstallGMCommands(const char* firstHandle);

int __fastcall CCommand_Ghost(const char* args, const char* commandLine);
int __fastcall CCommand_Invis(const char* args, const char* commandLine);
int __fastcall CCommand_BindPlayer(const char* args, const char* commandLine);
int __fastcall CCommand_Summon(const char* args, const char* commandLine);
int __fastcall CCommand_ShowLabel(const char* args, const char* commandLine);
int __fastcall CCommand_SetSecurity(const char* args, const char* commandLine);
int __fastcall CCommand_Nuke(const char* args, const char* commandLine);

int _E2_1(); int _E1_1(); int _E5_1(); void _E4_1();
int _E8_1(); void _E7_1(); int _E20_1(); void _E19_1();
int _E23_1(); void _E22_1(); int _E26_1(); void _E25_1();
int _E29_1(); void _E28_1(); int _E32_1(); void _E31_1();
int _E35_1(); void _E34_1(); int _E38_1(); void _E37_1();
int _E41_1(); void _E40_1(); int _E44_1(); void _E43_1();
int _E47_1(); void _E46_1(); int _E50_1(); void _E49_1();
int _E53_1(); void _E52_1(); int _E90_1(); void _E87_1();
int _E89_1(); void _E88_1();

