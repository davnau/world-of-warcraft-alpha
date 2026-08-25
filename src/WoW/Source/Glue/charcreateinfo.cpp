#include "WoW/Source/Glue/charcreateinfo.h"

#include "ENGINE/Source/Frame/frame.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Anim/anim.h"
#include "ENGINE/Source/Model/modelanim.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/modelmisc.h"
#include "ENGINE/Source/Services/texture.h"
#include "WoW/common/componentcore/charactercustomizationcore.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"
#include "WoW/Source/Net/netinternal.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>

float INFINITY_87 = 0.0F;
float TWO_PI_87 = 0.0F;
float OO_TWO_PI_87 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_18 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_18 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_18 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_18 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_18 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_18 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_18 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_18 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_18 = 0.0F;
float SECONDS_PER_HEARTBEAT_18 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_18 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_18 = 0.0F;

CHARCREATEINFO CCharCreateInfo::m_charInfo{};
CSimpleModel* CCharCreateInfo::m_charCustomizeFrame = nullptr;
unsigned CCharCreateInfo::m_selectedRace = 0;
unsigned CCharCreateInfo::m_selectedSex = 0;
unsigned CCharCreateInfo::m_selectedClass = 0;
float CCharCreateInfo::m_charFacing = 0.0F;
std::vector<unsigned> CCharCreateInfo::m_factionIndex;
std::vector<unsigned> CCharCreateInfo::m_raceIndex;
std::vector<unsigned> CCharCreateInfo::m_classIndex;

namespace {
std::vector<std::string> g_raceNames;
std::vector<std::string> g_classNames;
std::array<unsigned, 2> g_outfitCounts{1, 1};
bool g_charCreateScriptsRegistered = false;
std::uint32_t g_randomState = 0x12345678U;

struct ScriptFunction {
    const char* name;
    lua_CFunction method;
};

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
    default: return nullptr;
    }
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
    default: return nullptr;
    }
}

const std::vector<unsigned>& ClassesForRace(unsigned race)
{
    // Exact (race, class) pairs from DBFilesClient\\CharBaseInfo.dbc.
    static const std::array<std::vector<unsigned>, 8> classes = {{
        {1, 2, 4, 5, 8, 9},
        {1, 3, 4, 7, 9},
        {1, 2, 3, 5, 4},
        {1, 3, 4, 8, 5, 11},
        {1, 4, 5, 8, 9},
        {1, 3, 7, 11},
        {1, 4, 8, 9},
        {1, 4, 3, 5, 7},
    }};
    static const std::vector<unsigned> empty;
    return race >= 1U && race <= classes.size() ? classes[race - 1U] : empty;
}

struct TextureSectionRect {
    unsigned int x;
    unsigned int y;
    unsigned int width;
    unsigned int height;
};

// Exact s_sectionTable entries recovered from WoWClient.exe at VA 0x806C8C.
constexpr std::array<TextureSectionRect, NUM_TEXCOMPONENT_SECTIONS> kTextureSections{{
    {0, 0, 128, 64}, {0, 64, 128, 64}, {0, 128, 128, 32},
    {0, 160, 128, 32}, {0, 192, 128, 64}, {128, 0, 128, 64},
    {128, 64, 128, 32}, {128, 96, 128, 64}, {128, 160, 128, 64},
    {128, 224, 128, 32},
}};

bool CopyTextureImage(CTexture* destination, const CTexture* source) {
    if (!destination || !source || destination->mipBits.empty() || source->mipBits.empty())
        return false;
    auto& output = destination->mipBits[0];
    const auto& input = source->mipBits[0];
    if (!output.width || !output.height || !input.width || !input.height ||
        output.bits.size() < static_cast<std::size_t>(output.width) * output.height * 4U ||
        input.bits.size() < static_cast<std::size_t>(input.width) * input.height * 4U)
        return false;
    for (unsigned int y = 0; y < output.height; ++y) {
        const unsigned int sourceY = y * input.height / output.height;
        for (unsigned int x = 0; x < output.width; ++x) {
            const unsigned int sourceX = x * input.width / output.width;
            std::memcpy(output.bits.data() + (static_cast<std::size_t>(y) * output.width + x) * 4U,
                        input.bits.data() + (static_cast<std::size_t>(sourceY) * input.width + sourceX) * 4U,
                        4U);
        }
    }
    return true;
}

bool CompositeTexture(CTexture* destination, const char* filename,
                      TEXCOMPONENT_SECTIONS section) {
    if (!destination || !filename || !*filename || section < 0 ||
        section >= NUM_TEXCOMPONENT_SECTIONS || destination->mipBits.empty())
        return false;
    CTexture* source = TextureCreate(filename, nullptr, 0U, 1);
    if (!source || source->mipBits.empty()) {
        TextureRelease(source);
        return false;
    }
    auto& output = destination->mipBits[0];
    const auto& input = source->mipBits[0];
    const auto& rect = kTextureSections[section];
    if (output.bits.size() < static_cast<std::size_t>(output.width) * output.height * 4U ||
        input.bits.size() < static_cast<std::size_t>(input.width) * input.height * 4U ||
        !input.width || !input.height) {
        TextureRelease(source);
        return false;
    }
    const unsigned int width = (std::min)(rect.width, output.width > rect.x ? output.width - rect.x : 0U);
    const unsigned int height = (std::min)(rect.height, output.height > rect.y ? output.height - rect.y : 0U);
    for (unsigned int y = 0; y < height; ++y) {
        const unsigned int sourceY = y * input.height / rect.height;
        for (unsigned int x = 0; x < width; ++x) {
            const unsigned int sourceX = x * input.width / rect.width;
            const auto* foreground = input.bits.data() +
                (static_cast<std::size_t>(sourceY) * input.width + sourceX) * 4U;
            auto* background = output.bits.data() +
                (static_cast<std::size_t>(rect.y + y) * output.width + rect.x + x) * 4U;
            const unsigned int alpha = foreground[3];
            const unsigned int inverse = 255U - alpha;
            for (unsigned int channel = 0; channel < 3; ++channel)
                background[channel] = static_cast<std::uint8_t>(
                    (foreground[channel] * alpha + background[channel] * inverse + 127U) / 255U);
            background[3] = static_cast<std::uint8_t>(
                alpha + (background[3] * inverse + 127U) / 255U);
        }
    }
    TextureRelease(source);
    return true;
}

CTexture* BuildCharacterSkin(unsigned int race, unsigned int sex,
                             const CustomizationSelections& selection) {
    const char* baseName = CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_SKIN, 0, selection.skinColor);
    if (!baseName) return nullptr;
    CTexture* base = TextureCreate(baseName, nullptr, 0U, 0);
    if (!base || base->mipBits.empty()) return base;

    CTexture* result = TextureCreate(256U, 256U, GxTex_Argb8888, 0U);
    if (!result || !CopyTextureImage(result, base)) {
        TextureRelease(result);
        return base;
    }
    result->filename = std::string(baseName) + "#character-composite";
    result->opaque = base->opaque;
    TextureRelease(base);

    CompositeTexture(result, CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_FACEUPPER, selection.face,
        selection.skinColor), TCS_UPPERHEAD);
    CompositeTexture(result, CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_FACELOWER, selection.face,
        selection.skinColor), TCS_LOWERHEAD);

    // The recovered compositor sorts these by TEXLAYER: skin, cloth, armor.
    CompositeTexture(result, CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_FACIALUPPERHAIR,
        selection.facialStyle, selection.hairColor), TCS_UPPERHEAD);
    CompositeTexture(result, CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_FACIALLOWERHAIR,
        selection.facialStyle, selection.hairColor), TCS_LOWERHEAD);
    CompositeTexture(result, CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_SCALPUPPERHAIR,
        selection.hairStyle, selection.hairColor), TCS_UPPERHEAD);
    CompositeTexture(result, CharCustomizationGetTextureName(
        race, sex, CHARTEXTURESECTION_SCALPLOWERHAIR,
        selection.hairStyle, selection.hairColor), TCS_LOWERHEAD);
    return result;
}

CModel* CharacterCreateBackground()
{
    if (!CCharCreateInfo::m_charCustomizeFrame) return nullptr;
    return CCharCreateInfo::m_charCustomizeFrame->m_modelOwner
        ? CCharCreateInfo::m_charCustomizeFrame->m_modelOwner.get()
        : reinterpret_cast<CModel*>(CCharCreateInfo::m_charCustomizeFrame->m_model);
}

void AttachSelectedCharacter()
{
    auto* background = CharacterCreateBackground();
    const unsigned sex = std::min(CCharCreateInfo::m_selectedSex, 1U);
    auto* character = CCharCreateInfo::m_charInfo.characterModelOwner[sex].get();
    if (!background || !character || !ModelClearLink(*background, 0U)) return;
    if (!ModelAddLink(*background, 0U, *character, 1.0F)) return;

    const NTempest::C3Vector facing{
        std::cos(CCharCreateInfo::m_charFacing),
        std::sin(CCharCreateInfo::m_charFacing), 0.0F};
    if (background->shared && background->animData &&
        !background->shared->attachIdToIndex.empty()) {
        const auto attachment = background->shared->attachIdToIndex[0];
        if (attachment != ~0U) {
            const auto objectId = AnimGetAttachmentObjId(*background->animData, attachment);
            ModelApplyObjectFaceDir(*background, objectId, facing);
        }
    }
}
}

void CHARCREATEINFO::Shutdown(HOBJECT__*)
{
    for (unsigned sex = 0; sex < 2; ++sex) {
        delete reinterpret_cast<CCharGeoset*>(geosetHandle[sex]);
        TextureRelease(skinTexture[sex]);
        TextureRelease(hairTexture[sex]);
        characterModelOwner[sex].reset();
        characterModel[sex] = nullptr;
        geosetHandle[sex] = nullptr;
        characterComponent[sex] = nullptr;
        skinTexture[sex] = nullptr;
        hairTexture[sex] = nullptr;
    }
}

void CHARCREATEINFO::UpdateOutfit(int increment, unsigned race, unsigned sex)
{
    if (sex >= 2)
        return;
    const unsigned count = CCharCreateInfo::GetNumOutfits(28U * sex, race, sex);
    if (!count)
        return;
    const int next = static_cast<int>(selections[sex].outfit) + increment;
    selections[sex].outfit = static_cast<unsigned>((next % static_cast<int>(count) + count) % count);
    ChangeFaceTexture(reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(race)), sex);
    ChangeFacialHairTexture(reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(race)), sex);
    ChangeScalpHairTexture(reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(race)), sex);
}

void CHARCREATEINFO::ResetOutfitSelection(unsigned raceID, unsigned sex)
{
    if (sex >= 2)
        return;
    const unsigned count = CCharCreateInfo::GetNumOutfits(28U * sex, raceID, sex);
    selections[sex].outfit = count ? selections[sex].outfit % count : 0;
}

void CHARCREATEINFO::CommitGeoset(HCHARGEOSET__* geoset, unsigned sex)
{
    if (sex < 2)
        CharCustomizationCommitGeosets(reinterpret_cast<CCharGeoset*>(geoset));
}

void CHARCREATEINFO::UpdateCharacterInfo(unsigned, unsigned race, unsigned sex)
{
    if (sex >= 2)
        return;
    if (!characterModelOwner[sex]) {
        const char* modelName = CharacterModelName(race, sex);
        if (!modelName) return;
        characterModelOwner[sex] = ModelCreate(modelName);
        if (!characterModelOwner[sex] || !ModelIsLoaded(*characterModelOwner[sex])) {
            characterModelOwner[sex].reset();
            return;
        }
        characterModel[sex] = reinterpret_cast<HMODEL__*>(characterModelOwner[sex].get());
        ModelSetSequence(*characterModelOwner[sex], 0U);
    }
    if (!geosetHandle[sex]) {
        geosetHandle[sex] = CharCustomizationCreateGeosetHandle(characterModel[sex]);
        if (!geosetHandle[sex]) return;
        CharCustomizationInitBaseCharacter(
            reinterpret_cast<CCharGeoset*>(geosetHandle[sex]),
            g_defaultGeosetIDOffsets[CHARGEOSET_BEARD],
            g_defaultGeosetIDOffsets[CHARGEOSET_SIDEBURN],
            g_defaultGeosetIDOffsets[CHARGEOSET_MOUSTACHE], 2U);
    }
    ChangeSkinTexture(nullptr, 0, geosetHandle[sex], sex);
    ChangeFaceTexture(reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(race)), sex);
    ChangeFacialHairTexture(reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(race)), sex);
    ChangeScalpHairTexture(reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(race)), sex);
    ChangeHairGeosets(race, sex);
    BEARDSTYLEDATA facialStyle{};
    if (CharCustomizationGetBeardStyle(race, sex, selections[sex].facialStyle,
                                       &facialStyle)) {
        ChangeFacialHairGeosets(
            sex,
            reinterpret_cast<HCHARGEOSET__*>(static_cast<std::uintptr_t>(facialStyle.beardGeoset)),
            facialStyle.sideBurnGeoset, facialStyle.moustacheGeoset);
    }
}

void CHARCREATEINFO::ChangeHairGeosets(unsigned race, unsigned sex)
{
    if (sex < 2) {
        currentGeosets[sex][0] = selections[sex].hairStyle;
        CharCustomizationResetHairGeoset(
            reinterpret_cast<CCharGeoset*>(geosetHandle[sex]), race, sex,
            selections[sex].hairStyle);
    }
}

void CHARCREATEINFO::UpdateEquipment(CModel*, int, HCHARGEOSET__*, unsigned sex)
{
    if (sex < 2)
        currentGeosets[sex][1] = selections[sex].outfit;
}

void CHARCREATEINFO::ChangeSkinTexture(HOBJECT__*, int doNotCommitGeosets,
                                       HCHARGEOSET__*, unsigned sex)
{
    if (sex >= 2)
        return;
    characterComponent[sex] = reinterpret_cast<HTEXCOMPONENT__*>(static_cast<std::uintptr_t>(1));
    UpdateEquipment(nullptr, doNotCommitGeosets, geosetHandle[sex], sex);
}

void CHARCREATEINFO::ChangeFaceTexture(HMODEL__*, unsigned sex)
{
    if (sex < 2 && characterComponent[sex])
        currentGeosets[sex][2] = selections[sex].face;
}

void CHARCREATEINFO::ChangeFacialHairTexture(HMODEL__*, unsigned sex)
{
    if (sex < 2 && characterComponent[sex])
        currentGeosets[sex][3] = selections[sex].facialStyle;
}

void CHARCREATEINFO::ChangeFacialHairGeosets(unsigned sex, HCHARGEOSET__*,
                                             unsigned sideburn, unsigned moustache)
{
    if (sex < 2) {
        currentGeosets[sex][4] = sideburn;
        currentGeosets[sex][5] = moustache;
        auto* geoset = reinterpret_cast<CCharGeoset*>(geosetHandle[sex]);
        if (geoset) {
            BEARDSTYLEDATA style{};
            CharCustomizationGetBeardStyle(CCharCreateInfo::GetSelectedRaceID(), sex,
                                            selections[sex].facialStyle, &style);
            geoset->ShowGeosetSection(CHARGEOSET_BEARD, style.beardGeoset, 1);
            sideburn = style.sideBurnGeoset;
            moustache = style.moustacheGeoset;
            geoset->ShowGeosetSection(CHARGEOSET_SIDEBURN, sideburn, 0);
            geoset->ShowGeosetSection(CHARGEOSET_MOUSTACHE, moustache, 0);
        }
    }
}

void CHARCREATEINFO::ChangeScalpHairTexture(HMODEL__*, unsigned sex)
{
    if (sex < 2 && characterComponent[sex])
        currentGeosets[sex][6] = selections[sex].hairStyle;
}

void CHARCREATEINFO::UpdateGeosets(unsigned beard, unsigned sideburn,
                                   HCHARGEOSET__* moustache, unsigned sex)
{
    if (sex < 2) {
        ChangeFacialHairGeosets(sex, moustache, beard, sideburn);
        CharCustomizationInitBaseCharacter(
            reinterpret_cast<CCharGeoset*>(geosetHandle[sex]), beard, sideburn,
            static_cast<unsigned>(reinterpret_cast<std::uintptr_t>(moustache)), 2U);
    }
}

void CHARCREATEINFO::FindRange(unsigned group, unsigned* start, unsigned* end)
{
    if (start) *start = 100U * group + 1U;
    if (end) *end = 100U * group + 99U;
}

void CHARCREATEINFO::CommitTexture(int race, unsigned sex)
{
    if (sex >= 2U || !characterModelOwner[sex]) return;
    const auto& selection = selections[sex];
    auto* newSkin = BuildCharacterSkin(static_cast<unsigned int>(race), sex, selection);
    const char* hairName = CharCustomizationGetTextureName(
        static_cast<unsigned int>(race), sex, CHARTEXTURESECTION_HAIR,
        selection.hairStyle, selection.hairColor);
    auto* newHair = hairName ? TextureCreate(hairName, nullptr, 0U, 0) : nullptr;
    ComplexModelReplaceTexture(*characterModelOwner[sex], 1U, newSkin, false);
    ComplexModelReplaceTexture(*characterModelOwner[sex], 6U, newHair, false);
    TextureRelease(skinTexture[sex]);
    TextureRelease(hairTexture[sex]);
    skinTexture[sex] = newSkin;
    hairTexture[sex] = newHair;
}

void CCharCreateInfo::Initialize()
{
    m_charInfo = CHARCREATEINFO{};
    m_raceIndex = {1, 2, 3, 4, 5, 6, 7, 8};
    m_classIndex = ClassesForRace(1);
    m_factionIndex = {1, 2};
    g_raceNames = {"Human", "Orc", "Dwarf", "Night Elf", "Undead", "Tauren", "Gnome", "Troll"};
    g_classNames.clear();
    for (const unsigned classID : m_classIndex)
        g_classNames.emplace_back(ClassName(classID));
    m_selectedRace = m_selectedClass = m_selectedSex = 0;
    if (!m_classIndex.empty()) {
        m_charInfo.selections[0].classID = m_classIndex[0];
        m_charInfo.selections[1].classID = m_classIndex[0];
    }
    m_charFacing = 0.0F;
    g_outfitCounts = {1, 1};
}

void CCharCreateInfo::Shutdown()
{
    m_charInfo.Shutdown();
    m_factionIndex.clear();
    m_raceIndex.clear();
    m_classIndex.clear();
    g_raceNames.clear();
    g_classNames.clear();
}

void CCharCreateInfo::SetCharCustomizeFrame(CSimpleModel* frame)
{
    m_charCustomizeFrame = frame;
}

void CCharCreateInfo::SetCharCustomizeModel(const char* filename)
{
    if (!m_charCustomizeFrame || !filename || !*filename) return;
    CModelCreate createData{};
    createData.flags = 4U;
    m_charCustomizeFrame->SetModel(filename, &createData, nullptr);
}

unsigned CCharCreateInfo::GetNumOutfits(unsigned, int, unsigned sex)
{
    return sex < 2 ? g_outfitCounts[sex] : 0;
}

const void* CCharCreateInfo::GetOutfit(unsigned, int, unsigned sex, unsigned outfitID)
{
    return sex < 2 && outfitID < g_outfitCounts[sex]
        ? static_cast<const void*>(&g_outfitCounts[sex]) : nullptr;
}

void CCharCreateInfo::ResetCharCustomizeInfo()
{
    m_selectedRace = 0;
    m_selectedClass = 0;
    m_selectedSex = 0;
    m_charInfo.selections[0] = {};
    m_charInfo.selections[1] = {};
    UpdateAvailableClasses();
    if (!m_classIndex.empty()) {
        m_charInfo.selections[0].classID = m_classIndex[0];
        m_charInfo.selections[1].classID = m_classIndex[0];
    }
    InitializeCharacterInfo(0, 0);
}

unsigned RandomSelection(unsigned count)
{
    if (!count)
        return 0;
    g_randomState = g_randomState * 1664525U + 1013904223U;
    return g_randomState % count;
}

void CCharCreateInfo::SetCharFacing(float facing)
{
    m_charFacing = facing;
    AttachSelectedCharacter();
}

const char* CCharCreateInfo::GetRaceNameByIndex(unsigned index)
{
    return index < g_raceNames.size() ? g_raceNames[index].c_str() : nullptr;
}

void CCharCreateInfo::UpdateAvailableClasses()
{
    m_classIndex = ClassesForRace(GetSelectedRaceID());
    g_classNames.clear();
    for (const unsigned classID : m_classIndex) {
        const char* name = ClassName(classID);
        g_classNames.emplace_back(name ? name : "");
    }
    if (m_selectedClass >= m_classIndex.size())
        m_selectedClass = 0;
}

const char* CCharCreateInfo::GetClassNameByIndex(unsigned index)
{
    return index < g_classNames.size() ? g_classNames[index].c_str() : nullptr;
}

unsigned CCharCreateInfo::GetSelectedRaceID()
{
    return m_selectedRace < m_raceIndex.size() ? m_raceIndex[m_selectedRace] : 0;
}

unsigned CCharCreateInfo::GetSelectedSexID() { return m_selectedSex; }
unsigned CCharCreateInfo::GetSelectedClassID()
{
    return m_selectedClass < m_classIndex.size() ? m_classIndex[m_selectedClass] : 0;
}

void CCharCreateInfo::UpdateAllCharacterInfo(unsigned race, int sex, unsigned updateModel)
{
    InitializeCharacterInfo(updateModel, race);
    m_charInfo.CommitGeoset(m_charInfo.geosetHandle[m_selectedSex], m_selectedSex);
    (void)sex;
}

void CCharCreateInfo::InitializeCharacterInfo(unsigned doNotCommitGeosets, unsigned modelFlags)
{
    const unsigned sex = std::min(m_selectedSex, 1U);
    m_charInfo.UpdateCharacterInfo(modelFlags, GetSelectedRaceID(), sex);
    if (!doNotCommitGeosets)
        m_charInfo.CommitTexture(static_cast<int>(GetSelectedRaceID()), sex);
}

void CCharCreateInfo::UpdateCharacterInfo(unsigned flags)
{
    InitializeCharacterInfo(flags, GetSelectedRaceID());
}

void CCharCreateInfo::UpdateEquipment(int sex, unsigned flags)
{
    if (sex < 0 || sex >= 2) return;
    m_charInfo.UpdateEquipment(nullptr, static_cast<int>(flags),
                               m_charInfo.geosetHandle[sex], static_cast<unsigned>(sex));
}

void CCharCreateInfo::ChangeSkinTexture(int sex, unsigned flags)
{
    if (sex >= 2) return;
    m_charInfo.ChangeSkinTexture(nullptr, static_cast<int>(flags),
                                 m_charInfo.geosetHandle[sex], static_cast<unsigned>(sex));
}

void CCharCreateInfo::ChangeFaceTexture(unsigned sex)
{
    if (sex < 2) m_charInfo.ChangeFaceTexture(nullptr, sex);
}

void CCharCreateInfo::ChangeFacialHairTexture(unsigned sex)
{
    if (sex < 2) m_charInfo.ChangeFacialHairTexture(nullptr, sex);
}

void CCharCreateInfo::ChangeFacialHairGeosets(unsigned sex)
{
    if (sex < 2) m_charInfo.ChangeFacialHairGeosets(sex, nullptr, 0, 0);
}

void CCharCreateInfo::ChangeScalpHairTexture(unsigned sex)
{
    if (sex < 2) m_charInfo.ChangeScalpHairTexture(nullptr, sex);
}

void CCharCreateInfo::ChangeHairGeosets(unsigned sex)
{
    if (sex < 2) m_charInfo.ChangeHairGeosets(GetSelectedRaceID(), sex);
}

void CCharCreateInfo::UpdateGeosets(unsigned sex)
{
    if (sex < 2) m_charInfo.UpdateGeosets(0, 0, nullptr, sex);
}

void CCharCreateInfo::CommitCurrentGeoset(HCHARGEOSET__* geoset, unsigned sex)
{
    m_charInfo.CommitGeoset(geoset, sex);
}

void CCharCreateInfo::SetSelectedRace(unsigned race, int updateModel)
{
    if (!m_raceIndex.empty())
        m_selectedRace = std::min(race, static_cast<unsigned>(m_raceIndex.size() - 1));
    UpdateAvailableClasses();
    if (updateModel) {
        m_charInfo.Shutdown();
        m_selectedClass = 0;
        const unsigned classID = GetSelectedClassID();
        m_charInfo.selections[0] = {};
        m_charInfo.selections[1] = {};
        for (unsigned sex = 0; sex < 2; ++sex) {
            auto& selection = m_charInfo.selections[sex];
            selection.classID = classID;
            int skins = 0;
            int faces = 0;
            CharCustomizationGetNumSkinTextures(GetSelectedRaceID(), sex, &skins, nullptr);
            CharCustomizationNumFaces(GetSelectedRaceID(), sex, &faces, nullptr);
            selection.skinColor = RandomSelection(static_cast<unsigned>((std::max)(skins, 0)));
            selection.hairColor = RandomSelection(
                CharCustomizationNumHairColors(GetSelectedRaceID(), sex));
            selection.hairStyle = RandomSelection(
                CharCustomizationNumHairStyles(GetSelectedRaceID(), sex));
            selection.facialStyle = RandomSelection(
                CharCustomizationNumBeardStyles(GetSelectedRaceID(), sex));
            selection.face = RandomSelection(static_cast<unsigned>((std::max)(faces, 0)));
        }
        UpdateAllCharacterInfo(GetSelectedRaceID(), static_cast<int>(m_selectedSex), 0);
        AttachSelectedCharacter();
    }
}

void CCharCreateInfo::SetSelectedSex(unsigned sex)
{
    m_selectedSex = std::min(sex, 1U);
    UpdateAllCharacterInfo(GetSelectedRaceID(), static_cast<int>(m_selectedSex), 0);
    AttachSelectedCharacter();
}

void CCharCreateInfo::SetSelectedClass(unsigned classIndex)
{
    if (classIndex >= m_classIndex.size())
        return;
    if (classIndex == m_selectedClass) {
        const unsigned classID = m_classIndex[classIndex];
        m_charInfo.selections[0].classID = classID;
        m_charInfo.selections[1].classID = classID;
        return;
    }
    m_selectedClass = classIndex;
    const unsigned classID = GetSelectedClassID();
    m_charInfo.selections[0].classID = classID;
    m_charInfo.selections[1].classID = classID;
    const unsigned raceID = GetSelectedRaceID();
    m_charInfo.ResetOutfitSelection(raceID, m_selectedSex);
    UpdateEquipment(static_cast<int>(m_selectedSex), 1U);
    m_charInfo.UpdateOutfit(0, raceID, m_selectedSex);
    m_charInfo.CommitTexture(static_cast<int>(raceID), m_selectedSex);
    CommitCurrentGeoset(m_charInfo.geosetHandle[m_selectedSex], m_selectedSex);
}

unsigned CCharCreateInfo::GetNumCharCustomizations(int customization)
{
    const unsigned race = GetSelectedRaceID();
    const unsigned sex = m_selectedSex;
    if (!race || sex >= 2U) return 0;
    int count = 0;
    switch (customization) {
    case 0:
        CharCustomizationGetNumSkinTextures(race, sex, &count, nullptr);
        break;
    case 1:
        CharCustomizationNumFaces(race, sex, &count, nullptr);
        break;
    case 2:
        return CharCustomizationNumHairStyles(race, sex);
    case 3:
        return CharCustomizationNumHairColors(race, sex);
    case 4:
        return CharCustomizationNumBeardStyles(race, sex);
    case 5:
        return GetNumOutfits(28U * sex, static_cast<int>(race), sex);
    default:
        return 0;
    }
    return static_cast<unsigned>((std::max)(count, 0));
}

void CCharCreateInfo::CycleCharCustomization(int customization, int delta)
{
    if (delta == 0 || customization < 0 || customization > 5)
        return;
    if (customization == 5) {
        m_charInfo.UpdateOutfit(delta, GetSelectedRaceID(), m_selectedSex);
        UpdateAllCharacterInfo(GetSelectedRaceID(), static_cast<int>(m_selectedSex), 0);
        return;
    }
    const unsigned count = GetNumCharCustomizations(customization);
    if (count < 2U)
        return;
    auto& selection = m_charInfo.selections[m_selectedSex];
    unsigned* value = nullptr;
    switch (customization) {
    case 0: value = &selection.skinColor; break;
    case 1: value = &selection.face; break;
    case 2: value = &selection.hairStyle; break;
    case 3: value = &selection.hairColor; break;
    case 4: value = &selection.facialStyle; break;
    default: break;
    }
    if (value) {
        const int next = static_cast<int>(*value) + delta;
        *value = static_cast<unsigned>((next % static_cast<int>(count) +
                                        static_cast<int>(count)) % static_cast<int>(count));
    }
    UpdateAllCharacterInfo(GetSelectedRaceID(), static_cast<int>(m_selectedSex), 0);
}

void CCharCreateInfo::RandomizeCharCustomization()
{
    auto& selection = m_charInfo.selections[m_selectedSex];
    selection.outfit = RandomSelection(std::max(1U, g_outfitCounts[m_selectedSex]));
    selection.skinColor = RandomSelection(GetNumCharCustomizations(0));
    selection.hairColor = RandomSelection(GetNumCharCustomizations(3));
    selection.hairStyle = RandomSelection(GetNumCharCustomizations(2));
    selection.facialStyle = RandomSelection(GetNumCharCustomizations(4));
    selection.face = RandomSelection(GetNumCharCustomizations(1));
    UpdateAllCharacterInfo(GetSelectedRaceID(), static_cast<int>(m_selectedSex), 0);
}

void CCharCreateInfo::CreateCharacter(const char* name)
{
    const int result = ClientServices_CharacterValidateName(name);
    if (result != 65) {
        const char* token = ClientServices_GetErrorToken(result);
        const char* text = token ? FrameScript_GetText(
            token, -1, GENDER_NOT_APPLICABLE) : nullptr;
        FrameScript_SignalEvent(3, "%s%s", "OKAY",
                                text && *text ? text : "Invalid character name.");
        return;
    }

    CHARACTER_CREATE_INFO createInfo{};
    std::snprintf(createInfo.name, sizeof(createInfo.name), "%s", name);
    createInfo.raceID = static_cast<std::uint8_t>(GetSelectedRaceID());
    createInfo.sexID = static_cast<std::uint8_t>(m_selectedSex);
    const CustomizationSelections& selection = m_charInfo.selections[m_selectedSex];
    createInfo.classID = static_cast<std::uint8_t>(selection.classID);
    createInfo.skinID = static_cast<std::uint8_t>(selection.skinColor);
    createInfo.faceID = static_cast<std::uint8_t>(selection.face);
    createInfo.hairStyleID = static_cast<std::uint8_t>(selection.hairStyle);
    createInfo.hairColorID = static_cast<std::uint8_t>(selection.hairColor);
    createInfo.facialHairStyleID = static_cast<std::uint8_t>(selection.facialStyle);
    createInfo.outfitID = static_cast<std::uint8_t>(selection.outfit);
    CGlueMgr::CreateCharacter(&createInfo);
}

void CCharCreateInfo::SetAvailableRaces(const std::vector<unsigned>& ids,
                                        const std::vector<std::string>& names)
{
    m_raceIndex = ids;
    g_raceNames = names;
}

void CCharCreateInfo::SetAvailableClasses(const std::vector<unsigned>& ids,
                                          const std::vector<std::string>& names)
{
    m_classIndex = ids;
    g_classNames = names;
}

void ReportMissingComponentTextures(int, int)
{
}

int Script_SetCharCustomizeFrame(lua_State* state)
{
    const char* name = luaL_checkstring(state, 1);
    if (auto* frame = dynamic_cast<CSimpleModel*>(SimpleFrameRegistryGetEntry(nullptr, name)))
        CCharCreateInfo::SetCharCustomizeFrame(frame);
    return 0;
}

int Script_SetCharCustomizeBackground(lua_State* state)
{
    const char* filename = luaL_checkstring(state, 1);
    CCharCreateInfo::SetCharCustomizeModel(filename);
    return 0;
}

int Script_ResetCharCustomize(lua_State*)
{
    CCharCreateInfo::ResetCharCustomizeInfo();
    return 0;
}

int Script_GetNameForRace(lua_State* state)
{
    const char* name = CCharCreateInfo::GetRaceNameByIndex(CCharCreateInfo::m_selectedRace);
    const char* clientName = RaceClientName(CCharCreateInfo::GetSelectedRaceID());
    if (name) lua_pushstring(state, name); else lua_pushnil(state);
    if (clientName) lua_pushstring(state, clientName); else lua_pushnil(state);
    return 2;
}

int Script_GetFactionForRace(lua_State* state)
{
    const unsigned race = CCharCreateInfo::GetSelectedRaceID();
    if (race == 1 || race == 3 || race == 4 || race == 7) {
        lua_pushstring(state, "Alliance");
        lua_pushstring(state, "Alliance");
    } else if (race == 2 || race == 5 || race == 6 || race == 8) {
        lua_pushstring(state, "Horde");
        lua_pushstring(state, "Horde");
    } else {
        lua_pushnil(state);
        lua_pushnil(state);
    }
    return 2;
}

int Script_GetAvailableRaces(lua_State* state)
{
    for (unsigned i = 0; i < CCharCreateInfo::m_raceIndex.size(); ++i) {
        const char* name = CCharCreateInfo::GetRaceNameByIndex(i);
        if (name) lua_pushstring(state, name); else lua_pushnil(state);
    }
    return static_cast<int>(CCharCreateInfo::m_raceIndex.size());
}

int Script_GetClassesForRace(lua_State* state)
{
    for (unsigned i = 0; i < CCharCreateInfo::m_classIndex.size(); ++i) {
        const char* name = CCharCreateInfo::GetClassNameByIndex(i);
        if (name) lua_pushstring(state, name); else lua_pushnil(state);
    }
    return static_cast<int>(CCharCreateInfo::m_classIndex.size());
}

int Script_GetSelectedRace(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(CCharCreateInfo::m_selectedRace + 1));
    const char* name = CCharCreateInfo::GetRaceNameByIndex(CCharCreateInfo::m_selectedRace);
    if (name) lua_pushstring(state, name); else lua_pushnil(state);
    return 2;
}

int Script_GetSelectedSex(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(CCharCreateInfo::GetSelectedSexID() + 1));
    return 1;
}

int Script_GetSelectedClass(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(CCharCreateInfo::m_selectedClass + 1));
    const char* name = CCharCreateInfo::GetClassNameByIndex(CCharCreateInfo::m_selectedClass);
    if (name) lua_pushstring(state, name); else lua_pushnil(state);
    return 2;
}

int Script_SetSelectedRace(lua_State* state)
{
    const auto scriptIndex = static_cast<unsigned>(luaL_checknumber(state, 1));
    CCharCreateInfo::SetSelectedRace(scriptIndex - 1U, 0);
    return 0;
}

int Script_SetSelectedSex(lua_State* state)
{
    CCharCreateInfo::SetSelectedSex(static_cast<unsigned>(luaL_checknumber(state, 1)) - 1U);
    return 0;
}

int Script_SetSelectedClass(lua_State* state)
{
    const auto scriptIndex = static_cast<unsigned>(luaL_checknumber(state, 1));
    CCharCreateInfo::SetSelectedClass(scriptIndex - 1U);
    return 0;
}

int Script_UpdateCustomizationBackground(lua_State*)
{
    CCharCreateInfo::SetSelectedRace(CCharCreateInfo::m_selectedRace, 1);
    return 0;
}

int Script_HasCharCustomization(lua_State* state)
{
    const int customization = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    if (CCharCreateInfo::GetNumCharCustomizations(customization) > 1)
        lua_pushnumber(state, 1.0);
    else
        lua_pushnil(state);
    return 1;
}

int Script_CycleCharCustomization(lua_State* state)
{
    const int customization = static_cast<int>(luaL_checknumber(state, 1)) - 1;
    const int delta = static_cast<int>(luaL_checknumber(state, 2));
    CCharCreateInfo::CycleCharCustomization(customization, delta);
    return 0;
}

int Script_RandomizeCharCustomization(lua_State*)
{
    CCharCreateInfo::RandomizeCharCustomization();
    return 0;
}

int Script_GetCharacterFacing(lua_State* state)
{
    lua_pushnumber(state, static_cast<lua_Number>(CCharCreateInfo::m_charFacing * 57.29578F));
    return 1;
}

int Script_SetCharacterFacing(lua_State* state)
{
    CCharCreateInfo::SetCharFacing(static_cast<float>(luaL_checknumber(state, 1)) * 0.017453292F);
    return 1;
}

int Script_CreateCharacter(lua_State* state)
{
    CCharCreateInfo::CreateCharacter(lua_tostring(state, 1));
    return 0;
}

namespace {
const std::array<ScriptFunction, 20> s_scriptFunctions{{
    {"SetCharCustomizeFrame", Script_SetCharCustomizeFrame},
    {"SetCharCustomizeBackground", Script_SetCharCustomizeBackground},
    {"ResetCharCustomize", Script_ResetCharCustomize},
    {"GetNameForRace", Script_GetNameForRace},
    {"GetFactionForRace", Script_GetFactionForRace},
    {"GetAvailableRaces", Script_GetAvailableRaces},
    {"GetClassesForRace", Script_GetClassesForRace},
    {"GetSelectedRace", Script_GetSelectedRace},
    {"GetSelectedSex", Script_GetSelectedSex},
    {"GetSelectedClass", Script_GetSelectedClass},
    {"SetSelectedRace", Script_SetSelectedRace},
    {"SetSelectedSex", Script_SetSelectedSex},
    {"SetSelectedClass", Script_SetSelectedClass},
    {"UpdateCustomizationBackground", Script_UpdateCustomizationBackground},
    {"HasCharCustomization", Script_HasCharCustomization},
    {"CycleCharCustomization", Script_CycleCharCustomization},
    {"RandomizeCharCustomization", Script_RandomizeCharCustomization},
    {"GetCharacterFacing", Script_GetCharacterFacing},
    {"SetCharacterFacing", Script_SetCharacterFacing},
    {"CreateCharacter", Script_CreateCharacter},
}};
}

void CharCreateRegisterScriptFunctions()
{
    if (g_charCreateScriptsRegistered) return;
    for (const auto& entry : s_scriptFunctions)
        FrameScript_RegisterFunction(entry.name, entry.method);
    g_charCreateScriptsRegistered = true;
}

void CharCreateUnregisterScriptFunctions()
{
    if (!g_charCreateScriptsRegistered) return;
    for (const auto& entry : s_scriptFunctions)
        FrameScript_UnregisterFunction(entry.name);
    g_charCreateScriptsRegistered = false;
}

int _E2_87() { return _E1_87(); }
int _E1_87() { INFINITY_87 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_87() { _E4_87(); return 0; }
void _E4_87() { TWO_PI_87 = 3.1415927F + 3.1415927F; }
int _E8_87() { _E7_87(); return 0; }
void _E7_87() { OO_TWO_PI_87 = 1.0F / TWO_PI_87; }

#define CHARCREATE_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
CHARCREATE_CONST_PAIR(_E14_10, _E13_5, MAX_DUEL_DISTANCE_SQUARED_18, 100.0F)
CHARCREATE_CONST_PAIR(_E17_8, _E16_12, MAX_TRADE_DISTANCE_SQUARED_18, 123.45679F)
CHARCREATE_CONST_PAIR(_E20_26, _E19_13, MAX_SHOP_DISTANCE_SQUARED_18, 30.864195F)
CHARCREATE_CONST_PAIR(_E23_28, _E22_19, MAX_INSPECT_DISTANCE_SQUARED_18, 30.864195F)
CHARCREATE_CONST_PAIR(_E26_24, _E25_17, MAX_LOOT_DISTANCE_SQUARED_18, 25.0F)
CHARCREATE_CONST_PAIR(_E29_23, _E28_11, MAX_BIND_DISTANCE_SQUARED_18, 100.0F)
CHARCREATE_CONST_PAIR(_E32_21, _E31_8, MAX_SITCHAIR_DISTANCE_SQUARED_18, 0.25F)
CHARCREATE_CONST_PAIR(_E35_21, _E34_9, MAX_SITCHAIRUSE_DISTANCE_SQUARED_18, 9.0F)
CHARCREATE_CONST_PAIR(_E38_21, _E37_8, MAX_SHARE_DISTANCE_SQUARED_18, 10000.0F)
CHARCREATE_CONST_PAIR(_E41_19, _E40_9, SECONDS_PER_HEARTBEAT_18, 5.0F)
CHARCREATE_CONST_PAIR(_E44_21, _E43_8, SPELL_FOCUS_DISTANCE_SQUARED_18, 2500.0F)
CHARCREATE_CONST_PAIR(_E47_23, _E46_6, MAX_OBJ_INTEREST_RADIUS_SQUARED_18, 10000.0F)
#undef CHARCREATE_CONST_PAIR

int _E83_2() { _E80_3(); return _E82_3(); }
void _E80_3() { if (++WowConnectionInitializer::count == 1) WowConnectionInitializer::Initialize(); }
int _E82_3() { return std::atexit(_E81_2); }
void _E81_2() { if (WowConnectionInitializer::count > 0 && --WowConnectionInitializer::count == 0) WowConnectionInitializer::Destroy(); }

int _E107_0() { _E103_2(); return _E106_2(); }
int _E103_2() { CCharCreateInfo::m_factionIndex.clear(); return 0; }
int _E106_2() { return std::atexit(_E104_0); }
void _E104_0() { CCharCreateInfo::m_factionIndex.clear(); }
int _E112_0() { _E109_0(); return _E111(); }
int _E109_0() { CCharCreateInfo::m_raceIndex.clear(); return 0; }
int _E111() { return std::atexit(_E110_0); }
void _E110_0() { CCharCreateInfo::m_raceIndex.clear(); }
int _E117() { _E114(); return _E116_0(); }
int _E114() { CCharCreateInfo::m_classIndex.clear(); return 0; }
int _E116_0() { return std::atexit(_E115_0); }
void _E115_0() { CCharCreateInfo::m_classIndex.clear(); }
int _E123() { _E119_0(); return _E122_0(); }
unsigned _E119_0() { CCharCreateInfo::m_charInfo = CHARCREATEINFO{}; return 20; }
int _E122_0() { return std::atexit([] { CCharCreateInfo::m_charInfo.Shutdown(); }); }
void _E120(HOBJECT__*) { CCharCreateInfo::m_charInfo.Shutdown(); }
