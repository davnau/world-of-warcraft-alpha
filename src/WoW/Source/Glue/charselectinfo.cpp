#include "WoW/Source/Glue/charselectinfo.h"

#include "ENGINE/Source/Frame/frame.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Model/modelanim.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/modelmisc.h"
#include "ENGINE/Source/Services/texture.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/Net/netinternal.h"
#include "WoW/Source/Object/objectclient/object_c.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"
#include "WoW/common/componentcore/charactercustomizationcore.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <array>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <limits>

float INFINITY_88 = 0.0F;
float TWO_PI_88 = 0.0F;
float OO_TWO_PI_88 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_19 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_19 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_19 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_19 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_19 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_19 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_19 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_19 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_19 = 0.0F;
float SECONDS_PER_HEARTBEAT_19 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_19 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_19 = 0.0F;

CSimpleModel* CCharSelectInfo::m_modelFrame = nullptr;
unsigned CCharSelectInfo::m_selectionIndex = 0;

namespace {
std::vector<CHARINFO> s_charList;
bool s_registered = false;

struct ScriptFunction {
    const char* name;
    lua_CFunction method;
};

const char* RaceName(unsigned id)
{
    switch (id) {
    case 1: return "Human";
    case 2: return "Orc";
    case 3: return "Dwarf";
    case 4: return "Night Elf";
    case 5: return "Undead";
    case 6: return "Tauren";
    case 7: return "Gnome";
    case 8: return "Troll";
    default: return "";
    }
}

const char* RaceClientName(unsigned id)
{
    switch (id) {
    case 1: return "Human";
    case 2: return "Orc";
    case 3: return "Dwarf";
    case 4: return "NightElf";
    case 5: return "Scourge";
    case 6: return "Tauren";
    case 7: return "Gnome";
    case 8: return "Troll";
    default: return "";
    }
}

const char* ClassName(unsigned id)
{
    switch (id) {
    case 1: return "Warrior";
    case 2: return "Paladin";
    case 3: return "Hunter";
    case 4: return "Rogue";
    case 5: return "Priest";
    case 7: return "Shaman";
    case 8: return "Mage";
    case 9: return "Warlock";
    case 11: return "Druid";
    default: return "";
    }
}

void ResetRecord(CHARINFO& record)
{
    record = CHARINFO{};
}

const char* CharacterModelName(unsigned race, unsigned sex)
{
    static constexpr const char* models[8][2] = {
        {"Character\\Human\\Male\\HumanMale.MDX", "Character\\Human\\Female\\HumanFemale.mdx"},
        {"Character\\Orc\\Male\\OrcMale.mdx", "Character\\Orc\\Female\\OrcFemale.mdx"},
        {"Character\\Dwarf\\Male\\DwarfMale.mdx", "Character\\Dwarf\\Female\\DwarfFemale.mdx"},
        {"Character\\NightElf\\Male\\NightElfMale.mdx", "Character\\NightElf\\Female\\NightElfFemale.mdx"},
        {"Character\\Scourge\\Male\\ScourgeMale.mdx", "Character\\Scourge\\Female\\ScourgeFemale.mdx"},
        {"Character\\Tauren\\Male\\TaurenMale.mdx", "Character\\Tauren\\Female\\TaurenFemale.mdx"},
        {"Character\\Gnome\\Male\\GnomeMale.mdx", "Character\\Gnome\\Female\\GnomeFemale.mdx"},
        {"Character\\Troll\\Male\\TrollMale.mdx", "Character\\Troll\\Female\\TrollFemale.mdx"},
    };
    return race >= 1U && race <= 8U && sex < 2U ? models[race - 1U][sex] : nullptr;
}

CModel* CharacterSelectBackground()
{
    if (!CCharSelectInfo::m_modelFrame) return nullptr;
    return CCharSelectInfo::m_modelFrame->m_modelOwner
        ? CCharSelectInfo::m_modelFrame->m_modelOwner.get()
        : reinterpret_cast<CModel*>(CCharSelectInfo::m_modelFrame->m_model);
}
}

void CHARINFO::ChangeSkinTexture(HOBJECT__*)
{
    TextureRelease(m_skinTexture);
    TextureRelease(m_hairTexture);
    m_skinTexture = nullptr;
    m_hairTexture = nullptr;
    m_characterComponent = nullptr;

    if (!m_characterModelOwner || m_characterInfo.sexID >= 2U)
        return;

    const char* raceName = RaceClientName(m_characterInfo.raceID);
    if (!raceName || !*raceName)
        return;

    const char* sexName = m_characterInfo.sexID == 0U ? "Male" : "Female";
    char skinName[260]{};
    char hairName[260]{};
    std::snprintf(skinName, sizeof(skinName),
                  "Character\\%s\\%s\\%s%sSkin00_%02u.blp",
                  raceName, sexName, raceName, sexName,
                  static_cast<unsigned>(m_characterInfo.skinID));
    std::snprintf(hairName, sizeof(hairName),
                  "Character\\%s\\Hair%02u_%02u.blp", raceName,
                  static_cast<unsigned>(m_characterInfo.hairStyleID),
                  static_cast<unsigned>(m_characterInfo.hairColorID));

    m_skinTexture = TextureCreate(skinName, nullptr, 0U, 0);
    m_hairTexture = TextureCreate(hairName, nullptr, 0U, 0);
    const auto skinReplacements = ComplexModelReplaceTexture(
        *m_characterModelOwner, 1U, m_skinTexture, false);
    const auto hairReplacements = ComplexModelReplaceTexture(
        *m_characterModelOwner, 6U, m_hairTexture, false);

    // Recovered ChangeSkinTexture creates a character-geoset handle, restores
    // the naked/base sections, selects the current hair section, applies item
    // geosets, and commits.  The item component path is still being restored,
    // but the base commit is mandatory: an uncommitted model draws every
    // mutually-exclusive body, facial-hair and clothing geoset at once.
    CCharGeoset geosets;
    geosets.m_charModel = reinterpret_cast<HMODEL__*>(m_characterModelOwner.get());
    CharCustomizationInitBaseCharacter(
        &geosets,
        g_defaultGeosetIDOffsets[CHARGEOSET_BEARD],
        g_defaultGeosetIDOffsets[CHARGEOSET_SIDEBURN],
        g_defaultGeosetIDOffsets[CHARGEOSET_MOUSTACHE], 2U);
    CharCustomizationResetHairGeoset(
        &geosets, m_characterInfo.raceID, m_characterInfo.sexID,
        m_characterInfo.hairStyleID);
    CharCustomizationCommitGeosets(&geosets);

    std::fprintf(stderr,
                 "CHARSELECT_TEXTURE guid=%llu race=%u sex=%u skin=%s %ux%u replace=%u "
                 "hair=%s %ux%u replace=%u\n",
                 static_cast<unsigned long long>(m_characterInfo.guid),
                 static_cast<unsigned>(m_characterInfo.raceID),
                 static_cast<unsigned>(m_characterInfo.sexID), skinName,
                 m_skinTexture ? m_skinTexture->gxWidth : 0U,
                 m_skinTexture ? m_skinTexture->gxHeight : 0U, skinReplacements,
                 hairName, m_hairTexture ? m_hairTexture->gxWidth : 0U,
                 m_hairTexture ? m_hairTexture->gxHeight : 0U, hairReplacements);
}

void CHARINFO::CommitTexture(int)
{
}

void CHARINFO::UpdateTabardTexture()
{
    if (m_characterComponent)
        CommitTexture(0);
}

void CHARINFO::UpdateCharacterInfo(const char* modelName, const void*, HMODEL__* backgroundModel)
{
    if (!modelName || !*modelName)
        modelName = CharacterModelName(m_characterInfo.raceID, m_characterInfo.sexID);
    CModelCreate createData{};
    createData.flags = 0x44U;
    createData.sequenceNames = g_animationNames;
    createData.numSequences = static_cast<std::uint32_t>(g_animationNameCount);
    m_characterModelOwner = modelName
        ? ModelCreate(modelName, &createData) : std::shared_ptr<CModel>{};
    m_characterModel = reinterpret_cast<HMODEL__*>(m_characterModelOwner.get());
    if (m_characterModelOwner) ModelSetSequence(*m_characterModelOwner, 0U);
    m_petModel = m_characterInfo.petDisplayInfoID != 0
        ? reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(1)) : nullptr;
    auto* background = reinterpret_cast<CModel*>(backgroundModel);
    if (background && m_characterModelOwner && ModelClearLink(*background, 0U))
        ModelAddLink(*background, 0U, *m_characterModelOwner, 1.0F);
    ChangeSkinTexture();
    UpdateTabardTexture();
}

void CHARINFO::Destroy()
{
    TextureRelease(m_skinTexture);
    TextureRelease(m_hairTexture);
    m_skinTexture = nullptr;
    m_hairTexture = nullptr;
    m_characterModelOwner.reset();
    m_characterModel = nullptr;
    m_characterComponent = nullptr;
    m_petModel = nullptr;
}

void CCharSelectInfo::Initialize()
{
    s_charList.clear();
    m_selectionIndex = 0;
}

void CCharSelectInfo::Shutdown()
{
    for (auto& item : s_charList)
        item.Destroy();
    s_charList.clear();
    m_selectionIndex = 0;
    m_modelFrame = nullptr;
}

void CCharSelectInfo::SetModelFrame(CSimpleModel* frame)
{
    m_modelFrame = frame;
}

void CCharSelectInfo::SetBackgroundModel(const char* filename)
{
    if (!m_modelFrame || !filename || !*filename) return;
    CModelCreate createData{};
    createData.flags = 4U;
    m_modelFrame->SetModel(filename, &createData, nullptr);
}

void CCharSelectInfo::ClearCharacterModel()
{
    if (CModel* background = CharacterSelectBackground())
        ModelClearLink(*background, 0U);
}

void CCharSelectInfo::ClearPetModel()
{
    if (CModel* background = CharacterSelectBackground())
        ModelClearLink(*background, 1U);
}

CHARINFO* CCharSelectInfo::GetSelectedCharacterInfo()
{
    return m_selectionIndex < s_charList.size() ? &s_charList[m_selectionIndex] : nullptr;
}

void __fastcall CCharSelectInfo::EnumerateCharactersCallback(CHARACTER_INFO* characterInfo,
                                                              void*)
{
    if (!characterInfo || s_charList.size() >= 10)
        return;
    CHARINFO record;
    std::memcpy(&record.m_characterInfo, characterInfo, sizeof(record.m_characterInfo));
    s_charList.push_back(record);
}

void CCharSelectInfo::GuildCallback(const std::uint64_t* guid, int, int, char valid)
{
    if (!guid || !valid)
        return;
    for (auto& item : s_charList) {
        if (item.m_characterInfo.guildID == *guid)
            item.UpdateTabardTexture();
    }
}

void CCharSelectInfo::UpdateCharacterList()
{
    for (auto& item : s_charList)
        item.Destroy();
    s_charList.clear();
    ClientServices_EnumerateCharacters(EnumerateCharactersCallback, nullptr);
    std::fprintf(stderr, "CHARSELECT_LIST count=%zu\n", s_charList.size());
    SelectCharacter(s_charList.empty() ? -1 : 0);
    FrameScript_SignalEvent(6);
}

void CCharSelectInfo::UpdateCharacterInfo()
{
    CHARINFO* selected = GetSelectedCharacterInfo();
    if (!selected)
        return;
    selected->UpdateCharacterInfo(nullptr, nullptr,
                                  m_modelFrame ? m_modelFrame->m_model : nullptr);
}

void CCharSelectInfo::ChangeSkinTexture()
{
    if (CHARINFO* selected = GetSelectedCharacterInfo())
        selected->ChangeSkinTexture();
}

void SetHandsState(HMODEL__*, int itemSlot, int inventoryType, int)
{
    if (itemSlot != 0 && (inventoryType == 15 || (inventoryType > 15 && inventoryType <= 17)))
        SetHandState(static_cast<unsigned>(inventoryType), itemSlot,
                     inventoryType == 15 ? 8U : 13U, nullptr);
}

void SetHandState(unsigned invType, int itemSlot, unsigned startFinger, HMODEL__* lastFinger)
{
    if (itemSlot == 14)
        ResetFingersSeq(nullptr, nullptr, invType, reinterpret_cast<CModel*>(lastFinger));
    else
        SetFingersSeq(nullptr, invType, startFinger, reinterpret_cast<CModel*>(lastFinger));
}

void SetFingersSeq(CModel*, unsigned, unsigned, CModel*)
{
}

void ResetFingersSeq(CModel*, CModel**, unsigned, CModel*)
{
}

void CCharSelectInfo::SelectCharacter(int index)
{
    if (index < 0 || index >= static_cast<int>(s_charList.size()))
        index = 0;
    m_selectionIndex = static_cast<unsigned>(index);
    UpdateCharacterInfo();
    FrameScript_SignalEvent(7, "%d", m_selectionIndex + 1);
}

unsigned CCharSelectInfo::GetNumCharacters()
{
    return static_cast<unsigned>(s_charList.size());
}

void CCharSelectInfo::SetCharacterList(const CHARACTER_INFO* records, std::size_t count)
{
    Initialize();
    const std::size_t limited = std::min<std::size_t>(count, 10);
    for (std::size_t i = 0; i < limited; ++i)
        EnumerateCharactersCallback(const_cast<CHARACTER_INFO*>(records + i), nullptr);
}

const std::vector<CHARINFO>& CCharSelectInfo::GetCharacterList()
{
    return s_charList;
}

int Script_SetCharSelectModelFrame(lua_State* state)
{
    const char* name = luaL_checkstring(state, 1);
    if (auto* frame = dynamic_cast<CSimpleModel*>(SimpleFrameRegistryGetEntry(nullptr, name)))
        CCharSelectInfo::SetModelFrame(frame);
    return 0;
}

int Script_SetCharSelectBackground(lua_State* state)
{
    const char* filename = luaL_checkstring(state, 1);
    CCharSelectInfo::SetBackgroundModel(filename);
    return 0;
}

int Script_GetCharacterListUpdate(lua_State*)
{
    CCharSelectInfo::ClearCharacterModel();
    CCharSelectInfo::ClearPetModel();
    CGlueMgr::GetCharacterList();
    return 0;
}

int Script_GetNumCharacters(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(CCharSelectInfo::GetNumCharacters()));
    return 1;
}

int Script_GetCharacterInfo(lua_State* state)
{
    const int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    const auto& characters = CCharSelectInfo::GetCharacterList();
    if (index < 0 || static_cast<std::size_t>(index) >= characters.size()) {
        lua_pushnil(state);
        lua_pushnil(state);
        lua_pushnil(state);
        lua_pushnumber(state, 0.0);
        lua_pushnil(state);
        lua_pushnil(state);
        return 6;
    }

    const CHARACTER_INFO& info = characters[static_cast<std::size_t>(index)].m_characterInfo;
    lua_pushstring(state, info.name);
    lua_pushstring(state, RaceName(info.raceID));
    lua_pushstring(state, ClassName(info.classID));
    lua_pushnumber(state, static_cast<lua_Number>(info.experienceLevel));
    // AreaTable is populated by the client data service later in startup.
    lua_pushnil(state);
    lua_pushstring(state, RaceClientName(info.raceID));
    return 6;
}

int Script_SelectCharacter(lua_State* state)
{
    int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    if (index < -1 || index >= static_cast<int>(CCharSelectInfo::GetNumCharacters()))
        index = -1;
    CCharSelectInfo::SelectCharacter(index);
    return 0;
}

int Script_DeleteCharacter(lua_State* state)
{
    const int index = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    const auto& characters = CCharSelectInfo::GetCharacterList();
    if (index >= 0 && static_cast<std::size_t>(index) < characters.size())
        CGlueMgr::DeleteCharacter(characters[static_cast<std::size_t>(index)].m_characterInfo.guid);
    return 0;
}

namespace {
const std::array<ScriptFunction, 7> s_scriptFunctions{{
    {"SetCharSelectModelFrame", Script_SetCharSelectModelFrame},
    {"SetCharSelectBackground", Script_SetCharSelectBackground},
    {"GetCharacterListUpdate", Script_GetCharacterListUpdate},
    {"GetNumCharacters", Script_GetNumCharacters},
    {"GetCharacterInfo", Script_GetCharacterInfo},
    {"SelectCharacter", Script_SelectCharacter},
    {"DeleteCharacter", Script_DeleteCharacter},
}};
}

void CharSelectRegisterScriptFunctions()
{
    if (s_registered) return;
    for (const auto& entry : s_scriptFunctions)
        FrameScript_RegisterFunction(entry.name, entry.method);
    s_registered = true;
}

void CharSelectUnregisterScriptFunctions()
{
    if (!s_registered) return;
    for (const auto& entry : s_scriptFunctions)
        FrameScript_UnregisterFunction(entry.name);
    s_registered = false;
}

int _E2_88() { return _E1_88(); }
int _E1_88() { INFINITY_88 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_88() { _E4_88(); return 0; }
void _E4_88() { TWO_PI_88 = 3.1415927F + 3.1415927F; }
int _E8_88() { _E7_88(); return 0; }
void _E7_88() { OO_TWO_PI_88 = 1.0F / TWO_PI_88; }

#define CHARSELECT_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
CHARSELECT_CONST_PAIR(_E14_11, _E13_6, MAX_DUEL_DISTANCE_SQUARED_19, 100.0F)
CHARSELECT_CONST_PAIR(_E17_9, _E16_13, MAX_TRADE_DISTANCE_SQUARED_19, 123.45679F)
CHARSELECT_CONST_PAIR(_E20_27, _E19_14, MAX_SHOP_DISTANCE_SQUARED_19, 30.864195F)
CHARSELECT_CONST_PAIR(_E23_29, _E22_20, MAX_INSPECT_DISTANCE_SQUARED_19, 30.864195F)
CHARSELECT_CONST_PAIR(_E26_25, _E25_18, MAX_LOOT_DISTANCE_SQUARED_19, 25.0F)
CHARSELECT_CONST_PAIR(_E29_24, _E28_12, MAX_BIND_DISTANCE_SQUARED_19, 100.0F)
CHARSELECT_CONST_PAIR(_E32_22, _E31_9, MAX_SITCHAIR_DISTANCE_SQUARED_19, 0.25F)
CHARSELECT_CONST_PAIR(_E35_22, _E34_10, MAX_SITCHAIRUSE_DISTANCE_SQUARED_19, 9.0F)
CHARSELECT_CONST_PAIR(_E38_22, _E37_9, MAX_SHARE_DISTANCE_SQUARED_19, 10000.0F)
CHARSELECT_CONST_PAIR(_E41_20, _E40_10, SECONDS_PER_HEARTBEAT_19, 5.0F)
CHARSELECT_CONST_PAIR(_E44_22, _E43_9, SPELL_FOCUS_DISTANCE_SQUARED_19, 2500.0F)
CHARSELECT_CONST_PAIR(_E47_24, _E46_7, MAX_OBJ_INTEREST_RADIUS_SQUARED_19, 10000.0F)
#undef CHARSELECT_CONST_PAIR

int _E83_3() { _E80_4(); return _E82_4(); }
void _E80_4() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E82_4() { return std::atexit(_E81_3); }
void _E81_3() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }

int _E106_3() { _E103_3(); return _E105_1(); }
int _E103_3() { s_charList.clear(); return 0; }
int _E105_1() { return std::atexit(_E104_1); }
void _E104_1() { CCharSelectInfo::Shutdown(); }
