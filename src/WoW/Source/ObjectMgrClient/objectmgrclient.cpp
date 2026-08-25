#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"

#include "WoW/Source/Magic/spell_c.h"
#include "WoW/Source/clientruntime.h"

#include "Storm/h/SZip.h"
#include "WoW/Source/Object/objectclient/player_c.h"
#include "WoW/Source/UI/gameui.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

#include <bit>
#include <algorithm>
#include <cmath>
#include <cstdio>
#include <memory>
#include <unordered_set>
#include <vector>

void FrameScript_SignalEvent(unsigned eventId);
void FrameScript_SignalEvent(unsigned eventId, const char* format, ...);

struct ClntObjMgr {
    int playerType = 0;
    void* clientPtr = nullptr;
    void* net = nullptr;
    unsigned mapID = 0;
    std::uint64_t activePlayer = 0;
    NTempest::C3Vector activePosition{};
    float activeFacing = 0.0F;
    bool localPlayerInGame = false;
    std::vector<CLNT_VISIBLE_OBJECT> visibleObjects;
    std::vector<CLNT_ITEM_OBJECT> itemObjects;
    std::array<std::uint64_t, 69> inventorySlots{};
    std::unordered_set<std::uint64_t> requestedPlayerNames;
    std::unordered_set<std::uint32_t> requestedCreatureNames;
    std::unordered_set<std::uint64_t> pendingPlayerNames;
    std::unordered_set<std::uint32_t> pendingCreatureNames;
};

namespace {
constexpr unsigned SMSG_UPDATE_OBJECT = 0x00A9;
constexpr unsigned SMSG_DESTROY_OBJECT = 0x00AA;
constexpr unsigned SMSG_MONSTER_MOVE = 0x00DA;
constexpr unsigned MSG_MOVE_TELEPORT_ACK = 0x00C7;
constexpr unsigned SMSG_COMPRESSED_UPDATE_OBJECT = 0x01E7;
constexpr unsigned CMSG_NAME_QUERY = 0x0050;
constexpr unsigned CMSG_CREATURE_QUERY = 0x0060;
constexpr unsigned SCRIPT_UNIT_HEALTH = 0xA0;
constexpr unsigned SCRIPT_UNIT_MAXHEALTH = 0xA1;
constexpr unsigned SCRIPT_UNIT_MANA = 0xA2;
constexpr unsigned SCRIPT_UNIT_MAXMANA = 0xA3;
constexpr unsigned SCRIPT_UNIT_DISPLAYPOWER = 0xA4;
constexpr unsigned SCRIPT_UNIT_LEVEL = 0xA5;
constexpr unsigned SCRIPT_UNIT_FACTION = 0xA6;
constexpr unsigned SCRIPT_UNIT_NAME_UPDATE = 0xB4;
constexpr unsigned SCRIPT_UNIT_PORTRAIT_UPDATE = 0xB5;
constexpr unsigned SCRIPT_UNIT_MODEL_CHANGED = 0xB6;
constexpr unsigned SCRIPT_PLAYER_XP_UPDATE = 0xB9;
std::unique_ptr<ClntObjMgr> s_manager;
ClntObjMgr* s_current = nullptr;

template <typename T>
bool Read(CDataStore* message, T& value) {
    if (!message || message->Failed()) return false;
    message->Get(&value);
    return !message->Failed();
}

bool Skip(CDataStore* message, unsigned bytes) {
    void* data = nullptr;
    return message && message->GetDataInSitu(&data, bytes) && !message->Failed();
}

bool ReadUpdateMask(CDataStore* message,
                    std::vector<std::uint32_t>* fieldValues = nullptr,
                    std::vector<std::uint8_t>* fieldPresent = nullptr) {
    std::uint8_t blockCount = 0;
    if (!Read(message, blockCount)) return false;
    std::vector<std::uint32_t> masks(blockCount);
    if (blockCount && (!message->GetArray(masks.data(), blockCount) || message->Failed()))
        return false;
    if (fieldValues)
        fieldValues->assign(static_cast<std::size_t>(blockCount) * 32U, 0U);
    if (fieldPresent)
        fieldPresent->assign(static_cast<std::size_t>(blockCount) * 32U, 0U);
    for (std::size_t block = 0; block < masks.size(); ++block) {
        for (unsigned bit = 0; bit < 32U; ++bit) {
            if ((masks[block] & (1U << bit)) == 0U) continue;
            std::uint32_t value = 0;
            if (!Read(message, value)) return false;
            if (fieldValues) (*fieldValues)[block * 32U + bit] = value;
            if (fieldPresent) (*fieldPresent)[block * 32U + bit] = 1U;
        }
    }
    return !message->Failed();
}

CLNT_VISIBLE_OBJECT* FindVisibleObject(std::uint64_t guid) {
    if (!s_current) return nullptr;
    const auto found = std::find_if(
        s_current->visibleObjects.begin(), s_current->visibleObjects.end(),
        [guid](const CLNT_VISIBLE_OBJECT& object) { return object.guid == guid; });
    return found == s_current->visibleObjects.end() ? nullptr : &*found;
}

CLNT_VISIBLE_OBJECT& UpsertVisibleObject(std::uint64_t guid) {
    if (CLNT_VISIBLE_OBJECT* object = FindVisibleObject(guid)) return *object;
    s_current->visibleObjects.push_back({});
    s_current->visibleObjects.back().guid = guid;
    return s_current->visibleObjects.back();
}

CLNT_ITEM_OBJECT* FindItemObject(std::uint64_t guid) {
    if (!s_current) return nullptr;
    const auto found = std::find_if(
        s_current->itemObjects.begin(), s_current->itemObjects.end(),
        [guid](const CLNT_ITEM_OBJECT& item) { return item.guid == guid; });
    return found == s_current->itemObjects.end() ? nullptr : &*found;
}

CLNT_ITEM_OBJECT& UpsertItemObject(std::uint64_t guid) {
    if (CLNT_ITEM_OBJECT* item = FindItemObject(guid)) return *item;
    s_current->itemObjects.push_back({});
    s_current->itemObjects.back().guid = guid;
    return s_current->itemObjects.back();
}

std::uint64_t FieldGuid(const std::vector<std::uint32_t>& values,
                        std::size_t field) {
    if (field + 1U >= values.size()) return 0U;
    return static_cast<std::uint64_t>(values[field]) |
        (static_cast<std::uint64_t>(values[field + 1U]) << 32U);
}

void ApplyVisibleFields(CLNT_VISIBLE_OBJECT& object,
                        const std::vector<std::uint32_t>& values,
                        const std::vector<std::uint8_t>* present = nullptr) {
    const auto has = [&values, present](std::size_t field) {
        return field < values.size() &&
            (!present || (field < present->size() && (*present)[field] != 0U));
    };
    if (has(3U) && values[3U] != 0U) object.entryId = values[3U];
    if (has(4U)) {
        const float scale = std::bit_cast<float>(values[4U]);
        if (std::isfinite(scale) && scale > 0.0F) object.scale = scale;
    }
    if (has(0x16U)) object.health = values[0x16U];
    for (std::size_t index = 0; index < object.power.size(); ++index) {
        if (has(0x17U + index)) object.power[index] = values[0x17U + index];
    }
    if (has(0x1BU)) object.maxHealth = values[0x1BU];
    for (std::size_t index = 0; index < object.maxPower.size(); ++index) {
        if (has(0x1CU + index)) object.maxPower[index] = values[0x1CU + index];
    }
    if (has(0x20U)) object.level = values[0x20U];
    if (has(0x21U)) object.faction = values[0x21U];
    if (has(0x22U)) {
        object.unitBytes0 = values[0x22U];
        object.race = static_cast<std::uint8_t>(object.unitBytes0);
        object.classId = static_cast<std::uint8_t>(object.unitBytes0 >> 8U);
        object.sex = static_cast<std::uint8_t>(object.unitBytes0 >> 16U);
        object.powerType = static_cast<std::uint8_t>(object.unitBytes0 >> 24U);
    }
    if (has(0x97U)) object.displayId = values[0x97U];
    if (object.objectType == 7U) {
        if (has(6U) || has(7U))
            object.ownerGuid = FieldGuid(values, 6U);
        if (has(8U)) object.facing = std::bit_cast<float>(values[8U]);
        if (has(9U)) object.position.x = std::bit_cast<float>(values[9U]);
        if (has(10U)) object.position.y = std::bit_cast<float>(values[10U]);
        if (has(11U)) object.position.z = std::bit_cast<float>(values[11U]);
        if (has(12U)) object.displayId = values[12U];
        if (has(32U)) object.corpseBytes1 = values[32U];
        if (has(33U)) object.corpseBytes2 = values[33U];
        object.sex = static_cast<std::uint8_t>(object.corpseBytes1 >> 8U);
        object.race = static_cast<std::uint8_t>(object.corpseBytes1 >> 16U);
    }
    if (object.objectType == 4U) {
        if (has(0x14CU)) object.experience = values[0x14CU];
        if (has(0x14DU)) object.nextLevelExperience = values[0x14DU];
    }
}

void SignalUnitStateChanged(std::uint64_t guid) {
    const char* unit = nullptr;
    if (s_current && guid == s_current->activePlayer) {
        unit = "player";
    } else {
        std::uint64_t target = 0;
        if (ClientRuntime_GetSelectedTarget(&target) && guid == target)
            unit = "target";
    }
    if (!unit) return;
    FrameScript_SignalEvent(SCRIPT_UNIT_HEALTH, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_MAXHEALTH, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_MANA, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_MAXMANA, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_DISPLAYPOWER, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_LEVEL, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_FACTION, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_NAME_UPDATE, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_PORTRAIT_UPDATE, "%s", unit);
    FrameScript_SignalEvent(SCRIPT_UNIT_MODEL_CHANGED, "%s", unit);
    if (guid == s_current->activePlayer)
        FrameScript_SignalEvent(SCRIPT_PLAYER_XP_UPDATE);
}

void SignalInventoryChanged() {
    // Recovered SCRIPTEVENT values.  The archive ContainerFrame.lua listens
    // for BAG_UPDATE(0), while PaperDollFrame listens for the unit event.
    FrameScript_SignalEvent(0xB7U, "%s", "player");
    FrameScript_SignalEvent(0x131U, "%d", 0);
}

bool StorePlayerInventory(
    const std::vector<std::uint32_t>& values,
    const std::vector<std::uint8_t>* present = nullptr) {
    if (!s_current) return false;
    struct FieldRange {
        std::size_t firstField;
        unsigned firstSlot;
        unsigned count;
    };
    static constexpr FieldRange ranges[] = {
        {0xB8U, 0U, 23U},       // PLAYER_FIELD_INV_SLOT_HEAD
        {0xB8U + 0x2EU, 23U, 16U}, // PLAYER_FIELD_PACK_SLOT_1
        {0xB8U + 0x4EU, 39U, 24U}, // PLAYER_FIELD_BANK_SLOT_1
        {0xB8U + 0x7EU, 63U, 6U},  // PLAYER_FIELD_BANKBAG_SLOT_1
    };
    bool changed = false;
    for (const FieldRange& range : ranges) {
        for (unsigned index = 0; index < range.count; ++index) {
            const std::size_t field = range.firstField + index * 2U;
            const unsigned slot = range.firstSlot + index;
            if (field + 1U >= values.size() || slot >= s_current->inventorySlots.size())
                continue;
            const bool lowPresent = !present ||
                (field < present->size() && (*present)[field] != 0U);
            const bool highPresent = !present ||
                (field + 1U < present->size() && (*present)[field + 1U] != 0U);
            if (!lowPresent && !highPresent) continue;
            const std::uint64_t oldGuid = s_current->inventorySlots[slot];
            const std::uint32_t low = lowPresent ? values[field]
                : static_cast<std::uint32_t>(oldGuid);
            const std::uint32_t high = highPresent ? values[field + 1U]
                : static_cast<std::uint32_t>(oldGuid >> 32U);
            const std::uint64_t guid = static_cast<std::uint64_t>(low) |
                (static_cast<std::uint64_t>(high) << 32U);
            if (guid != oldGuid) {
                s_current->inventorySlots[slot] = guid;
                changed = true;
            }
        }
    }
    return changed;
}

void RequestVisibleObjectName(const CLNT_VISIBLE_OBJECT& object) {
    if (!s_current || object.guid == 0U) return;
    if (object.objectType == 4U) {
        if (object.guid == s_current->activePlayer ||
            s_current->requestedPlayerNames.contains(object.guid)) {
            return;
        }
        s_current->pendingPlayerNames.insert(object.guid);
    } else if (object.objectType == 3U && object.entryId != 0U) {
        if (s_current->requestedCreatureNames.contains(object.entryId)) return;
        s_current->pendingCreatureNames.insert(object.entryId);
    }
}

struct ParsedMovement {
    NTempest::C3Vector position{};
    float facing = 0.0F;
    std::uint32_t flags = 0U;
};

bool ReadMovement(CDataStore* message, ParsedMovement& movement) {
    std::uint64_t transportGuid = 0;
    float transport[4]{};
    float pitch = 0.0F;
    std::uint32_t fallTime = 0;
    float speeds[4]{};
    if (!Read(message, transportGuid) ||
        !message->GetArray(transport, 4) ||
        !Read(message, movement.position.x) ||
        !Read(message, movement.position.y) ||
        !Read(message, movement.position.z) ||
        !Read(message, movement.facing) || !Read(message, pitch) ||
        !Read(message, movement.flags) || !Read(message, fallTime) ||
        !message->GetArray(speeds, 4) || message->Failed()) {
        return false;
    }

    // CClientMoveUpdate carries its spline tail only when MOVEMENTFLAG_SPLINE
    // is present.  Consume the same conditional fields as the recovered
    // operator>> so the following object update remains aligned.
    if ((movement.flags & 0x04000000U) != 0U) {
        std::uint32_t splineFlags = 0;
        if (!Read(message, splineFlags)) return false;
        if ((splineFlags & 0x00010000U) != 0U && !Skip(message, 12U)) return false;
        if ((splineFlags & 0x00020000U) != 0U && !Skip(message, 8U)) return false;
        if ((splineFlags & 0x00040000U) != 0U && !Skip(message, 4U)) return false;
        if (!Skip(message, 8U)) return false;
        std::uint32_t pointCount = 0;
        if (!Read(message, pointCount) || pointCount > 0x10000U ||
            !Skip(message, pointCount * 12U)) {
            return false;
        }
    }
    return !message->Failed();
}

int __fastcall ActiveTeleportHandler(void*, unsigned msgId, unsigned,
                                     CDataStore* message) {
    if (msgId != MSG_MOVE_TELEPORT_ACK || !message)
        return 0;
    ParsedMovement movement;
    std::uint64_t transportGuid = 0U;
    float transport[4]{};
    float pitch = 0.0F;
    if (!Read(message, transportGuid) ||
        !message->GetArray(transport, 4U) ||
        !Read(message, movement.position.x) ||
        !Read(message, movement.position.y) ||
        !Read(message, movement.position.z) ||
        !Read(message, movement.facing) || !Read(message, pitch) ||
        !Read(message, movement.flags) || message->Failed()) {
        return 0;
    }
    CWorld::RelocatePlayer(movement.position, movement.facing);
    if (s_current) {
        if (CLNT_VISIBLE_OBJECT* player = FindVisibleObject(
                s_current->activePlayer)) {
            player->position = movement.position;
            player->facing = movement.facing;
        }
    }

    // CGUnit_C::OnTeleportAck acknowledges the completed local placement with
    // the bare movement opcode; the server performs resurrection only after
    // receiving this handshake.
    CDataStore acknowledgement;
    acknowledgement.Put<std::uint32_t>(MSG_MOVE_TELEPORT_ACK);
    ClientServices_Send(&acknowledgement);
    std::fprintf(stderr,
                 "OBJECT_ACTIVE_TELEPORT pos=(%.3f,%.3f,%.3f) facing=%.3f\n",
                 movement.position.x, movement.position.y,
                 movement.position.z, movement.facing);
    return 1;
}

bool ReadCreateObject(CDataStore* message) {
    std::uint64_t guid = 0;
    std::uint8_t objectType = 0;
    if (!Read(message, guid) || !Read(message, objectType)) return false;

    ParsedMovement movement;
    if (!ReadMovement(message, movement)) return false;

    std::uint32_t updateFlags = 0;
    std::uint32_t attackCycle = 0;
    std::uint32_t timerID = 0;
    std::uint64_t victim = 0;
    std::vector<std::uint32_t> fieldValues;
    if (!Read(message, updateFlags) || !Read(message, attackCycle) ||
        !Read(message, timerID) || !Read(message, victim) ||
        !ReadUpdateMask(message, &fieldValues))
        return false;

    if (s_current && objectType >= 1U && objectType <= 2U) {
        CLNT_ITEM_OBJECT& item = UpsertItemObject(guid);
        item.objectType = objectType;
        if (fieldValues.size() > 3U) item.entryId = fieldValues[3U];
        item.ownerGuid = FieldGuid(fieldValues, 6U);
        item.containedGuid = FieldGuid(fieldValues, 8U);
        if (fieldValues.size() > 12U) item.stackCount = fieldValues[12U];
        if (fieldValues.size() > 19U) item.flags = fieldValues[19U];
        SignalInventoryChanged();
    } else if (s_current &&
               (objectType == 3U || objectType == 4U || objectType == 7U)) {
        CLNT_VISIBLE_OBJECT& object = UpsertVisibleObject(guid);
        object.objectType = objectType;
        object.position = movement.position;
        object.facing = movement.facing;
        ApplyVisibleFields(object, fieldValues);
        RequestVisibleObjectName(object);
        if (objectType == 3U) {
            static unsigned creatureReports = 0U;
            if (creatureReports < 32U) {
                std::fprintf(stderr,
                             "OBJECT_CREATURE_CREATE guid=%llu entry=%u display=%u "
                             "scale=%.3f level=%u faction=%u pos=(%.3f,%.3f,%.3f)\n",
                             static_cast<unsigned long long>(guid), object.entryId,
                             object.displayId, object.scale, object.level,
                             object.faction, object.position.x, object.position.y,
                             object.position.z);
                ++creatureReports;
            }
        }
        if (objectType == 4U && StorePlayerInventory(fieldValues))
            SignalInventoryChanged();
    }

    if (objectType == 4 && (updateFlags & 1U) != 0 && s_current) {
        const bool firstActivation = s_current->activePlayer == 0;
        s_current->activePlayer = guid;
        s_current->activePosition = movement.position;
        s_current->activeFacing = movement.facing;
        if (firstActivation) {
            CGGameUI::EnterWorld();
            SignalUnitStateChanged(guid);
            SetLocalPlayerInGame();
            s_current->localPlayerInGame = true;
            const CLNT_VISIBLE_OBJECT* playerObject = FindVisibleObject(guid);
            std::fprintf(stderr,
                         "OBJECT_SELF_CREATE guid=%llu pos=%.3f,%.3f,%.3f facing=%.3f "
                         "health=%u/%u power=%u/%u type=%u race=%u class=%u display=%u\n",
                         static_cast<unsigned long long>(guid), movement.position.x,
                         movement.position.y, movement.position.z, movement.facing,
                         playerObject ? playerObject->health : 0U,
                         playerObject ? playerObject->maxHealth : 0U,
                         playerObject ? playerObject->power[std::min<std::size_t>(
                             playerObject->powerType, playerObject->power.size() - 1U)] : 0U,
                         playerObject ? playerObject->maxPower[std::min<std::size_t>(
                             playerObject->powerType, playerObject->maxPower.size() - 1U)] : 0U,
                         playerObject ? playerObject->powerType : 0U,
                         playerObject ? playerObject->race : 0U,
                         playerObject ? playerObject->classId : 0U,
                         playerObject ? playerObject->displayId : 0U);
        }
    }
    return true;
}

bool ReadMovementUpdate(CDataStore* message) {
    std::uint64_t guid = 0;
    ParsedMovement movement;
    if (!Read(message, guid) || !ReadMovement(message, movement)) return false;
    if (CLNT_VISIBLE_OBJECT* object = FindVisibleObject(guid)) {
        object->position = movement.position;
        object->facing = movement.facing;
    }
    // The recovered client ignores server movement echoes for the active
    // mover, preserving local prediction until a correction packet arrives.
    return true;
}

bool ReadValuesUpdate(CDataStore* message) {
    std::uint64_t guid = 0;
    std::vector<std::uint32_t> fieldValues;
    std::vector<std::uint8_t> fieldPresent;
    if (!Read(message, guid) ||
        !ReadUpdateMask(message, &fieldValues, &fieldPresent)) return false;
    if (CLNT_ITEM_OBJECT* item = FindItemObject(guid)) {
        const auto has = [&fieldPresent](std::size_t field) {
            return field < fieldPresent.size() && fieldPresent[field] != 0U;
        };
        if (has(3U)) item->entryId = fieldValues[3U];
        if (has(6U) || has(7U)) item->ownerGuid = FieldGuid(fieldValues, 6U);
        if (has(8U) || has(9U)) item->containedGuid = FieldGuid(fieldValues, 8U);
        if (has(12U)) item->stackCount = fieldValues[12U];
        if (has(19U)) item->flags = fieldValues[19U];
        SignalInventoryChanged();
    } else if (CLNT_VISIBLE_OBJECT* object = FindVisibleObject(guid)) {
        const auto has = [&fieldPresent](std::size_t field) {
            return field < fieldPresent.size() && fieldPresent[field] != 0U;
        };
        ApplyVisibleFields(*object, fieldValues, &fieldPresent);
        if (object->objectType == 4U &&
            StorePlayerInventory(fieldValues, &fieldPresent)) {
            SignalInventoryChanged();
        }
        RequestVisibleObjectName(*object);
        SignalUnitStateChanged(guid);
    }
    return true;
}

void RemoveTrackedObject(std::uint64_t guid) {
    if (!s_current) return;
    std::erase_if(s_current->visibleObjects,
                  [guid](const CLNT_VISIBLE_OBJECT& object) {
                      return object.guid == guid;
                  });
    std::erase_if(s_current->itemObjects,
                  [guid](const CLNT_ITEM_OBJECT& item) {
                      return item.guid == guid;
                  });
    bool inventoryChanged = false;
    for (std::uint64_t& slotGuid : s_current->inventorySlots) {
        if (slotGuid == guid) {
            slotGuid = 0U;
            inventoryChanged = true;
        }
    }
    if (inventoryChanged) SignalInventoryChanged();
}

bool ReadOutOfRangeObjects(CDataStore* message) {
    std::uint32_t count = 0;
    if (!Read(message, count) || !s_current) return false;
    for (std::uint32_t index = 0; index < count; ++index) {
        std::uint64_t guid = 0;
        if (!Read(message, guid)) return false;
        RemoveTrackedObject(guid);
    }
    return true;
}


bool ReadInRangeObjects(CDataStore* message) {
    std::uint32_t count = 0;
    if (!Read(message, count)) return false;
    return count <= 0x100000U && Skip(message, count * 8U);
}
}

CClientMoveUpdate::CClientMoveUpdate() { }
CMirrorHandler::~CMirrorHandler() { }
CMoveSpline::CMoveSpline() { }
CMovementStatus::CMovementStatus() { }
CMovementStatus::~CMovementStatus() { }
void AllocNewObj() { }
void AssignMirrorHandler() { }
void CCommand_ObjUsage() { }
void CallBlockMirrorHandlers() { }
void CallBlockMirrorHandlersIfChanged() { }
void CallMirrorHandlers() { }
void ClearObjectMirrorHandlers() { }
ClntObjMgr* ClntObjMgrCreate(int playerType, void* clientPtr) {
    s_manager = std::make_unique<ClntObjMgr>();
    s_manager->playerType = playerType;
    s_manager->clientPtr = clientPtr;
    return s_manager.get();
}
void ClntObjMgrDestroy() { }
void ClntObjMgrDestroyShared() { s_current = nullptr; s_manager.reset(); }
void ClntObjMgrDestruct() { }
void ClntObjMgrEnumVisibleObjects() { }
void ClntObjMgrFreeObject() { }
std::uint64_t ClntObjMgrGetActivePlayer() { return s_current ? s_current->activePlayer : 0; }
void ClntObjMgrGetClientPtr() { }
ClntObjMgr* ClntObjMgrGetCurrent() { return s_current; }
unsigned ClntObjMgrGetMapID() { return s_current ? s_current->mapID : 0; }
void ClntObjMgrGetMovementGlobals() { }
void* ClntObjMgrGetNet() { return s_current ? s_current->net : nullptr; }
int ClntObjMgrGetPlayerType() { return s_current ? s_current->playerType : 0; }
void ClntObjMgrHideObject() { }
void ClntObjMgrInitialize() {
    ClientServices_SetMessageHandler(SMSG_UPDATE_OBJECT, ObjectUpdateHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_DESTROY_OBJECT,
                                     ObjectDestroyHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_MONSTER_MOVE, OnMonsterMoveEvent, nullptr);
    ClientServices_SetMessageHandler(MSG_MOVE_TELEPORT_ACK,
                                     ActiveTeleportHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_COMPRESSED_UPDATE_OBJECT,
                                     ObjectCompressedUpdateHandler, nullptr);
    // Recovered CGUnit_C::Initialize installs the magic-client handlers as
    // part of object-manager startup.
    Spell_C_Initialize();
}
void ClntObjMgrInitializeShared() { }
void ClntObjMgrIsValid() { }
void ClntObjMgrObjectInRange() { }
void ClntObjMgrObjectOutOfRange() { }
void ClntObjMgrObjectPtr() { }
void ClntObjMgrSetActivePlayer(std::uint64_t guid) { if (s_current) s_current->activePlayer = guid; }
void ClntObjMgrSetCurrent(ClntObjMgr* manager) { s_current = manager; }
void ClntObjMgrSetMapID(unsigned mapID) { if (s_current) s_current->mapID = mapID; }
void ClntObjMgrSetMovementGlobals() { }
void ClntObjMgrSetNet(void* net) { if (s_current) s_current->net = net; }
void ClntObjMgrSetObjMirrorHandler() { }
void ClntObjMgrSetTypeMirrorHandler() { }
void ClntObjMgrShowObject() { }
void ClntObjMgrUnsetObjMirrorHandler() { }
void ClntObjMgrUnsetTypeMirrorHandler() { }
void CreateMessage() { }
void CreateObject() { }
void FillInObjectData() { }
void FillInPartialObjectData() { }
void FindActiveObj() { }
void GetDataBaseOffset() { }
void GetMirrorHandler() { }
void GetNumDwordBlocks() { }
void GetObjectPtr() { }
void GetOffsetSectionId() { }
void GetSectionId() { }
void GetUpdateObject() { }
void InRangeMessage() { }
void IncTypeId() { }
void InitObject() { }
void IsMaskBitSet() { }
void MirrorHandlerAdvanceBlock() { }
void MirrorHandlerRemoveQueued() { }
int __fastcall ObjectUpdateHandler(void*, unsigned msgId, unsigned,
                                   CDataStore* message) {
    if (msgId != SMSG_UPDATE_OBJECT || !message) return 0;
    std::uint32_t updateCount = 0;
    if (!Read(message, updateCount)) return 0;
    for (std::uint32_t index = 0; index < updateCount; ++index) {
        std::uint8_t updateType = 0;
        if (!Read(message, updateType)) return 0;
        switch (updateType) {
        case 0:
            if (!ReadValuesUpdate(message)) return 0;
            break;
        case 1:
            if (!ReadMovementUpdate(message)) return 0;
            break;
        case 2:
            if (!ReadCreateObject(message)) return 0;
            break;
        case 3:
            if (!ReadOutOfRangeObjects(message)) return 0;
            break;
        case 4:
            if (!ReadInRangeObjects(message)) return 0;
            break;
        default:
            std::fprintf(stderr, "OBJECT_UPDATE_UNHANDLED type=%u remaining=%u\n",
                         updateType, message->m_size - message->m_read);
            return 0;
        }
    }
    return message->Failed() ? 0 : 1;
}

int __fastcall ObjectDestroyHandler(void*, unsigned msgId, unsigned,
                                    CDataStore* message) {
    std::uint64_t guid = 0U;
    if (msgId != SMSG_DESTROY_OBJECT || !Read(message, guid)) return 0;
    RemoveTrackedObject(guid);
    return 1;
}

int __fastcall ObjectCompressedUpdateHandler(void* param, unsigned msgId,
                                             unsigned eventTime,
                                             CDataStore* message) {
    if (msgId != SMSG_COMPRESSED_UPDATE_OBJECT || !message) return 0;
    std::uint32_t uncompressedBytes = 0;
    if (!Read(message, uncompressedBytes) || uncompressedBytes == 0) return 0;
    const unsigned compressedBytes = message->m_size - message->m_read;
    const auto* zlib = message->m_data + message->m_read;
    if (compressedBytes < 6 || (zlib[0] & 0x0FU) != 8U) return 0;
    std::vector<std::uint8_t> output(uncompressedBytes);
    unsigned actual = uncompressedBytes;
    if (!StormInflateRaw(zlib + 2, compressedBytes - 6, output.data(), &actual) ||
        actual != uncompressedBytes)
        return 0;
    CDataStore decompressed(output.data(), actual);
    const int result = ObjectUpdateHandler(param, SMSG_UPDATE_OBJECT, eventTime,
                                           &decompressed);
    std::fprintf(stderr, "OBJECT_UPDATE_COMPRESSED bytes=%u result=%d\n",
                 uncompressedBytes, result);
    return result;
}

int __fastcall OnMonsterMoveEvent(void*, unsigned msgId, unsigned,
                                  CDataStore* message) {
    if (msgId != SMSG_MONSTER_MOVE || !message) return 0;

    std::uint64_t guid = 0;
    NTempest::C3Vector start{};
    std::uint32_t splineTime = 0;
    std::uint8_t splineType = 0;
    std::uint32_t splineFlags = 0;
    std::uint32_t durationMs = 0;
    std::uint32_t pointCount = 0;
    if (!Read(message, guid) || !Read(message, start.x) ||
        !Read(message, start.y) || !Read(message, start.z) ||
        !Read(message, splineTime) || !Read(message, splineType) ||
        !Read(message, splineFlags) || !Read(message, durationMs) ||
        !Read(message, pointCount) || pointCount > 0x10000U) {
        return 0;
    }

    std::vector<NTempest::C3Vector> path;
    path.reserve(static_cast<std::size_t>(pointCount) + 1U);
    path.push_back(start);
    for (std::uint32_t index = 0; index < pointCount; ++index) {
        NTempest::C3Vector point{};
        if (!Read(message, point.x) || !Read(message, point.y) ||
            !Read(message, point.z)) {
            return 0;
        }
        path.push_back(point);
    }

    CLNT_VISIBLE_OBJECT* object = FindVisibleObject(guid);
    if (!object || path.size() < 2U) return message->Failed() ? 0 : 1;

    float distance = 0.0F;
    for (std::size_t index = 1; index < path.size(); ++index) {
        const NTempest::C3Vector delta = path[index] - path[index - 1U];
        distance += std::sqrt(delta.SquaredMag());
    }
    object->position = start;
    object->movementPath = std::move(path);
    object->movementElapsed = 0.0F;
    object->movementDuration =
        (std::max)(0.001F, static_cast<float>(durationMs) * 0.001F);
    object->movementSpeed = distance / object->movementDuration;
    object->moving = distance > 0.001F && durationMs != 0U;
    if (object->moving) {
        const NTempest::C3Vector delta =
            object->movementPath[1] - object->movementPath[0];
        object->facing = std::atan2(delta.y, delta.x);
    }
    static unsigned reports = 0U;
    if (reports++ < 32U) {
        std::fprintf(stderr,
                     "OBJECT_MONSTER_MOVE guid=%llu points=%u duration=%u "
                     "speed=%.3f flags=%08X type=%u\n",
                     static_cast<unsigned long long>(guid), pointCount,
                     durationMs, object->movementSpeed, splineFlags,
                     static_cast<unsigned>(splineType));
    }
    return message->Failed() ? 0 : 1;
}

const NTempest::C3Vector& ClntObjMgrGetActivePosition() {
    static const NTempest::C3Vector zero{};
    return s_current ? s_current->activePosition : zero;
}
float ClntObjMgrGetActiveFacing() {
    return s_current ? s_current->activeFacing : 0.0F;
}
void ClntObjMgrSetActivePosition(const NTempest::C3Vector& position) {
    if (!s_current) return;
    s_current->activePosition = position;
    if (CLNT_VISIBLE_OBJECT* object = FindVisibleObject(s_current->activePlayer))
        object->position = position;
}
void ClntObjMgrSetActiveFacing(float facing) {
    if (!s_current) return;
    constexpr float twoPi = 6.2831855F;
    while (facing < 0.0F) facing += twoPi;
    while (facing > twoPi) facing -= twoPi;
    s_current->activeFacing = facing;
    if (CLNT_VISIBLE_OBJECT* object = FindVisibleObject(s_current->activePlayer))
        object->facing = facing;
}
const std::vector<CLNT_VISIBLE_OBJECT>& ClntObjMgrGetVisibleObjects() {
    static const std::vector<CLNT_VISIBLE_OBJECT> empty;
    return s_current ? s_current->visibleObjects : empty;
}
const CLNT_VISIBLE_OBJECT* ClntObjMgrGetVisibleObject(std::uint64_t guid) {
    return FindVisibleObject(guid);
}
const CLNT_ITEM_OBJECT* ClntObjMgrGetItem(std::uint64_t guid) {
    return FindItemObject(guid);
}
std::uint64_t ClntObjMgrGetInventoryGuid(unsigned slot) {
    return s_current && slot < s_current->inventorySlots.size()
        ? s_current->inventorySlots[slot] : 0U;
}
const CLNT_ITEM_OBJECT* ClntObjMgrGetInventoryItem(unsigned slot) {
    return FindItemObject(ClntObjMgrGetInventoryGuid(slot));
}
int ClntObjMgrFindInventorySlot(std::uint32_t itemEntry) {
    if (!s_current || itemEntry == 0U) return -1;
    for (unsigned slot = 0U; slot < s_current->inventorySlots.size(); ++slot) {
        const CLNT_ITEM_OBJECT* item = ClntObjMgrGetInventoryItem(slot);
        if (item && item->entryId == itemEntry) return static_cast<int>(slot);
    }
    return -1;
}
void ClntObjMgrUpdateVisibleObjects(float elapsedSec) {
    if (!s_current || elapsedSec <= 0.0F) return;
    for (CLNT_VISIBLE_OBJECT& object : s_current->visibleObjects) {
        if (!object.moving || object.movementPath.size() < 2U) continue;

        object.movementElapsed += (std::min)(elapsedSec, 0.25F);
        const float progress = (std::clamp)(
            object.movementElapsed / object.movementDuration, 0.0F, 1.0F);

        float totalDistance = 0.0F;
        for (std::size_t index = 1; index < object.movementPath.size(); ++index) {
            const NTempest::C3Vector delta =
                object.movementPath[index] - object.movementPath[index - 1U];
            totalDistance += std::sqrt(delta.SquaredMag());
        }
        float remaining = totalDistance * progress;
        for (std::size_t index = 1; index < object.movementPath.size(); ++index) {
            const NTempest::C3Vector& from = object.movementPath[index - 1U];
            const NTempest::C3Vector& to = object.movementPath[index];
            const NTempest::C3Vector delta = to - from;
            const float segment = std::sqrt(delta.SquaredMag());
            if (remaining <= segment || index + 1U == object.movementPath.size()) {
                const float local = segment > 0.0001F
                    ? (std::clamp)(remaining / segment, 0.0F, 1.0F)
                    : 1.0F;
                object.position = from + delta * local;
                if (segment > 0.0001F)
                    object.facing = std::atan2(delta.y, delta.x);
                break;
            }
            remaining -= segment;
        }
        if (progress >= 1.0F) {
            object.position = object.movementPath.back();
            object.moving = false;
            object.movementSpeed = 0.0F;
            object.movementPath.clear();
        }
    }
}
void ClntObjMgrPollNameQueries() {
    if (!s_current) return;

    // Packet callbacks run inside NetClient's receive dispatch.  The original
    // cache queued misses and sent them later; sending from ReadCreateObject
    // can re-enter the connection lock and leave the loading screen stalled.
    const std::vector<std::uint64_t> players(
        s_current->pendingPlayerNames.begin(),
        s_current->pendingPlayerNames.end());
    const std::vector<std::uint32_t> creatures(
        s_current->pendingCreatureNames.begin(),
        s_current->pendingCreatureNames.end());
    s_current->pendingPlayerNames.clear();
    s_current->pendingCreatureNames.clear();

    for (const std::uint64_t guid : players) {
        if (!s_current->requestedPlayerNames.insert(guid).second) continue;
        CDataStore message;
        message.Put(CMSG_NAME_QUERY)->Put(guid);
        ClientServices_Send(&message);
    }
    for (const std::uint32_t entry : creatures) {
        if (!s_current->requestedCreatureNames.insert(entry).second) continue;
        CDataStore message;
        message.Put(CMSG_CREATURE_QUERY)->Put(entry);
        ClientServices_Send(&message);
    }
}
void OnObjectDestroy() { }
void OutOfRangeMessage() { }
void PartialUpdateFromFullUpdate() { }
void PostInitObject() { }
void PostMovementUpdate() { }
void ProcessObjHandlersQueue() { }
void SavePreviousValue() { }
void SetObjectBlock() { }
void SetupObjectStorage() { }
void SkipCreateObject() { }
void SkipPartialObjectUpdate() { }
void SkipSetOfObjects() { }
void UnassignMirrorHandler() { }
void UpdateInRangeObjects() { }
void UpdateObject() { }
void UpdateObjectMovement() { }
void UpdateOutOfRangeObjects() { }
