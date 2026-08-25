#pragma once

#include "WoW/Source/Object/objectclient/bag_c.h"
#include "WoW/Source/Object/objectclient/item_c.h"

#include <array>
#include <cstdint>

class CGContainer_C {
public:
    explicit CGContainer_C(unsigned int* storage = nullptr);
    void Disable(int shutdown = 0);
    CGBag_C* GetBag();
    double GetCloseXOffset() const;
    double GetCloseYOffset() const;
    unsigned int GetHeight() const;
    double GetSlotXOffset() const;
    double GetSlotYOffset() const;
    unsigned int GetWidth() const;
    unsigned int OffsetOf(int field) const;
    void Reenable();
    int SetBlock(unsigned int index, unsigned int data);
    void SetData(const void* data, unsigned int bytes);
    void SetStorage(unsigned int* storage);
    ~CGContainer_C();

private:
    CGItem_C m_item;
    CGBag_C m_bag;
    std::array<std::uint8_t, 168> m_data{};
};

// Recovered symbol declarations for objectclient/container_c.cpp
