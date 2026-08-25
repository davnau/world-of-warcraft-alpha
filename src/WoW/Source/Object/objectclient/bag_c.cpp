#include "WoW/Source/Object/objectclient/bag_c.h"

#include "WoW/Source/Object/objectclient/item_c.h"

#include <algorithm>

namespace {

struct ItemTypeCountData {
    int entryID;
    int count;
};

int GetItemTypeCountCallback(const CGItem_C* item, void* parameter) {
    auto* data = static_cast<ItemTypeCountData*>(parameter);
    if (data && (data->entryID == -1 ||
                 static_cast<int>(item->GetClassID()) == data->entryID)) {
        data->count += static_cast<int>(item->GetStackCount());
    }
    return 0;
}

struct ItemIDData { int entryID; };

int FindItemIDCallback(const CGItem_C* item, void* parameter) {
    const auto* data = static_cast<const ItemIDData*>(parameter);
    return data && static_cast<int>(item->GetEntryID()) == data->entryID;
}

struct ItemClassData { int classID; int subclassMask; };

int FindItemClassCallback(const CGItem_C* item, void* parameter) {
    const auto* data = static_cast<const ItemClassData*>(parameter);
    if (!data || static_cast<int>(item->GetClassID()) != data->classID)
        return 0;
    return data->subclassMask == 0 ||
           (data->subclassMask & (1 << item->GetSubtypeID())) != 0;
}

} // namespace

unsigned int CGBag_C::GetWidth(unsigned int offset) const {
    const unsigned int remaining =
        offset < m_slots.size() ? static_cast<unsigned int>(m_slots.size() - offset) : 0;
    return std::min(remaining, 4u);
}

unsigned int CGBag_C::GetHeight(unsigned int offset) const {
    const unsigned int remaining =
        offset < m_slots.size() ? static_cast<unsigned int>(m_slots.size() - offset) : 0;
    return (remaining + 3) >> 2;
}

int CGBag_C::GetItemTypeCount(int entryID, unsigned int flags) {
    ItemTypeCountData data{entryID, 0};
    FindItem(GetItemTypeCountCallback, &data, flags);
    return data.count;
}

CGItem_C* CGBag_C::FindItem(CGBagItemPredicate predicate, void* parameter,
                            unsigned int flags) {
    std::uint64_t bagGUID = 0;
    unsigned int slot = 0;
    return FindItem(predicate, parameter, &bagGUID, &slot, flags);
}

CGItem_C* CGBag_C::FindItem(CGBagItemPredicate predicate, void* parameter,
                            std::uint64_t* bagGUID, unsigned int* slot,
                            unsigned int flags) {
    if (!predicate) {
        if (bagGUID) *bagGUID = 0;
        if (slot) *slot = 0;
        return nullptr;
    }

    unsigned int effectiveFlags = flags;
    if (m_isInventory && (effectiveFlags & 7u) == 0)
        effectiveFlags |= 7u;

    for (unsigned int i = 0; i < m_slots.size(); ++i) {
        const bool allowed = !m_isInventory ||
            (i <= 0x12u && (effectiveFlags & 1u)) ||
            (i >= 0x13u && i <= 0x16u && (effectiveFlags & 2u)) ||
            (i >= 0x17u && i <= 0x26u && (effectiveFlags & 4u)) ||
            (i >= 0x27u && i <= 0x3Eu && (effectiveFlags & 8u));
        if (!allowed)
            continue;

        CGItem_C* item = m_slots[i];
        if (!item || item->IsDisabled())
            continue;
        if (predicate(item, parameter)) {
            if (bagGUID) *bagGUID = m_guid;
            if (slot) *slot = i;
            return item;
        }
        if (item->GetBag() && !(effectiveFlags & 0x10u)) {
            CGItem_C* nested = item->GetBag()->FindItem(
                predicate, parameter, bagGUID, slot, effectiveFlags);
            if (nested)
                return nested;
        }
    }

    if (bagGUID) *bagGUID = 0;
    if (slot) *slot = 0;
    return nullptr;
}

CGItem_C* CGBag_C::FindItemOfType(int entryID, unsigned int flags) {
    std::uint64_t bagGUID = 0;
    unsigned int slot = 0;
    return FindItemOfType(entryID, &bagGUID, &slot, flags);
}

CGItem_C* CGBag_C::FindItemOfType(int entryID, std::uint64_t* bagGUID,
                                  unsigned int* slot, unsigned int flags) {
    ItemIDData data{entryID};
    return FindItem(FindItemIDCallback, &data, bagGUID, slot, flags);
}

CGItem_C* CGBag_C::FindItemOfClass(unsigned int classID, int subclassMask,
                                   unsigned int flags) {
    std::uint64_t bagGUID = 0;
    unsigned int slot = 0;
    return FindItemOfClass(static_cast<int>(classID), subclassMask,
                           &bagGUID, &slot, flags);
}

CGItem_C* CGBag_C::FindItemOfClass(int classID, int subclassMask,
                                   std::uint64_t* bagGUID, unsigned int* slot,
                                   unsigned int flags) {
    ItemClassData data{classID, subclassMask};
    return FindItem(FindItemClassCallback, &data, bagGUID, slot, flags);
}

GAME_ERROR_TYPE CGBag_C::GetGameError(int error) const {
    switch (error) {
    case 0: return GERR_NONE;
    case 1: return GERR_CANT_EQUIP_LEVEL_I;
    case 2: return GERR_CANT_EQUIP_SKILL;
    case 3: return GERR_WRONG_SLOT;
    case 5: return GERR_BAG_IN_BAG;
    case 6: return GERR_AMMO_ONLY;
    case 7: return GERR_PROFICIENCY_NEEDED;
    case 8: case 11: case 12: case 17: return GERR_NO_SLOT_AVAILABLE;
    case 9: case 10: return GERR_CANT_EQUIP_EVER;
    case 13: return GERR_2HSKILLNOTFOUND;
    case 14: case 15: return GERR_WRONG_BAG_TYPE;
    case 16: return GERR_ITEM_MAX_COUNT;
    case 18: case 52: return GERR_CANT_STACK;
    case 19: return GERR_NOT_EQUIPPABLE;
    case 20: return GERR_CANT_SWAP;
    case 21: return GERR_SLOT_EMPTY;
    case 22: case 51: return GERR_ITEM_NOT_FOUND;
    case 23: return GERR_DROP_BOUND_ITEM;
    case 24: return GERR_OUT_OF_RANGE;
    case 25: return GERR_TOO_FEW_TO_SPLIT;
    case 26: return GERR_SPLIT_FAILED;
    case 28: return GERR_NOT_ENOUGH_GOLD;
    case 29: return GERR_NOT_A_BAG;
    case 30: return GERR_DESTROY_NONEMPTY_BAG;
    case 31: return GERR_NOT_OWNER;
    case 32: return GERR_ONLY_ONE_QUIVER;
    case 33: return GERR_NO_BANK_SLOT;
    case 34: return GERR_NO_BANK_HERE;
    case 35: return GERR_ITEM_LOCKED;
    case 36: return GERR_PLAYER_DEAD;
    case 37: return GERR_CLIENT_LOCKED_OUT;
    case 39: return GERR_CANT_WRAP_STACKABLE;
    case 40: return GERR_CANT_WRAP_EQUIPPED;
    case 41: return GERR_CANT_WRAP_WRAPPED;
    case 42: return GERR_CANT_WRAP_BOUND;
    case 43: return GERR_CANT_WRAP_UNIQUE;
    case 44: return GERR_CANT_WRAP_BAGS;
    case 47: return GERR_LOOT_GONE;
    case 48: return GERR_INV_FULL;
    case 49: case 54: return GERR_VENDOR_SOLD_OUT;
    default: return GERR_BAG_FULL;
    }
}
