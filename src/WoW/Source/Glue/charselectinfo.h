#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <vector>

struct lua_State;
struct HOBJECT__;
struct HMODEL__;
struct HTEXCOMPONENT__;
class CTexture;
struct CModel;
class CSimpleModel;
struct CModelCreate;

struct CHARACTER_INFO {
    std::uint64_t guid = 0;
    char name[48]{};
    std::uint32_t mapID = 0;
    std::uint32_t zoneID = 0;
    std::uint32_t guildID = 0;
    NTempest::C3Vector position{};
    std::uint32_t inventoryItemDisplayID[20]{};
    std::uint32_t inventoryItemType[20]{};
    std::uint32_t petDisplayInfoID = 0;
    std::uint32_t petExperienceLevel = 0;
    std::uint32_t petCreatureFamilyID = 0;
    std::uint8_t raceID = 0;
    std::uint8_t classID = 0;
    std::uint8_t sexID = 0;
    std::uint8_t skinID = 0;
    std::uint8_t faceID = 0;
    std::uint8_t hairStyleID = 0;
    std::uint8_t hairColorID = 0;
    std::uint8_t facialHairStyleID = 0;
    std::uint8_t experienceLevel = 0;
};

struct CHARINFO {
    CHARACTER_INFO m_characterInfo{};
    int m_eStyle = -1;
    int m_eColor = -1;
    int m_bStyle = -1;
    int m_bColor = -1;
    int m_background = -1;
    std::shared_ptr<CModel> m_characterModelOwner;
    HMODEL__* m_characterModel = nullptr;
    HTEXCOMPONENT__* m_characterComponent = nullptr;
    CTexture* m_skinTexture = nullptr;
    CTexture* m_hairTexture = nullptr;
    HMODEL__* m_petModel = nullptr;

    void ChangeSkinTexture(HOBJECT__* oldComponent = nullptr);
    void CommitTexture(int force);
    void UpdateTabardTexture();
    void UpdateCharacterInfo(const char* modelName, const void* modelRecord,
                             HMODEL__* backgroundModel);
    void Destroy();
};

class CCharSelectInfo {
public:
    static void Initialize();
    static void Shutdown();
    static void SetModelFrame(CSimpleModel* frame);
    static void SetBackgroundModel(const char* filename);
    static void ClearCharacterModel();
    static void ClearPetModel();
    static CHARINFO* GetSelectedCharacterInfo();
    static void __fastcall EnumerateCharactersCallback(CHARACTER_INFO* characterInfo,
                                                        void* param);
    static void GuildCallback(const std::uint64_t* guid, int a2, int a3, char valid);
    static void UpdateCharacterList();
    static void UpdateCharacterInfo();
    static void ChangeSkinTexture();
    static void SelectCharacter(int index);
    static unsigned GetNumCharacters();

    static void SetCharacterList(const CHARACTER_INFO* records, std::size_t count);
    static const std::vector<CHARINFO>& GetCharacterList();

    static CSimpleModel* m_modelFrame;
    static unsigned m_selectionIndex;
};

void SetHandsState(HMODEL__* model, int itemSlot, int a3, int itemInventoryType);
void SetHandState(unsigned invType, int a2, unsigned startFinger, HMODEL__* lastFinger);
void SetFingersSeq(CModel* sequence, unsigned a2, unsigned startFinger, CModel* lastFinger);
void ResetFingersSeq(CModel* model, CModel** startFinger, unsigned a3, CModel* lastFinger);

int Script_SetCharSelectModelFrame(lua_State* state);
int Script_SetCharSelectBackground(lua_State* state);
int Script_GetCharacterListUpdate(lua_State* state);
int Script_GetNumCharacters(lua_State* state);
int Script_GetCharacterInfo(lua_State* state);
int Script_SelectCharacter(lua_State* state);
int Script_DeleteCharacter(lua_State* state);
void CharSelectRegisterScriptFunctions();
void CharSelectUnregisterScriptFunctions();

extern float INFINITY_88;
extern float TWO_PI_88;
extern float OO_TWO_PI_88;
extern float MAX_DUEL_DISTANCE_SQUARED_19;
extern float MAX_TRADE_DISTANCE_SQUARED_19;
extern float MAX_SHOP_DISTANCE_SQUARED_19;
extern float MAX_INSPECT_DISTANCE_SQUARED_19;
extern float MAX_LOOT_DISTANCE_SQUARED_19;
extern float MAX_BIND_DISTANCE_SQUARED_19;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_19;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_19;
extern float MAX_SHARE_DISTANCE_SQUARED_19;
extern float SECONDS_PER_HEARTBEAT_19;
extern float SPELL_FOCUS_DISTANCE_SQUARED_19;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_19;

int _E2_88(); int _E1_88(); int _E5_88(); void _E4_88(); int _E8_88(); void _E7_88();
int _E14_11(); void _E13_6(); int _E17_9(); void _E16_13();
int _E20_27(); void _E19_14(); int _E23_29(); void _E22_20();
int _E26_25(); void _E25_18(); int _E29_24(); void _E28_12();
int _E32_22(); void _E31_9(); int _E35_22(); void _E34_10();
int _E38_22(); void _E37_9(); int _E41_20(); void _E40_10();
int _E44_22(); void _E43_9(); int _E47_24(); void _E46_7();
int _E83_3(); void _E80_4(); int _E82_4(); void _E81_3();
int _E106_3(); int _E103_3(); int _E105_1(); void _E104_1();
