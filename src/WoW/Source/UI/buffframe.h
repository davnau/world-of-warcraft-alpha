#pragma once

class CGBuffBar {
public:
    void EnterWorld();
    void GetBuffByFilter();
    void GetBuffByIndex();
    void GetBuffTimeLeftByIndex();
    void InitializeGame();
    void LeaveWorld();
    void ShutdownGame();
    void UpdateBuffs();
    void UpdateDuration();
};

class CGBuffDesc {
public:
    CGBuffDesc();
    void SetAuraIndex();
};


