#include "WoW/Source/SoundInterface/soundinterfacevocal.h"

#include "ENGINE/Source/Os/W32/ossound.h"
#include "WoW/Source/DB/DBClient/dbclient.h"

#include <algorithm>
#include <limits>
#include <vector>

float INFINITY_458 = 0.0F;
float TWO_PI_458 = 0.0F;
float OO_TWO_PI_458 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_31 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_31 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_31 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_31 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_31 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_31 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_31 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_31 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_31 = 0.0F;
float SECONDS_PER_HEARTBEAT_31 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_31 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_31 = 0.0F;

namespace {
std::vector<MACRODESC> s_macroRaceDescs;
bool s_registered = false;

int SoundIdFor(const VocalMacroCategory& category, int macroCategory) {
    if (macroCategory < 0 || macroCategory >= 12 || category.race >= s_macroRaceDescs.size()) return 0;
    const auto& desc = s_macroRaceDescs[category.race];
    const unsigned voice = std::min(category.voice, 2U);
    return desc.soundID[macroCategory][voice];
}
}

int Script_PlayVocalCategory(lua_State* state) {
    return FrameScript_Invoke(state, "PlayVocalCategory");
}

void SndInterfaceRegisterVocalScriptFunctions() {
    s_macroRaceDescs.assign(static_cast<std::size_t>(std::max(0, g_chrRacesDB.m_maxID + 1)), {});
    for (const auto& record : g_soundCharacterMacroLinesDB.m_records) {
        if (record.m_Race < 0 || static_cast<std::size_t>(record.m_Race) >= s_macroRaceDescs.size() ||
            record.m_Category < 0 || record.m_Category >= 12 || record.m_Sex < 0 || record.m_Sex >= 3) continue;
        s_macroRaceDescs[record.m_Race].soundID[record.m_Category][record.m_Sex] = record.m_SoundID;
    }
    s_registered = true;
}

void SndInterfaceUnregisterVocalScriptFunctions() {
    s_macroRaceDescs.clear();
    s_registered = false;
}

bool SndInterfaceVocalScriptFunctionsRegistered() { return s_registered; }

void SoundInterfacePlayVocalMacro(const VocalMacroCategory* category, int macroCategory) {
    if (!category || !(category->flags & 0x10) || macroCategory < 0 || macroCategory >= 12) return;
    const int soundId = SoundIdFor(*category, macroCategory);
    if (!soundId) return;
    Sound* sound = Sound::Alloc();
    if (!sound) return;
    sound->SetVolume(1.0F);
    if (!sound->Play2D()) Sound::KillSound(sound);
}

int _E2_458() { return _E1_458(); }
int _E1_458() { INFINITY_458 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_428() { _E4_428(); return 0; } void _E4_428() { TWO_PI_458 = 6.2831854F; }
int _E8_417() { _E7_428(); return 0; } void _E7_428() { OO_TWO_PI_458 = 1.0F / TWO_PI_458; }
#define VOCAL_CONST_PAIR(thunk, body, variable, value) int thunk() { body(); return 0; } void body() { variable = value; }
VOCAL_CONST_PAIR(_E21_31, _E20_82, MAX_DUEL_DISTANCE_SQUARED_31, 100.0F)
VOCAL_CONST_PAIR(_E24_33, _E23_82, MAX_TRADE_DISTANCE_SQUARED_31, 123.45679F)
VOCAL_CONST_PAIR(_E27_87, _E26_86, MAX_SHOP_DISTANCE_SQUARED_31, 30.864195F)
VOCAL_CONST_PAIR(_E30_85, _E29_89, MAX_INSPECT_DISTANCE_SQUARED_31, 30.864195F)
VOCAL_CONST_PAIR(_E33_73, _E32_79, MAX_LOOT_DISTANCE_SQUARED_31, 25.0F)
VOCAL_CONST_PAIR(_E36_100, _E35_82, MAX_BIND_DISTANCE_SQUARED_31, 100.0F)
VOCAL_CONST_PAIR(_E39_98, _E38_79, MAX_SITCHAIR_DISTANCE_SQUARED_31, 0.25F)
VOCAL_CONST_PAIR(_E42_94, _E41_74, MAX_SITCHAIRUSE_DISTANCE_SQUARED_31, 9.0F)
VOCAL_CONST_PAIR(_E45_92, _E44_75, MAX_SHARE_DISTANCE_SQUARED_31, 10000.0F)
VOCAL_CONST_PAIR(_E48_98, _E47_77, SECONDS_PER_HEARTBEAT_31, 5.0F)
VOCAL_CONST_PAIR(_E51_95, _E50_67, SPELL_FOCUS_DISTANCE_SQUARED_31, 2500.0F)
VOCAL_CONST_PAIR(_E54_99, _E53_68, MAX_OBJ_INTEREST_RADIUS_SQUARED_31, 10000.0F)
#undef VOCAL_CONST_PAIR
int _E62_54() { _E59_57(); return _E61_58(); }
int _E59_57() { s_macroRaceDescs.clear(); return 0; }
int _E61_58() { return 0; }
void _E60_92() { s_macroRaceDescs.clear(); }
