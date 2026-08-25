#pragma once

#include <cstdint>
#include <vector>

class CObjectHeap {
public:
    CObjectHeap() = default;
    CObjectHeap(const CObjectHeap&) = delete;
    CObjectHeap& operator=(const CObjectHeap&) = delete;
    CObjectHeap(CObjectHeap&& other) noexcept;
    CObjectHeap& operator=(CObjectHeap&& other) noexcept;
    ~CObjectHeap();

    int Allocate(unsigned objSize, unsigned heapObjects);
    int New(unsigned objSize, unsigned heapObjects, unsigned* index);
    void Delete(unsigned index, unsigned objSize, unsigned heapObjects);
    void* Ptr(unsigned index, unsigned objSize, unsigned heapObjects);

    void* m_obj = nullptr;
    unsigned* m_indexStack = nullptr;
    unsigned m_allocated = 0;
    unsigned m_bytes = 0;

private:
    std::vector<std::uint8_t> m_storage;
    std::vector<unsigned> m_freeIndices;
};

class CObjectHeapList {
public:
    int New(unsigned* index);
    void* Ptr(unsigned index);
    void Delete(unsigned index);
    int BlocksAllocated() const;

    std::vector<CObjectHeap> m_heaps;
    unsigned m_objSize = 0;
    unsigned m_objsPerBlock = 0;
    unsigned m_numFullHeaps = 0;
    char m_heapName[80]{};
};

void ObjectAllocInitialize(const char* previous);
int CCommand_HeapUsage();
unsigned __fastcall ObjectAllocAddHeap(unsigned objSize, unsigned objsPerBlock, const char* name);
int __fastcall ObjectAllocUsage(unsigned heapId);
int __fastcall ObjectAlloc(unsigned heapId, unsigned* handle);
void __fastcall ObjectFree(unsigned handle);
void* __fastcall ObjectPtr(unsigned handle);
void ObjectAllocDestroy(const char* firstHandle);

int _E16_9();
int _E13_2();
int _E15_6();
void _E14_7();
int _E21_18();
void* _E18_2();
int _E20_22();
void _E19_9();
int _E2_83();
int _E1_83();
int _E5_83();
void _E4_83();
int _E8_83();
void _E7_83();

