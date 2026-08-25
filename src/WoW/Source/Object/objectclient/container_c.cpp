#include "WoW/Source/Object/objectclient/container_c.h"

#include <algorithm>
#include <cstring>

CGContainer_C::CGContainer_C(unsigned int* storage) {
    SetStorage(storage);
}

void CGContainer_C::Disable(int) { m_item.Disable(); }

CGBag_C* CGContainer_C::GetBag() { return &m_bag; }

double CGContainer_C::GetCloseXOffset() const { return 0.0; }
double CGContainer_C::GetCloseYOffset() const { return 0.037; }
double CGContainer_C::GetSlotXOffset() const { return 0.025; }
double CGContainer_C::GetSlotYOffset() const { return -0.035; }

unsigned int CGContainer_C::GetWidth() const { return m_bag.GetWidth(); }
unsigned int CGContainer_C::GetHeight() const { return m_bag.GetHeight(); }

void CGContainer_C::Reenable() {
    m_item.Reenable();
    // The recovered client re-enqueues every non-empty slot with the object
    // manager.  The local reconstruction keeps the same traversal and leaves
    // manager notification to the object-manager integration layer.
    for (CGItem_C* item : m_bag.Slots()) {
        if (item && item->IsDisabled()) item->Reenable();
    }
}

int CGContainer_C::SetBlock(unsigned int index, unsigned int data) {
    if (index >= m_data.size() / sizeof(unsigned int)) return 1;
    std::memcpy(m_data.data() + index * sizeof(unsigned int), &data,
                sizeof(data));
    return 1;
}

void CGContainer_C::SetData(const void* data, unsigned int bytes) {
    if (!data) return;
    const unsigned int count = std::min(bytes, static_cast<unsigned>(m_data.size()));
    std::memcpy(m_data.data(), data, count);
    if (count >= sizeof(unsigned int)) {
        unsigned int slotCount = 0;
        std::memcpy(&slotCount, m_data.data(), sizeof(slotCount));
        m_bag.Slots().resize(std::min(slotCount, 20u));
    }
}

void CGContainer_C::SetStorage(unsigned int* storage) {
    if (!storage) return;
    SetData(storage, static_cast<unsigned>(m_data.size()));
}

unsigned int CGContainer_C::OffsetOf(int field) const {
    if (field == 0) return 0;
    if (field == 1) return 24;
    if (field == 2) return 144;
    return static_cast<unsigned>(-1);
}

CGContainer_C::~CGContainer_C() = default;
