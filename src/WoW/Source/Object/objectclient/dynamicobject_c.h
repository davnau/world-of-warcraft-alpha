#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <array>
#include <cstdint>

struct CClientObjCreate;

class CGDynamicObject_C {
public:
    CGDynamicObject_C(unsigned* storage = nullptr, unsigned eventTime = 0, CClientObjCreate* init = nullptr);
    ~CGDynamicObject_C();
    void SetStorage(unsigned* storage);
    void PostInit(const CClientObjCreate* init = nullptr);
    int UpdateModelLoadStatus();
    void ObjectVisKitProc(int kitID = 0);
    void ClearSound();
    void Disable(int shutdown = 0);
    void Reenable();
    int SetBlock(unsigned index, unsigned data);
    void SetData(const void* data, unsigned bytes);
    unsigned OffsetOf(int field) const;
    const void* GetVisualEffectNameRec() const;
    const char* GetModelFileName() const;
    void HandleAnimEvent(unsigned eventID = 0);
    void AnimFinished();
    NTempest::C3Vector* GetPosition(NTempest::C3Vector* result);
    double GetFacing() const;
    double GetScale() const;

    std::uint64_t m_guid = 0;
    int m_spellVisualID = 0;
    NTempest::C3Vector m_position{};
    float m_facing = 0.0F;
    float m_scale = 1.0F;
    std::array<unsigned, 6> m_blocks{};
    bool m_disabled = false;
    bool m_loaded = false;
    bool m_soundActive = false;
    unsigned m_lastAnimEvent = 0;
    unsigned m_eventTime = 0;
};

int AnimEventCallback(std::uint64_t* guid);
int AnimFinishedCallback(std::uint64_t* guid);
