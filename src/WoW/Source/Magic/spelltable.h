#pragma once

#include "WoW/Source/DB/DBClient/AutoCode/skilllineabilityrec.h"

#include <cstdint>
#include <vector>

class SkillLineTable {
public:
    void Initialize();
    void AddPetAbility(const SkillLineAbilityRec* ability);
    std::vector<const SkillLineAbilityRec*> m_petAbilities;
};

void SpellTableInitialize();
void SpellTableDestroy();
const SkillLineAbilityRec* SpellTableLookupPetAbility(int spellID, unsigned context);

extern SkillLineTable g_skillLineTable;
extern float INFINITY_373;
int _E2_373(); int _E1_373(); int _E5_343(); void _E4_343(); int _E8_333(); void _E7_343();
int _E20_75(); void _E19_57(); int _E23_77(); void _E22_65(); int _E26_74(); void _E25_65();
int _E29_73(); void _E28_101(); int _E32_65(); void _E31_99(); int _E98_3(); void _E97_4();
int _E101_24(); void _E100_28(); int _E104_28(); void _E103_7(); int _E107_25(); void _E106_5();
int _E110_26(); void _E109_3(); int _E113_26(); void _E112_2(); int _E116_25(); void _E115_3();
int _E119_26(); void _E118_2(); int _E122_25(); void _E121_1(); int _E125_24(); void _E124_2();
int _E128_24(); void _E127_1(); int _E131_23(); void _E130_1(); int _E150_6(); void _E147_1();
int _E149_5(); void _E148_5();
