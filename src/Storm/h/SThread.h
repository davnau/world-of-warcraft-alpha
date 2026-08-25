#pragma once

#include "Storm/h/storm.h"

class CCritSect {
public:
    CRITICAL_SECTION m_critsect;
    CCritSect();
    ~CCritSect();
    void Enter();
    void Leave();
    int TryEnter();
};

struct CInitCritSect {
    long m_spinLock;
    CCritSect* m_critsect;
    std::uint8_t m_critsectData[24];
    int Enter();
    void Leave();
};

using SInitCritSect = CInitCritSect;

class CDebugSCritSect : public SCritSect {
public:
    std::uint8_t m_debugData[12];
    CDebugSCritSect();
    ~CDebugSCritSect();
    void Enter(const char* file = nullptr, unsigned int line = 0);
    int TryEnter(const char* file = nullptr, unsigned int line = 0);
    void Leave(const char* file = nullptr, unsigned int line = 0);
    static void DumpAllEntries();
};

class CSRWLock {
public:
    std::uint8_t m_opaqueData[12];
    CSRWLock();
    ~CSRWLock();
    void Enter(int forWriting);
    void Leave(int fromWriting);
    int TryEnter(int forWriting);
};

class CDebugSRWLock : public CSRWLock {
public:
    std::uint8_t m_debugData[12];
    CDebugSRWLock();
    ~CDebugSRWLock();
    void Enter(int forWriting, const char* file = nullptr, unsigned int line = 0);
    int TryEnter(int forWriting, const char* file = nullptr, unsigned int line = 0);
    void Leave(int fromWriting, const char* file = nullptr, unsigned int line = 0);
    static void DumpAllEntries();
};

class SSyncObject {
public:
    HANDLE m_handle;
    SSyncObject();
    SSyncObject(const SSyncObject& rhs);
    ~SSyncObject();
    SSyncObject& operator=(const SSyncObject& rhs);
    BOOL Valid() const;
    void Close();
    DWORD Wait(DWORD timeoutMs = INFINITE) const;
protected:
    void Copy(const SSyncObject& rhs);
};

class SEvent : public SSyncObject {
public:
    SEvent(BOOL manualReset = TRUE, BOOL initialValue = FALSE);
    BOOL Set();
    BOOL Reset();
};

class SThread : public SSyncObject {
public:
    BOOL Create(unsigned int (__stdcall* startAddress)(void*), void* parameter, char* threadName = nullptr);
};

class SSemaphore : public SSyncObject {
public:
    SSemaphore(LONG initialCount, LONG maximumCount);
    BOOL Signal(LONG count = 1);
};

class SMutex : public SSyncObject {
public:
    SMutex();
    SMutex(int initialOwner, const char* name);
    explicit SMutex(const char* name);
    void Create(BOOL initialOwner, const char* name);
    void Open(const char* name);
    BOOL Release();
};

DWORD __fastcall WaitMultiple(unsigned int count, const SSyncObject* objects, BOOL waitAll, DWORD timeoutMs);
DWORD __fastcall WaitMultiplePtr(unsigned int count, SSyncObject* const* objectPtrs, BOOL waitAll, DWORD timeoutMs);
void __fastcall SCreateProcess(char* applicationName, char* commandLine,
    void (__fastcall* callbackWhenProcessCompletes)(void*), void* callbackData);
DWORD __stdcall SGetCurrentThreadId();
int __fastcall SGetCurrentThreadPriority();
void SSetCurrentThreadPriority(int priority);
void* __fastcall SCreateThread(unsigned int (__stdcall* startAddress)(void*), void* parameter,
    unsigned int creationFlags, unsigned int* threadId, char* threadName);

#if defined(_M_IX86)
static_assert(sizeof(CCritSect) == 24);
static_assert(sizeof(CInitCritSect) == 32);
static_assert(sizeof(CDebugSCritSect) == 36);
static_assert(sizeof(CSRWLock) == 12);
static_assert(sizeof(CDebugSRWLock) == 24);
static_assert(sizeof(SSyncObject) == 4);
static_assert(sizeof(SEvent) == 4);
static_assert(sizeof(SThread) == 4);
static_assert(sizeof(SSemaphore) == 4);
static_assert(sizeof(SMutex) == 4);
#endif
