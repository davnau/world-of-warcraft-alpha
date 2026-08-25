#include "WoW/Source/UI/actionbarframe.h"

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "Storm/h/storm.h"
#include "WoW/Source/DB/DBClient/dbcache.h"
#include "WoW/Source/Glue/cgluemgr.h"
#include "WoW/Source/Glue/charselectinfo.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"
#include "WoW/Source/clientruntime.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

std::array<int, CGActionBar::NUM_ACTION_BUTTONS> CGActionBar::m_slotActions{};
unsigned CGActionBar::m_bonusPage = 0U;

namespace {
constexpr unsigned SMSG_ACTION_BUTTONS = 0x011CU;
constexpr unsigned SMSG_INITIAL_SPELLS = 0x011DU;
constexpr unsigned CMSG_ITEM_QUERY_SINGLE = 0x0056U;
constexpr unsigned CMSG_SET_ACTION_BUTTON = 0x011BU;
constexpr unsigned CMSG_CAST_SPELL = 0x0121U;
constexpr unsigned CMSG_ATTACKSWING = 0x0134U;
constexpr unsigned CMSG_USE_ITEM = 0x00ABU;

constexpr unsigned SCRIPT_ACTIONBAR_SHOWGRID = 0xC9U;
constexpr unsigned SCRIPT_ACTIONBAR_HIDEGRID = 0xCAU;
constexpr unsigned SCRIPT_ACTIONBAR_PAGE_CHANGED = 0xCBU;
constexpr unsigned SCRIPT_ACTIONBAR_SLOT_CHANGED = 0xCCU;
constexpr unsigned SCRIPT_ACTIONBAR_UPDATE_STATE = 0xCDU;
constexpr unsigned SCRIPT_ACTIONBAR_UPDATE_USABLE = 0xCEU;
constexpr unsigned SCRIPT_ACTIONBAR_UPDATE_COOLDOWN = 0xCFU;
constexpr unsigned SCRIPT_UPDATE_BONUS_ACTIONBAR = 0xD0U;

struct DbcFile {
    std::vector<std::uint8_t> bytes;
    std::uint32_t records = 0;
    std::uint32_t fields = 0;
    std::uint32_t recordSize = 0;
    std::uint32_t stringSize = 0;

    const std::uint8_t* Record(std::uint32_t row) const {
        return row < records
            ? bytes.data() + 20U + static_cast<std::size_t>(row) * recordSize
            : nullptr;
    }
    const char* String(std::uint32_t offset) const {
        if (offset >= stringSize) return "";
        const std::size_t base =
            20U + static_cast<std::size_t>(records) * recordSize;
        return reinterpret_cast<const char*>(bytes.data() + base + offset);
    }
};

struct SpellArt {
    std::uint32_t iconID = 0U;
    std::uint32_t activeIconID = 0U;
    std::uint32_t firstEffect = 0U;
    bool requiresSelectedUnit = false;
};

bool s_artLoadAttempted = false;
bool s_artLoaded = false;
std::unordered_map<int, SpellArt> s_spellArt;
std::unordered_map<std::uint32_t, std::string> s_spellIcons;
std::unordered_map<std::uint32_t, std::string> s_itemIcons;
std::unordered_set<std::uint16_t> s_knownSpells;
std::unordered_set<int> s_pendingItemQueries;

std::uint32_t ReadU32(const void* source) {
    std::uint32_t value = 0U;
    std::memcpy(&value, source, sizeof(value));
    return value;
}

bool IsSelectedUnitTarget(std::uint32_t target) {
    // SpellImplicitTargets from this client's Spell.dbc.  These modes are
    // resolved from the currently selected unit by Spell_C_CastSpell in the
    // recovered client; TARGET_SELF (1) deliberately is not in this list.
    switch (target) {
        case 6U:  // TARGET_ENEMY_UNIT
        case 21U: // TARGET_SELECTED_FRIEND
        case 25U: // TARGET_UNIT
        case 35U: // TARGET_SINGLE_PARTY
        case 36U: // TARGET_HOSTILE_UNIT_SELECTION
            return true;
        default:
            return false;
    }
}

bool LoadDbcFile(const char* filename, std::uint32_t minimumFields,
                 DbcFile& dbc) {
    void* data = nullptr;
    unsigned size = 0U;
    if (!SFile::LoadFile(filename, &data, &size, 1U, nullptr) || !data)
        return false;
    const auto* input = static_cast<const std::uint8_t*>(data);
    const bool headerValid =
        size >= 20U && std::memcmp(input, "WDBC", 4U) == 0;
    if (headerValid) {
        dbc.records = ReadU32(input + 4U);
        dbc.fields = ReadU32(input + 8U);
        dbc.recordSize = ReadU32(input + 12U);
        dbc.stringSize = ReadU32(input + 16U);
    }
    const std::size_t stringOffset =
        20U + static_cast<std::size_t>(dbc.records) * dbc.recordSize;
    const bool valid = headerValid && dbc.fields >= minimumFields &&
        dbc.recordSize >= minimumFields * 4U && stringOffset <= size &&
        dbc.stringSize <= size - stringOffset;
    if (valid) dbc.bytes.assign(input, input + size);
    SFile::Unload(data);
    return valid;
}

bool LoadActionArt() {
    if (s_artLoadAttempted) return s_artLoaded;
    s_artLoadAttempted = true;

    DbcFile spellDbc;
    DbcFile spellIconDbc;
    DbcFile itemDisplayDbc;
    if (!LoadDbcFile("DBFilesClient\\Spell.dbc", 104U, spellDbc) ||
        !LoadDbcFile("DBFilesClient\\SpellIcon.dbc", 2U, spellIconDbc) ||
        !LoadDbcFile("DBFilesClient\\ItemDisplayInfo.dbc", 6U,
                     itemDisplayDbc)) {
        std::fprintf(stderr, "ACTIONBAR_DBC_LOAD_FAILED\n");
        return false;
    }

    for (std::uint32_t row = 0U; row < spellIconDbc.records; ++row) {
        const std::uint8_t* record = spellIconDbc.Record(row);
        s_spellIcons[ReadU32(record)] =
            spellIconDbc.String(ReadU32(record + 4U));
    }
    for (std::uint32_t row = 0U; row < spellDbc.records; ++row) {
        const std::uint8_t* record = spellDbc.Record(row);
        SpellArt art;
        art.firstEffect = ReadU32(record + 53U * 4U);
        art.iconID = ReadU32(record + 102U * 4U);
        art.activeIconID = ReadU32(record + 103U * 4U);
        for (std::uint32_t effect = 0U; effect < 3U; ++effect) {
            if (ReadU32(record + (53U + effect) * 4U) == 0U) continue;
            art.requiresSelectedUnit = art.requiresSelectedUnit ||
                IsSelectedUnitTarget(ReadU32(record + (71U + effect) * 4U)) ||
                IsSelectedUnitTarget(ReadU32(record + (74U + effect) * 4U));
        }
        s_spellArt[static_cast<int>(ReadU32(record))] = art;
    }
    for (std::uint32_t row = 0U; row < itemDisplayDbc.records; ++row) {
        const std::uint8_t* record = itemDisplayDbc.Record(row);
        s_itemIcons[ReadU32(record)] =
            itemDisplayDbc.String(ReadU32(record + 5U * 4U));
    }
    s_artLoaded = true;
    std::fprintf(stderr,
                 "ACTIONBAR_DBC spells=%zu spellIcons=%zu itemIcons=%zu\n",
                 s_spellArt.size(), s_spellIcons.size(), s_itemIcons.size());
    return true;
}

void RequestItem(int itemID) {
    if (itemID <= 0 || g_itemDBCache.GetRecord(itemID) ||
        !s_pendingItemQueries.insert(itemID).second) {
        return;
    }
    CDataStore message;
    message.Put(CMSG_ITEM_QUERY_SINGLE)->Put(static_cast<std::uint32_t>(itemID));
    ClientServices_Send(&message);
}

int __fastcall InitialSpellsHandler(void*, unsigned msgId, unsigned,
                                    CDataStore* message) {
    if (msgId != SMSG_INITIAL_SPELLS || !message) return 0;
    std::uint8_t spellState = 0U;
    std::uint16_t count = 0U;
    message->Get(&spellState)->Get(&count);
    s_knownSpells.clear();
    for (std::uint16_t index = 0U; index < count; ++index) {
        std::uint16_t spellID = 0U;
        std::uint16_t slot = 0U;
        message->Get(&spellID)->Get(&slot);
        if (message->Failed()) return 0;
        CGActionBar::AddKnownSpell(spellID, slot);
    }
    std::uint16_t cooldownCount = 0U;
    message->Get(&cooldownCount);
    for (std::uint16_t index = 0U; index < cooldownCount; ++index) {
        std::uint16_t spellID = 0U;
        std::uint16_t itemID = 0U;
        std::uint16_t category = 0U;
        std::int32_t recovery = 0;
        std::int32_t categoryRecovery = 0;
        message->Get(&spellID)->Get(&itemID)->Get(&category)
            ->Get(&recovery)->Get(&categoryRecovery);
        if (message->Failed()) return 0;
    }
    std::fprintf(stderr, "ACTIONBAR_INITIAL_SPELLS count=%u state=%u\n",
                 count, static_cast<unsigned>(spellState));
    FrameScript_SignalEvent(0xF4U); // SCRIPT_SPELLS_CHANGED
    return 1;
}

int __fastcall ActionButtonsHandler(void*, unsigned msgId, unsigned,
                                    CDataStore* message) {
    if (msgId != SMSG_ACTION_BUTTONS || !message) return 0;
    std::array<int, CGActionBar::NUM_ACTION_BUTTONS> buttons{};
    if (!message->GetArray(buttons.data(),
                           static_cast<unsigned>(buttons.size())) ||
        message->Failed()) {
        return 0;
    }
    CGActionBar::LoadInitialButtons(buttons);
    return 1;
}

unsigned ScriptSlot(lua_State* state, const char* usage) {
    if (!lua_isnumber(state, 1)) luaL_error(state, "%s", usage);
    const int slot = static_cast<int>(lua_tonumber(state, 1));
    return slot > 0 ? static_cast<unsigned>(slot - 1) :
                      CGActionBar::NUM_ACTION_BUTTONS;
}

void PushBooleanOrNil(lua_State* state, bool value) {
    if (value) lua_pushnumber(state, 1.0);
    else lua_pushnil(state);
}
}

void CGActionBar::InitializeGame() {
    m_slotActions.fill(0);
    m_bonusPage = 0U;
    s_knownSpells.clear();
    s_pendingItemQueries.clear();
    ClientServices_SetMessageHandler(SMSG_INITIAL_SPELLS,
                                     InitialSpellsHandler, nullptr);
    ClientServices_SetMessageHandler(SMSG_ACTION_BUTTONS,
                                     ActionButtonsHandler, nullptr);
}

void CGActionBar::EnterWorld() { UpdateBonusBar(); }
void CGActionBar::ShutdownGame() {}

void CGActionBar::LoadInitialButtons(
    const std::array<int, NUM_ACTION_BUTTONS>& buttons) {
    m_slotActions.fill(0);
    unsigned populated = 0U;
    for (unsigned slot = 0U; slot < NUM_ACTION_BUTTONS; ++slot) {
        const int action = buttons[slot];
        if (action == 0) continue;
        if (action > 0 && !s_knownSpells.empty() &&
            !s_knownSpells.contains(static_cast<std::uint16_t>(action))) {
            continue;
        }
        m_slotActions[slot] = action;
        ++populated;
        std::fprintf(stderr, "ACTIONBAR_SLOT_LOAD slot=%u action=%d\n",
                     slot + 1U, action);
        if (action < 0) RequestItem(-action);
    }

    const bool firstPageEmpty = std::all_of(
        m_slotActions.begin(), m_slotActions.begin() + 12,
        [](int action) { return action == 0; });
    const bool warriorPagePresent = std::any_of(
        m_slotActions.begin() + 72, m_slotActions.begin() + 84,
        [](int action) { return action != 0; });
    const unsigned classID = CGlueMgr::m_characterInfo
        ? CGlueMgr::m_characterInfo->classID : 0U;
    // m_bonusPage is the SpellShapeshiftForm bonus-action-bar offset, not an
    // absolute action page.  ActionButton_GetPagedID adds it after the six
    // regular pages, so offset 1 maps the warrior page to slots 73..84.
    m_bonusPage = classID == 1U && warriorPagePresent ? 1U :
        (firstPageEmpty && warriorPagePresent ? 1U : 0U);

    for (unsigned slot = 0U; slot < NUM_ACTION_BUTTONS; ++slot) {
        if (m_slotActions[slot] != 0)
            FrameScript_SignalEvent(SCRIPT_ACTIONBAR_SLOT_CHANGED,
                                    "%d", static_cast<int>(slot + 1U));
    }
    FrameScript_SignalEvent(SCRIPT_UPDATE_BONUS_ACTIONBAR);
    std::fprintf(stderr,
                 "ACTIONBAR_BUTTONS populated=%u bonusOffset=%u class=%u\n",
                 populated, m_bonusPage, classID);
}

void CGActionBar::AddKnownSpell(std::uint16_t spellID, std::uint16_t) {
    if (spellID != 0U) s_knownSpells.insert(spellID);
}

int CGActionBar::GetAction(unsigned slot) {
    return slot < NUM_ACTION_BUTTONS ? m_slotActions[slot] : 0;
}

void CGActionBar::AddAction(int action) {
    const auto found = std::find(m_slotActions.begin(), m_slotActions.end(), 0);
    if (found == m_slotActions.end()) return;
    SetAction(static_cast<unsigned>(found - m_slotActions.begin()), action);
}

const char* CGActionBar::GetAttackTexture() {
    for (unsigned slot = 0U; slot < NUM_ACTION_BUTTONS; ++slot)
        if (IsAttackAction(slot)) return GetTexture(slot);
    return nullptr;
}

const char* CGActionBar::GetItemTexture(int itemID) {
    if (itemID <= 0 || !LoadActionArt()) return nullptr;
    const ItemStats_C* item = g_itemDBCache.GetRecord(itemID);
    if (!item) {
        RequestItem(itemID);
        return nullptr;
    }
    s_pendingItemQueries.erase(itemID);
    const auto icon = s_itemIcons.find(
        static_cast<std::uint32_t>(item->m_displayInfoID));
    return icon == s_itemIcons.end() || icon->second.empty()
        ? nullptr : icon->second.c_str();
}

void CGActionBar::GetCooldown(unsigned, unsigned* startTime,
                              unsigned* duration, unsigned* enable) {
    if (startTime) *startTime = 0U;
    if (duration) *duration = 0U;
    if (enable) *enable = 1U;
}

int CGActionBar::GetCount(unsigned slot) {
    return GetAction(slot) < 0 ? 1 : 0;
}

const char* CGActionBar::GetTexture(unsigned slot) {
    const int action = GetAction(slot);
    if (action == 0 || !LoadActionArt()) return nullptr;
    if (action > 0) {
        const auto spell = s_spellArt.find(action);
        if (spell == s_spellArt.end()) return nullptr;
        const std::uint32_t iconID = IsToggledAction(slot) &&
            spell->second.activeIconID != 0U
            ? spell->second.activeIconID : spell->second.iconID;
        const auto icon = s_spellIcons.find(iconID);
        return icon == s_spellIcons.end() || icon->second.empty()
            ? nullptr : icon->second.c_str();
    }

    return GetItemTexture(-action);
}

void CGActionBar::HideGrid() {
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_HIDEGRID);
}

bool CGActionBar::IsAttackAction(unsigned slot) {
    const int action = GetAction(slot);
    if (action <= 0 || !LoadActionArt()) return false;
    const auto spell = s_spellArt.find(action);
    return spell != s_spellArt.end() && spell->second.firstEffect == 78U;
}

bool CGActionBar::IsCurrentAction(unsigned) { return false; }
bool CGActionBar::IsToggledAction(unsigned) { return false; }

bool CGActionBar::IsUsableAction(unsigned slot, bool* noMana) {
    if (noMana) *noMana = false;
    return GetAction(slot) != 0;
}

void CGActionBar::PickupAction(unsigned) {}
void CGActionBar::PrecacheButtonArt(unsigned slot) { (void)GetTexture(slot); }
void CGActionBar::PutActionInSlot(unsigned) {}

void CGActionBar::RemoveAction(unsigned slot) {
    if (slot >= NUM_ACTION_BUTTONS) return;
    m_slotActions[slot] = 0;
    SlotChanged(slot);
}

void CGActionBar::RemoveSpell(int spell) {
    for (unsigned slot = 0U; slot < NUM_ACTION_BUTTONS; ++slot)
        if (m_slotActions[slot] == spell) RemoveAction(slot);
}

void CGActionBar::ReplaceSpell(int oldSpell, int newSpell) {
    for (unsigned slot = 0U; slot < NUM_ACTION_BUTTONS; ++slot)
        if (m_slotActions[slot] == oldSpell) SetAction(slot, newSpell);
}

void CGActionBar::SetAction(unsigned slot, int action) {
    if (slot >= NUM_ACTION_BUTTONS) return;
    m_slotActions[slot] = action;
    if (action < 0) RequestItem(-action);
    SlotChanged(slot);
}

void CGActionBar::ShowGrid() {
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_SHOWGRID);
}

void CGActionBar::SlotChanged(unsigned slot) {
    if (slot >= NUM_ACTION_BUTTONS) return;
    CDataStore message;
    message.Put(CMSG_SET_ACTION_BUTTON)
        ->Put(slot)->Put(m_slotActions[slot]);
    ClientServices_Send(&message);
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_SLOT_CHANGED,
                            "%d", static_cast<int>(slot + 1U));
}

void CGActionBar::UpdateBonusBar() {
    const bool firstPageEmpty = std::all_of(
        m_slotActions.begin(), m_slotActions.begin() + 12,
        [](int action) { return action == 0; });
    const bool warriorPagePresent = std::any_of(
        m_slotActions.begin() + 72, m_slotActions.begin() + 84,
        [](int action) { return action != 0; });
    const unsigned classID = CGlueMgr::m_characterInfo
        ? CGlueMgr::m_characterInfo->classID : 0U;
    // The recovered function derives this from the active unit's shapeshift
    // form.  That mirror field is not retained yet, but Alpha's starting-data
    // contract is unambiguous: warriors have an empty normal page and their
    // base actions in bonus page 1 (slots 73..84).  CGlueMgr releases its
    // character record while entering the world, so do not erase that page
    // merely because the glue record is no longer available.
    m_bonusPage = warriorPagePresent &&
        (classID == 1U || firstPageEmpty || m_bonusPage == 1U) ? 1U : 0U;
    std::fprintf(stderr,
                 "ACTIONBAR_BONUS_UPDATE offset=%u class=%u firstEmpty=%d "
                 "warriorPage=%d\n",
                 m_bonusPage, classID, firstPageEmpty ? 1 : 0,
                 warriorPagePresent ? 1 : 0);
    FrameScript_SignalEvent(SCRIPT_UPDATE_BONUS_ACTIONBAR);
}

void CGActionBar::UpdateCooldowns() {
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_UPDATE_COOLDOWN);
}

void CGActionBar::UpdateItem(int itemID) {
    s_pendingItemQueries.erase(itemID);
    for (unsigned slot = 0U; slot < NUM_ACTION_BUTTONS; ++slot) {
        if (m_slotActions[slot] == -itemID)
            FrameScript_SignalEvent(SCRIPT_ACTIONBAR_SLOT_CHANGED,
                                    "%d", static_cast<int>(slot + 1U));
    }
}

bool CGActionBar::HasKnownSpells() { return !s_knownSpells.empty(); }

void CGActionBar::RefreshItemActions() {
    for (const int action : m_slotActions)
        if (action < 0) UpdateItem(-action);
}

void CGActionBar::UpdateSelection() {
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_UPDATE_STATE);
}

void CGActionBar::UpdateUsable() {
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_UPDATE_USABLE);
}

void CGActionBar::UseAction(unsigned slot, bool) {
    if (CWorld::IsPlayerDead()) return;
    const int action = GetAction(slot);
    if (action < 0) {
        const int inventorySlot = ClntObjMgrFindInventorySlot(
            static_cast<std::uint32_t>(-action));
        if (inventorySlot < 0 || inventorySlot > 255) return;
        CDataStore message;
        message.Put(CMSG_USE_ITEM)
            ->Put(static_cast<std::uint8_t>(23U))
            ->Put(static_cast<std::uint8_t>(inventorySlot))
            ->Put(static_cast<std::uint8_t>(0U))
            ->Put(static_cast<std::uint16_t>(0U));
        ClientServices_Send(&message);
        return;
    }
    if (action == 0) return;

    const bool artLoaded = LoadActionArt();
    const auto art = s_spellArt.find(action);
    const bool requiresSelectedUnit = artLoaded &&
        art != s_spellArt.end() && art->second.requiresSelectedUnit;
    std::uint64_t targetGuid = 0U;
    const bool hasSelectedUnit =
        ClientRuntime_GetSelectedTarget(&targetGuid) != 0 &&
        ClntObjMgrGetVisibleObject(targetGuid) != nullptr;
    const bool isAutoAttack = artLoaded && art != s_spellArt.end() &&
        art->second.firstEffect == 78U;
    if (isAutoAttack) {
        if (!hasSelectedUnit) {
            std::fprintf(stderr,
                         "ACTIONBAR_ATTACK_NEEDS_TARGET slot=%u spell=%d\n",
                         slot + 1U, action);
            return;
        }
        CDataStore message;
        message.Put(CMSG_ATTACKSWING)->Put(targetGuid);
        std::fprintf(stderr,
                     "ACTIONBAR_ATTACK slot=%u spell=%d target=%llu\n",
                     slot + 1U, action,
                     static_cast<unsigned long long>(targetGuid));
        ClientServices_Send(&message);
        return;
    }
    if (requiresSelectedUnit && !hasSelectedUnit) {
        std::fprintf(stderr,
                     "ACTIONBAR_CAST_NEEDS_TARGET slot=%u spell=%d\n",
                     slot + 1U, action);
        return;
    }

    const std::uint16_t targetMask =
        requiresSelectedUnit ? static_cast<std::uint16_t>(0x0002U) : 0U;
    CDataStore message;
    message.Put(CMSG_CAST_SPELL)
        ->Put(static_cast<std::uint32_t>(action))
        ->Put(targetMask);
    if ((targetMask & 0x0802U) != 0U) message.Put(targetGuid);
    std::fprintf(stderr,
                 "ACTIONBAR_CAST slot=%u spell=%d mask=0x%04X target=%llu\n",
                 slot + 1U, action, static_cast<unsigned>(targetMask),
                 static_cast<unsigned long long>(targetGuid));
    ClientServices_Send(&message);
}

int Script_GetActionTexture(lua_State* state) {
    const unsigned slot = ScriptSlot(state, "Usage: GetActionTexture(slot)");
    const char* texture = CGActionBar::GetTexture(slot);
    static unsigned loggedCalls = 0U;
    if (loggedCalls < 96U) {
        std::fprintf(stderr, "ACTIONBAR_TEXTURE slot=%u action=%d texture=%s\n",
                     slot + 1U, CGActionBar::GetAction(slot),
                     texture ? texture : "<none>");
        ++loggedCalls;
    }
    if (texture) lua_pushstring(state, texture);
    else lua_pushnil(state);
    return 1;
}

int Script_GetActionCount(lua_State* state) {
    lua_pushnumber(state, CGActionBar::GetCount(
        ScriptSlot(state, "Usage: GetActionCount(slot)")));
    return 1;
}

int Script_GetActionCooldown(lua_State* state) {
    unsigned startTime = 0U;
    unsigned duration = 0U;
    unsigned enable = 0U;
    CGActionBar::GetCooldown(
        ScriptSlot(state, "Usage: GetActionCooldown(slot)"),
        &startTime, &duration, &enable);
    lua_pushnumber(state, static_cast<double>(startTime) * 0.001);
    lua_pushnumber(state, static_cast<double>(duration) * 0.001);
    lua_pushnumber(state, static_cast<double>(enable));
    return 3;
}

int Script_HasAction(lua_State* state) {
    const unsigned slot = ScriptSlot(state, "Usage: HasAction(slot)");
    const int action = CGActionBar::GetAction(slot);
    static unsigned loggedCalls = 0U;
    if (loggedCalls < 96U) {
        std::fprintf(stderr, "ACTIONBAR_HAS slot=%u action=%d\n",
                     slot + 1U, action);
        ++loggedCalls;
    }
    PushBooleanOrNil(state, action != 0);
    return 1;
}

int Script_UseAction(lua_State* state) {
    const unsigned slot = ScriptSlot(state, "Usage: UseAction(slot[, checkCursor])");
    bool checkCursor = true;
    if (lua_isstring(state, 2)) {
        const char* value = lua_tostring(state, 2);
        checkCursor = value && _stricmp(value, "false") != 0 &&
            std::strcmp(value, "0") != 0;
    }
    std::fprintf(stderr,
                 "ACTIONBAR_USE slot=%u action=%d checkCursor=%d\n",
                 slot + 1U, CGActionBar::GetAction(slot),
                 checkCursor ? 1 : 0);
    CGActionBar::UseAction(slot, checkCursor);
    return 0;
}

int Script_PickupAction(lua_State* state) {
    CGActionBar::PickupAction(
        ScriptSlot(state, "Usage: PickupAction(slot)"));
    return 0;
}

int Script_PlaceAction(lua_State* state) {
    CGActionBar::PutActionInSlot(
        ScriptSlot(state, "Usage: PlaceAction(slot)"));
    return 0;
}

int Script_IsAttackAction(lua_State* state) {
    PushBooleanOrNil(state, CGActionBar::IsAttackAction(
        ScriptSlot(state, "Usage: IsAttackAction(slot)")));
    return 1;
}

int Script_IsCurrentAction(lua_State* state) {
    PushBooleanOrNil(state, CGActionBar::IsCurrentAction(
        ScriptSlot(state, "Usage: IsCurrentAction(slot)")));
    return 1;
}

int Script_IsUsableAction(lua_State* state) {
    bool noMana = false;
    const bool usable = CGActionBar::IsUsableAction(
        ScriptSlot(state, "Usage: IsUsableAction(slot)"), &noMana);
    PushBooleanOrNil(state, usable);
    PushBooleanOrNil(state, noMana);
    return 2;
}

int Script_GetBonusBarOffset(lua_State* state) {
    static unsigned loggedCalls = 0U;
    if (loggedCalls < 32U) {
        std::fprintf(stderr, "ACTIONBAR_GET_BONUS offset=%u\n",
                     CGActionBar::m_bonusPage);
        ++loggedCalls;
    }
    lua_pushnumber(state, static_cast<double>(CGActionBar::m_bonusPage));
    return 1;
}

int Script_ChangeActionBarPage(lua_State*) {
    FrameScript_SignalEvent(SCRIPT_ACTIONBAR_PAGE_CHANGED);
    return 0;
}

int Script_PrecacheSpellArt(lua_State* state) {
    if (lua_isnumber(state, 1))
        CGActionBar::PrecacheButtonArt(ScriptSlot(state, ""));
    return 0;
}

void ActionBarRegisterScriptFunctions() {
    static constexpr struct {
        const char* name;
        lua_CFunction function;
    } functions[] = {
        {"GetActionTexture", Script_GetActionTexture},
        {"GetActionCount", Script_GetActionCount},
        {"GetActionCooldown", Script_GetActionCooldown},
        {"HasAction", Script_HasAction},
        {"UseAction", Script_UseAction},
        {"PickupAction", Script_PickupAction},
        {"PlaceAction", Script_PlaceAction},
        {"IsAttackAction", Script_IsAttackAction},
        {"IsCurrentAction", Script_IsCurrentAction},
        {"IsUsableAction", Script_IsUsableAction},
        {"GetBonusBarOffset", Script_GetBonusBarOffset},
        {"ChangeActionBarPage", Script_ChangeActionBarPage},
        {"PrecacheSpellArt", Script_PrecacheSpellArt},
    };
    for (const auto& function : functions)
        FrameScript_RegisterFunction(function.name, function.function);
}

void ActionBarUnregisterScriptFunctions() {
    static constexpr const char* names[] = {
        "GetActionTexture", "GetActionCount", "GetActionCooldown",
        "HasAction", "UseAction", "PickupAction", "PlaceAction",
        "IsAttackAction", "IsCurrentAction", "IsUsableAction",
        "GetBonusBarOffset", "ChangeActionBarPage", "PrecacheSpellArt",
    };
    for (const char* name : names) FrameScript_UnregisterFunction(name);
}
