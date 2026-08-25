#pragma once

#include <cstdint>

class CGameObjectDef {
public:
    enum Type : int {
        TYPE_DOOR = 0,
        TYPE_BUTTON,
        TYPE_QUESTGIVER,
        TYPE_CHEST,
        TYPE_BINDER,
        TYPE_GENERIC,
        TYPE_TRAP,
        TYPE_CHAIR,
        TYPE_SPELL_FOCUS,
        TYPE_TEXT,
        TYPE_GOOBER,
        TYPE_TRANSPORT,
        TYPE_AREADAMAGE,
        TYPE_CAMERA,
        TYPE_MAP_OBJECT,
        TYPE_MO_TRANSPORT,
        TYPE_DUEL_ARBITER,
        TYPE_FISHINGNODE,
        TYPE_RITUAL,
        NUM_GAMEOBJECT_TYPE,
    };

    enum Property : int {
        PROP_TYPE = 0,
        PROP_STARTOPEN,
        PROP_STARTDESTROYED,
        PROP_AUTOCLOSE,
        PROP_LOCK,
        PROP_QUESTLISTID,
        PROP_CHESTLOOT,
        PROP_CHESTLOOTTIME,
        PROP_CONSUMABLE,
        PROP_CHARGES,
        PROP_SPELL,
        PROP_CHAIRSLOTS,
        PROP_CHAIRHEIGHT,
        PROP_CHESTLOOTEDEVENT,
        PROP_SPELLFOCUSTYPE,
        PROP_TEXTID,
        PROP_TEXTLANGUAGE,
        PROP_TEXTMATERIAL,
        PROP_HIGHLIGHT,
        PROP_FLOATINGTOOLTIP,
        PROP_QUESTID,
        PROP_EVENTID,
        PROP_CUSTOMANIM,
        PROP_COOLDOWN,
        PROP_RADIUS,
        PROP_CHESTMINRESTOCK,
        PROP_CHESTMAXRESTOCK,
        PROP_DAMAGE_MIN,
        PROP_DAMAGE_MAX,
        PROP_DAMAGE_SCHOOL,
        PROP_LINKED_TRAP,
        PROP_TRAP_LEVEL,
        PROP_STARTDELAY,
        PROP_CAMERAID,
        PROP_CASTERS,
        PROP_TAXIPATHID1,
        PROP_TAXIPATHID2,
        PROP_MOVESPEED,
        NUM_PROP,
    };

    enum ValueType : int {
        VALUE_TYPE_TYPE = 0,
        VALUE_TYPE_BOOL,
        VALUE_TYPE_INT,
        VALUE_TYPE_FLOAT,
        VALUE_TYPE_NAME,
        VALUE_TYPE_LOCK,
        VALUE_TYPE_QUESTGIVER,
        VALUE_TYPE_LOOTTABLE,
        VALUE_TYPE_SPELL,
        VALUE_TYPE_PAGETEXT,
        VALUE_TYPE_PAGEMATERIAL,
        VALUE_TYPE_TRAP,
        VALUE_TYPE_CAMERA,
        NUM_VALUE_TYPE,
    };

    enum BaseType : int {
        BASE_TYPE_NUMBER = 0,
        BASE_TYPE_ENUM,
        BASE_TYPE_STRING,
        NUM_BASE_TYPE,
    };

    enum OwnerType : int {
        OWNER_TYPE_TERRAIN = 0,
        OWNER_TYPE_MAPOBJ,
        OWNER_TYPE_SPAWNER,
        NUM_OWNER_TYPE,
    };

    struct EnumValue {
        int count;
        const char** list;
        int defaultIndex;
    };

    struct NumberValue {
        float min;
        float max;
        float step;
        float defaultValue;
    };

    struct StringValue {
        const char* defaultValue;
    };

    union ValueInfo {
        EnumValue e;
        NumberValue n;
        StringValue s;
    };

    struct PropertyInfo {
        const char* name;
        int valueType;
        int valueBaseType;
        const ValueInfo* valueInfo;
    };

    struct ObjectInfo {
        const char* name;
        int numProperties;
        const int* propertyInfo;
        const ValueInfo* const* valueInfo;
    };

    static const char* __fastcall NameFromPropId(unsigned int propId);
    static int __fastcall PropIdFromName(const char* name);
    static int __fastcall GetPropValueType(unsigned int propId);
    static int __fastcall GetPropValueBaseType(unsigned int propId);
    static const char* __fastcall NameFromTypeId(unsigned int typeId);
    static int __fastcall TypeIdFromName(const char* name);
    static int __fastcall GetNumProps(unsigned int typeId);
    static int __fastcall GetPropId(unsigned int typeId, int propNum);
    static int __fastcall GetPropNum(int typeId, int propId);
    static const ValueInfo* __fastcall GetPropValueInfo(unsigned int typeId, int propNum);
};

extern CGameObjectDef::PropertyInfo s_propertyInfo[CGameObjectDef::NUM_PROP];
extern CGameObjectDef::ObjectInfo s_objectInfo[CGameObjectDef::NUM_GAMEOBJECT_TYPE];

extern float MAX_DUEL_DISTANCE_SQUARED_96;
extern float MAX_TRADE_DISTANCE_SQUARED_96;
extern float MAX_SHOP_DISTANCE_SQUARED_96;
extern float MAX_INSPECT_DISTANCE_SQUARED_96;
extern float MAX_LOOT_DISTANCE_SQUARED_96;
extern float MAX_BIND_DISTANCE_SQUARED_96;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_96;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_96;
extern float MAX_SHARE_DISTANCE_SQUARED_96;
extern float SECONDS_PER_HEARTBEAT_96;
extern float SPELL_FOCUS_DISTANCE_SQUARED_95;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_96;

int _E1_291();
int _E2_291();
int _E4_288();
int _E5_288();
int _E7_288();
int _E8_288();
int _E10();
int _E11();
int _E13_9();
int _E14_14();
int _E16_21();
int _E17_18();
int _E19_46();
int _E20_63();
int _E22_54();
int _E23_69();
int _E25_55();
int _E26_64();
int _E28_94();
int _E29_65();
int _E31_91();
int _E32_57();
int _E34_70();
int _E35_40();
int _E37_65();
int _E38_39();
