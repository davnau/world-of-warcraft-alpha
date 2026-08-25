#pragma once

class CGLootInfo {
public:
    void ClearSlot();
    void CoinsCleared();
    void EnterWorld();
    void GetLootCoin();
    void GetLootItem();
    void GetLootQuality();
    void GetLootQuantity();
    void GetLootSlotLink();
    void GetLootSlotText();
    void GetLootSlotTexture();
    void GetLootType();
    void GetNumItems();
    void HasLoot();
    void InitializeGame();
    void LeaveWorld();
    void LootButtonItemStatsCallback();
    void LootSlot();
    void SetObject();
    void ShutdownGame();
};


