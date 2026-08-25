#include "Storm/h/SMem.h"

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <mutex>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

struct HeapRecord {
    HANDLE heap;
    bool owned;
    std::string filename;
    int lineNumber;
    unsigned int maximumSize;
};

struct Allocation {
    unsigned int bytes;
    unsigned int flags;
    HeapRecord* heap;
    std::string filename;
    int lineNumber;
};

std::recursive_mutex s_memoryLock;
HeapRecord s_defaultHeap{GetProcessHeap(), false, "Storm default heap", 0, 0};
std::vector<HeapRecord*> s_heaps{&s_defaultHeap};
std::unordered_map<void*, Allocation> s_allocations;
unsigned char s_debugFlags = 0;

HeapRecord* Heap(HSHEAP__* handle) {
    return handle ? reinterpret_cast<HeapRecord*>(handle) : &s_defaultHeap;
}
HSHEAP__* Public(HeapRecord* heap) { return reinterpret_cast<HSHEAP__*>(heap); }

void FillHeapDetails(HeapRecord* heap, _SMEMHEAPDETAILS* details) {
    if (!details) return;
    const unsigned int callerSize = details->size;
    _SMEMHEAPDETAILS value{};
    value.size = sizeof(value);
    value.handle = Public(heap);
    strcpy_s(value.filename, heap->filename.c_str());
    value.linenumber = heap->lineNumber;
    value.regions = 1;
    value.maximumsize = heap->maximumSize;
    for (const auto& [pointer, allocation] : s_allocations) {
        (void)pointer;
        if (allocation.heap == heap) {
            ++value.allocatedblocks;
            value.allocatedbytes += allocation.bytes;
        }
    }
    value.committedbytes = value.allocatedbytes;
    value.reservedbytes = value.allocatedbytes;
    std::memcpy(details, &value, std::min<std::size_t>(callerSize ? callerSize : sizeof(value), sizeof(value)));
}

} // namespace

void* __stdcall SMemAlloc(unsigned int bytes, const char* filename, int lineNumber, unsigned int flags) {
    void* pointer = SMemHeapAlloc(Public(&s_defaultHeap), flags, bytes);
    if (pointer) {
        std::lock_guard lock(s_memoryLock);
        auto& allocation = s_allocations.at(pointer);
        allocation.filename = filename ? filename : "";
        allocation.lineNumber = lineNumber;
    }
    return pointer;
}

int __stdcall SMemInitialize() {
    std::lock_guard lock(s_memoryLock);
    if (!s_defaultHeap.heap) s_defaultHeap.heap = GetProcessHeap();
    if (std::find(s_heaps.begin(), s_heaps.end(), &s_defaultHeap) == s_heaps.end()) s_heaps.push_back(&s_defaultHeap);
    return 1;
}

int __stdcall SMemDestroy() {
    std::lock_guard lock(s_memoryLock);
    for (auto& [pointer, allocation] : s_allocations) HeapFree(allocation.heap->heap, 0, pointer);
    s_allocations.clear();
    for (HeapRecord* heap : s_heaps) {
        if (heap != &s_defaultHeap) {
            if (heap->owned && heap->heap) HeapDestroy(heap->heap);
            delete heap;
        }
    }
    s_heaps.assign(1, &s_defaultHeap);
    return 1;
}

int __stdcall SMemDumpState(void (__stdcall* outputProc)(HOUTPUTCONTEXT__*, const char*), HOUTPUTCONTEXT__* outputContext) {
    if (!outputProc) return 0;
    std::lock_guard lock(s_memoryLock);
    char line[256]{};
    sprintf_s(line, "Storm heaps=%u allocations=%u\n", static_cast<unsigned int>(s_heaps.size()),
        static_cast<unsigned int>(s_allocations.size()));
    outputProc(outputContext, line);
    for (HeapRecord* heap : s_heaps) {
        _SMEMHEAPDETAILS details{sizeof(details)};
        FillHeapDetails(heap, &details);
        sprintf_s(line, "heap=%p blocks=%u bytes=%u caller=%s:%d\n", static_cast<void*>(heap),
            details.allocatedblocks, details.allocatedbytes, details.filename, details.linenumber);
        outputProc(outputContext, line);
    }
    return 1;
}

int __stdcall SMemDumpStateEx(char* destination) {
    if (!destination) return 0;
    FILE* output = nullptr;
    if (fopen_s(&output, destination, "wt") != 0 || !output) return 0;
    std::lock_guard lock(s_memoryLock);
    std::fprintf(output, "Storm heaps=%zu allocations=%zu\n", s_heaps.size(), s_allocations.size());
    for (HeapRecord* heap : s_heaps) {
        _SMEMHEAPDETAILS details{sizeof(details)};
        FillHeapDetails(heap, &details);
        std::fprintf(output, "heap=%p blocks=%u bytes=%u caller=%s:%d\n", static_cast<void*>(heap),
            details.allocatedblocks, details.allocatedbytes, details.filename, details.linenumber);
    }
    std::fclose(output);
    return 1;
}

int __stdcall SMemMarkAllHeapsEx(char*) { return 1; }

int __stdcall SMemFindNextBlock(HSHEAP__* handle, void* previousBlock, void** nextBlock, _SMEMBLOCKDETAILS* details) {
    if (!nextBlock) return 0;
    std::lock_guard lock(s_memoryLock);
    HeapRecord* heap = Heap(handle);
    bool takeNext = previousBlock == nullptr;
    for (const auto& [pointer, allocation] : s_allocations) {
        if (allocation.heap != heap) continue;
        if (takeNext) {
            *nextBlock = pointer;
            if (details) {
                const unsigned int callerSize = details->size;
                _SMEMBLOCKDETAILS value{sizeof(value), pointer, 1, 1, allocation.bytes, 0, allocation.flags};
                std::memcpy(details, &value, std::min<std::size_t>(callerSize ? callerSize : sizeof(value), sizeof(value)));
            }
            return 1;
        }
        if (pointer == previousBlock) takeNext = true;
    }
    *nextBlock = nullptr;
    return 0;
}

void __stdcall SMemHeapGetDetails(HSHEAP__* handle, _SMEMHEAPDETAILS* details) {
    std::lock_guard lock(s_memoryLock);
    FillHeapDetails(Heap(handle), details);
}

int __stdcall SMemFindNextHeap(HSHEAP__* previousHeap, HSHEAP__** nextHeap, _SMEMHEAPDETAILS* details) {
    if (!nextHeap) return 0;
    std::lock_guard lock(s_memoryLock);
    auto found = previousHeap ? std::find(s_heaps.begin(), s_heaps.end(), Heap(previousHeap)) : s_heaps.end();
    auto next = previousHeap ? (found == s_heaps.end() ? found : std::next(found)) : s_heaps.begin();
    if (next == s_heaps.end()) { *nextHeap = nullptr; return 0; }
    *nextHeap = Public(*next);
    FillHeapDetails(*next, details);
    return 1;
}
int __stdcall SMemFindNextHeap2(HSHEAP__* previousHeap, HSHEAP__** nextHeap, _SMEMHEAPDETAILS* details) {
    return SMemFindNextHeap(previousHeap, nextHeap, details);
}

int __stdcall SMemFree(void* ptr, const char*, int, unsigned int flags) {
    if (!ptr) return 1;
    std::lock_guard lock(s_memoryLock);
    const auto found = s_allocations.find(ptr);
    if (found == s_allocations.end()) return 0;
    const BOOL result = HeapFree(found->second.heap->heap, flags & HEAP_NO_SERIALIZE, ptr);
    if (result) s_allocations.erase(found);
    return result;
}

unsigned int __stdcall SMemGetAllocated(unsigned int* allocated, unsigned int* committed, unsigned int* reserved) {
    std::lock_guard lock(s_memoryLock);
    unsigned int total = 0;
    for (const auto& [pointer, allocation] : s_allocations) { (void)pointer; total += allocation.bytes; }
    if (allocated) *allocated = total;
    if (committed) *committed = total;
    if (reserved) *reserved = total;
    return total;
}

HSHEAP__* __stdcall SMemGetHeapByCaller(const char* filename, unsigned int lineNumber) {
    std::lock_guard lock(s_memoryLock);
    for (HeapRecord* heap : s_heaps) {
        if (heap->lineNumber == static_cast<int>(lineNumber) && heap->filename == (filename ? filename : "")) return Public(heap);
    }
    return Public(&s_defaultHeap);
}
HSHEAP__* __stdcall SMemGetHeapByPtr(void* ptr) {
    std::lock_guard lock(s_memoryLock);
    const auto found = s_allocations.find(ptr);
    return found == s_allocations.end() ? nullptr : Public(found->second.heap);
}
unsigned int __stdcall SMemGetSize(void* ptr, const char*, unsigned int) {
    std::lock_guard lock(s_memoryLock);
    const auto found = s_allocations.find(ptr);
    return found == s_allocations.end() ? UINT_MAX : found->second.bytes;
}

void* __stdcall SMemHeapAlloc(HSHEAP__* handle, unsigned int flags, unsigned int bytes) {
    std::lock_guard lock(s_memoryLock);
    HeapRecord* heap = Heap(handle);
    const DWORD nativeFlags = (flags & 8U ? HEAP_ZERO_MEMORY : 0U) | (flags & HEAP_NO_SERIALIZE);
    void* pointer = HeapAlloc(heap->heap, nativeFlags, bytes ? bytes : 1U);
    if (pointer) s_allocations.emplace(pointer, Allocation{bytes, flags, heap, {}, 0});
    return pointer;
}

HSHEAP__* __stdcall SMemHeapCreate(unsigned int options, SIZE_T initialSize, unsigned int maximumSize,
    const char* filename, int lineNumber) {
    HANDLE native = HeapCreate(options, initialSize, maximumSize);
    if (!native) return nullptr;
    auto heap = new (std::nothrow) HeapRecord{native, true, filename ? filename : "", lineNumber, maximumSize};
    if (!heap) { HeapDestroy(native); return nullptr; }
    std::lock_guard lock(s_memoryLock);
    s_heaps.push_back(heap);
    return Public(heap);
}

int __stdcall SMemHeapDestroy(HSHEAP__* handle) {
    HeapRecord* heap = Heap(handle);
    if (heap == &s_defaultHeap) return 0;
    std::lock_guard lock(s_memoryLock);
    for (auto it = s_allocations.begin(); it != s_allocations.end();) {
        if (it->second.heap == heap) { HeapFree(heap->heap, 0, it->first); it = s_allocations.erase(it); }
        else ++it;
    }
    const BOOL result = !heap->owned || HeapDestroy(heap->heap);
    s_heaps.erase(std::remove(s_heaps.begin(), s_heaps.end(), heap), s_heaps.end());
    delete heap;
    return result;
}
int __stdcall SMemHeapFree(HSHEAP__* handle, unsigned int flags, void* ptr) {
    std::lock_guard lock(s_memoryLock);
    const auto found = s_allocations.find(ptr);
    if (found == s_allocations.end() || found->second.heap != Heap(handle)) return 0;
    return SMemFree(ptr, nullptr, 0, flags);
}
void* __stdcall SMemHeapReAlloc(HSHEAP__* handle, unsigned int flags, void* ptr, unsigned int bytes) {
    std::lock_guard lock(s_memoryLock);
    HeapRecord* heap = Heap(handle);
    const auto found = s_allocations.find(ptr);
    if (found == s_allocations.end() || found->second.heap != heap) return nullptr;
    void* replacement = HeapReAlloc(heap->heap, flags & (HEAP_ZERO_MEMORY | HEAP_NO_SERIALIZE), ptr, bytes ? bytes : 1U);
    if (!replacement) return nullptr;
    Allocation allocation = found->second;
    allocation.bytes = bytes;
    s_allocations.erase(found);
    s_allocations.emplace(replacement, std::move(allocation));
    return replacement;
}
signed int __stdcall SMemHeapSize(HSHEAP__* handle, unsigned int, void* ptr) {
    std::lock_guard lock(s_memoryLock);
    const auto found = s_allocations.find(ptr);
    return found == s_allocations.end() || found->second.heap != Heap(handle) ? -1 : static_cast<int>(found->second.bytes);
}

BOOL __stdcall SMemIsValidPointer(void* address, UINT_PTR size, int) {
    if (!address) return FALSE;
    std::lock_guard lock(s_memoryLock);
    const auto target = reinterpret_cast<std::uintptr_t>(address);
    for (const auto& [pointer, allocation] : s_allocations) {
        const auto start = reinterpret_cast<std::uintptr_t>(pointer);
        if (target >= start && target + size >= target && target + size <= start + allocation.bytes) return TRUE;
    }
    return FALSE;
}
void* __stdcall SMemReAlloc(void* ptr, unsigned int bytes, const char*, int, unsigned int flags) {
    if (!ptr) return SMemAlloc(bytes, nullptr, 0, flags);
    std::lock_guard lock(s_memoryLock);
    const auto found = s_allocations.find(ptr);
    return found == s_allocations.end() ? nullptr : SMemHeapReAlloc(Public(found->second.heap), flags, ptr, bytes);
}
void __stdcall SMemSetDebugFlags(char flags, char changeMask) {
    s_debugFlags = static_cast<unsigned char>((s_debugFlags & ~changeMask) | (flags & changeMask));
}
void SMemTrace() {}
