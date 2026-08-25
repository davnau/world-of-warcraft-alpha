#include "WoW/Source/Object/objectclient/corpse_c.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>

namespace {
float INFINITY_279 = 0.0F;
bool s_corpseStaticsInitialized = false;
}

int _E1_279();
int _E4_276();
int _E7_276();

int _E2_279() { return _E1_279(); }
int _E1_279() { INFINITY_279 = std::numeric_limits<float>::infinity(); return 0x7F800000; }
int _E5_276() { return _E4_276(); }
int _E4_276() { return 0; }
int _E8_276() { return _E7_276(); }
int _E7_276() { return 0; }

CGCorpse_C::CGCorpse_C(unsigned*, unsigned eventTime, CClientObjCreate*)
    : m_corpse(&m_data), m_eventTime(eventTime) {
    m_data = {};
    InitComponents();
}

CGCorpse_C::~CGCorpse_C() {
    m_model = nullptr;
    m_geosetHandle = nullptr;
    m_texComponent = nullptr;
    m_corpse = nullptr;
}

void CGCorpse_C::SetStorage(unsigned*) {
    if (m_corpse == nullptr) m_corpse = &m_data;
}

void CGCorpse_C::PostInit(const CClientObjCreate*) {
    AddComponents();
    InitPreferredGeosets();
    m_componentsInitialized = true;
}

void CGCorpse_C::Disable(int) { m_disabled = true; }
void CGCorpse_C::Reenable() { m_disabled = false; }

int CGCorpse_C::SetBlock(unsigned index, unsigned data) {
    if (index < m_baseBlocks.size()) {
        m_baseBlocks[index] = data;
        return 1;
    }
    const unsigned offset = index - static_cast<unsigned>(m_baseBlocks.size());
    if (offset >= m_heightBlocks.size()) return 0;
    m_heightBlocks[offset] = data;
    return 1;
}

void CGCorpse_C::SetData(const void* data, unsigned bytes) {
    if (data == nullptr || m_corpse == nullptr) return;
    const unsigned count = std::min<unsigned>(bytes, sizeof(CGCorpseData));
    std::memcpy(m_corpse, data, count);
}

unsigned CGCorpse_C::OffsetOf(int field) const {
    if (field == 0) return 0;
    if (field == 7) return 24;
    return std::numeric_limits<unsigned>::max();
}

const char* CGCorpse_C::GetModelFileName() const {
    static char filename[64]{};
    if (m_corpse == nullptr || m_corpse->displayID == 0) return "NoName";
    std::snprintf(filename, sizeof(filename), "Creature\\Display_%d.m2", m_corpse->displayID);
    return filename;
}

void CGCorpse_C::InitComponents() {
    m_componentsInitialized = true;
    m_model = reinterpret_cast<HMODEL__*>(static_cast<std::uintptr_t>(1));
    m_geosetHandle = reinterpret_cast<HCHARGEOSET__*>(static_cast<std::uintptr_t>(1));
    m_texComponent = reinterpret_cast<HTEXCOMPONENT__*>(static_cast<std::uintptr_t>(1));
    InitPreferredGeosets();
}

int CGCorpse_C::ShouldRender(unsigned worldStatus) {
    return !m_disabled && m_componentsInitialized && (worldStatus & 1U) != 0;
}

void CGCorpse_C::CommitTexture(int) { m_textureCommitted = m_texComponent != nullptr; }

void CGCorpse_C::InitPreferredGeosets() {
    m_preferredGeosets.fill(0);
    if (m_corpse != nullptr) {
        m_preferredGeosets[0] = static_cast<unsigned>(m_corpse->hairStyleID);
        m_preferredGeosets[1] = static_cast<unsigned>(m_corpse->facialHairStyleID);
    }
    m_preferredGeosets[7] = 2;
}

void CGCorpse_C::OnLeftClick() {}

void CGCorpse_C::OnRightClick() {
    // The network request is emitted by the owning object manager in the
    // restored client.  Keep the interaction observable here without
    // dereferencing an unavailable object-manager pointer.
    m_eventTime = m_eventTime + 1;
}

void CGCorpse_C::AddComponents() {
    if (m_corpse == nullptr) return;
    for (unsigned slot = 0; slot < m_corpse->equipment.size(); ++slot) {
        if (m_corpse->equipment[slot] != 0)
            AddComponent(static_cast<int>(m_corpse->equipment[slot]), 0, slot, 0);
    }
}

void CGCorpse_C::AddComponent(int displayID, unsigned, unsigned slot, int commit) {
    if (slot >= m_corpse->equipment.size()) return;
    m_corpse->equipment[slot] = static_cast<std::uint32_t>(displayID);
    if (commit != 0) CommitTexture(commit);
}

void CGCorpse_C::GetWorldMatrix(NTempest::C3Vector* worldMatrix) const {
    if (worldMatrix != nullptr && m_corpse != nullptr) *worldMatrix = m_corpse->position;
}

bool CGCorpse_C::IsUnderWater() const { return m_corpse != nullptr && m_corpse->position.z < 0.0F; }

void CGCorpse_C::OnDeathAnimEnd() { m_deathAnimationFinished = true; }

NTempest::C3Vector* CGCorpse_C::GetPosition(NTempest::C3Vector* result) {
    if (result != nullptr) *result = m_corpse != nullptr ? m_corpse->position : NTempest::C3Vector();
    return result;
}

double CGCorpse_C::GetFacing() const { return m_corpse != nullptr ? m_corpse->facing : 0.0; }
int CGCorpse_C::CanHighlight() const { return !m_disabled ? 1 : 0; }

int DrownAnimCallback(std::uint64_t*) { return 1; }
void DrownAnimCallback() {}
