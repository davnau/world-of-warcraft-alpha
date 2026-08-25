#include "WoW/Source/DB/DBClient/dbclient.h"

#include "WoW/Source/DB/DBClient/sdbwmoareatable.h"

#include <algorithm>
#include <cstdint>
#include <vector>

WowClientDB<ResistancesRec> g_resistancesDB;
WowClientDB<StringLookupsRec> g_stringLookupsDB;
WowClientDB<TerrainTypeRec> g_terrainTypeDB;
WowClientDB<ItemClassRec> g_itemClassDB;
WowClientDB<ItemSubClassRec> g_itemSubClassDB;
WowClientDB<SoundProviderPreferencesRec> g_soundProviderPreferencesDB;
WowClientDB<SpellVisualEffectNameRec> g_spellVisualEffectNameDB;
WowClientDB<SoundEntriesRec> g_soundEntriesDB;
WowClientDB<SoundWaterTypeRec> g_soundWaterTypeDB;
WowClientDB<SoundCharacterMacroLinesRec> g_soundCharacterMacroLinesDB;
WowClientDB<ChrRacesRec> g_chrRacesDB;
WowClientDB<NamesProfanityRec> g_namesProfanityDB;
WowClientDB<NamesReservedRec> g_namesReservedDB;
WowClientDB<TaxiNodesRec> g_taxiNodesDB;
WowClientDB<TaxiPathRec> g_taxiPathDB;
WowClientDB<TaxiPathNodeRec> g_taxiPathNodeDB;

namespace {
int s_physicalDamageClassID = -1;
int s_firstNonPhysicalDamageClass = -1;
std::vector<const ResistancesRec*> s_damageTypeRecordIDs;
std::vector<unsigned> s_terrainSoundType;
std::vector<const ItemSubClassRec*> s_weaponSubClasses;
const ItemSubClassRec* s_unarmedWeaponSubclass = nullptr;
const ItemClassRec* s_weaponClassRecPtr = nullptr;
const SoundProviderPreferencesRec* s_defaultIndoorProviderPrefs = nullptr;
const SoundProviderPreferencesRec* s_defaultOutdoorProviderPrefs = nullptr;
bool s_staticDBLoaded = false;
}

void StaticDBLoadAll() {
    g_resistancesDB.Load();
    g_stringLookupsDB.Load();
    g_terrainTypeDB.Load();
    g_itemClassDB.Load();
    g_itemSubClassDB.Load();
    g_soundProviderPreferencesDB.Load();
    g_namesProfanityDB.Load();
    g_namesReservedDB.Load();
    g_taxiNodesDB.Load();
    g_taxiPathDB.Load();
    g_taxiPathNodeDB.Load();
    s_staticDBLoaded = true;
}

void CheckDamageClassConsistency() {
    s_physicalDamageClassID = -1;
    s_firstNonPhysicalDamageClass = -1;
    s_damageTypeRecordIDs.assign(g_resistancesDB.m_records.size(), nullptr);
    bool foundNonPhysical = false;
    for (std::size_t i = 0; i < g_resistancesDB.m_records.size(); ++i) {
        auto& record = g_resistancesDB.m_records[i];
        if (record.m_ID == 0 && i != 0) record.m_ID = static_cast<int>(i);
        s_damageTypeRecordIDs[i] = &record;
        if (record.m_Flags & 1) s_physicalDamageClassID = static_cast<int>(i);
        else if (!foundNonPhysical) {
            s_firstNonPhysicalDamageClass = static_cast<int>(i);
            foundNonPhysical = true;
        }
    }
}

unsigned InitTerrainSoundTypeIDs() {
    s_terrainSoundType.assign(g_terrainTypeDB.m_maxID + 1 > 0 ? g_terrainTypeDB.m_maxID + 1 : 0, 0);
    for (std::size_t i = 0; i < g_terrainTypeDB.m_records.size() && i < s_terrainSoundType.size(); ++i)
        s_terrainSoundType[i] = g_terrainTypeDB.m_records[i].m_SoundID;
    return static_cast<unsigned>(s_terrainSoundType.size());
}

const ItemClassRec* LocateWeaponSubclass() {
    s_weaponClassRecPtr = nullptr;
    for (const auto& record : g_itemClassDB.m_records) {
        if (record.m_flags & 1) {
            s_weaponClassRecPtr = &record;
            break;
        }
    }
    return s_weaponClassRecPtr;
}

unsigned InitWeaponSubclasses() {
    s_weaponSubClasses.clear();
    s_unarmedWeaponSubclass = nullptr;
    LocateWeaponSubclass();
    const int weaponClass = s_weaponClassRecPtr ? 2 : -1;
    for (const auto& record : g_itemSubClassDB.m_records) {
        if (record.m_classID == weaponClass) {
            if (record.m_subClassID >= static_cast<int>(s_weaponSubClasses.size()))
                s_weaponSubClasses.resize(record.m_subClassID + 1, nullptr);
            s_weaponSubClasses[record.m_subClassID] = &record;
            if (record.m_subClassID == 0) s_unarmedWeaponSubclass = &record;
        }
    }
    return static_cast<unsigned>(s_weaponSubClasses.size());
}

void InitSoundProviderPreferences() {
    s_defaultIndoorProviderPrefs = nullptr;
    s_defaultOutdoorProviderPrefs = nullptr;
    for (const auto& record : g_soundProviderPreferencesDB.m_records) {
        if ((record.m_Flags & 1) != 0) s_defaultOutdoorProviderPrefs = &record;
        if ((record.m_Flags & 2) != 0) s_defaultIndoorProviderPrefs = &record;
    }
}

void ClientDBInitialize() {
    StaticDBLoadAll();
    CheckDamageClassConsistency();
    InitTerrainSoundTypeIDs();
    InitWeaponSubclasses();
    InitSoundProviderPreferences();
    SDBItemSubclassInitialize();
}

void ClientDBShutdown() {
    s_damageTypeRecordIDs.clear();
    s_terrainSoundType.clear();
    s_weaponSubClasses.clear();
    s_unarmedWeaponSubclass = nullptr;
    s_defaultIndoorProviderPrefs = nullptr;
    s_defaultOutdoorProviderPrefs = nullptr;
    g_resistancesDB.Clear();
    g_stringLookupsDB.Clear();
    g_terrainTypeDB.Clear();
    g_itemClassDB.Clear();
    g_itemSubClassDB.Clear();
    g_soundProviderPreferencesDB.Clear();
    g_namesProfanityDB.Clear();
    g_namesReservedDB.Clear();
    g_taxiNodesDB.Clear();
    g_taxiPathDB.Clear();
    g_taxiPathNodeDB.Clear();
    s_staticDBLoaded = false;
    SDBItemSubclassDestroy();
}

const char* __fastcall ClientDBStringLookup(int stringID) {
    if (auto* record = g_stringLookupsDB.Lookup(stringID)) return record->m_String;
    return nullptr;
}

unsigned __fastcall GetPhysicalDamageClassID() { return static_cast<unsigned>(s_physicalDamageClassID); }
unsigned __fastcall GetFirstNonPhysicalID() { return static_cast<unsigned>(s_firstNonPhysicalDamageClass); }

const ResistancesRec* __fastcall GetDamageClassRecord(int id) {
    return g_resistancesDB.Lookup(id);
}

unsigned __fastcall ClientDBLookupTerrainSoundID(unsigned terrainTypeID) {
    return terrainTypeID < s_terrainSoundType.size() ? s_terrainSoundType[terrainTypeID] : 0;
}

const ItemSubClassRec* WeaponSubclass(unsigned id) {
    return id < s_weaponSubClasses.size() ? s_weaponSubClasses[id] : nullptr;
}

int __fastcall ClientDBGetWeaponSubclassParrySeq(unsigned id) {
    const auto* record = WeaponSubclass(id);
    return record ? record->m_weaponParrySeq : 0;
}

int __fastcall ClientDBGetWeaponSubclassReadySeq(unsigned id) {
    const auto* record = WeaponSubclass(id);
    return record ? record->m_weaponReadySeq : 0;
}

int __fastcall ClientDBGetWeaponSubclassWeaponSeq(unsigned id) {
    const auto* record = WeaponSubclass(id);
    return record ? record->m_weaponAttackSeq : 0;
}

unsigned __fastcall ClientDBGetNumWeaponSubclasses() {
    return static_cast<unsigned>(s_weaponSubClasses.size());
}

int __fastcall ClientDBWeaponSubclassSetsFingerSeq(unsigned id) {
    const auto* record = WeaponSubclass(id);
    return record ? record->m_flags & 2 : 0;
}

int __fastcall ClientDBGetUnarmedWeapon() {
    return s_unarmedWeaponSubclass ? s_unarmedWeaponSubclass->m_subClassID : 0;
}

const SoundProviderPreferencesRec* __fastcall ClientDBGetDefaultIndoorProviderPrefs() {
    return s_defaultIndoorProviderPrefs;
}

const SoundProviderPreferencesRec* __fastcall ClientDBGetDefaultOutdoorProviderPrefs() {
    return s_defaultOutdoorProviderPrefs;
}
template class WowClientDB<AreaMIDIAmbiencesRec>;
template class WowClientDB<AreaPOIRec>;
template class WowClientDB<AreaTableRec>;
template class WowClientDB<AreaTriggerRec>;
template class WowClientDB<AttackAnimKitsRec>;
template class WowClientDB<AttackAnimTypesRec>;
template class WowClientDB<BankBagSlotPricesRec>;
template class WowClientDB<CameraShakesRec>;
template class WowClientDB<CharacterCreateCamerasRec>;
template class WowClientDB<CharacterFacialHairStylesRec>;
template class WowClientDB<CharBaseInfoRec>;
template class WowClientDB<CharHairGeosetsRec>;
template class WowClientDB<CharStartOutfitRec>;
template class WowClientDB<CharTextureVariationsV2Rec>;
template class WowClientDB<CharVariationsRec>;
template class WowClientDB<ChrClassesRec>;
template class WowClientDB<ChrProficiencyRec>;
template class WowClientDB<ChrRacesRec>;
template class WowClientDB<CinematicCameraRec>;
template class WowClientDB<CinematicSequencesRec>;
template class WowClientDB<CreatureDisplayInfoExtraRec>;
template class WowClientDB<CreatureDisplayInfoRec>;
template class WowClientDB<CreatureFamilyRec>;
template class WowClientDB<CreatureModelDataRec>;
template class WowClientDB<CreatureSoundDataRec>;
template class WowClientDB<CreatureTypeRec>;
template class WowClientDB<DeathThudLookupsRec>;
template class WowClientDB<EmoteAnimsRec>;
template class WowClientDB<EmotesRec>;
template class WowClientDB<EmotesTextDataRec>;
template class WowClientDB<EmotesTextRec>;
template class WowClientDB<FactionGroupRec>;
template class WowClientDB<FactionRec>;
template class WowClientDB<FactionTemplateRec>;
template class WowClientDB<FootprintTexturesRec>;
template class WowClientDB<FootstepTerrainLookupRec>;
template class WowClientDB<GameObjectDisplayInfoRec>;
template class WowClientDB<GroundEffectDoodadRec>;
template class WowClientDB<GroundEffectTextureRec>;
template class WowClientDB<HelmetGeosetVisDataRec>;
template class WowClientDB<ItemClassRec>;
template class WowClientDB<ItemDisplayInfoRec>;
template class WowClientDB<ItemGroupSoundsRec>;
template class WowClientDB<ItemSubClassRec>;
template class WowClientDB<ItemVisualEffectsRec>;
template class WowClientDB<ItemVisualsRec>;
template class WowClientDB<LanguagesRec>;
template class WowClientDB<LanguageWordsRec>;
template class WowClientDB<LockRec>;
template class WowClientDB<LockTypeRec>;
template class WowClientDB<MapRec>;
template class WowClientDB<MaterialRec>;
template class WowClientDB<NamesProfanityRec>;
template class WowClientDB<NamesReservedRec>;
template class WowClientDB<NPCSoundsRec>;
template class WowClientDB<PageTextMaterialRec>;
template class WowClientDB<PaperDollItemFrameRec>;
template class WowClientDB<QuestInfoRec>;
template class WowClientDB<QuestSortRec>;
template class WowClientDB<ResistancesRec>;
template class WowClientDB<SheatheSoundLookupsRec>;
template class WowClientDB<SkillLineAbilityRec>;
template class WowClientDB<SkillLineRec>;
template class WowClientDB<SoundCharacterMacroLinesRec>;
template class WowClientDB<SoundEntriesRec>;
template class WowClientDB<SoundProviderPreferencesRec>;
template class WowClientDB<SoundSamplePreferencesRec>;
template class WowClientDB<SoundWaterTypeRec>;
template class WowClientDB<SpellAuraNamesRec>;
template class WowClientDB<SpellCastTimesRec>;
template class WowClientDB<SpellChainEffectsRec>;
template class WowClientDB<SpellDispelTypeRec>;
template class WowClientDB<SpellDurationRec>;
template class WowClientDB<SpellEffectCameraShakesRec>;
template class WowClientDB<SpellEffectNamesRec>;
template class WowClientDB<SpellFocusObjectRec>;
template class WowClientDB<SpellIconRec>;
template class WowClientDB<SpellItemEnchantmentRec>;
template class WowClientDB<SpellRadiusRec>;
template class WowClientDB<SpellRangeRec>;
template class WowClientDB<SpellRec>;
template class WowClientDB<SpellShapeshiftFormRec>;
template class WowClientDB<SpellVisualAnimNameRec>;
template class WowClientDB<SpellVisualEffectNameRec>;
template class WowClientDB<SpellVisualKitRec>;
template class WowClientDB<SpellVisualPrecastTransitionsRec>;
template class WowClientDB<SpellVisualRec>;
template class WowClientDB<StringLookupsRec>;
template class WowClientDB<TabardBackgroundTexturesRec>;
template class WowClientDB<TabardEmblemTexturesRec>;
template class WowClientDB<TaxiNodesRec>;
template class WowClientDB<TaxiPathNodeRec>;
template class WowClientDB<TaxiPathRec>;
template class WowClientDB<TerrainTypeRec>;
template class WowClientDB<TerrainTypeSoundsRec>;
template class WowClientDB<TransportAnimationRec>;
template class WowClientDB<UISoundLookupsRec>;
template class WowClientDB<UnitBloodLevelsRec>;
template class WowClientDB<UnitBloodRec>;
template class WowClientDB<VideoHardwareRec>;
template class WowClientDB<VocalUISoundsRec>;
template class WowClientDB<WeaponImpactSoundsRec>;
template class WowClientDB<WeaponSwingSounds2Rec>;
template class WowClientDB<WMOAreaTableRec>;
template class WowClientDB<WorldMapAreaRec>;
template class WowClientDB<WorldMapContinentRec>;
template class WowClientDB<WorldSafeLocsRec>;
template class WowClientDB<ZoneMusicRec>;
