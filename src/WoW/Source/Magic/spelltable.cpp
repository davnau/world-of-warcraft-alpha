#include "WoW/Source/Magic/spelltable.h"

#include <algorithm>
#include <limits>

SkillLineTable g_skillLineTable;
float INFINITY_373 = 0.0F;

int _E2_373() { return _E1_373(); }
int _E1_373() { INFINITY_373 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_343() { _E4_343(); return 0; }
void _E4_343() {}
int _E8_333() { _E7_343(); return 0; }
void _E7_343() {}

void SkillLineTable::Initialize() {
    m_petAbilities.clear();
}
void SkillLineTable::AddPetAbility(const SkillLineAbilityRec* ability) {
    if (ability == nullptr) return;
    const auto it = std::find(m_petAbilities.begin(), m_petAbilities.end(), ability);
    if (it == m_petAbilities.end()) m_petAbilities.push_back(ability);
}
void SpellTableInitialize() { g_skillLineTable.Initialize(); }
void SpellTableDestroy() { g_skillLineTable.Initialize(); }
const SkillLineAbilityRec* SpellTableLookupPetAbility(int spellID, unsigned context) {
    for (const SkillLineAbilityRec* ability : g_skillLineTable.m_petAbilities) {
        if (ability != nullptr && ability->m_spellID == spellID &&
            (context == 0 || ability->m_skillLineID == static_cast<int>(context))) {
            return ability;
        }
    }
    return nullptr;
}

#define SPELLTABLE_INIT(n) int _E##n() { return 0; } void _E##n##_dummy() {}
int _E20_75(){return 0;} void _E19_57(){}
int _E23_77(){return 0;} void _E22_65(){}
int _E26_74(){return 0;} void _E25_65(){}
int _E29_73(){return 0;} void _E28_101(){}
int _E32_65(){return 0;} void _E31_99(){}
int _E98_3(){return 0;} void _E97_4(){}
int _E101_24(){return 0;} void _E100_28(){}
int _E104_28(){return 0;} void _E103_7(){}
int _E107_25(){return 0;} void _E106_5(){}
int _E110_26(){return 0;} void _E109_3(){}
int _E113_26(){return 0;} void _E112_2(){}
int _E116_25(){return 0;} void _E115_3(){}
int _E119_26(){return 0;} void _E118_2(){}
int _E122_25(){return 0;} void _E121_1(){}
int _E125_24(){return 0;} void _E124_2(){}
int _E128_24(){return 0;} void _E127_1(){}
int _E131_23(){return 0;} void _E130_1(){}
int _E150_6(){return 0;} void _E147_1(){}
int _E149_5(){return 0;} void _E148_5(){}
#undef SPELLTABLE_INIT
