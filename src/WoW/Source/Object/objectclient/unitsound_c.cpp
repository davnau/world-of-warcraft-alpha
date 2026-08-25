#include "WoW/Source/Object/objectclient/unitsound_c.h"

unsigned g_unitSoundEvents = 0;
namespace { bool s_unitSoundInitialized = false; }

void CGUnit_C::GetImpactType() { m_lastSoundID = 1; ++m_soundEventCount; }
void CGUnit_C::GetWeaponSwingType() { m_lastSoundID = 2; ++m_soundEventCount; }
void CGUnit_C::HandleFootfallAnimEvent() { m_lastSoundID = 3; ++m_soundEventCount; }
void CGUnit_C::HandlePlayStandSound() { PlayStandSound(); }
void CGUnit_C::InitializeLoopSound() { m_loopSoundID = 1; }
void CGUnit_C::KillCreatureLoopSound() { m_loopSoundID = 0; }
void CGUnit_C::KillSpellLoopedSound() { m_loopSoundID = 0; }
void CGUnit_C::PlayCustomAttackSound() { m_lastSoundID = 4; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayDeathThud() { m_lastSoundID = 5; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayFidgetSound() { m_lastSoundID = 6; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayFoleySound() { m_lastSoundID = 7; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayImpactSound() { m_lastSoundID = 8; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayNPCSound() { m_lastSoundID = 9; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayParrySound() { m_lastSoundID = 10; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlaySpellLoopedSound() { m_loopSoundID = 11; ++g_unitSoundEvents; }
void CGUnit_C::PlaySplashSound() { m_lastSoundID = 12; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayStandSound() { m_lastSoundID = 13; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::PlayUnitSound() { m_lastSoundID = 14; ++m_soundEventCount; ++g_unitSoundEvents; }
void CGUnit_C::SetCustomAttackSound() { m_lastSoundID = 4; }

void CheckUnitPlaySound() { ++g_unitSoundEvents; }
void CheckUnitSoundTimer() { ++g_unitSoundEvents; }
void ClearDeathThudSounds() { g_unitSoundEvents = 0; }
void GenerateDeathThudSounds() { ++g_unitSoundEvents; }
void GetFidgetSoundID() { ++g_unitSoundEvents; }
void GetSoundID() { ++g_unitSoundEvents; }
void UnitSoundInitialize() { s_unitSoundInitialized = true; g_unitSoundEvents = 0; }
void UnitSoundShutdown() { s_unitSoundInitialized = false; }
