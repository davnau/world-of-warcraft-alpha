#pragma once

class CGPetInfo {
public:
    void ClearActions();
    void EnterWorld();
    void GetModeToken();
    void GetOrdersToken();
    void HideGrid();
    void InitializeGame();
    void LeaveWorld();
    void PetAbandon();
    void PetAggressiveMode();
    void PetAttackTarget();
    void PetDefensiveMode();
    void PetDismiss();
    void PetFollow();
    void PetPassiveMode();
    void PetRename();
    void PetWait();
    void PutActionInSlot();
    void SendPetAction();
    void SetAction();
    void SetPet();
    void SetPetMode();
    void SetPetModeAndOrders();
    void SetPetOrders();
    void ShowGrid();
    void ShutdownGame();
    void ToggleAutocast();
    void UpdateCooldowns();
};


