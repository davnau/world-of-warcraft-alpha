#pragma once

#include <cstdint>

class MISSILENODE {
public:
    void CheckModelLoadStatus();
    MISSILENODE();
    ~MISSILENODE();
    bool modelLoaded = false;
    bool active = false;
    std::uint64_t target = 0;
};

class NODEBASE {
public:
    void CheckModelLoadStatus();
    void ClearDeathHoldTimer();
    void SetDeathHoldTimer();
    ~NODEBASE();
    unsigned deathHoldTimer = 0;
    bool modelLoaded = false;
};

class ONESHOTEFFECTNODE {
public:
    void CheckModelLoadStatus();
    void ReleaseDeathHolds();
    ~ONESHOTEFFECTNODE();
    bool modelLoaded = false;
    bool deathHoldReleased = false;
};

class ONESHOTSTANDALONEEFFECTNODE {
public:
    void CheckModelLoadStatus();
    ONESHOTSTANDALONEEFFECTNODE();
    void ReleaseDeathHolds();
    ~ONESHOTSTANDALONEEFFECTNODE();
    bool modelLoaded = false;
    bool deathHoldReleased = false;
};

class PERSISTENTUNITEFFECT {
public:
    void Clear();
    bool active = false;
    int spellID = 0;
};

extern unsigned g_unitEffectUpdateCount;
extern unsigned g_unitEffectRenderCount;

// Recovered symbol declarations for objectclient/uniteffects.cpp
