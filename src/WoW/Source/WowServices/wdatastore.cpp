#include "WoW/Source/WowServices/wdatastore.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>

namespace {
bool s_heapsInitialized = false;
float INFINITY_260 = 0.0F;
float TWO_PI_257 = 0.0F;
float OO_TWO_PI_257 = 0.0F;

unsigned RoundBufferSize(unsigned bytes)
{
    return (bytes + 255u) & ~255u;
}
}

void WDataStore::StaticInitialize()
{
    s_heapsInitialized = true;
}

void WDataStore::StaticDestroy()
{
    s_heapsInitialized = false;
}

void WDataStore::InternalInitialize(std::uint8_t** data, unsigned* base, unsigned* alloc)
{
    m_bufferObj = nullptr;
    if (data) *data = nullptr;
    if (base) *base = 0;
    if (alloc) *alloc = 0;
}

void WDataStore::InternalDestroy(std::uint8_t** data, unsigned* base, unsigned* alloc)
{
    if (m_bufferObj) {
        ::operator delete(m_bufferObj);
        m_bufferObj = nullptr;
    } else if (data && *data) {
        ::operator delete(*data);
    }
    if (data) *data = nullptr;
    if (base) *base = 0;
    if (alloc) *alloc = 0;
}

int WDataStore::InternalFetchWrite(unsigned pos, unsigned bytes, std::uint8_t** data,
                                   unsigned* base, unsigned* alloc,
                                   const char*, int)
{
    if (!data || !base || !alloc) return 0;
    if (!s_heapsInitialized) StaticInitialize();
    const unsigned required = RoundBufferSize(pos + bytes);
    if (required < *alloc) return 1;

    std::uint8_t* oldData = *data;
    const unsigned oldSize = *alloc;
    if (required > 0x4000u) {
        auto* replacement = static_cast<std::uint8_t*>(::operator new(required, std::nothrow));
        if (!replacement) return 0;
        if (oldData && oldSize) std::memcpy(replacement, oldData, oldSize);
        if (m_bufferObj) ::operator delete(m_bufferObj);
        else ::operator delete(oldData);
        m_bufferObj = nullptr;
        *data = replacement;
        *alloc = required;
        return 1;
    }

    const unsigned poolSize = required > 0x300u ? 0x4000u : 0x300u;
    auto* block = static_cast<std::uint8_t*>(::operator new(sizeof(unsigned) + poolSize, std::nothrow));
    if (!block) return 0;
    *reinterpret_cast<unsigned*>(block) = poolSize;
    auto* replacement = block + sizeof(unsigned);
    if (oldData && oldSize) std::memcpy(replacement, oldData, oldSize);
    if (m_bufferObj) ::operator delete(m_bufferObj);
    else ::operator delete(oldData);
    m_bufferObj = block;
    *data = replacement;
    *alloc = poolSize;
    return 1;
}

int WDataStore::AllocBuffer(unsigned bytes)
{
    if (!s_heapsInitialized) StaticInitialize();
    if (bytes > 0x4000u)
        return reinterpret_cast<int>(::operator new(bytes, std::nothrow));
    auto* block = static_cast<std::uint8_t*>(::operator new(sizeof(unsigned) + (bytes > 0x300u ? 0x4000u : 0x300u), std::nothrow));
    if (!block) return 4;
    *reinterpret_cast<unsigned*>(block) = bytes > 0x300u ? 0x4000u : 0x300u;
    return reinterpret_cast<int>(block + sizeof(unsigned));
}

void WDataStore::FreeBuffer(void* data, unsigned bytes)
{
    if (!data) return;
    if (bytes > 0x4000u)
        ::operator delete(data);
    else
        ::operator delete(static_cast<std::uint8_t*>(data) - sizeof(unsigned));
}

int _E2_260() { return _E1_260(); }
int _E1_260() { INFINITY_260 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_257() { _E4_257(); return 0; }
void _E4_257() { TWO_PI_257 = 3.1415927F + 3.1415927F; }
int _E8_257() { _E7_257(); return 0; }
void _E7_257() { OO_TWO_PI_257 = 1.0F / TWO_PI_257; }

