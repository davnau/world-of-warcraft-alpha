#include "WoW/Source/ObjectAlloc/objectalloc.h"

#include "WoW/Source/Console/consolecommand.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <new>

namespace {
std::vector<CObjectHeapList> s_heapLists;
bool s_initialized = false;
float INFINITY_83 = 0.0F;
float TWO_PI_83 = 0.0F;
float OO_TWO_PI_83 = 0.0F;
}

CObjectHeap::CObjectHeap(CObjectHeap&& other) noexcept
    : m_obj(other.m_obj), m_indexStack(other.m_indexStack), m_allocated(other.m_allocated),
      m_bytes(other.m_bytes), m_storage(std::move(other.m_storage)),
      m_freeIndices(std::move(other.m_freeIndices)) {
    other.m_obj = nullptr;
    other.m_indexStack = nullptr;
    other.m_allocated = 0;
    other.m_bytes = 0;
}

CObjectHeap& CObjectHeap::operator=(CObjectHeap&& other) noexcept {
    if (this == &other) return *this;
    this->~CObjectHeap();
    m_obj = other.m_obj;
    m_indexStack = other.m_indexStack;
    m_allocated = other.m_allocated;
    m_bytes = other.m_bytes;
    m_storage = std::move(other.m_storage);
    m_freeIndices = std::move(other.m_freeIndices);
    other.m_obj = nullptr;
    other.m_indexStack = nullptr;
    other.m_allocated = 0;
    other.m_bytes = 0;
    return *this;
}

CObjectHeap::~CObjectHeap() {
    m_obj = nullptr;
    m_indexStack = nullptr;
    m_allocated = 0;
    m_bytes = 0;
}

int CObjectHeap::Allocate(unsigned objSize, unsigned heapObjects) {
    if (objSize == 0 || heapObjects == 0) return 0;
    m_storage.assign(static_cast<std::size_t>(objSize) * heapObjects, 0);
    m_freeIndices.resize(heapObjects);
    for (unsigned i = 0; i < heapObjects; ++i) m_freeIndices[i] = i;
    m_obj = m_storage.data();
    m_indexStack = m_freeIndices.data();
    m_allocated = 0;
    m_bytes = objSize * heapObjects;
    return m_obj != nullptr ? 1 : 0;
}

int CObjectHeap::New(unsigned, unsigned heapObjects, unsigned* index) {
    if (!index || !m_obj || m_allocated >= heapObjects) return 0;
    *index = m_freeIndices[m_allocated++];
    return 1;
}

void CObjectHeap::Delete(unsigned index, unsigned, unsigned heapObjects) {
    if (!m_obj || index >= heapObjects || m_allocated == 0) return;
    auto it = std::find(m_freeIndices.begin(), m_freeIndices.begin() + m_allocated, index);
    if (it != m_freeIndices.begin() + m_allocated) return;
    m_freeIndices[--m_allocated] = index;
    m_indexStack = m_freeIndices.data();
}

void* CObjectHeap::Ptr(unsigned index, unsigned objSize, unsigned heapObjects) {
    if (!m_obj || index >= heapObjects) return nullptr;
    return static_cast<std::uint8_t*>(m_obj) + static_cast<std::size_t>(index) * objSize;
}

int CObjectHeapList::New(unsigned* index) {
    if (!index || m_objSize == 0 || m_objsPerBlock == 0) return 0;
    unsigned heap = 0;
    while (heap < m_heaps.size() && m_heaps[heap].m_allocated >= m_objsPerBlock) ++heap;
    if (heap == m_heaps.size()) {
        m_heaps.emplace_back();
        if (!m_heaps.back().Allocate(m_objSize, m_objsPerBlock)) return 0;
    }
    if (!m_heaps[heap].New(m_objSize, m_objsPerBlock, index)) return 0;
    if (m_heaps[heap].m_allocated == m_objsPerBlock) ++m_numFullHeaps;
    *index += heap * m_objsPerBlock;
    return 1;
}

void* CObjectHeapList::Ptr(unsigned index) {
    if (m_objsPerBlock == 0) return nullptr;
    const unsigned heap = index / m_objsPerBlock;
    const unsigned slot = index % m_objsPerBlock;
    if (heap >= m_heaps.size()) return nullptr;
    return m_heaps[heap].Ptr(slot, m_objSize, m_objsPerBlock);
}

void CObjectHeapList::Delete(unsigned index) {
    if (m_objsPerBlock == 0) return;
    const unsigned heap = index / m_objsPerBlock;
    const unsigned slot = index % m_objsPerBlock;
    if (heap >= m_heaps.size()) return;
    if (m_heaps[heap].m_allocated == m_objsPerBlock && m_numFullHeaps) --m_numFullHeaps;
    m_heaps[heap].Delete(slot, m_objSize, m_objsPerBlock);
}

int CObjectHeapList::BlocksAllocated() const {
    int result = 0;
    for (const auto& heap : m_heaps) result += static_cast<int>(heap.m_allocated);
    return result;
}

void ObjectAllocInitialize(const char* previous) {
    if (s_initialized) return;
    s_initialized = true;
    ConsoleCommandRegister(previous, "heapusage", CLIENT_COMMAND_CATEGORY_GAME, nullptr);
}

int CCommand_HeapUsage() {
    int blocks = 0;
    for (const auto& heap : s_heapLists) blocks += heap.BlocksAllocated();
    return blocks;
}

unsigned __fastcall ObjectAllocAddHeap(unsigned objSize, unsigned objsPerBlock, const char* name) {
    if (objSize == 0 || objsPerBlock == 0 || s_heapLists.size() >= 32) return ~0u;
    CObjectHeapList list;
    list.m_objSize = objSize;
    list.m_objsPerBlock = objsPerBlock;
    if (name) std::strncpy(list.m_heapName, name, sizeof(list.m_heapName) - 1);
    s_heapLists.push_back(std::move(list));
    return static_cast<unsigned>(s_heapLists.size() - 1);
}

int __fastcall ObjectAllocUsage(unsigned heapId) {
    return heapId < s_heapLists.size() ? s_heapLists[heapId].BlocksAllocated() : 0;
}

int __fastcall ObjectAlloc(unsigned heapId, unsigned* handle) {
    if (!handle || heapId >= s_heapLists.size()) return 0;
    unsigned index = 0;
    if (!s_heapLists[heapId].New(&index)) return 0;
    *handle = (heapId << 27) | (index & 0x07FFFFFFu);
    return 1;
}

void __fastcall ObjectFree(unsigned handle) {
    const unsigned heapId = handle >> 27;
    if (heapId < s_heapLists.size()) s_heapLists[heapId].Delete(handle & 0x07FFFFFFu);
}

void* __fastcall ObjectPtr(unsigned handle) {
    const unsigned heapId = handle >> 27;
    return heapId < s_heapLists.size() ? s_heapLists[heapId].Ptr(handle & 0x07FFFFFFu) : nullptr;
}

void ObjectAllocDestroy(const char* firstHandle) {
    if (firstHandle) ConsoleCommandUnregister(firstHandle);
    s_heapLists.clear();
    s_initialized = false;
}

int _E16_9() { _E13_2(); return _E15_6(); }
int _E13_2() { return 0; }
int _E15_6() { return 0; }
void _E14_7() { s_heapLists.clear(); }
int _E21_18() { _E18_2(); return _E20_22(); }
void* _E18_2() { return nullptr; }
int _E20_22() { return 0; }
void _E19_9() {}
int _E2_83() { return _E1_83(); }
int _E1_83() { INFINITY_83 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_83() { _E4_83(); return 0; }
void _E4_83() { TWO_PI_83 = 3.1415927F + 3.1415927F; }
int _E8_83() { _E7_83(); return 0; }
void _E7_83() { OO_TWO_PI_83 = 1.0F / TWO_PI_83; }

