#pragma once

class CGSpellBook {
public:
    void AddKnownSpell();
    void AddPetSpell();
    void CastSpell();
    void ClearPetSpells();
    void ClearSpells();
    void DelKnownSpell();
    void InitializeGame();
    void IsSelectedSlot();
    void IsToggledSpell();
    void PickupSpell();
    void ReplaceSpell();
    void SendSpellSlot();
    void SetSpell();
    void ShutdownGame();
    void UpdateCooldowns();
    void UpdateSelection();
    void UpdateSpells();
};


