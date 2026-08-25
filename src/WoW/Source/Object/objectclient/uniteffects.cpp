#include "WoW/Source/Object/objectclient/uniteffects.h"

#include <algorithm>

unsigned g_unitEffectUpdateCount = 0;
unsigned g_unitEffectRenderCount = 0;

namespace {
bool s_unitEffectsInitialized = false;
bool s_spellPrecastActive = false;
unsigned s_activeMissiles = 0;
unsigned s_activeAuras = 0;
}

void UnitEffectsInitialize();

void MISSILENODE::CheckModelLoadStatus() { modelLoaded = active; }
MISSILENODE::MISSILENODE() : modelLoaded(false), active(false), target(0) {}
MISSILENODE::~MISSILENODE() { active = false; }
void NODEBASE::CheckModelLoadStatus() { modelLoaded = deathHoldTimer == 0; }
void NODEBASE::ClearDeathHoldTimer() { deathHoldTimer = 0; }
void NODEBASE::SetDeathHoldTimer() { deathHoldTimer = 1; }
NODEBASE::~NODEBASE() { ClearDeathHoldTimer(); }
void ONESHOTEFFECTNODE::CheckModelLoadStatus() { modelLoaded = true; }
void ONESHOTEFFECTNODE::ReleaseDeathHolds() { deathHoldReleased = true; }
ONESHOTEFFECTNODE::~ONESHOTEFFECTNODE() { ReleaseDeathHolds(); }
void ONESHOTSTANDALONEEFFECTNODE::CheckModelLoadStatus() { modelLoaded = true; }
ONESHOTSTANDALONEEFFECTNODE::ONESHOTSTANDALONEEFFECTNODE() : modelLoaded(false), deathHoldReleased(false) {}
void ONESHOTSTANDALONEEFFECTNODE::ReleaseDeathHolds() { deathHoldReleased = true; }
ONESHOTSTANDALONEEFFECTNODE::~ONESHOTSTANDALONEEFFECTNODE() { ReleaseDeathHolds(); }
void PERSISTENTUNITEFFECT::Clear() { active = false; spellID = 0; }

void AddUnitDeathHold() { ++s_activeAuras; }
void CheckReinitTimer() { if (!s_unitEffectsInitialized) UnitEffectsInitialize(); }
void CreateModel() { ++s_activeAuras; }
void DeathHoldEventTimerHandler() { s_activeAuras = 0; }
void DecorateEffectFilename() {}
void GetMissileTargetLocation() {}
void GetMissileTargetPosition() {}
void InitializeModel() { s_unitEffectsInitialized = true; }
void MoveMissile() { ++g_unitEffectUpdateCount; }
void OneShotEndHandler() { if (s_activeAuras != 0) --s_activeAuras; }
void PreloadModel() { s_unitEffectsInitialized = true; }
void PreloadModelsByKit() { s_unitEffectsInitialized = true; }
void PurgeTimerHandler() { s_activeMissiles = 0; }
void RenderMissiles() { g_unitEffectRenderCount += s_activeMissiles; }
void RenderModel() { ++g_unitEffectRenderCount; }
void SpellAnimEventCallback() { ++g_unitEffectUpdateCount; }
void SpellAreaAnimEventCallback() { ++g_unitEffectUpdateCount; }
void SpellCameraShakeCallback() { ++g_unitEffectRenderCount; }
void SpellSoundEffectCallback() { ++g_unitEffectRenderCount; }
void SpellUnitAnimEventCallback() { ++g_unitEffectUpdateCount; }
void UnitEffectAddMissile() { ++s_activeMissiles; }
void UnitEffectClear() { s_activeMissiles = 0; s_activeAuras = 0; }
void UnitEffectClearSpellPrecast() { s_spellPrecastActive = false; }
void UnitEffectCreateAuraModel() { ++s_activeAuras; }
void UnitEffectCreateWorldModelAura() { ++s_activeAuras; }
void UnitEffectGetLinkPointFromAttachment() {}
void UnitEffectGetSpecialVisual() {}
void UnitEffectIsAuraWorldObject() {}
void UnitEffectOneShot() { ++s_activeAuras; }
void UnitEffectPreloadSpellEffects() { s_unitEffectsInitialized = true; }
void UnitEffectUpdate() { ++g_unitEffectUpdateCount; }
void UnitEffectsInitialize() {
    s_unitEffectsInitialized = true;
    s_spellPrecastActive = false;
    s_activeMissiles = 0;
    s_activeAuras = 0;
    g_unitEffectUpdateCount = 0;
    g_unitEffectRenderCount = 0;
}
void UnitEffectsShutdown() {
    UnitEffectClear();
    s_unitEffectsInitialized = false;
}
