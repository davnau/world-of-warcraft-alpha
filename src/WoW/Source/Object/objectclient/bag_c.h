#pragma once

#include <cstdint>
#include <vector>

class CGItem_C;

enum GAME_ERROR_TYPE : std::int32_t {
    GERR_INV_FULL = 0x000,
    GERR_CANT_EQUIP_LEVEL_I = 0x001,
    GERR_CANT_EQUIP_SKILL = 0x002,
    GERR_CANT_EQUIP_EVER = 0x003,
    GERR_PROFICIENCY_NEEDED = 0x004,
    GERR_WRONG_SLOT = 0x005,
    GERR_BAG_FULL = 0x006,
    GERR_DESTROY_NONEMPTY_BAG = 0x007,
    GERR_BAG_IN_BAG = 0x008,
    GERR_AMMO_ONLY = 0x009,
    GERR_NO_SLOT_AVAILABLE = 0x00A,
    GERR_WRONG_BAG_TYPE = 0x00B,
    GERR_ITEM_MAX_COUNT = 0x00C,
    GERR_NOT_EQUIPPABLE = 0x00D,
    GERR_CANT_STACK = 0x00E,
    GERR_CANT_SWAP = 0x00F,
    GERR_SLOT_EMPTY = 0x010,
    GERR_ITEM_NOT_FOUND = 0x011,
    GERR_TOO_FEW_TO_SPLIT = 0x012,
    GERR_SPLIT_FAILED = 0x013,
    GERR_NOT_ENOUGH_GOLD = 0x014,
    GERR_NOT_A_BAG = 0x015,
    GERR_NOT_OWNER = 0x016,
    GERR_ONLY_ONE_QUIVER = 0x017,
    GERR_NO_BANK_SLOT = 0x018,
    GERR_NO_BANK_HERE = 0x019,
    GERR_ITEM_LOCKED = 0x01A,
    GERR_VENDOR_SOLD_OUT = 0x01E,
    GERR_DROP_BOUND_ITEM = 0x022,
    GERR_OUT_OF_RANGE = 0x070,
    GERR_PLAYER_DEAD = 0x071,
    GERR_CLIENT_LOCKED_OUT = 0x072,
    GERR_2HSKILLNOTFOUND = 0x08C,
    GERR_CANT_WRAP_STACKABLE = 0x0FE,
    GERR_CANT_WRAP_EQUIPPED = 0x0FF,
    GERR_CANT_WRAP_WRAPPED = 0x100,
    GERR_CANT_WRAP_BOUND = 0x101,
    GERR_CANT_WRAP_UNIQUE = 0x102,
    GERR_CANT_WRAP_BAGS = 0x103,
    GERR_LOOT_GONE = 0x109,
    GERR_NONE = 0x129
};

using CGBagItemPredicate = int (*)(const CGItem_C*, void*);

class CGBag_C {
public:
    CGBag_C() = default;

    unsigned int GetWidth(unsigned int offset = 0) const;
    unsigned int GetHeight(unsigned int offset = 0) const;
    int GetItemTypeCount(int entryID, unsigned int flags = 0);
    CGItem_C* FindItem(CGBagItemPredicate predicate, void* parameter,
                       unsigned int flags = 0);
    CGItem_C* FindItem(CGBagItemPredicate predicate, void* parameter,
                       std::uint64_t* bagGUID, unsigned int* slot,
                       unsigned int flags = 0);
    CGItem_C* FindItemOfType(int entryID, unsigned int flags = 0);
    CGItem_C* FindItemOfType(int entryID, std::uint64_t* bagGUID,
                             unsigned int* slot, unsigned int flags = 0);
    CGItem_C* FindItemOfClass(unsigned int classID, int subclassMask,
                              unsigned int flags = 0);
    CGItem_C* FindItemOfClass(int classID, int subclassMask,
                              std::uint64_t* bagGUID, unsigned int* slot,
                              unsigned int flags = 0);
    GAME_ERROR_TYPE GetGameError(int error) const;

    void SetInventory(bool inventory) { m_isInventory = inventory; }
    void SetGuid(std::uint64_t guid) { m_guid = guid; }
    void SetSlots(const std::vector<CGItem_C*>& slots) { m_slots = slots; }
    std::vector<CGItem_C*>& Slots() { return m_slots; }
    const std::vector<CGItem_C*>& Slots() const { return m_slots; }

private:
    std::vector<CGItem_C*> m_slots;
    std::uint64_t m_guid = 0;
    bool m_isInventory = false;
};

// Recovered symbol declarations for objectclient/bag_c.cpp
