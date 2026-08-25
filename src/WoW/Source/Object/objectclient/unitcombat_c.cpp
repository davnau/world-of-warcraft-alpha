#include "WoW/Source/Object/objectclient/unitcombat_c.h"

#include <algorithm>
#include <limits>

namespace {
bool s_unitCombatInitialized = false;
unsigned s_lastCombatEvent = 0;
bool s_debugHitRolls = false;
}

void ATTACKROUNDINFO::PI() { attackerSequence = 0; victimSequence = 0; pending = false; }
void ATTACKROUNDINFO::UI() { pending = false; ++attackerSequence; ++victimSequence; }

void CGUnit_C::SetAction(unsigned action) { m_combatAction = action; }
void CGUnit_C::AddVictimDeathHold() { m_deathHold = true; SetAction(1); }
void CGUnit_C::AdjustVictimState() { m_pendingVictimFeedback = false; SetAction(2); }
void CGUnit_C::AttachResEffectModel() { m_resEffectActive = true; }
void CGUnit_C::AttackAnimEndHandler() { m_attacking = false; SetAction(3); }
void CGUnit_C::AttackUnit() { m_attacking = true; SetAction(4); ++m_attackerSequence; }
void CGUnit_C::CheckPendingVictimFeedback() { if (m_pendingVictimFeedback) DoVictimFeedback(); }
void CGUnit_C::ClearMeleeDeathHold() { m_deathHold = false; }
void CGUnit_C::ClearResEffectModel() { m_resEffectActive = false; }
void CGUnit_C::DetatchResEffectModel() { ClearResEffectModel(); }
void CGUnit_C::DetermineAttackerSequence() { m_attackerSequence = (m_attackerSequence + 1) % 4; }
void CGUnit_C::DetermineBloodLinkPoint() { SetAction(5); }
void CGUnit_C::DetermineParrySequence() { m_victimSequence = 2; SetAction(6); }
void CGUnit_C::DetermineReadySequence() { m_victimSequence = 0; SetAction(7); }
void CGUnit_C::DetermineWoundSequence() { m_victimSequence = 1; SetAction(8); }
void CGUnit_C::DoVictimFeedback() { m_pendingVictimFeedback = false; SetAction(9); }
void CGUnit_C::DodgeAnimEndHandler() { SetAction(10); }
void CGUnit_C::GetAttackerAnimEx() { DetermineAttackerSequence(); }
void CGUnit_C::GetAttackingWeapon() { SetAction(11); }
void CGUnit_C::GetDefendingItem() { SetAction(12); }
void CGUnit_C::GetParryingItem() { SetAction(13); }
void CGUnit_C::GetUnitSize() { m_unitSize = std::max(0.0F, m_unitSize); }
void CGUnit_C::HandleCombatAnimEvent() { ++s_lastCombatEvent; SetAction(s_lastCombatEvent); }
void CGUnit_C::HandleMirrorTimerDamage() { m_pendingVictimFeedback = true; SetAction(14); }
void CGUnit_C::InitializeResEffectModel() { m_resEffectActive = false; }
void CGUnit_C::IsAttackAnimState() { m_attacking = true; }
void CGUnit_C::IsPreemptableWoundAnimState() { m_pendingVictimFeedback = true; }
void CGUnit_C::OnAttackStart() { m_attacking = true; SetAction(15); }
void CGUnit_C::OnAttackStop() { m_attacking = false; SetAction(16); }
void CGUnit_C::OnAttackSwing() { m_attacking = true; ++m_attackerSequence; SetAction(17); }
void CGUnit_C::OnAttackerStateChange() { DetermineAttackerSequence(); }
void CGUnit_C::OnCombatModeTimer() { if (m_attacking) ++m_debugHitRolls; }
void CGUnit_C::OnDeathAnimate() { m_deathHold = true; m_attacking = false; SetAction(18); }
void CGUnit_C::PerformSpellProcImpact() { SetAction(19); }
void CGUnit_C::PlayDeathThudCameraShake() { SetAction(20); }
void CGUnit_C::QueueVictimAnim() { m_pendingVictimFeedback = true; SetAction(21); }
void CGUnit_C::ResetFingersSeq() { m_victimSequence = 0; }
void CGUnit_C::SetAttackerAnimation() { SetAction(m_attackerSequence); }
void CGUnit_C::SetDebugHitRolls() { s_debugHitRolls = !s_debugHitRolls; }
void CGUnit_C::SetFingersSeq() { ++m_victimSequence; }
void CGUnit_C::SetHandState() { m_handState = 1; }
void CGUnit_C::SetHandsState() { m_handState = 2; }
void CGUnit_C::SetMeleeDeathHold() { m_deathHold = true; }
void CGUnit_C::SetVictimAnimation() { SetAction(m_victimSequence); }
void CGUnit_C::ShowBloodSpurt() { SetAction(22); }
void CGUnit_C::ShowPlayerXPGained() { SetAction(23); }
void CGUnit_C::ShowWorldText() { SetAction(24); }
void CGUnit_C::StopAttack() { m_attacking = false; SetAction(25); }
void CGUnit_C::UpdateReadyAnim() { DetermineReadySequence(); }
void CGUnit_C::WoundAnimEndHandler() { DetermineWoundSequence(); }

DAMAGELOGBASE::DAMAGELOGBASE() = default;
void ENCHANTMENTLOG::PI() {}
void ENCHANTMENTLOG::UI() {}
void ENVIRONMENTALDAMAGE::PI() {}
void ENVIRONMENTALDAMAGE::UI() {}
HITSPRITE::~HITSPRITE() = default;
void MIRRORTIMERDAMAGE::PI() {}
void MIRRORTIMERDAMAGE::UI() {}
void PARTYKILLLOG::PI() {}
void PARTYKILLLOG::UI() {}
void RESISTLOG::PI() {}
void RESISTLOG::UI() {}
void SPELLLOG::PI() {}
void SPELLLOG::UI() {}
void SPELLMISSLOG::PI() {}
void SPELLMISSLOG::UI() {}

void FindAnimation() { s_lastCombatEvent = 0; }
void LoadAnimKitTable() { s_unitCombatInitialized = true; }
void OnUnitCombatEvent() { ++s_lastCombatEvent; }
void OnUnitDamageDone() { ++s_lastCombatEvent; }
void OnUnitDamageTaken() { ++s_lastCombatEvent; }
void UnitCombatClientInitialize() { s_unitCombatInitialized = true; s_lastCombatEvent = 0; }
void UnitCombatClientShutdown() { s_unitCombatInitialized = false; s_lastCombatEvent = 0; }
