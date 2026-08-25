#pragma once

#include "Storm/h/stpl.h"

#include <Windows.h>
#include <objidl.h>
#include <cstdint>

struct ASYNCLOADER {
    void* asyncLoader = nullptr;
    TSGrowableArray<char> buffer;

    ASYNCLOADER();
    ~ASYNCLOADER();
    void Clear();
};

class CMyLoader;

class CMyIStream final : public IStream {
public:
    CMyIStream();
    ~CMyIStream();

    HRESULT Attach(const char* fileName, CMyLoader* loader);
    void Detach();
    HRESULT GetLoader(CMyLoader** loader);

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;
    HRESULT STDMETHODCALLTYPE Read(void* destination, ULONG bytes, ULONG* bytesRead) override;
    HRESULT STDMETHODCALLTYPE Write(const void*, ULONG, ULONG*) override;
    HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER move, DWORD origin, ULARGE_INTEGER* newPosition) override;
    HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER) override;
    HRESULT STDMETHODCALLTYPE CopyTo(IStream*, ULARGE_INTEGER, ULARGE_INTEGER*, ULARGE_INTEGER*) override;
    HRESULT STDMETHODCALLTYPE Commit(DWORD) override;
    HRESULT STDMETHODCALLTYPE Revert() override;
    HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) override;
    HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) override;
    HRESULT STDMETHODCALLTYPE Stat(STATSTG* status, DWORD flags) override;
    HRESULT STDMETHODCALLTYPE Clone(IStream** stream) override;

private:
    volatile LONG m_cRef;
    CMyLoader* m_loader;
    std::int64_t m_cursor;
    ASYNCLOADER* m_asyncLoader;
};

class CMyLoader final : public IUnknown {
public:
    CMyLoader();
    ~CMyLoader();

    HRESULT Init();
    HRESULT GetObjectA(const char* fileName, REFIID iid, void** object);
    HRESULT SetObject(void* object);
    HRESULT SetSearchDirectory(const wchar_t* path);
    HRESULT ScanDirectory(REFGUID objectClass, const wchar_t* extension, const wchar_t* scanName);
    HRESULT CacheObject(IUnknown* object);
    HRESULT ReleaseObject(IUnknown* object);
    HRESULT ClearCache(REFGUID objectClass);
    HRESULT EnableCache(REFGUID objectClass, BOOL enable);
    HRESULT EnumObject(REFGUID objectClass, DWORD index, void* objectDescription);

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** object) override;
    ULONG STDMETHODCALLTYPE AddRef() override;
    ULONG STDMETHODCALLTYPE Release() override;

    const wchar_t* SearchPath() const;

private:
    volatile LONG m_cRef;
    wchar_t m_wzSearchPath[MAX_PATH];
};

void InitLoader(ASYNCLOADER* loader, const char* fileName);
void MIDI_CleanupSegment();
void __stdcall PostLoadCallback(void* parameter);
