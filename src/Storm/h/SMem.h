#pragma once

#include "Storm/h/sapibase.h"

struct HOUTPUTCONTEXT__;

struct _SMEMBLOCKDETAILS {
    unsigned int size;
    void* ptr;
    int allocated;
    int valid;
    unsigned int bytes;
    unsigned int overhead;
    unsigned int flags;
};

struct _SMEMHEAPDETAILS {
    unsigned int size;
    HSHEAP__* handle;
    char filename[MAX_PATH];
    int linenumber;
    unsigned int regions;
    unsigned int committedbytes;
    unsigned int reservedbytes;
    unsigned int maximumsize;
    unsigned int allocatedblocks;
    unsigned int allocatedbytes;
};

void* __stdcall SMemAlloc(unsigned int bytes, const char* filename, int lineNumber, unsigned int flags);
int __stdcall SMemDestroy();
int __stdcall SMemDumpState(void (__stdcall* outputProc)(HOUTPUTCONTEXT__*, const char*), HOUTPUTCONTEXT__* outputContext);
int __stdcall SMemDumpStateEx(char* destination);
int __stdcall SMemMarkAllHeapsEx(char* marker);
int __stdcall SMemFindNextBlock(HSHEAP__* heap, void* previousBlock, void** nextBlock, _SMEMBLOCKDETAILS* details);
void __stdcall SMemHeapGetDetails(HSHEAP__* heap, _SMEMHEAPDETAILS* details);
int __stdcall SMemFindNextHeap(HSHEAP__* previousHeap, HSHEAP__** nextHeap, _SMEMHEAPDETAILS* details);
int __stdcall SMemFindNextHeap2(HSHEAP__* previousHeap, HSHEAP__** nextHeap, _SMEMHEAPDETAILS* details);
int __stdcall SMemFree(void* ptr, const char* filename, int lineNumber, unsigned int flags);
unsigned int __stdcall SMemGetAllocated(unsigned int* allocated, unsigned int* committed, unsigned int* reserved);
HSHEAP__* __stdcall SMemGetHeapByCaller(const char* filename, unsigned int lineNumber);
HSHEAP__* __stdcall SMemGetHeapByPtr(void* ptr);
unsigned int __stdcall SMemGetSize(void* ptr, const char* filename, unsigned int lineNumber);
void* __stdcall SMemHeapAlloc(HSHEAP__* handle, unsigned int flags, unsigned int bytes);
HSHEAP__* __stdcall SMemHeapCreate(unsigned int options, SIZE_T initialSize, unsigned int maximumSize,
    const char* filename, int lineNumber);
int __stdcall SMemHeapDestroy(HSHEAP__* handle);
int __stdcall SMemHeapFree(HSHEAP__* handle, unsigned int flags, void* ptr);
void* __stdcall SMemHeapReAlloc(HSHEAP__* handle, unsigned int flags, void* ptr, unsigned int bytes);
signed int __stdcall SMemHeapSize(HSHEAP__* handle, unsigned int flags, void* ptr);
int __stdcall SMemInitialize();
BOOL __stdcall SMemIsValidPointer(void* address, UINT_PTR size, int forWriting);
void* __stdcall SMemReAlloc(void* ptr, unsigned int bytes, const char* filename, int lineNumber, unsigned int flags);
void __stdcall SMemSetDebugFlags(char flags, char changeMask);
void SMemTrace();

#if defined(_M_IX86)
static_assert(sizeof(_SMEMBLOCKDETAILS) == 28);
static_assert(sizeof(_SMEMHEAPDETAILS) == 296);
#endif
