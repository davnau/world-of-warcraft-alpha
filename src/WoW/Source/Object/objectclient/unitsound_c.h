#pragma once

class CGUnit_C {
public:
    void GetImpactType();
    void GetWeaponSwingType();
    void HandleFootfallAnimEvent();
    void HandlePlayStandSound();
    void InitializeLoopSound();
    void KillCreatureLoopSound();
    void KillSpellLoopedSound();
    void PlayCustomAttackSound();
    void PlayDeathThud();
    void PlayFidgetSound();
    void PlayFoleySound();
    void PlayImpactSound();
    void PlayNPCSound();
    void PlayParrySound();
    void PlaySpellLoopedSound();
    void PlaySplashSound();
    void PlayStandSound();
    void PlayUnitSound();
    void SetCustomAttackSound();
    unsigned m_lastSoundID = 0;
    unsigned m_loopSoundID = 0;
    unsigned m_soundEventCount = 0;
};

extern unsigned g_unitSoundEvents;

// Recovered symbol declarations for objectclient/unitsound_c.cpp
