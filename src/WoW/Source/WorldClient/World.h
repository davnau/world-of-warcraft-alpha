#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

class CWorld {
public:
    static void Initialize();
    static void Destroy();
    static bool LoadMap(const char* mapName, const NTempest::C3Vector& position,
                        int preLoad);
    static bool LoadMap(int mapID, const NTempest::C3Vector& position, int preLoad);
    static void UnloadMap();
    static void SetEnvironment();
    static void SelectLight(void* parameter, NTempest::C3Vector worldPosition,
                            const NTempest::C3Vector* cameraWorldPosition,
                            unsigned maxLightsToUse);
    static bool Intersect(const NTempest::C3Vector& start,
                          const NTempest::C3Vector& end,
                          NTempest::C3Vector* hitPoint = nullptr,
                          NTempest::C3Vector* hitNormal = nullptr,
                          float* hitFraction = nullptr,
                          bool includeTerrain = true,
                          bool includeObjects = true);
    static NTempest::C3Vector ResolvePlayerMovement(
        const NTempest::C3Vector& start,
        const NTempest::C3Vector& desired,
        float radius = 0.35F,
        float height = 1.8F);
    static float CalcAltitude(float x, float y, float radius);
    static void SetCameraPosition(const NTempest::C3Vector& position);
    static void RelocatePlayer(const NTempest::C3Vector& position,
                               float facing);
    static void SendPlayerMovementUpdate(unsigned opcode = 0x00E9U,
                                         unsigned moveFlags = 0U);
    static void BeginSpellCast(std::uint64_t casterGuid,
                               std::uint64_t targetGuid,
                               unsigned spellID,
                               unsigned castTimeMs);
    static void LaunchSpell(std::uint64_t casterGuid,
                            std::uint64_t targetGuid,
                            unsigned spellID);
    static void PlayMeleeAttack(std::uint64_t attackerGuid,
                                std::uint64_t victimGuid);
    static void NotifyPlayerDeath(std::uint64_t killerGuid);
    static bool IsPlayerDead();
    static void Jump(unsigned timestamp);
    static void UpdatePlayerMovement(float elapsedSec);
    static void Update(float elapsedSec);
    static void Render();
    static void RenderAlpha();
    static bool IsLoaded();
    static const char* GetMapName();
    static const NTempest::C3Vector& PlayerPosition();
};
