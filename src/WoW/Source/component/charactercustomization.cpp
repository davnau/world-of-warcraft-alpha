#include "WoW/Source/component/charactercustomization.h"
#include "ENGINE/Source/Model/modelmisc.h"

namespace {
constexpr bool g_charGeosetIsOverlay[NUM_CHARGEOSETS] = {};
constexpr CHARACTER_GEOSET_SECTIONS s_clothingGeosetRanges[] = {
    CHARGEOSET_GLOVE, CHARGEOSET_BOOT, CHARGEOSET_SLEEVES,
    CHARGEOSET_PANTS, CHARGEOSET_DOUBLET, CHARGEOSET_PANTDOUBLET,
    CHARGEOSET_TABARD, CHARGEOSET_ROBE, CHARGEOSET_LOINCLOTH,
};
}

void ComponentCharacterCustomizationInitialize() { CharCustomizationInitialize(); }
void ComponentCharacterCustomizationShutdown() { CharCustomizationShutdown(); }

void CCharGeoset::ShowGeosetSection(HMODEL__* model, CHARACTER_GEOSET_SECTIONS section,
                                    unsigned int geosetNumber, int hideRemainder) {
    if (section < 0 || section >= NUM_CHARGEOSETS || model == nullptr || geosetNumber >= 100)
        return;
    if (hideRemainder)
        HideGeosetSection(model, section);
    unsigned int value = geosetNumber;
    if (value == 0)
        value = g_charGeosetIsOverlay[section] ? 0u : 1u;
    const unsigned int previous = m_currentGeosets[section];
    if (previous != value && previous != 0 && !hideRemainder)
        ModelHideGeosets(*reinterpret_cast<CModel*>(model), previous + 100u * section, true);
    m_currentGeosets[section] = value;
    m_flags |= 1;
}

void CCharGeoset::HideGeosetSection(HMODEL__* model, CHARACTER_GEOSET_SECTIONS section) {
    if (section < 0 || section >= NUM_CHARGEOSETS || model == nullptr)
        return;
    ModelHideGeosetsRange(*reinterpret_cast<CModel*>(model), 100u * section + 1u,
                          100u * section + 99u, true);
    m_currentGeosets[section] = 0;
    m_flags |= 1;
}

void __fastcall CharCustomizationCommitItemGeosets(CCharGeoset* geoset, int commit,
                                                   HMODEL__* paperDollModel) {
    if (geoset == nullptr)
        return;
    CharCustomizationCommitItemGeosets(geoset, commit);
    if (paperDollModel == nullptr)
        return;
    for (const auto section : s_clothingGeosetRanges) {
        const unsigned index = static_cast<unsigned>(section);
        if ((geoset->m_workingGeosetInfo.flags[index - CHARGEOSET_GLOVE] & 1u) != 0)
            geoset->HideGeosetSection(paperDollModel, section);
        else
            geoset->ShowGeosetSection(paperDollModel, section,
                                      geoset->m_workingGeosetInfo.currentGeosets[index - CHARGEOSET_GLOVE], 0);
    }
}

int _E1_89(){return 0;} int _E2_89(){return _E1_89();} void _E138_3(){} int _E139_2(){_E138_3();return 0;} int _E140_3(){return _E139_2();}
void _E143_3(){} int _E144_1(){_E143_3();return 0;} int _E145_1(){return _E144_1();} void _E148(){} int _E149(){_E148();return 0;} int _E150(){return _E149();} int _E153(){return 0;}
void _E4_89(){} int _E5_89(){_E4_89();return 0;} void _E7_89(){} int _E8_89(){_E7_89();return 0;}
void _E19_15(){} int _E20_28(){_E19_15();return 0;} void _E22_21(){} int _E23_30(){_E22_21();return 0;}
void _E25_19(){} int _E26_26(){_E25_19();return 0;} void _E28_13(){} int _E29_25(){_E28_13();return 0;}
void _E31_10(){} int _E32_23(){_E31_10();return 0;} void _E34_11(){} int _E35_23(){_E34_11();return 0;}
void _E37_10(){} int _E38_23(){_E37_10();return 0;} void _E40_11(){} int _E41_21(){_E40_11();return 0;}
void _E43_10(){} int _E44_23(){_E43_10();return 0;} void _E46_8(){} int _E47_25(){_E46_8();return 0;}
void _E49_10(){} int _E50_22(){_E49_10();return 0;} void _E52_9(){} int _E53_20(){_E52_9();return 0;}
void _E55_5(){} int _E56_0(){_E55_5();return 0;} void _E58_4(){} int _E59_11(){_E58_4();return 0;}
void _E61_5(){} int _E62_10(){_E61_5();return 0;} void _E64_2(){} int _E65_6(){_E64_2();return 0;} void _E67_1(){} int _E68_5(){_E67_1();return 0;}
