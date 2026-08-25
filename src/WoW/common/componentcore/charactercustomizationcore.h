#pragma once

#include "Storm/h/stpl.h"
#include "WoW/common/compdecorationcore.h"

#include <cstdint>

class CStatus;
class CTexComponent;
struct HMODEL__;
struct HTEXCOMPONENT__;
class CTexture;
using HTEXTURE__ = CTexture;
struct HCHARGEOSET__;

enum CHARACTER_ITEM_GEOSETS : int {
    CHARITEMGEOSETS_GLOVES = 0,
    CHARITEMGEOSETS_BOOTS,
    CHARITEMGEOSETS_SLEEVES,
    CHARITEMGEOSETS_PANTS,
    CHARITEMGEOSETS_DOUBLET,
    CHARITEMGEOSETS_PANTDOUBLET,
    CHARITEMGEOSETS_TABARD,
    CHARITEMGEOSETS_ROBE,
    CHARITEMGEOSETS_LOINCLOTH,
    NUM_CHARITEMGEOSETS,
    INVALID_CHARITEMGEOSET = -1,
};

enum CHARACTER_GEOSET_SECTIONS : int {
    CHARGEOSET_HAIR = 0,
    CHARGEOSET_BEARD,
    CHARGEOSET_SIDEBURN,
    CHARGEOSET_MOUSTACHE,
    CHARGEOSET_GLOVE,
    CHARGEOSET_BOOT,
    CHARGEOSET_OBSOLETEDONTUSEME,
    CHARGEOSET_EAR,
    CHARGEOSET_SLEEVES,
    CHARGEOSET_PANTS,
    CHARGEOSET_DOUBLET,
    CHARGEOSET_PANTDOUBLET,
    CHARGEOSET_TABARD,
    CHARGEOSET_ROBE,
    CHARGEOSET_LOINCLOTH,
    NUM_CHARGEOSETS,
    CHARGEOSET_NONE = -1,
};

enum INVENTORY_TYPES : int {
    INDEX_NON_EQUIP_TYPE = 0,
    INDEX_HEAD_TYPE,
    INDEX_NECK_TYPE,
    INDEX_SHOULDER_TYPE,
    INDEX_BODY_TYPE,
    INDEX_CHEST_TYPE,
    INDEX_WAIST_TYPE,
    INDEX_LEGS_TYPE,
    INDEX_FEET_TYPE,
    INDEX_WRIST_TYPE,
    INDEX_HAND_TYPE,
    INDEX_FINGER_TYPE,
    INDEX_TRINKET_TYPE,
    INDEX_WEAPON_TYPE,
    INDEX_SHIELD_TYPE,
    INDEX_RANGED_TYPE,
    INDEX_CLOAK_TYPE,
    INDEX_2HWEAPON_TYPE,
    INDEX_BAG_TYPE,
    INDEX_TABARD_TYPE,
    INDEX_ROBE_TYPE,
    INDEX_WEAPONMAINHAND_TYPE,
    INDEX_WEAPONOFFHAND_TYPE,
    INDEX_HOLDABLE_TYPE,
    INDEX_AMMO_TYPE,
    INDEX_THROWN_TYPE,
    INDEX_RANGEDRIGHT_TYPE,
    INDEX_NUMSLOTS,
};

enum CHARTEXTURESECTIONID : int {
    CHARTEXTURESECTION_SKIN = 0,
    CHARTEXTURESECTION_NAKEDSKINPELVIS,
    CHARTEXTURESECTION_NAKEDSKINTORSO,
    CHARTEXTURESECTION_SKINEXTRA,
    CHARTEXTURESECTION_FACELOWER,
    CHARTEXTURESECTION_FACEUPPER,
    CHARTEXTURESECTION_HAIR,
    CHARTEXTURESECTION_SCALPLOWERHAIR,
    CHARTEXTURESECTION_SCALPUPPERHAIR,
    CHARTEXTURESECTION_FACIALLOWERHAIR,
    CHARTEXTURESECTION_FACIALUPPERHAIR,
    CHARTEXTURESECTION_NUM,
};

struct INTDATA { int theInt = 0; };
struct FACIALGEOSETS {
    unsigned int beardGeoset = 0;
    unsigned int sideBurnGeoset = 0;
    unsigned int moustacheGeoset = 0;
};
struct FACIALVARIATIONS { TSFixedArray<FACIALGEOSETS> facialGeosets; };
struct VARIATIONS {
    unsigned int textureHolds[4]{};
    FACIALVARIATIONS facialVariations;
    TSGrowableArray<INTDATA> hairGeosets;
    VARIATIONS() : hairGeosets(16) {}
};
struct CAMERAFILENAMES { char fileName[3][260]{}; };
struct HOLDINFO {
    CHARACTER_ITEM_GEOSETS geosetGroup = INVALID_CHARITEMGEOSET;
    TEXCOMPONENT_SECTIONS holdSection = TCS_INVALIDSECTION;
};
struct INVHOLDINFO {
    HOLDINFO holdInfo[2];
    INVHOLDINFO() = default;
    INVHOLDINFO(CHARACTER_ITEM_GEOSETS geoset0, TEXCOMPONENT_SECTIONS section0,
                CHARACTER_ITEM_GEOSETS geoset1, TEXCOMPONENT_SECTIONS section1);
};
struct BEARDSTYLEDATA {
    unsigned int beardGeoset = 0;
    unsigned int sideBurnGeoset = 0;
    unsigned int moustacheGeoset = 0;
};
struct ItemDisplayInfoRec {
    int m_ID = 0;
    int m_geosetGroup[4]{};
    int m_flags = 0;
};
struct CharGeosetInfo {
    int highestPriority[9]{};
    unsigned int currentGeosets[9]{};
    unsigned int geosetCurrentlyUsedBy[9]{};
    unsigned int disabledByFlags[9]{};
    unsigned int flags[9]{};
    unsigned int inventoryTypeGeosets[9][27]{};

    CharGeosetInfo();
    CharGeosetInfo(const CharGeosetInfo& rhs);
    void UpdateGeosetDisplay(const ItemDisplayInfoRec* displayInfoRec, unsigned int itemInventoryType,
                             HTEXCOMPONENT__* component, int playerRace);
    void ShowInventoryTypeTextureHolds(HTEXCOMPONENT__* component, INVENTORY_TYPES inventoryType,
                                       int adding);
    void Clear();
    void RemoveGeosetInfo(const ItemDisplayInfoRec* displayInfoRec, INVENTORY_TYPES inventoryType,
                          HTEXCOMPONENT__* component);
    int ShowingSameGeosetsAs(const CharGeosetInfo* rhs) const;
};
struct CCharGeoset {
    HMODEL__* m_charModel = nullptr;
    HMODEL__* m_paperDollModel = nullptr;
    CharGeosetInfo m_geosetInfo;
    CharGeosetInfo m_workingGeosetInfo;
    int m_flags = 0;
    unsigned int m_currentGeosets[15]{};

    void ClearGeosets();
    void Commit();
    void CommitGeosets(HMODEL__* model);
    void CommitWorkingGeosetInfo();
    void AddItemGeoset(const ItemDisplayInfoRec* displayInfoRec, unsigned int itemInventoryType,
                       HTEXCOMPONENT__* component, int playerRace);
    void RemoveItemGeoset(const ItemDisplayInfoRec* displayInfoRec, unsigned int itemInventoryType,
                          HTEXCOMPONENT__* component);
    void ShowGeosetSection(CHARACTER_GEOSET_SECTIONS section, unsigned int geosetNumber,
                           int hideRemainder);
    void HideGeosetSection(CHARACTER_GEOSET_SECTIONS section);
    void ShowGeosetSection(HMODEL__* model, CHARACTER_GEOSET_SECTIONS section,
                           unsigned int geosetNumber, int hideRemainder);
    void HideGeosetSection(HMODEL__* model, CHARACTER_GEOSET_SECTIONS section);
    void EnableHairGeosets(unsigned int race, unsigned int sex, unsigned int hairStyleID);
};
struct CHARACTERVARIATIONS {
    TSGrowableArray<const char*> color;
    CHARACTERVARIATIONS() : color(16) {}
};
struct CHARACTERSEXVARIATIONS {
    TSGrowableArray<CHARACTERVARIATIONS> names[11];
    int firstNPCVar[11]{};
    int lastNPCVar[11]{};
    CHARACTERSEXVARIATIONS() : names{TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16),
                                      TSGrowableArray<CHARACTERVARIATIONS>(16)} {}
    void GetNumVariations(CHARTEXTURESECTIONID section, int* pcVars, int* npcVars);
};
struct CHARACTERRACEVARIATIONS {
    CHARACTERSEXVARIATIONS sex[3];
    CHARACTERRACEVARIATIONS() = default;
};

extern TSFixedArray<TSFixedArray<VARIATIONS>> s_characterVariations;
extern TSFixedArray<CAMERAFILENAMES> s_cameraFileNames;
extern TSGrowableArray<CHARACTERRACEVARIATIONS> s_raceTextureFileNames;
extern INVHOLDINFO s_itemTypeTextureHolds[27];
extern unsigned int g_defaultGeosetIDOffsets[15];

const char* __fastcall GetCameraFileName(unsigned int sex, unsigned int camera);
void CharCustomizationInitialize();
int InitializeCameraFileNames();
int InitializeTextureFileNames();
unsigned int FillInMissingTextureFileNames();
void __fastcall ReadTextureFileNames(int index);
int InitializeHairGeosets();
void InitializeTextureHoldLayers();
void InitializeFacialHairVariations();
void CharCustomizationShutdown();
const char* CharCustomizationGetTextureName(unsigned int raceID, unsigned int sexID,
                                             CHARTEXTURESECTIONID section,
                                             unsigned int variationID,
                                             unsigned int colorID, int isNPC = 0);
void __fastcall CharCustomizationGetNumSkinTextures(unsigned int sexID, unsigned int raceID,
                                                     int* pcVars, int* npcVars);
HTEXTURE__* __fastcall CharCustomizationLoadSkin(const char* skinName, CStatus* status,
                                                  unsigned int raceID, unsigned int sexID,
                                                  int textureNumber, int isNPC);
HTEXTURE__* __fastcall CharCustomizationSetSkin(unsigned int raceID, unsigned int colorID,
                                                 unsigned int sexID, unsigned int textureNumber,
                                                 int isNPC);
void __fastcall CharCustomizationNumFaces(unsigned int sexID, unsigned int raceID,
                                           int* pcVars, int* npcVars);
void __fastcall CharCustomizationSetFaceTexture(HTEXCOMPONENT__* texComponent, CTexComponent* component,
                                                unsigned int raceID, unsigned int sexID,
                                                CHARACTERSEXVARIATIONS* variations, unsigned int colorID,
                                                int isNPC);
unsigned int __fastcall CharCustomizationNumHairColors(unsigned int sexID, unsigned int raceID);
void __fastcall CharCustomizationSetHairTexture(HTEXCOMPONENT__* texComponent, char* unused,
                                                unsigned int raceID, const char* sexID,
                                                unsigned int hairID, unsigned int colorID);
void __fastcall CharCustomizationInitBaseCharacter(CCharGeoset* geoset, unsigned int beard,
                                                   unsigned int sideBurn, unsigned int moustache,
                                                   unsigned int ear);
void __fastcall CharCustomizationResetHairGeoset(CCharGeoset* geoset, unsigned int race,
                                                  unsigned int sex, unsigned int hairStyleID);
int __fastcall CharCustomizationGetNakedSectionName(unsigned int sexID, unsigned int raceID,
                                                    unsigned int skinID, unsigned int underwearSection,
                                                    char* outBuffer, unsigned int outBufferSize, int isNPC);
void __fastcall CharCustomizationCommitItemGeosets(CCharGeoset* geoset, int commit);
void __fastcall CharCustomizationClearItemGeosets(CCharGeoset* geoset);
void __fastcall CharCustomizationAddItemGeosets(CCharGeoset* geoset, const ItemDisplayInfoRec* displayInfoRec,
                                                unsigned int itemInventoryType, HTEXCOMPONENT__* component,
                                                int playerRace);
void __fastcall CharCustomizationRemoveItemGeosets(CCharGeoset* geoset,
                                                   const ItemDisplayInfoRec* displayInfoRec,
                                                   unsigned int itemInventoryType, HTEXCOMPONENT__* component);
unsigned int __fastcall CharCustomizationNumHairStyles(unsigned int sexID, unsigned int raceID);
unsigned int __fastcall CharCustomizationGetHairGeoset(unsigned int sex, unsigned int race, unsigned int hair);
unsigned int __fastcall CharCustomizationNumBeardStyles(unsigned int sexID, unsigned int raceID);
unsigned int __fastcall CharCustomizationGetBeardStyle(unsigned int sexID, unsigned int raceID,
                                                        unsigned int facialHairID, BEARDSTYLEDATA* data);
void __fastcall CharCustomizationSetFacialTexture(HTEXCOMPONENT__* texComponent, CTexComponent* component,
                                                  unsigned int raceID, const char* sexID,
                                                  unsigned int facialID, unsigned int colorID);
HCHARGEOSET__* __fastcall CharCustomizationCreateGeosetHandle(HMODEL__* model);
void __fastcall CharCustomizationShowGeoset(CCharGeoset* geoset, CHARACTER_GEOSET_SECTIONS section,
                                            unsigned int geosetNumber);
void __fastcall CharCustomizationHideGeosetSection(CCharGeoset* geoset,
                                                    CHARACTER_GEOSET_SECTIONS section);
void __fastcall CharCustomizationCommitGeosets(CCharGeoset* geoset);
void __fastcall CharCustomizationGetTextureLayerHolds(unsigned int sexID, unsigned int raceID,
                                                      unsigned int* textureLayerHolds,
                                                      unsigned int numTextureLayerHolds);
void __fastcall CharCustomizationSetPaperDollGeoset(HCHARGEOSET__* handle, HMODEL__* paperDollModel,
                                                    int modelHandle);

void __fastcall CharCustomizationCommitItemGeosets(CCharGeoset* geoset, int commit,
                                                   HMODEL__* paperDollModel);

int _E137_3();
int _E142_2();
int _E147();
INVHOLDINFO* _E152();
