#pragma once

#include "WoW/Source/DB/DBClient/AutoCode/itemclassrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/itemsubclassrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/namesprofanityrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/namesreservedrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/spellvisualeffectnamerec.h"
#include "WoW/Source/DB/DBClient/AutoCode/resistancesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/soundproviderpreferencesrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/stringlookupsrec.h"
#include "WoW/Source/DB/DBClient/AutoCode/terraintyperec.h"
#include "WoW/Source/DB/DBClient/sdbitemsubclass.h"
#include "WoW/Source/DB/DBClient/wowclientdb.h"

void ClientDBInitialize();
void CheckDamageClassConsistency();
unsigned InitTerrainSoundTypeIDs();
unsigned InitWeaponSubclasses();
const ItemClassRec* LocateWeaponSubclass();
void InitSoundProviderPreferences();
void ClientDBShutdown();
const char* __fastcall ClientDBStringLookup(int stringID);
unsigned __fastcall GetPhysicalDamageClassID();
unsigned __fastcall GetFirstNonPhysicalID();
const ResistancesRec* __fastcall GetDamageClassRecord(int id);
unsigned __fastcall ClientDBLookupTerrainSoundID(unsigned terrainTypeID);
int __fastcall ClientDBGetWeaponSubclassParrySeq(unsigned subClassID);
int __fastcall ClientDBGetWeaponSubclassReadySeq(unsigned subClassID);
int __fastcall ClientDBGetWeaponSubclassWeaponSeq(unsigned subClassID);
unsigned __fastcall ClientDBGetNumWeaponSubclasses();
int __fastcall ClientDBWeaponSubclassSetsFingerSeq(unsigned subClassID);
int __fastcall ClientDBGetUnarmedWeapon();
const SoundProviderPreferencesRec* __fastcall ClientDBGetDefaultIndoorProviderPrefs();
const SoundProviderPreferencesRec* __fastcall ClientDBGetDefaultOutdoorProviderPrefs();
void StaticDBLoadAll();

extern WowClientDB<ResistancesRec> g_resistancesDB;
extern WowClientDB<StringLookupsRec> g_stringLookupsDB;
extern WowClientDB<TerrainTypeRec> g_terrainTypeDB;
extern WowClientDB<ItemClassRec> g_itemClassDB;
extern WowClientDB<ItemSubClassRec> g_itemSubClassDB;
extern WowClientDB<SoundProviderPreferencesRec> g_soundProviderPreferencesDB;
extern WowClientDB<SpellVisualEffectNameRec> g_spellVisualEffectNameDB;
extern WowClientDB<SoundEntriesRec> g_soundEntriesDB;
extern WowClientDB<SoundWaterTypeRec> g_soundWaterTypeDB;
extern WowClientDB<SoundCharacterMacroLinesRec> g_soundCharacterMacroLinesDB;
extern WowClientDB<ChrRacesRec> g_chrRacesDB;
extern WowClientDB<NamesProfanityRec> g_namesProfanityDB;
extern WowClientDB<NamesReservedRec> g_namesReservedDB;
extern WowClientDB<TaxiNodesRec> g_taxiNodesDB;
extern WowClientDB<TaxiPathRec> g_taxiPathDB;
extern WowClientDB<TaxiPathNodeRec> g_taxiPathNodeDB;
