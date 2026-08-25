#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <array>
#include <cstdint>

struct CClientObjCreate;
struct HCHARGEOSET__;
struct HMODEL__;
struct HTEXCOMPONENT__;
struct HOBJECT__;

struct CGCorpseData {
    std::uint64_t owner = 0;
    int displayID = 0;
    int raceID = 0;
    int sex = 0;
    int hairStyleID = 0;
    int facialHairStyleID = 0;
    NTempest::C3Vector position{};
    float facing = 0.0F;
    std::array<std::uint32_t, 16> equipment{};
};

class CGCorpse_C {
public:
    CGCorpse_C(unsigned* storage = nullptr, unsigned eventTime = 0, CClientObjCreate* init = nullptr);
    ~CGCorpse_C();
    void SetStorage(unsigned* storage);
    void PostInit(const CClientObjCreate* init = nullptr);
    void Disable(int shutdown = 0);
    void Reenable();
    int SetBlock(unsigned index, unsigned data);
    void SetData(const void* data, unsigned bytes);
    unsigned OffsetOf(int field) const;
    const char* GetModelFileName() const;
    void InitComponents();
    int ShouldRender(unsigned worldStatus = 1);
    void CommitTexture(int force = 0);
    void InitPreferredGeosets();
    void OnLeftClick();
    void OnRightClick();
    void AddComponents();
    void AddComponent(int displayID, unsigned inventoryType, unsigned slot, int commit);
    void GetWorldMatrix(NTempest::C3Vector* worldMatrix) const;
    bool IsUnderWater() const;
    void OnDeathAnimEnd();
    NTempest::C3Vector* GetPosition(NTempest::C3Vector* result);
    double GetFacing() const;
    int CanHighlight() const;

    CGCorpseData* m_corpse = nullptr;
    HMODEL__* m_model = nullptr;
    HCHARGEOSET__* m_geosetHandle = nullptr;
    HTEXCOMPONENT__* m_texComponent = nullptr;
    std::array<unsigned, 8> m_preferredGeosets{};
    std::array<unsigned, 6> m_baseBlocks{};
    std::array<unsigned, 36> m_heightBlocks{};
    bool m_disabled = false;
    bool m_componentsInitialized = false;
    bool m_textureCommitted = false;
    bool m_deathAnimationFinished = false;
    unsigned m_eventTime = 0;
    CGCorpseData m_data{};
};

int DrownAnimCallback(std::uint64_t* guid);
void DrownAnimCallback();
