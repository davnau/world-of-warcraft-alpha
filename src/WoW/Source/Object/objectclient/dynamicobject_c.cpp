#include "WoW/Source/Object/objectclient/dynamicobject_c.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <limits>

namespace {
float INFINITY_280 = 0.0F;
}

int _E1_280();
int _E4_277();
int _E7_277();

int _E2_280() { return _E1_280(); }
int _E1_280() { INFINITY_280 = std::numeric_limits<float>::infinity(); return 0x7F800000; }
int _E5_277() { return _E4_277(); }
int _E4_277() { return 0; }
int _E8_277() { return _E7_277(); }
int _E7_277() { return 0; }

CGDynamicObject_C::CGDynamicObject_C(unsigned*, unsigned eventTime, CClientObjCreate*)
    : m_eventTime(eventTime) {}

CGDynamicObject_C::~CGDynamicObject_C() { ClearSound(); }

void CGDynamicObject_C::SetStorage(unsigned*) {}

void CGDynamicObject_C::PostInit(const CClientObjCreate*) {
    m_loaded = true;
    UpdateModelLoadStatus();
}

int CGDynamicObject_C::UpdateModelLoadStatus() {
    m_loaded = m_spellVisualID != 0 || m_position.x != 0.0F ||
               m_position.y != 0.0F || m_position.z != 0.0F;
    return m_loaded ? 1 : 0;
}

void CGDynamicObject_C::ObjectVisKitProc(int kitID) {
    m_spellVisualID = kitID;
    m_loaded = kitID != 0;
}

void CGDynamicObject_C::ClearSound() { m_soundActive = false; }
void CGDynamicObject_C::Disable(int) { m_disabled = true; ClearSound(); }
void CGDynamicObject_C::Reenable() { m_disabled = false; m_loaded = true; }

int CGDynamicObject_C::SetBlock(unsigned index, unsigned data) {
    if (index >= m_blocks.size()) return 0;
    m_blocks[index] = data;
    if (index == 0) m_spellVisualID = static_cast<int>(data);
    return 1;
}

void CGDynamicObject_C::SetData(const void* data, unsigned bytes) {
    if (data == nullptr) return;
    const unsigned count = std::min<unsigned>(bytes, sizeof(m_blocks));
    std::memcpy(m_blocks.data(), data, count);
    if (count >= sizeof(unsigned)) m_spellVisualID = static_cast<int>(m_blocks[0]);
}

unsigned CGDynamicObject_C::OffsetOf(int field) const {
    if (field == 0) return 0;
    if (field == 7) return 24;
    return std::numeric_limits<unsigned>::max();
}

const void* CGDynamicObject_C::GetVisualEffectNameRec() const {
    return m_spellVisualID == 0 ? nullptr : reinterpret_cast<const void*>(
        static_cast<std::uintptr_t>(static_cast<unsigned>(m_spellVisualID)));
}

const char* CGDynamicObject_C::GetModelFileName() const {
    static char filename[64]{};
    if (m_spellVisualID == 0) return "NoName";
    std::snprintf(filename, sizeof(filename), "Spells\\Visual_%d.m2", m_spellVisualID);
    return filename;
}

void CGDynamicObject_C::HandleAnimEvent(unsigned eventID) {
    m_lastAnimEvent = eventID;
    m_soundActive = eventID != 0;
}

void CGDynamicObject_C::AnimFinished() {
    m_lastAnimEvent = 0;
    m_loaded = false;
}

NTempest::C3Vector* CGDynamicObject_C::GetPosition(NTempest::C3Vector* result) {
    if (result != nullptr) *result = m_position;
    return result;
}

double CGDynamicObject_C::GetFacing() const { return m_facing; }
double CGDynamicObject_C::GetScale() const { return m_scale; }

int AnimEventCallback(std::uint64_t*) { return 1; }
int AnimFinishedCallback(std::uint64_t*) { return 1; }
