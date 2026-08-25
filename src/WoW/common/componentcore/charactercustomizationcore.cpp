#include "WoW/common/componentcore/charactercustomizationcore.h"
#include "ENGINE/Source/Model/modelmisc.h"
#include "ENGINE/Source/Services/texture.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

TSFixedArray<TSFixedArray<VARIATIONS>> s_characterVariations;
TSFixedArray<CAMERAFILENAMES> s_cameraFileNames;
TSGrowableArray<CHARACTERRACEVARIATIONS> s_raceTextureFileNames;
INVHOLDINFO s_itemTypeTextureHolds[27];
unsigned int g_defaultGeosetIDOffsets[15] = {
    1, 1, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0,
};

namespace {
struct TextureVariationRecord {
    unsigned int race = 0;
    unsigned int sex = 0;
    unsigned int section = 0;
    unsigned int variation = 0;
    unsigned int color = 0;
    unsigned int isNPC = 0;
    std::string texture;
};

struct HairGeosetRecord {
    unsigned int race = 0;
    unsigned int sex = 0;
    unsigned int variation = 0;
    int geoset = 0;
};

struct FacialGeosetRecord {
    unsigned int race = 0;
    unsigned int sex = 0;
    unsigned int variation = 0;
    unsigned int beard = 0;
    unsigned int moustache = 0;
    unsigned int sideburn = 0;
};

std::vector<TextureVariationRecord> s_textureVariationRecords;
std::vector<HairGeosetRecord> s_hairGeosetRecords;
std::vector<FacialGeosetRecord> s_facialGeosetRecords;
bool s_customizationDataLoaded = false;

std::uint32_t ReadU32(const std::uint8_t* data) {
    std::uint32_t value = 0;
    std::memcpy(&value, data, sizeof(value));
    return value;
}

bool LoadDBC(const char* filename, unsigned int expectedFields,
             std::vector<std::uint8_t>& bytes, unsigned int& records,
             unsigned int& recordSize, const char*& strings,
             unsigned int& stringSize) {
    void* data = nullptr;
    unsigned int size = 0;
    if (!SFile::LoadFile(filename, &data, &size, 1, nullptr) || !data)
        return false;
    const auto* input = static_cast<const std::uint8_t*>(data);
    if (size < 20U || std::memcmp(input, "WDBC", 4) != 0 ||
        ReadU32(input + 8) != expectedFields) {
        SFile::Unload(data);
        return false;
    }
    records = ReadU32(input + 4);
    recordSize = ReadU32(input + 12);
    stringSize = ReadU32(input + 16);
    const std::size_t stringOffset = 20U + static_cast<std::size_t>(records) * recordSize;
    if (recordSize < expectedFields * 4U || stringOffset + stringSize > size) {
        SFile::Unload(data);
        return false;
    }
    bytes.assign(input, input + size);
    SFile::Unload(data);
    strings = reinterpret_cast<const char*>(bytes.data() + stringOffset);
    return true;
}

void LoadTextureVariations() {
    std::vector<std::uint8_t> bytes;
    unsigned int records = 0, recordSize = 0, stringSize = 0;
    const char* strings = nullptr;
    if (!LoadDBC("DBFilesClient\\CharTextureVariationsV2.dbc", 8, bytes,
                 records, recordSize, strings, stringSize))
        return;
    s_textureVariationRecords.reserve(records);
    for (unsigned int row = 0; row < records; ++row) {
        const auto* record = bytes.data() + 20U + static_cast<std::size_t>(row) * recordSize;
        const unsigned int nameOffset = ReadU32(record + 28);
        if (nameOffset >= stringSize) continue;
        TextureVariationRecord value;
        value.race = ReadU32(record + 4);
        value.sex = ReadU32(record + 8);
        value.section = ReadU32(record + 12);
        value.variation = ReadU32(record + 16);
        value.color = ReadU32(record + 20);
        value.isNPC = ReadU32(record + 24);
        value.texture = strings + nameOffset;
        if (!value.texture.empty()) s_textureVariationRecords.push_back(std::move(value));
    }
}

void LoadHairGeosets() {
    std::vector<std::uint8_t> bytes;
    unsigned int records = 0, recordSize = 0, stringSize = 0;
    const char* strings = nullptr;
    if (!LoadDBC("DBFilesClient\\CharHairGeosets.dbc", 6, bytes,
                 records, recordSize, strings, stringSize))
        return;
    s_hairGeosetRecords.reserve(records);
    for (unsigned int row = 0; row < records; ++row) {
        const auto* record = bytes.data() + 20U + static_cast<std::size_t>(row) * recordSize;
        HairGeosetRecord value;
        value.race = ReadU32(record + 4);
        value.sex = ReadU32(record + 8);
        value.variation = ReadU32(record + 12);
        value.geoset = static_cast<int>(ReadU32(record + 16));
        if (ReadU32(record + 20) != 0U) value.geoset = -value.geoset;
        s_hairGeosetRecords.push_back(value);
    }
}

void LoadFacialGeosets() {
    std::vector<std::uint8_t> bytes;
    unsigned int records = 0, recordSize = 0, stringSize = 0;
    const char* strings = nullptr;
    if (!LoadDBC("DBFilesClient\\CharacterFacialHairStyles.dbc", 6, bytes,
                 records, recordSize, strings, stringSize))
        return;
    s_facialGeosetRecords.reserve(records);
    for (unsigned int row = 0; row < records; ++row) {
        const auto* record = bytes.data() + 20U + static_cast<std::size_t>(row) * recordSize;
        s_facialGeosetRecords.push_back({
            ReadU32(record), ReadU32(record + 4), ReadU32(record + 8),
            ReadU32(record + 12), ReadU32(record + 16), ReadU32(record + 20)});
    }
}

void EnsureCustomizationData() {
    if (s_customizationDataLoaded) return;
    s_customizationDataLoaded = true;
    LoadTextureVariations();
    LoadHairGeosets();
    LoadFacialGeosets();
    std::fprintf(stderr, "CHARCUSTOM data textures=%zu hair=%zu facial=%zu\n",
                 s_textureVariationRecords.size(), s_hairGeosetRecords.size(),
                 s_facialGeosetRecords.size());
}

unsigned int TextureVariationCount(unsigned int race, unsigned int sex,
                                   unsigned int section, unsigned int isNPC) {
    EnsureCustomizationData();
    unsigned int count = 0;
    for (const auto& record : s_textureVariationRecords) {
        if (record.race == race && record.sex == sex && record.section == section &&
            record.isNPC == isNPC)
            count = (std::max)(count, record.variation + 1U);
    }
    return count;
}

unsigned int TextureColorCount(unsigned int race, unsigned int sex,
                               unsigned int section, unsigned int variation,
                               unsigned int isNPC) {
    EnsureCustomizationData();
    unsigned int count = 0;
    for (const auto& record : s_textureVariationRecords) {
        if (record.race == race && record.sex == sex && record.section == section &&
            record.variation == variation && record.isNPC == isNPC)
            count = (std::max)(count, record.color + 1U);
    }
    return count;
}

const TextureVariationRecord* FindTextureVariation(unsigned int race, unsigned int sex,
                                                    unsigned int section,
                                                    unsigned int variation,
                                                    unsigned int color,
                                                    unsigned int isNPC) {
    const unsigned int variations = TextureVariationCount(race, sex, section, isNPC);
    if (!variations) return nullptr;
    variation %= variations;
    unsigned int colors = TextureColorCount(race, sex, section, variation, isNPC);
    if (!colors) {
        variation = 0;
        colors = TextureColorCount(race, sex, section, variation, isNPC);
    }
    if (!colors) return nullptr;
    color %= colors;
    for (const auto& record : s_textureVariationRecords) {
        if (record.race == race && record.sex == sex && record.section == section &&
            record.variation == variation && record.color == color && record.isNPC == isNPC)
            return &record;
    }
    return nullptr;
}

int HairGeosetValue(unsigned int race, unsigned int sex, unsigned int variation) {
    EnsureCustomizationData();
    const unsigned int count = [&] {
        unsigned int result = 0;
        for (const auto& record : s_hairGeosetRecords)
            if (record.race == race && record.sex == sex)
                result = (std::max)(result, record.variation + 1U);
        return result;
    }();
    if (!count) return 0;
    variation %= count;
    for (const auto& record : s_hairGeosetRecords)
        if (record.race == race && record.sex == sex && record.variation == variation)
            return record.geoset;
    return 0;
}

constexpr CHARACTER_GEOSET_SECTIONS kClothingRanges[9] = {
    CHARGEOSET_GLOVE, CHARGEOSET_BOOT, CHARGEOSET_SLEEVES, CHARGEOSET_PANTS,
    CHARGEOSET_DOUBLET, CHARGEOSET_PANTDOUBLET, CHARGEOSET_TABARD, CHARGEOSET_ROBE,
    CHARGEOSET_LOINCLOTH,
};

// Exact data table at WoWClient.exe VA 0x00852500. A zero selection keeps
// overlay sections hidden; non-overlay sections fall back to geoset 1.
constexpr int kCharGeosetIsOverlay[NUM_CHARGEOSETS] = {
    0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1, 0, 0,
};

void ClearHold(INVHOLDINFO& hold) {
    for (auto& info : hold.holdInfo) {
        info.geosetGroup = INVALID_CHARITEMGEOSET;
        info.holdSection = TCS_INVALIDSECTION;
    }
}
}

INVHOLDINFO::INVHOLDINFO(CHARACTER_ITEM_GEOSETS geoset0, TEXCOMPONENT_SECTIONS section0,
                         CHARACTER_ITEM_GEOSETS geoset1, TEXCOMPONENT_SECTIONS section1) {
    holdInfo[0] = {geoset0, section0};
    holdInfo[1] = {geoset1, section1};
}

CharGeosetInfo::CharGeosetInfo() { Clear(); }
CharGeosetInfo::CharGeosetInfo(const CharGeosetInfo& rhs) { *this = rhs; }

void CharGeosetInfo::Clear() {
    for (int geoset = 0; geoset < 9; ++geoset) {
        highestPriority[geoset] = -1;
        currentGeosets[geoset] = 1;
        geosetCurrentlyUsedBy[geoset] = 0;
        disabledByFlags[geoset] = 0;
        flags[geoset] = 0;
        std::fill(std::begin(inventoryTypeGeosets[geoset]),
                  std::end(inventoryTypeGeosets[geoset]), 0U);
    }
}

void CharGeosetInfo::UpdateGeosetDisplay(const ItemDisplayInfoRec* displayInfoRec,
                                         unsigned int itemInventoryType,
                                         HTEXCOMPONENT__*, int) {
    if (displayInfoRec == nullptr || itemInventoryType >= 27) return;
    for (int slot = 0; slot < 4; ++slot) {
        const int group = slot;
        const unsigned int value = static_cast<unsigned int>(std::max(displayInfoRec->m_geosetGroup[slot], 0));
        if (value == 0) continue;
        inventoryTypeGeosets[group][itemInventoryType] = value + 1;
        if (highestPriority[group] <= static_cast<int>(itemInventoryType)) {
            highestPriority[group] = static_cast<int>(itemInventoryType);
            currentGeosets[group] = value + 1;
            geosetCurrentlyUsedBy[group] = itemInventoryType;
        }
        flags[group] |= 1;
    }
}

void CharGeosetInfo::ShowInventoryTypeTextureHolds(HTEXCOMPONENT__*, INVENTORY_TYPES inventoryType,
                                                   int) {
    if (inventoryType <= INDEX_NON_EQUIP_TYPE || inventoryType >= INDEX_NUMSLOTS) return;
}

void CharGeosetInfo::RemoveGeosetInfo(const ItemDisplayInfoRec* displayInfoRec,
                                      INVENTORY_TYPES inventoryType, HTEXCOMPONENT__*) {
    if (displayInfoRec == nullptr || inventoryType <= INDEX_NON_EQUIP_TYPE ||
        inventoryType >= INDEX_NUMSLOTS) return;
    for (int group = 0; group < 9; ++group) {
        inventoryTypeGeosets[group][inventoryType] = 0;
        if (geosetCurrentlyUsedBy[group] == static_cast<unsigned int>(inventoryType)) {
            currentGeosets[group] = 0;
            geosetCurrentlyUsedBy[group] = 0;
            highestPriority[group] = -1;
        }
    }
}

int CharGeosetInfo::ShowingSameGeosetsAs(const CharGeosetInfo* rhs) const {
    if (rhs == nullptr) return 0;
    return std::memcmp(this, rhs, sizeof(*this)) == 0;
}

void CCharGeoset::ClearGeosets() {
    m_geosetInfo.Clear();
    m_workingGeosetInfo.Clear();
    for (const auto section : kClothingRanges)
        ShowGeosetSection(section, 0, 0);
}

void CCharGeoset::Commit() {
    CommitGeosets(m_charModel);
    CommitGeosets(m_paperDollModel);
    m_flags &= ~1;
}

void CCharGeoset::CommitGeosets(HMODEL__* model) {
    if (model == nullptr || (m_flags & 1) == 0) return;
    auto& concreteModel = *reinterpret_cast<CModel*>(model);
    for (unsigned int section = 0; section < NUM_CHARGEOSETS; ++section) {
        if (m_currentGeosets[section] != 0U)
            ModelHideGeosets(concreteModel,
                             100U * section + m_currentGeosets[section], false);
    }
    if ((m_flags & 2) != 0)
        ModelHideGeosets(concreteModel, 1U, false);
    ModelHideGeosets(concreteModel, 0U, false);
    ModelOptimizeVisibleGeosets(concreteModel);
}

void CCharGeoset::CommitWorkingGeosetInfo() {
    if (!m_workingGeosetInfo.ShowingSameGeosetsAs(&m_geosetInfo))
        m_geosetInfo = m_workingGeosetInfo;
    m_flags |= 1;
}

void CCharGeoset::AddItemGeoset(const ItemDisplayInfoRec* displayInfoRec,
                                unsigned int itemInventoryType, HTEXCOMPONENT__* component, int playerRace) {
    m_workingGeosetInfo.UpdateGeosetDisplay(displayInfoRec, itemInventoryType, component, playerRace);
}

void CCharGeoset::RemoveItemGeoset(const ItemDisplayInfoRec* displayInfoRec,
                                   unsigned int itemInventoryType, HTEXCOMPONENT__* component) {
    m_workingGeosetInfo.RemoveGeosetInfo(displayInfoRec, static_cast<INVENTORY_TYPES>(itemInventoryType), component);
}

void CCharGeoset::ShowGeosetSection(CHARACTER_GEOSET_SECTIONS section, unsigned int geosetNumber,
                                    int hideRemainder) {
    if (section < 0 || section >= NUM_CHARGEOSETS || m_charModel == nullptr || geosetNumber >= 100U)
        return;
    if (hideRemainder) HideGeosetSection(section);
    if (geosetNumber == 0U)
        geosetNumber = kCharGeosetIsOverlay[section] == 0 ? 1U : 0U;
    if (geosetNumber != m_currentGeosets[section]) {
        const auto previous = m_currentGeosets[section];
        if (previous != 0U && !hideRemainder) {
            ModelHideGeosets(*reinterpret_cast<CModel*>(m_charModel),
                             100U * section + previous, true);
            if (m_paperDollModel)
                ModelHideGeosets(*reinterpret_cast<CModel*>(m_paperDollModel),
                                 100U * section + previous, true);
        }
        m_currentGeosets[section] = geosetNumber;
        m_flags |= 1;
    }
}

void CCharGeoset::HideGeosetSection(CHARACTER_GEOSET_SECTIONS section) {
    if (section < 0 || section >= NUM_CHARGEOSETS || m_charModel == nullptr) return;
    const auto first = 100U * section + 1U;
    const auto last = 100U * section + 99U;
    m_currentGeosets[section] = 0;
    ModelHideGeosetsRange(*reinterpret_cast<CModel*>(m_charModel), first, last, true);
    if (m_paperDollModel)
        ModelHideGeosetsRange(*reinterpret_cast<CModel*>(m_paperDollModel), first, last, true);
}

void CCharGeoset::EnableHairGeosets(unsigned int race, unsigned int sex,
                                    unsigned int hairStyleID) {
    const int geoset = HairGeosetValue(race, sex, hairStyleID);
    ShowGeosetSection(CHARGEOSET_HAIR,
                      static_cast<unsigned int>(geoset < 0 ? -geoset : geoset), 1);
    if (geoset < 0) m_flags |= 2;
    else m_flags &= ~2;
}

const char* __fastcall GetCameraFileName(unsigned int sex, unsigned int camera) {
    if (sex >= s_cameraFileNames.m_count || camera >= 3) return nullptr;
    return s_cameraFileNames.m_data[sex].fileName[camera];
}

void CharCustomizationInitialize() {
    EnsureCustomizationData();
    InitializeCameraFileNames();
    InitializeTextureFileNames();
    InitializeHairGeosets();
    InitializeTextureHoldLayers();
    InitializeFacialHairVariations();
}

int InitializeCameraFileNames() {
    if (!s_cameraFileNames.SetCount(2)) return 0;
    for (unsigned int sex = 0; sex < 2; ++sex) {
        std::snprintf(s_cameraFileNames.m_data[sex].fileName[0], 260,
                      "Character\\Camera\\%sGlueCamera.mdl", sex == 0 ? "Male" : "Female");
        std::snprintf(s_cameraFileNames.m_data[sex].fileName[1], 260,
                      "Character\\Camera\\%sCustomizeCamera.mdl", sex == 0 ? "Male" : "Female");
        std::snprintf(s_cameraFileNames.m_data[sex].fileName[2], 260,
                      "Character\\Camera\\%sPaperDollCamera.mdl", sex == 0 ? "Male" : "Female");
    }
    return 1;
}

int InitializeTextureFileNames() { EnsureCustomizationData(); return 1; }

unsigned int FillInMissingTextureFileNames() { return 0; }
void __fastcall ReadTextureFileNames(int) {}
int InitializeHairGeosets() { EnsureCustomizationData(); return 1; }

void InitializeTextureHoldLayers() {
    for (auto& hold : s_itemTypeTextureHolds) ClearHold(hold);
    s_itemTypeTextureHolds[INDEX_FEET_TYPE] = INVHOLDINFO(
        CHARITEMGEOSETS_DOUBLET, TCS_LEGUPPER,
        INVALID_CHARITEMGEOSET, TCS_INVALIDSECTION);
    s_itemTypeTextureHolds[INDEX_LEGS_TYPE] = INVHOLDINFO(
        CHARITEMGEOSETS_TABARD, TCS_LEGUPPER,
        CHARITEMGEOSETS_DOUBLET, TCS_LEGUPPER);
}

void InitializeFacialHairVariations() { EnsureCustomizationData(); }

void CharCustomizationShutdown() {
    s_characterVariations.SetCount(0);
    s_cameraFileNames.SetCount(0);
    s_raceTextureFileNames.SetCount(0);
    s_textureVariationRecords.clear();
    s_hairGeosetRecords.clear();
    s_facialGeosetRecords.clear();
    s_customizationDataLoaded = false;
}

const char* CharCustomizationGetTextureName(unsigned int raceID, unsigned int sexID,
                                             CHARTEXTURESECTIONID section,
                                             unsigned int variationID,
                                             unsigned int colorID, int isNPC) {
    const auto* record = FindTextureVariation(raceID, sexID,
        static_cast<unsigned int>(section), variationID, colorID, isNPC != 0 ? 1U : 0U);
    return record ? record->texture.c_str() : nullptr;
}

void __fastcall CharCustomizationGetNumSkinTextures(unsigned int raceID, unsigned int sexID,
                                                     int* pcVars, int* npcVars) {
    if (pcVars != nullptr)
        *pcVars = static_cast<int>(TextureColorCount(
            raceID, sexID, CHARTEXTURESECTION_SKIN, 0, 0));
    if (npcVars != nullptr)
        *npcVars = static_cast<int>(TextureColorCount(
            raceID, sexID, CHARTEXTURESECTION_SKIN, 0, 1));
}

HTEXTURE__* __fastcall CharCustomizationLoadSkin(const char*, CStatus*, unsigned int, unsigned int,
                                                  int, int) {
    return nullptr;
}

HTEXTURE__* __fastcall CharCustomizationSetSkin(unsigned int, unsigned int, unsigned int, unsigned int, int) {
    return nullptr;
}

void __fastcall CharCustomizationNumFaces(unsigned int raceID, unsigned int sexID,
                                          int* pcVars, int* npcVars) {
    if (pcVars != nullptr)
        *pcVars = static_cast<int>(TextureVariationCount(
            raceID, sexID, CHARTEXTURESECTION_FACEUPPER, 0));
    if (npcVars != nullptr)
        *npcVars = static_cast<int>(TextureVariationCount(
            raceID, sexID, CHARTEXTURESECTION_FACEUPPER, 1));
}

void __fastcall CharCustomizationSetFaceTexture(HTEXCOMPONENT__*, CTexComponent*, unsigned int,
                                                unsigned int, CHARACTERSEXVARIATIONS*, unsigned int, int) {}

unsigned int __fastcall CharCustomizationNumHairColors(unsigned int raceID, unsigned int sexID) {
    return TextureColorCount(raceID, sexID, CHARTEXTURESECTION_HAIR, 0, 0);
}
void __fastcall CharCustomizationSetHairTexture(HTEXCOMPONENT__*, char*, unsigned int, const char*,
                                                unsigned int, unsigned int) {}

void __fastcall CharCustomizationInitBaseCharacter(CCharGeoset* geoset, unsigned int beard,
                                                   unsigned int sideBurn, unsigned int moustache,
                                                   unsigned int ear) {
    if (geoset == nullptr) return;
    geoset->ShowGeosetSection(CHARGEOSET_BEARD, beard, 1);
    geoset->ShowGeosetSection(CHARGEOSET_SIDEBURN, sideBurn, 1);
    geoset->ShowGeosetSection(CHARGEOSET_MOUSTACHE, moustache, 1);
    geoset->ShowGeosetSection(CHARGEOSET_EAR, ear, 1);
    // Exact recovered section list.  Section 6 is obsolete and section 7 was
    // already selected explicitly as the ear geoset above.
    constexpr CHARACTER_GEOSET_SECTIONS baseSections[] = {
        CHARGEOSET_GLOVE, CHARGEOSET_BOOT, CHARGEOSET_SLEEVES,
        CHARGEOSET_PANTS, CHARGEOSET_DOUBLET, CHARGEOSET_PANTDOUBLET,
        CHARGEOSET_TABARD, CHARGEOSET_ROBE, CHARGEOSET_LOINCLOTH,
    };
    for (const CHARACTER_GEOSET_SECTIONS section : baseSections)
        geoset->ShowGeosetSection(section, g_defaultGeosetIDOffsets[section], 1);
    geoset->ClearGeosets();
}

void __fastcall CharCustomizationResetHairGeoset(CCharGeoset* geoset, unsigned int, unsigned int,
                                                  unsigned int hairStyleID) {
    if (geoset != nullptr) geoset->EnableHairGeosets(0, 0, hairStyleID);
}

int __fastcall CharCustomizationGetNakedSectionName(unsigned int, unsigned int, unsigned int,
                                                    unsigned int underwearSection, char* outBuffer,
                                                    unsigned int outBufferSize, int) {
    if (outBuffer == nullptr || outBufferSize == 0) return 0;
    const char* name = underwearSection < NUM_TEXCOMPONENT_SECTIONS
        ? s_sectionDirectorynames[underwearSection] : "";
    std::snprintf(outBuffer, outBufferSize, "%s", name);
    return 1;
}

void __fastcall CharCustomizationCommitItemGeosets(CCharGeoset* geoset, int commit) {
    if (geoset != nullptr) {
        geoset->CommitWorkingGeosetInfo();
        if (commit) geoset->Commit();
    }
}

void __fastcall CharCustomizationClearItemGeosets(CCharGeoset* geoset) {
    if (geoset != nullptr) geoset->ClearGeosets();
}

void __fastcall CharCustomizationAddItemGeosets(CCharGeoset* geoset,
                                                const ItemDisplayInfoRec* displayInfoRec,
                                                unsigned int itemInventoryType, HTEXCOMPONENT__* component,
                                                int playerRace) {
    if (geoset != nullptr) geoset->AddItemGeoset(displayInfoRec, itemInventoryType, component, playerRace);
}

void __fastcall CharCustomizationRemoveItemGeosets(CCharGeoset* geoset,
                                                   const ItemDisplayInfoRec* displayInfoRec,
                                                   unsigned int itemInventoryType, HTEXCOMPONENT__* component) {
    if (geoset != nullptr) geoset->RemoveItemGeoset(displayInfoRec, itemInventoryType, component);
}

unsigned int __fastcall CharCustomizationNumHairStyles(unsigned int raceID, unsigned int sexID) {
    EnsureCustomizationData();
    unsigned int count = 0;
    for (const auto& record : s_hairGeosetRecords)
        if (record.race == raceID && record.sex == sexID)
            count = (std::max)(count, record.variation + 1U);
    return count;
}
unsigned int __fastcall CharCustomizationGetHairGeoset(unsigned int raceID, unsigned int sexID,
                                                        unsigned int hair) {
    const int geoset = HairGeosetValue(raceID, sexID, hair);
    return static_cast<unsigned int>(geoset < 0 ? -geoset : geoset);
}
unsigned int __fastcall CharCustomizationNumBeardStyles(unsigned int raceID, unsigned int sexID) {
    EnsureCustomizationData();
    unsigned int count = 0;
    for (const auto& record : s_facialGeosetRecords)
        if (record.race == raceID && record.sex == sexID)
            count = (std::max)(count, record.variation + 1U);
    return count;
}

unsigned int __fastcall CharCustomizationGetBeardStyle(unsigned int raceID, unsigned int sexID,
                                                        unsigned int facialHairID,
                                                        BEARDSTYLEDATA* data) {
    if (data != nullptr) *data = BEARDSTYLEDATA{};
    const unsigned int count = CharCustomizationNumBeardStyles(raceID, sexID);
    if (!count || !data) return 0;
    facialHairID %= count;
    for (const auto& record : s_facialGeosetRecords) {
        if (record.race != raceID || record.sex != sexID ||
            record.variation != facialHairID)
            continue;
        data->beardGeoset = record.beard + 1U;
        data->sideBurnGeoset = record.sideburn + 1U;
        data->moustacheGeoset = record.moustache + 1U;
        return 1;
    }
    return 0;
}

void __fastcall CharCustomizationSetFacialTexture(HTEXCOMPONENT__*, CTexComponent*, unsigned int,
                                                  const char*, unsigned int, unsigned int) {}

HCHARGEOSET__* __fastcall CharCustomizationCreateGeosetHandle(HMODEL__* model) {
    if (model == nullptr) return nullptr;
    auto* geoset = new CCharGeoset();
    geoset->m_charModel = model;
    return reinterpret_cast<HCHARGEOSET__*>(geoset);
}

void __fastcall CharCustomizationShowGeoset(CCharGeoset* geoset, CHARACTER_GEOSET_SECTIONS section,
                                            unsigned int geosetNumber) {
    if (geoset != nullptr) geoset->ShowGeosetSection(section, geosetNumber, 0);
}

void __fastcall CharCustomizationHideGeosetSection(CCharGeoset* geoset,
                                                    CHARACTER_GEOSET_SECTIONS section) {
    if (geoset != nullptr) geoset->HideGeosetSection(section);
}

void __fastcall CharCustomizationCommitGeosets(CCharGeoset* geoset) {
    if (geoset != nullptr) geoset->Commit();
}

void __fastcall CharCustomizationGetTextureLayerHolds(unsigned int, unsigned int,
                                                      unsigned int* textureLayerHolds,
                                                      unsigned int numTextureLayerHolds) {
    for (unsigned int i = 0; i < numTextureLayerHolds; ++i)
        textureLayerHolds[i] = 0;
}

void __fastcall CharCustomizationSetPaperDollGeoset(HCHARGEOSET__*, HMODEL__*, int) {}

void CHARACTERSEXVARIATIONS::GetNumVariations(CHARTEXTURESECTIONID section, int* pcVars, int* npcVars) {
    if (section < 0 || section >= 11) return;
    if (pcVars != nullptr)
        *pcVars = firstNPCVar[section] == 0x7FFFFFFF
            ? static_cast<int>(names[section].m_count) : firstNPCVar[section];
    if (npcVars != nullptr) {
        const int last = lastNPCVar[section] == 0x7FFFFFFF
            ? static_cast<int>(names[section].m_count) - 1 : lastNPCVar[section];
        const int first = firstNPCVar[section] == 0x7FFFFFFF
            ? static_cast<int>(names[section].m_count) : firstNPCVar[section];
        *npcVars = std::max(last - first + 1, 0);
    }
}

int _E137_3() { s_characterVariations.SetCount(0); return 0; }
int _E142_2() { s_cameraFileNames.SetCount(0); return 0; }
int _E147() { s_raceTextureFileNames.SetCount(0); return 0; }
INVHOLDINFO* _E152() { InitializeTextureHoldLayers(); return s_itemTypeTextureHolds; }
