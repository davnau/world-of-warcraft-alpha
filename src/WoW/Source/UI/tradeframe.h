#pragma once

class CGTradeInfo {
public:
    void EnterWorld();
    void GetGameError();
    void HandleTradeMessage();
    void LeaveWorld();
    void PlayerAccept();
    void RemovePlayerItem();
    void SetPlayerItem();
    void SetTradePartner();
    void TargetAccept();
    void UnlockTradeItems();
    void Update();
    void UpdatePlayerItem();
};


