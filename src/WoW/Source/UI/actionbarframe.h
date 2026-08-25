#pragma once

#include <array>
#include <cstdint>

struct lua_State;

class CGActionBar {
public:
    static constexpr unsigned NUM_ACTION_BUTTONS = 120U;

    static void InitializeGame();
    static void EnterWorld();
    static void ShutdownGame();
    static void AddAction(int action);
    static const char* GetAttackTexture();
    static const char* GetItemTexture(int itemID);
    static void GetCooldown(unsigned slot, unsigned* startTime,
                            unsigned* duration, unsigned* enable);
    static int GetCount(unsigned slot);
    static const char* GetTexture(unsigned slot);
    static void HideGrid();
    static bool IsAttackAction(unsigned slot);
    static bool IsCurrentAction(unsigned slot);
    static bool IsToggledAction(unsigned slot);
    static bool IsUsableAction(unsigned slot, bool* noMana);
    static void PickupAction(unsigned slot);
    static void PrecacheButtonArt(unsigned slot);
    static void PutActionInSlot(unsigned slot);
    static void RemoveAction(unsigned slot);
    static void RemoveSpell(int spell);
    static void ReplaceSpell(int oldSpell, int newSpell);
    static void SetAction(unsigned slot, int action);
    static void ShowGrid();
    static void SlotChanged(unsigned slot);
    static void UpdateBonusBar();
    static void UpdateCooldowns();
    static void UpdateItem(int itemID);
    static void UpdateSelection();
    static void UpdateUsable();
    static void UseAction(unsigned slot, bool checkCursor);

    static void LoadInitialButtons(const std::array<int, NUM_ACTION_BUTTONS>& buttons);
    static void AddKnownSpell(std::uint16_t spellID, std::uint16_t slot);
    static int GetAction(unsigned slot);
    static bool HasKnownSpells();
    static void RefreshItemActions();

    static std::array<int, NUM_ACTION_BUTTONS> m_slotActions;
    static unsigned m_bonusPage;
};

void ActionBarRegisterScriptFunctions();
void ActionBarUnregisterScriptFunctions();
int Script_ChangeActionBarPage(lua_State* state);
int Script_GetActionCooldown(lua_State* state);
int Script_GetActionCount(lua_State* state);
int Script_GetActionTexture(lua_State* state);
int Script_GetBonusBarOffset(lua_State* state);
int Script_HasAction(lua_State* state);
int Script_IsAttackAction(lua_State* state);
int Script_IsCurrentAction(lua_State* state);
int Script_IsUsableAction(lua_State* state);
int Script_PickupAction(lua_State* state);
int Script_PlaceAction(lua_State* state);
int Script_PrecacheSpellArt(lua_State* state);
int Script_UseAction(lua_State* state);
