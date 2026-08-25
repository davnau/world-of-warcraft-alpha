#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

struct lua_State;
struct HOBJECT__;
struct HMODEL__;
struct HCHARGEOSET__;
struct HTEXCOMPONENT__;
class CTexture;
struct CModel;
class CSimpleModel;
struct CModelCreate;

struct CustomizationSelections {
    unsigned classID = 0;
    unsigned outfit = 0;
    unsigned skinColor = 0;
    unsigned hairColor = 0;
    unsigned hairStyle = 0;
    unsigned facialStyle = 0;
    unsigned face = 0;
};

struct CHARCREATEINFO {
    std::shared_ptr<CModel> characterModelOwner[2]{};
    HMODEL__* characterModel[2]{};
    HCHARGEOSET__* geosetHandle[2]{};
    HTEXCOMPONENT__* characterComponent[2]{};
    CTexture* skinTexture[2]{};
    CTexture* hairTexture[2]{};
    CustomizationSelections selections[2]{};
    float cameraHeight[2][2]{};
    float cameraRadius[2][2]{};
    float targetHeight[2][2]{};
    unsigned currentGeosets[3][15]{};

    void Shutdown(HOBJECT__* handle = nullptr);
    void UpdateOutfit(int increment, unsigned race, unsigned sex);
    void ResetOutfitSelection(unsigned raceID, unsigned sex);
    void CommitGeoset(HCHARGEOSET__* geoset, unsigned sex);
    void UpdateCharacterInfo(unsigned modelFlags, unsigned race, unsigned sex);
    void ChangeHairGeosets(unsigned race, unsigned sex);
    void UpdateEquipment(CModel* model, int doNotCommitGeosets,
                         HCHARGEOSET__* race, unsigned sex);
    void ChangeSkinTexture(HOBJECT__* oldComponent, int doNotCommitGeosets,
                           HCHARGEOSET__* race, unsigned sex);
    void ChangeFaceTexture(HMODEL__* race, unsigned sex);
    void ChangeFacialHairTexture(HMODEL__* race, unsigned sex);
    void ChangeFacialHairGeosets(unsigned sex, HCHARGEOSET__* beard,
                                 unsigned sideburn, unsigned moustache);
    void ChangeScalpHairTexture(HMODEL__* race, unsigned sex);
    void UpdateGeosets(unsigned beard, unsigned sideburn,
                       HCHARGEOSET__* moustache, unsigned sex);
    void FindRange(unsigned group, unsigned* start, unsigned* end);
    void CommitTexture(int race, unsigned sex);
};

class CCharCreateInfo {
public:
    static void Initialize();
    static void Shutdown();
    static void SetCharCustomizeFrame(CSimpleModel* frame);
    static void SetCharCustomizeModel(const char* filename);
    static unsigned GetNumOutfits(unsigned classID, int race, unsigned sex);
    static const void* GetOutfit(unsigned classID, int race, unsigned sex,
                                 unsigned outfitID);
    static void ResetCharCustomizeInfo();
    static void SetCharFacing(float facing);
    static const char* GetRaceNameByIndex(unsigned index);
    static void UpdateAvailableClasses();
    static const char* GetClassNameByIndex(unsigned index);
    static unsigned GetSelectedRaceID();
    static unsigned GetSelectedSexID();
    static unsigned GetSelectedClassID();
    static void UpdateAllCharacterInfo(unsigned race, int sex, unsigned updateModel);
    static void InitializeCharacterInfo(unsigned doNotCommitGeosets, unsigned modelFlags);
    static void UpdateCharacterInfo(unsigned flags);
    static void UpdateEquipment(int sex, unsigned flags);
    static void ChangeSkinTexture(int sex, unsigned flags);
    static void ChangeFaceTexture(unsigned sex);
    static void ChangeFacialHairTexture(unsigned sex);
    static void ChangeFacialHairGeosets(unsigned sex);
    static void ChangeScalpHairTexture(unsigned sex);
    static void ChangeHairGeosets(unsigned sex);
    static void UpdateGeosets(unsigned sex);
    static void CommitCurrentGeoset(HCHARGEOSET__* geoset, unsigned sex);
    static void SetSelectedRace(unsigned race, int updateModel);
    static void SetSelectedSex(unsigned sex);
    static void SetSelectedClass(unsigned classIndex);
    static unsigned GetNumCharCustomizations(int customization);
    static void CycleCharCustomization(int customization, int delta);
    static void RandomizeCharCustomization();
    static void CreateCharacter(const char* name);

    static void SetAvailableRaces(const std::vector<unsigned>& ids,
                                  const std::vector<std::string>& names);
    static void SetAvailableClasses(const std::vector<unsigned>& ids,
                                    const std::vector<std::string>& names);

    static CHARCREATEINFO m_charInfo;
    static CSimpleModel* m_charCustomizeFrame;
    static unsigned m_selectedRace;
    static unsigned m_selectedSex;
    static unsigned m_selectedClass;
    static float m_charFacing;
    static std::vector<unsigned> m_factionIndex;
    static std::vector<unsigned> m_raceIndex;
    static std::vector<unsigned> m_classIndex;
};

unsigned RandomSelection(unsigned count);
void ReportMissingComponentTextures(int sex, int race);

int Script_SetCharCustomizeFrame(lua_State* state);
int Script_SetCharCustomizeBackground(lua_State* state);
int Script_ResetCharCustomize(lua_State* state);
int Script_GetNameForRace(lua_State* state);
int Script_GetFactionForRace(lua_State* state);
int Script_GetAvailableRaces(lua_State* state);
int Script_GetClassesForRace(lua_State* state);
int Script_GetSelectedRace(lua_State* state);
int Script_GetSelectedSex(lua_State* state);
int Script_GetSelectedClass(lua_State* state);
int Script_SetSelectedRace(lua_State* state);
int Script_SetSelectedSex(lua_State* state);
int Script_SetSelectedClass(lua_State* state);
int Script_UpdateCustomizationBackground(lua_State* state);
int Script_HasCharCustomization(lua_State* state);
int Script_CycleCharCustomization(lua_State* state);
int Script_RandomizeCharCustomization(lua_State* state);
int Script_GetCharacterFacing(lua_State* state);
int Script_SetCharacterFacing(lua_State* state);
int Script_CreateCharacter(lua_State* state);
void CharCreateRegisterScriptFunctions();
void CharCreateUnregisterScriptFunctions();

extern float INFINITY_87;
extern float TWO_PI_87;
extern float OO_TWO_PI_87;
extern float MAX_DUEL_DISTANCE_SQUARED_18;
extern float MAX_TRADE_DISTANCE_SQUARED_18;
extern float MAX_SHOP_DISTANCE_SQUARED_18;
extern float MAX_INSPECT_DISTANCE_SQUARED_18;
extern float MAX_LOOT_DISTANCE_SQUARED_18;
extern float MAX_BIND_DISTANCE_SQUARED_18;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_18;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_18;
extern float MAX_SHARE_DISTANCE_SQUARED_18;
extern float SECONDS_PER_HEARTBEAT_18;
extern float SPELL_FOCUS_DISTANCE_SQUARED_18;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_18;

int _E2_87(); int _E1_87(); int _E5_87(); void _E4_87(); int _E8_87(); void _E7_87();
int _E14_10(); void _E13_5(); int _E17_8(); void _E16_12();
int _E20_26(); void _E19_13(); int _E23_28(); void _E22_19();
int _E26_24(); void _E25_17(); int _E29_23(); void _E28_11();
int _E32_21(); void _E31_8(); int _E35_21(); void _E34_9();
int _E38_21(); void _E37_8(); int _E41_19(); void _E40_9();
int _E44_21(); void _E43_8(); int _E47_23(); void _E46_6();
int _E83_2(); void _E80_3(); int _E82_3(); void _E81_2();
int _E107_0(); int _E103_2(); int _E106_2(); void _E104_0();
int _E112_0(); int _E109_0(); int _E111(); void _E110_0();
int _E117(); int _E114(); int _E116_0(); void _E115_0();
int _E123(); unsigned _E119_0(); int _E122_0(); void _E120(HOBJECT__* handle);
