#pragma once

class ATTACKROUNDINFO {
public:
    void PI();
    void UI();
    int attackerSequence = 0;
    int victimSequence = 0;
    bool pending = false;
};

class CGUnit_C {
public:
    void AddVictimDeathHold();
    void AdjustVictimState();
    void AttachResEffectModel();
    void AttackAnimEndHandler();
    void AttackUnit();
    void CheckPendingVictimFeedback();
    void ClearMeleeDeathHold();
    void ClearResEffectModel();
    void DetatchResEffectModel();
    void DetermineAttackerSequence();
    void DetermineBloodLinkPoint();
    void DetermineParrySequence();
    void DetermineReadySequence();
    void DetermineWoundSequence();
    void DoVictimFeedback();
    void DodgeAnimEndHandler();
    void GetAttackerAnimEx();
    void GetAttackingWeapon();
    void GetDefendingItem();
    void GetParryingItem();
    void GetUnitSize();
    void HandleCombatAnimEvent();
    void HandleMirrorTimerDamage();
    void InitializeResEffectModel();
    void IsAttackAnimState();
    void IsPreemptableWoundAnimState();
    void OnAttackStart();
    void OnAttackStop();
    void OnAttackSwing();
    void OnAttackerStateChange();
    void OnCombatModeTimer();
    void OnDeathAnimate();
    void PerformSpellProcImpact();
    void PlayDeathThudCameraShake();
    void QueueVictimAnim();
    void ResetFingersSeq();
    void SetAttackerAnimation();
    void SetDebugHitRolls();
    void SetFingersSeq();
    void SetHandState();
    void SetHandsState();
    void SetMeleeDeathHold();
    void SetVictimAnimation();
    void ShowBloodSpurt();
    void ShowPlayerXPGained();
    void ShowWorldText();
    void StopAttack();
    void UpdateReadyAnim();
    void WoundAnimEndHandler();
    void SetAction(unsigned action);
    unsigned m_combatAction = 0;
    unsigned m_attackerSequence = 0;
    unsigned m_victimSequence = 0;
    unsigned m_handState = 0;
    unsigned m_debugHitRolls = 0;
    bool m_attacking = false;
    bool m_deathHold = false;
    bool m_resEffectActive = false;
    bool m_pendingVictimFeedback = false;
    float m_unitSize = 1.0F;
};

class DAMAGELOGBASE {
public:
    DAMAGELOGBASE();
    int spellID = 0;
    int amount = 0;
    int school = 0;
};

class ENCHANTMENTLOG {
public:
    void PI();
    void UI();
};

class ENVIRONMENTALDAMAGE {
public:
    void PI();
    void UI();
};

class HITSPRITE {
public:
    ~HITSPRITE();
};

class MIRRORTIMERDAMAGE {
public:
    void PI();
    void UI();
};

class PARTYKILLLOG {
public:
    void PI();
    void UI();
};

class RESISTLOG {
public:
    void PI();
    void UI();
};

class SPELLLOG {
public:
    void PI();
    void UI();
};

class SPELLMISSLOG {
public:
    void PI();
    void UI();
};

// Recovered symbol declarations for objectclient/unitcombat_c.cpp
