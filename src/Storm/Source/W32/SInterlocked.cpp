#include "Storm/h/storm.h"

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x002434F0
long __fastcall SInterlockedExchangePointer(long* exchange, long value) {
    return InterlockedExchange(reinterpret_cast<volatile long*>(exchange), value);
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243500
void* __fastcall SInterlockedCompareExchangePointer(void** exchange, void* value, void* comparand) {
    return InterlockedCompareExchangePointer(reinterpret_cast<void* volatile*>(exchange), value, comparand);
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243520
long __fastcall SInterlockedIncrement(long* value) {
    return InterlockedIncrement(reinterpret_cast<volatile long*>(value));
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243530
long __fastcall SInterlockedDecrement(long* value) {
    return InterlockedDecrement(reinterpret_cast<volatile long*>(value));
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243540
long __fastcall SInterlockedExchangeAdd(long* value, long delta) {
    return InterlockedExchangeAdd(reinterpret_cast<volatile long*>(value), delta);
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243550
long __fastcall SInterlockedExchangeSub(long* value, long delta) {
    return InterlockedExchangeAdd(reinterpret_cast<volatile long*>(value), -delta);
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243560
long __fastcall SInterlockedExchange(long* exchange, long value) {
    return InterlockedExchange(reinterpret_cast<volatile long*>(exchange), value);
}

// PDB source: D:\build\buildWoW\Storm\Source\W32\SInterlocked.cpp
// RVA: 0x00243570
long __fastcall SInterlockedCompareExchange(long* exchange, long value, long comparand) {
    return InterlockedCompareExchange(reinterpret_cast<volatile long*>(exchange), value, comparand);
}
