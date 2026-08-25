#include "ENGINE/Source/Os/W32/ossoundmidi.h"
#include "ENGINE/Source/Os/W32/ossound.h"

#include <algorithm>
#include <cstring>
#include <cwchar>
#include <limits>
#include <string>

namespace {
constexpr const char* kMidiAlias = "WoWLegacyMidi";
bool s_midiInitialized = false;
bool s_midiPlaying = false;
bool s_midiLooped = false;
float s_midiVolume = 1.0f;
std::string s_midiFileName;
ASYNCLOADER s_MID;
ASYNCLOADER s_DLS;
CMyLoader* s_loader = nullptr;

std::string Quote(const char* text)
{
    std::string result = "\"";
    if (text) result += text;
    result += '"';
    return result;
}
}

ASYNCLOADER::ASYNCLOADER() : buffer(4096) {}
ASYNCLOADER::~ASYNCLOADER() { Clear(); }
void ASYNCLOADER::Clear()
{
    asyncLoader = nullptr;
    buffer.SetCount(0);
}

void InitLoader(ASYNCLOADER* loader, const char* fileName)
{
    if (!loader) return;
    loader->Clear();
    if (!fileName || !*fileName) return;

    HANDLE file = CreateFileA(fileName, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING,
                              FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN, nullptr);
    if (file == INVALID_HANDLE_VALUE) return;

    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file, &size) || size.QuadPart < 0 ||
        static_cast<unsigned long long>(size.QuadPart) > (std::numeric_limits<unsigned int>::max)() ||
        !loader->buffer.SetCount(static_cast<unsigned int>(size.QuadPart))) {
        CloseHandle(file);
        return;
    }

    DWORD total = 0;
    while (total < loader->buffer.Count()) {
        DWORD read = 0;
        const DWORD wanted = loader->buffer.Count() - total;
        if (!ReadFile(file, loader->buffer.Ptr() + total, wanted, &read, nullptr) || read == 0) break;
        total += read;
    }
    CloseHandle(file);
    if (total != loader->buffer.Count()) loader->Clear();
}

CMyIStream::CMyIStream() : m_cRef(1), m_loader(nullptr), m_cursor(0), m_asyncLoader(nullptr) {}
CMyIStream::~CMyIStream() { Detach(); }

HRESULT CMyIStream::Attach(const char* fileName, CMyLoader* loader)
{
    Detach();
    auto* data = new (std::nothrow) ASYNCLOADER;
    if (!data) return E_OUTOFMEMORY;
    InitLoader(data, fileName);
    if (!data->buffer.Count()) {
        delete data;
        return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
    }
    m_asyncLoader = data;
    m_loader = loader;
    if (m_loader) m_loader->AddRef();
    m_cursor = 0;
    return S_OK;
}

void CMyIStream::Detach()
{
    delete m_asyncLoader;
    m_asyncLoader = nullptr;
    if (m_loader) m_loader->Release();
    m_loader = nullptr;
    m_cursor = 0;
}

HRESULT CMyIStream::GetLoader(CMyLoader** loader)
{
    if (!loader) return E_POINTER;
    *loader = m_loader;
    if (*loader) (*loader)->AddRef();
    return *loader ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE CMyIStream::QueryInterface(REFIID iid, void** object)
{
    if (!object) return E_POINTER;
    *object = nullptr;
    if (iid == IID_IUnknown || iid == IID_ISequentialStream || iid == IID_IStream)
        *object = static_cast<IStream*>(this);
    if (!*object) return E_NOINTERFACE;
    AddRef();
    return S_OK;
}

ULONG STDMETHODCALLTYPE CMyIStream::AddRef() { return static_cast<ULONG>(InterlockedIncrement(&m_cRef)); }
ULONG STDMETHODCALLTYPE CMyIStream::Release()
{
    const LONG references = InterlockedDecrement(&m_cRef);
    if (!references) delete this;
    return static_cast<ULONG>(references);
}

HRESULT STDMETHODCALLTYPE CMyIStream::Read(void* destination, ULONG bytes, ULONG* bytesRead)
{
    if (bytesRead) *bytesRead = 0;
    if (!destination) return STG_E_INVALIDPOINTER;
    if (!m_asyncLoader || m_cursor < 0) return STG_E_REVERTED;
    const auto size = static_cast<std::int64_t>(m_asyncLoader->buffer.Count());
    const auto available = m_cursor < size ? size - m_cursor : 0;
    const ULONG count = static_cast<ULONG>((std::min)(available, static_cast<std::int64_t>(bytes)));
    if (count) std::memcpy(destination, m_asyncLoader->buffer.Ptr() + m_cursor, count);
    m_cursor += count;
    if (bytesRead) *bytesRead = count;
    return count == bytes ? S_OK : S_FALSE;
}

HRESULT STDMETHODCALLTYPE CMyIStream::Write(const void*, ULONG, ULONG* written)
{ if (written) *written = 0; return STG_E_ACCESSDENIED; }

HRESULT STDMETHODCALLTYPE CMyIStream::Seek(LARGE_INTEGER move, DWORD origin, ULARGE_INTEGER* position)
{
    if (!m_asyncLoader) return STG_E_REVERTED;
    std::int64_t base = 0;
    if (origin == STREAM_SEEK_CUR) base = m_cursor;
    else if (origin == STREAM_SEEK_END) base = m_asyncLoader->buffer.Count();
    else if (origin != STREAM_SEEK_SET) return STG_E_INVALIDFUNCTION;
    if ((move.QuadPart < 0 && base < -move.QuadPart) || move.QuadPart > (std::numeric_limits<std::int64_t>::max)() - base)
        return STG_E_INVALIDFUNCTION;
    const std::int64_t target = base + move.QuadPart;
    if (target < 0) return STG_E_INVALIDFUNCTION;
    m_cursor = target;
    if (position) position->QuadPart = static_cast<ULONGLONG>(m_cursor);
    return S_OK;
}

HRESULT STDMETHODCALLTYPE CMyIStream::SetSize(ULARGE_INTEGER) { return STG_E_ACCESSDENIED; }
HRESULT STDMETHODCALLTYPE CMyIStream::CopyTo(IStream* target, ULARGE_INTEGER bytes, ULARGE_INTEGER* read, ULARGE_INTEGER* written)
{
    if (read) read->QuadPart = 0;
    if (written) written->QuadPart = 0;
    if (!target) return STG_E_INVALIDPOINTER;
    char scratch[4096];
    ULONGLONG remaining = bytes.QuadPart;
    while (remaining) {
        const ULONG wanted = static_cast<ULONG>((std::min)(remaining, static_cast<ULONGLONG>(sizeof(scratch))));
        ULONG got = 0;
        const HRESULT readResult = Read(scratch, wanted, &got);
        if (!got) return readResult == S_FALSE ? S_OK : readResult;
        ULONG put = 0;
        const HRESULT writeResult = target->Write(scratch, got, &put);
        if (read) read->QuadPart += got;
        if (written) written->QuadPart += put;
        if (FAILED(writeResult) || put != got) return STG_E_MEDIUMFULL;
        remaining -= got;
    }
    return S_OK;
}
HRESULT STDMETHODCALLTYPE CMyIStream::Commit(DWORD) { return S_OK; }
HRESULT STDMETHODCALLTYPE CMyIStream::Revert() { return STG_E_REVERTED; }
HRESULT STDMETHODCALLTYPE CMyIStream::LockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return STG_E_INVALIDFUNCTION; }
HRESULT STDMETHODCALLTYPE CMyIStream::UnlockRegion(ULARGE_INTEGER, ULARGE_INTEGER, DWORD) { return STG_E_INVALIDFUNCTION; }
HRESULT STDMETHODCALLTYPE CMyIStream::Stat(STATSTG* status, DWORD flags)
{
    if (!status) return STG_E_INVALIDPOINTER;
    std::memset(status, 0, sizeof(*status));
    status->type = STGTY_STREAM;
    status->cbSize.QuadPart = m_asyncLoader ? m_asyncLoader->buffer.Count() : 0;
    status->grfMode = STGM_READ;
    if (!(flags & STATFLAG_NONAME)) status->pwcsName = nullptr;
    return S_OK;
}
HRESULT STDMETHODCALLTYPE CMyIStream::Clone(IStream** stream)
{
    if (!stream) return E_POINTER;
    *stream = nullptr;
    auto* clone = new (std::nothrow) CMyIStream;
    if (!clone) return E_OUTOFMEMORY;
    clone->m_asyncLoader = new (std::nothrow) ASYNCLOADER;
    if (!clone->m_asyncLoader || (m_asyncLoader && !clone->m_asyncLoader->buffer.Set(m_asyncLoader->buffer.Ptr(), m_asyncLoader->buffer.Count()))) {
        clone->Release();
        return E_OUTOFMEMORY;
    }
    clone->m_cursor = m_cursor;
    clone->m_loader = m_loader;
    if (clone->m_loader) clone->m_loader->AddRef();
    *stream = clone;
    return S_OK;
}

CMyLoader::CMyLoader() : m_cRef(1), m_wzSearchPath{} {}
CMyLoader::~CMyLoader() = default;
HRESULT CMyLoader::Init() { return S_OK; }
HRESULT CMyLoader::GetObjectA(const char* fileName, REFIID iid, void** object)
{
    if (!object) return E_POINTER;
    *object = nullptr;
    auto* stream = new (std::nothrow) CMyIStream;
    if (!stream) return E_OUTOFMEMORY;
    const HRESULT attached = stream->Attach(fileName, this);
    if (FAILED(attached)) { stream->Release(); return attached; }
    const HRESULT queried = stream->QueryInterface(iid, object);
    stream->Release();
    return queried;
}
HRESULT CMyLoader::SetObject(void*) { return E_NOTIMPL; }
HRESULT CMyLoader::SetSearchDirectory(const wchar_t* path)
{
    if (!path) return E_INVALIDARG;
    return wcscpy_s(m_wzSearchPath, path) == 0 ? S_OK : E_INVALIDARG;
}
HRESULT CMyLoader::ScanDirectory(REFGUID, const wchar_t*, const wchar_t*) { return E_NOTIMPL; }
HRESULT CMyLoader::CacheObject(IUnknown*) { return S_OK; }
HRESULT CMyLoader::ReleaseObject(IUnknown* object) { if (object) object->Release(); return S_OK; }
HRESULT CMyLoader::ClearCache(REFGUID) { return S_OK; }
HRESULT CMyLoader::EnableCache(REFGUID, BOOL) { return S_OK; }
HRESULT CMyLoader::EnumObject(REFGUID, DWORD, void*) { return S_FALSE; }
HRESULT STDMETHODCALLTYPE CMyLoader::QueryInterface(REFIID iid, void** object)
{
    if (!object) return E_POINTER;
    *object = iid == IID_IUnknown ? static_cast<IUnknown*>(this) : nullptr;
    if (!*object) return E_NOINTERFACE;
    AddRef();
    return S_OK;
}
ULONG STDMETHODCALLTYPE CMyLoader::AddRef() { return static_cast<ULONG>(InterlockedIncrement(&m_cRef)); }
ULONG STDMETHODCALLTYPE CMyLoader::Release()
{
    const LONG references = InterlockedDecrement(&m_cRef);
    if (!references) delete this;
    return static_cast<ULONG>(references);
}
const wchar_t* CMyLoader::SearchPath() const { return m_wzSearchPath; }

bool Sound::MIDI_Initialize()
{
    if (s_midiInitialized) return true;
    s_loader = new (std::nothrow) CMyLoader;
    if (!s_loader) return false;
    s_midiInitialized = SUCCEEDED(s_loader->Init());
    if (!s_midiInitialized) { s_loader->Release(); s_loader = nullptr; }
    return s_midiInitialized;
}

void Sound::MIDI_Shutdown()
{
    MIDI_Stop();
    s_MID.Clear();
    s_DLS.Clear();
    if (s_loader) s_loader->Release();
    s_loader = nullptr;
    s_midiInitialized = false;
}

bool Sound::MIDI_Play(const char* fileName, bool looped)
{
    if ((!s_midiInitialized && !MIDI_Initialize()) || !fileName || !*fileName) return false;
    MIDI_Stop();
    s_midiFileName = fileName;
    s_midiLooped = looped;
    const std::string open = "open " + Quote(fileName) + " type sequencer alias " + kMidiAlias;
    if (mciSendStringA(open.c_str(), nullptr, 0, nullptr) != 0) return false;
    MIDI_SetVolume(s_midiVolume);
    std::string play = std::string("play ") + kMidiAlias + " from 0";
    if (looped) play += " repeat";
    if (mciSendStringA(play.c_str(), nullptr, 0, nullptr) != 0) {
        mciSendStringA((std::string("close ") + kMidiAlias).c_str(), nullptr, 0, nullptr);
        return false;
    }
    s_midiPlaying = true;
    return true;
}

void Sound::MIDI_Stop()
{
    if (!s_midiPlaying && s_midiFileName.empty()) return;
    mciSendStringA((std::string("stop ") + kMidiAlias).c_str(), nullptr, 0, nullptr);
    mciSendStringA((std::string("close ") + kMidiAlias).c_str(), nullptr, 0, nullptr);
    s_midiPlaying = false;
    s_midiLooped = false;
    s_midiFileName.clear();
}

bool Sound::MIDI_Playing()
{
    if (!s_midiPlaying) return false;
    char mode[32]{};
    if (mciSendStringA((std::string("status ") + kMidiAlias + " mode").c_str(), mode, sizeof(mode), nullptr) != 0)
        return false;
    s_midiPlaying = _stricmp(mode, "playing") == 0;
    return s_midiPlaying;
}

void Sound::MIDI_SetVolume(float volume)
{
    s_midiVolume = (std::clamp)(volume, 0.0f, 1.0f);
    if (!s_midiPlaying) return;
    const int mciVolume = static_cast<int>(s_midiVolume * 1000.0f + 0.5f);
    const std::string command = std::string("setaudio ") + kMidiAlias + " volume to " + std::to_string(mciVolume);
    mciSendStringA(command.c_str(), nullptr, 0, nullptr);
}

void MIDI_CleanupSegment() { Sound::MIDI_Stop(); }
void __stdcall PostLoadCallback(void*) {}
