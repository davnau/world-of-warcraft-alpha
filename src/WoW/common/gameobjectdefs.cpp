#include "WoW/common/gameobjectdefs.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <limits>

CGameObjectDef::PropertyInfo s_propertyInfo[CGameObjectDef::NUM_PROP]{};
CGameObjectDef::ObjectInfo s_objectInfo[CGameObjectDef::NUM_GAMEOBJECT_TYPE]{};

namespace {

const char* kPropertyNames[CGameObjectDef::NUM_PROP] = {
    "type", "startOpen", "startDestroyed", "autoClose", "lock", "questListId",
    "chestLoot", "chestLootTime", "consumable", "charges", "spell", "chairSlots",
    "chairHeight", "chestLootedEvent", "spellFocusType", "textId", "textLanguage",
    "textMaterial", "highlight", "floatingTooltip", "questId", "eventId", "customAnim",
    "cooldown", "radius", "chestMinRestock", "chestMaxRestock", "damageMin",
    "damageMax", "damageSchool", "linkedTrap", "trapLevel", "startDelay", "cameraId",
    "casters", "taxiPathId1", "taxiPathId2", "moveSpeed",
};

const char* kTypeNames[CGameObjectDef::NUM_GAMEOBJECT_TYPE] = {
    "Door", "Button", "QuestGiver", "Chest", "Binder", "Generic", "Trap", "Chair",
    "SpellFocus", "Text", "Goober", "Transport", "AreaDamage", "Camera", "MapObject",
    "MoTransport", "DuelArbiter", "FishingNode", "Ritual",
};

CGameObjectDef::ValueInfo kDefaultValues[CGameObjectDef::NUM_PROP]{};
int kObjectProperties[CGameObjectDef::NUM_GAMEOBJECT_TYPE][CGameObjectDef::NUM_PROP]{};
int kObjectPropertyCounts[CGameObjectDef::NUM_GAMEOBJECT_TYPE]{};
const CGameObjectDef::ValueInfo* kObjectValueInfo[CGameObjectDef::NUM_GAMEOBJECT_TYPE][CGameObjectDef::NUM_PROP]{};

int PropertyValueType(int property) {
    if (property == CGameObjectDef::PROP_TYPE) return CGameObjectDef::VALUE_TYPE_TYPE;
    if (property == CGameObjectDef::PROP_RADIUS || property == CGameObjectDef::PROP_MOVESPEED)
        return CGameObjectDef::VALUE_TYPE_FLOAT;
    if (property == CGameObjectDef::PROP_STARTOPEN ||
        property == CGameObjectDef::PROP_STARTDESTROYED ||
        property == CGameObjectDef::PROP_AUTOCLOSE ||
        property == CGameObjectDef::PROP_CONSUMABLE ||
        property == CGameObjectDef::PROP_HIGHLIGHT ||
        property == CGameObjectDef::PROP_FLOATINGTOOLTIP)
        return CGameObjectDef::VALUE_TYPE_BOOL;
    return CGameObjectDef::VALUE_TYPE_INT;
}

int PropertyValueBaseType(int property) {
    return property == CGameObjectDef::PROP_TYPE ||
                   property == CGameObjectDef::PROP_LOCK ||
                   property == CGameObjectDef::PROP_DAMAGE_SCHOOL
               ? CGameObjectDef::BASE_TYPE_ENUM
               : CGameObjectDef::BASE_TYPE_NUMBER;
}

void AddProperty(int type, int property) {
    const int count = kObjectPropertyCounts[type];
    kObjectProperties[type][count] = property;
    kObjectValueInfo[type][count] = &kDefaultValues[property];
    kObjectPropertyCounts[type] = count + 1;
}

struct MetadataInit {
    MetadataInit() {
        for (int property = 0; property < CGameObjectDef::NUM_PROP; ++property) {
            kDefaultValues[property].n = {0.0F, 0.0F, 1.0F, 0.0F};
        }
        AddProperty(CGameObjectDef::TYPE_DOOR, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_DOOR, CGameObjectDef::PROP_STARTOPEN);
        AddProperty(CGameObjectDef::TYPE_DOOR, CGameObjectDef::PROP_STARTDESTROYED);
        AddProperty(CGameObjectDef::TYPE_DOOR, CGameObjectDef::PROP_AUTOCLOSE);
        AddProperty(CGameObjectDef::TYPE_DOOR, CGameObjectDef::PROP_LOCK);
        AddProperty(CGameObjectDef::TYPE_BUTTON, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_BUTTON, CGameObjectDef::PROP_STARTOPEN);
        AddProperty(CGameObjectDef::TYPE_BUTTON, CGameObjectDef::PROP_AUTOCLOSE);
        AddProperty(CGameObjectDef::TYPE_QUESTGIVER, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_QUESTGIVER, CGameObjectDef::PROP_QUESTLISTID);
        AddProperty(CGameObjectDef::TYPE_CHEST, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_CHEST, CGameObjectDef::PROP_CHESTLOOT);
        AddProperty(CGameObjectDef::TYPE_CHEST, CGameObjectDef::PROP_CHESTLOOTTIME);
        AddProperty(CGameObjectDef::TYPE_CHEST, CGameObjectDef::PROP_CONSUMABLE);
        AddProperty(CGameObjectDef::TYPE_CHEST, CGameObjectDef::PROP_CHARGES);
        AddProperty(CGameObjectDef::TYPE_CHEST, CGameObjectDef::PROP_CHESTLOOTEDEVENT);
        AddProperty(CGameObjectDef::TYPE_BINDER, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_GENERIC, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_GENERIC, CGameObjectDef::PROP_FLOATINGTOOLTIP);
        AddProperty(CGameObjectDef::TYPE_TRAP, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_TRAP, CGameObjectDef::PROP_SPELL);
        AddProperty(CGameObjectDef::TYPE_TRAP, CGameObjectDef::PROP_LINKED_TRAP);
        AddProperty(CGameObjectDef::TYPE_TRAP, CGameObjectDef::PROP_TRAP_LEVEL);
        AddProperty(CGameObjectDef::TYPE_TRAP, CGameObjectDef::PROP_STARTDELAY);
        AddProperty(CGameObjectDef::TYPE_CHAIR, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_CHAIR, CGameObjectDef::PROP_CHAIRSLOTS);
        AddProperty(CGameObjectDef::TYPE_CHAIR, CGameObjectDef::PROP_CHAIRHEIGHT);
        AddProperty(CGameObjectDef::TYPE_SPELL_FOCUS, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_SPELL_FOCUS, CGameObjectDef::PROP_SPELLFOCUSTYPE);
        AddProperty(CGameObjectDef::TYPE_TEXT, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_TEXT, CGameObjectDef::PROP_TEXTID);
        AddProperty(CGameObjectDef::TYPE_TEXT, CGameObjectDef::PROP_TEXTLANGUAGE);
        AddProperty(CGameObjectDef::TYPE_TEXT, CGameObjectDef::PROP_TEXTMATERIAL);
        AddProperty(CGameObjectDef::TYPE_GOOBER, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_GOOBER, CGameObjectDef::PROP_SPELL);
        AddProperty(CGameObjectDef::TYPE_TRANSPORT, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_AREADAMAGE, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_AREADAMAGE, CGameObjectDef::PROP_RADIUS);
        AddProperty(CGameObjectDef::TYPE_CAMERA, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_CAMERA, CGameObjectDef::PROP_CAMERAID);
        AddProperty(CGameObjectDef::TYPE_MAP_OBJECT, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_MO_TRANSPORT, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_DUEL_ARBITER, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_FISHINGNODE, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_RITUAL, CGameObjectDef::PROP_TYPE);
        AddProperty(CGameObjectDef::TYPE_RITUAL, CGameObjectDef::PROP_SPELL);

        for (int property = 0; property < CGameObjectDef::NUM_PROP; ++property) {
            s_propertyInfo[property] = {
                kPropertyNames[property],
                PropertyValueType(property),
                PropertyValueBaseType(property),
                &kDefaultValues[property],
            };
        }
        for (int type = 0; type < CGameObjectDef::NUM_GAMEOBJECT_TYPE; ++type) {
            s_objectInfo[type] = {
                kTypeNames[type],
                kObjectPropertyCounts[type],
                kObjectProperties[type],
                kObjectValueInfo[type],
            };
        }
    }
};

MetadataInit s_metadataInit;

bool EqualName(const char* left, const char* right) {
    if (left == nullptr || right == nullptr) return false;
    while (*left != '\0' && *right != '\0') {
        if (std::tolower(static_cast<unsigned char>(*left)) !=
            std::tolower(static_cast<unsigned char>(*right))) return false;
        ++left;
        ++right;
    }
    return *left == *right;
}

} // namespace

float INFINITY_291 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_96 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_96 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_96 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_96 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_96 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_96 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_96 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_96 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_96 = 0.0F;
float SECONDS_PER_HEARTBEAT_96 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_95 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_96 = 0.0F;

int _E1_291() { INFINITY_291 = std::numeric_limits<float>::infinity(); return 0x7F800000; }
int _E2_291() { return _E1_291(); }
int _E4_288() { MAX_DUEL_DISTANCE_SQUARED_96 = 10.0F * 10.0F; return 0; }
int _E5_288() { return _E4_288(); }
int _E7_288() { MAX_TRADE_DISTANCE_SQUARED_96 = 11.111111F * 11.111111F; return 0; }
int _E8_288() { return _E7_288(); }
int _E10() { MAX_SHOP_DISTANCE_SQUARED_96 = 5.5555553F * 5.5555553F; return 0; }
int _E11() { return _E10(); }
int _E13_9() { MAX_INSPECT_DISTANCE_SQUARED_96 = 5.5555553F * 5.5555553F; return 0; }
int _E14_14() { return _E13_9(); }
int _E16_21() { MAX_LOOT_DISTANCE_SQUARED_96 = 5.0F * 5.0F; return 0; }
int _E17_18() { return _E16_21(); }
int _E19_46() { MAX_BIND_DISTANCE_SQUARED_96 = 10.0F * 10.0F; return 0; }
int _E20_63() { return _E19_46(); }
int _E22_54() { MAX_SITCHAIR_DISTANCE_SQUARED_96 = 0.5F * 0.5F; return 0; }
int _E23_69() { return _E22_54(); }
int _E25_55() { MAX_SITCHAIRUSE_DISTANCE_SQUARED_96 = 3.0F * 3.0F; return 0; }
int _E26_64() { return _E25_55(); }
int _E28_94() { MAX_SHARE_DISTANCE_SQUARED_96 = 100.0F * 100.0F; return 0; }
int _E29_65() { return _E28_94(); }
int _E31_91() { SECONDS_PER_HEARTBEAT_96 = 5.0F; return 0; }
int _E32_57() { return _E31_91(); }
int _E34_70() { SPELL_FOCUS_DISTANCE_SQUARED_95 = 50.0F * 50.0F; return 0; }
int _E35_40() { return _E34_70(); }
int _E37_65() { MAX_OBJ_INTEREST_RADIUS_SQUARED_96 = 100.0F * 100.0F; return 0; }
int _E38_39() { return _E37_65(); }

const char* __fastcall CGameObjectDef::NameFromPropId(unsigned int propId) {
    return propId < NUM_PROP ? s_propertyInfo[propId].name : nullptr;
}

int __fastcall CGameObjectDef::PropIdFromName(const char* name) {
    if (name == nullptr) return -1;
    for (int i = 0; i < NUM_PROP; ++i) if (EqualName(name, s_propertyInfo[i].name)) return i;
    return -1;
}

int __fastcall CGameObjectDef::GetPropValueType(unsigned int propId) {
    return propId < NUM_PROP ? s_propertyInfo[propId].valueType : 0;
}

int __fastcall CGameObjectDef::GetPropValueBaseType(unsigned int propId) {
    return propId < NUM_PROP ? s_propertyInfo[propId].valueBaseType : 0;
}

const char* __fastcall CGameObjectDef::NameFromTypeId(unsigned int typeId) {
    return typeId < NUM_GAMEOBJECT_TYPE ? s_objectInfo[typeId].name : nullptr;
}

int __fastcall CGameObjectDef::TypeIdFromName(const char* name) {
    if (name == nullptr) return -1;
    for (int i = 0; i < NUM_GAMEOBJECT_TYPE; ++i) if (EqualName(name, s_objectInfo[i].name)) return i;
    return -1;
}

int __fastcall CGameObjectDef::GetNumProps(unsigned int typeId) {
    return typeId < NUM_GAMEOBJECT_TYPE ? s_objectInfo[typeId].numProperties : 0;
}

int __fastcall CGameObjectDef::GetPropId(unsigned int typeId, int propNum) {
    if (typeId >= NUM_GAMEOBJECT_TYPE || propNum < 0 || propNum >= s_objectInfo[typeId].numProperties) return -1;
    return s_objectInfo[typeId].propertyInfo[propNum];
}

int __fastcall CGameObjectDef::GetPropNum(int typeId, int propId) {
    if (typeId < 0 || typeId >= NUM_GAMEOBJECT_TYPE || propId < 0 || propId >= NUM_PROP) return -1;
    for (int i = 0; i < s_objectInfo[typeId].numProperties; ++i)
        if (s_objectInfo[typeId].propertyInfo[i] == propId) return i;
    return -1;
}

const CGameObjectDef::ValueInfo* __fastcall CGameObjectDef::GetPropValueInfo(unsigned int typeId, int propNum) {
    if (typeId >= NUM_GAMEOBJECT_TYPE || propNum < 0 || propNum >= s_objectInfo[typeId].numProperties) return nullptr;
    return s_objectInfo[typeId].valueInfo[propNum];
}
