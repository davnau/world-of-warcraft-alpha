#pragma once

#include "ENGINE/Source/Frame/frame.h"

#include <array>
#include <cstdint>

struct VocalMacroCategory {
    unsigned race = 0;
    unsigned sex = 0;
    unsigned flags = 0;
    unsigned voice = 0;
};

struct MACRODESC {
    std::array<std::array<int, 3>, 12> soundID{};
};

int Script_PlayVocalCategory(lua_State* state);
void SndInterfaceRegisterVocalScriptFunctions();
void SndInterfaceUnregisterVocalScriptFunctions();
void SoundInterfacePlayVocalMacro(const VocalMacroCategory* category, int macroCategory);
bool SndInterfaceVocalScriptFunctionsRegistered();

int _E2_458(); int _E1_458(); int _E5_428(); void _E4_428(); int _E8_417(); void _E7_428();
int _E21_31(); void _E20_82(); int _E24_33(); void _E23_82(); int _E27_87(); void _E26_86();
int _E30_85(); void _E29_89(); int _E33_73(); void _E32_79(); int _E36_100(); void _E35_82();
int _E39_98(); void _E38_79(); int _E42_94(); void _E41_74(); int _E45_92(); void _E44_75();
int _E48_98(); void _E47_77(); int _E51_95(); void _E50_67(); int _E54_99(); void _E53_68();
int _E62_54(); int _E59_57(); int _E61_58(); void _E60_92();
