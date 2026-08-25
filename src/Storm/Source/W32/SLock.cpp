#include "Storm/h/SThread.h"

#include <algorithm>
#include <array>
#include <cstring>

namespace {
CRITICAL_SECTION* Native(SCritSect* lock) { return reinterpret_cast<CRITICAL_SECTION*>(lock->m_opaqueData); }
const CRITICAL_SECTION* Native(const SCritSect* lock) { return reinterpret_cast<const CRITICAL_SECTION*>(lock->m_opaqueData); }
SRWLOCK* Native(CSRWLock* lock) { return reinterpret_cast<SRWLOCK*>(lock->m_opaqueData); }
} // namespace

SCritSect::SCritSect() : m_opaqueData{} { InitializeCriticalSection(Native(this)); }
SCritSect::~SCritSect() { DeleteCriticalSection(Native(this)); }
void SCritSect::Enter() { EnterCriticalSection(Native(this)); }
void SCritSect::Leave() { LeaveCriticalSection(Native(this)); }
int SCritSect::TryEnter() { return TryEnterCriticalSection(Native(this)); }

CCritSect::CCritSect() : m_critsect{} { InitializeCriticalSection(&m_critsect); }
CCritSect::~CCritSect() { DeleteCriticalSection(&m_critsect); }
void CCritSect::Enter() { EnterCriticalSection(&m_critsect); }
void CCritSect::Leave() { LeaveCriticalSection(&m_critsect); }
int CCritSect::TryEnter() { return TryEnterCriticalSection(&m_critsect); }

int CInitCritSect::Enter() {
    int initialized = 0;
    if (!m_critsect) {
        while (InterlockedExchange(&m_spinLock, 1) != 0) YieldProcessor();
        if (!m_critsect) {
            m_critsect = new (m_critsectData) CCritSect;
            initialized = 1;
        }
        InterlockedExchange(&m_spinLock, 0);
    }
    m_critsect->Enter();
    return initialized;
}
void CInitCritSect::Leave() { if (m_critsect) m_critsect->Leave(); }

CDebugSCritSect::CDebugSCritSect() : SCritSect(), m_debugData{} {}
CDebugSCritSect::~CDebugSCritSect() = default;
void CDebugSCritSect::Enter(const char*, unsigned int) { SCritSect::Enter(); }
int CDebugSCritSect::TryEnter(const char*, unsigned int) { return SCritSect::TryEnter(); }
void CDebugSCritSect::Leave(const char*, unsigned int) { SCritSect::Leave(); }
void CDebugSCritSect::DumpAllEntries() {}

CSRWLock::CSRWLock() : m_opaqueData{} { InitializeSRWLock(Native(this)); }
CSRWLock::~CSRWLock() = default;
void CSRWLock::Enter(int forWriting) {
    if (forWriting) AcquireSRWLockExclusive(Native(this));
    else AcquireSRWLockShared(Native(this));
}
void CSRWLock::Leave(int fromWriting) {
    if (fromWriting) ReleaseSRWLockExclusive(Native(this));
    else ReleaseSRWLockShared(Native(this));
}
int CSRWLock::TryEnter(int forWriting) {
    return forWriting ? TryAcquireSRWLockExclusive(Native(this)) : TryAcquireSRWLockShared(Native(this));
}

CDebugSRWLock::CDebugSRWLock() : CSRWLock(), m_debugData{} {}
CDebugSRWLock::~CDebugSRWLock() = default;
void CDebugSRWLock::Enter(int forWriting, const char*, unsigned int) { CSRWLock::Enter(forWriting); }
int CDebugSRWLock::TryEnter(int forWriting, const char*, unsigned int) { return CSRWLock::TryEnter(forWriting); }
void CDebugSRWLock::Leave(int fromWriting, const char*, unsigned int) { CSRWLock::Leave(fromWriting); }
void CDebugSRWLock::DumpAllEntries() {}

SSyncObject::SSyncObject() : m_handle(nullptr) {}
SSyncObject::SSyncObject(const SSyncObject& rhs) : m_handle(nullptr) { Copy(rhs); }
SSyncObject::~SSyncObject() { Close(); }
SSyncObject& SSyncObject::operator=(const SSyncObject& rhs) {
    if (this != &rhs) { Close(); Copy(rhs); }
    return *this;
}
void SSyncObject::Copy(const SSyncObject& rhs) {
    if (!rhs.m_handle) return;
    DuplicateHandle(GetCurrentProcess(), rhs.m_handle, GetCurrentProcess(), &m_handle, 0, FALSE, DUPLICATE_SAME_ACCESS);
}
BOOL SSyncObject::Valid() const { return m_handle != nullptr; }
void SSyncObject::Close() { if (m_handle) { CloseHandle(m_handle); m_handle = nullptr; } }
DWORD SSyncObject::Wait(DWORD timeoutMs) const { return m_handle ? WaitForSingleObject(m_handle, timeoutMs) : WAIT_FAILED; }

DWORD __fastcall WaitMultiple(unsigned int count, const SSyncObject* objects, BOOL waitAll, DWORD timeoutMs) {
    if (!objects || count == 0 || count > MAXIMUM_WAIT_OBJECTS) return WAIT_FAILED;
    std::array<HANDLE, MAXIMUM_WAIT_OBJECTS> handles{};
    for (unsigned int index = 0; index < count; ++index) handles[index] = objects[index].m_handle;
    return WaitForMultipleObjects(count, handles.data(), waitAll, timeoutMs);
}
DWORD __fastcall WaitMultiplePtr(unsigned int count, SSyncObject* const* objectPtrs, BOOL waitAll, DWORD timeoutMs) {
    if (!objectPtrs || count == 0 || count > MAXIMUM_WAIT_OBJECTS) return WAIT_FAILED;
    std::array<HANDLE, MAXIMUM_WAIT_OBJECTS> handles{};
    DWORD valid = 0;
    for (unsigned int index = 0; index < count; ++index) {
        if (objectPtrs[index] && objectPtrs[index]->m_handle) handles[valid++] = objectPtrs[index]->m_handle;
    }
    return valid ? WaitForMultipleObjects(valid, handles.data(), waitAll, timeoutMs) : WAIT_FAILED;
}

SEvent::SEvent(BOOL manualReset, BOOL initialValue) {
    m_handle = CreateEventA(nullptr, manualReset, initialValue, nullptr);
}
BOOL SEvent::Set() { return m_handle && SetEvent(m_handle); }
BOOL SEvent::Reset() { return m_handle && ResetEvent(m_handle); }

BOOL SThread::Create(unsigned int (__stdcall* startAddress)(void*), void* parameter, char* threadName) {
    Close();
    unsigned int id = 0;
    m_handle = static_cast<HANDLE>(SCreateThread(startAddress, parameter, 0, &id, threadName));
    return m_handle != nullptr;
}

SSemaphore::SSemaphore(LONG initialCount, LONG maximumCount) {
    m_handle = CreateSemaphoreA(nullptr, initialCount, maximumCount, nullptr);
}
BOOL SSemaphore::Signal(LONG count) { return m_handle && ReleaseSemaphore(m_handle, count, nullptr); }

SMutex::SMutex() { Create(FALSE, nullptr); }
SMutex::SMutex(int initialOwner, const char* name) { Create(initialOwner, name); }
SMutex::SMutex(const char* name) { Open(name); }
void SMutex::Create(BOOL initialOwner, const char* name) {
    Close();
    m_handle = CreateMutexA(nullptr, initialOwner, name);
}
void SMutex::Open(const char* name) {
    Close();
    m_handle = OpenMutexA(MUTEX_ALL_ACCESS, TRUE, name);
}
BOOL SMutex::Release() { return m_handle && ReleaseMutex(m_handle); }
