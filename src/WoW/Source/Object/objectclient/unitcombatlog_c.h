#pragma once

class ATTACKROUNDINFO {
public:
    ATTACKROUNDINFO();
};

class COMBATLOGDESC {
public:
    COMBATLOGDESC();
    void Clear();
    void LogAttack();
    void LogUnitGUID();
    void LogVictim();
    ~COMBATLOGDESC();
    unsigned attackCount = 0;
    unsigned victimCount = 0;
    unsigned guidCount = 0;
    bool valid = false;
};

class DAMAGELOGBASE {
public:
    DAMAGELOGBASE();
};

// Recovered symbol declarations for objectclient/unitcombatlog_c.cpp

extern unsigned g_unitCombatLogMessages;
extern bool g_unitCombatLogEnabled;
