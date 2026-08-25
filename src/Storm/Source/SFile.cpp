#include "Storm/h/SFile.h"
#include "Storm/h/SZip.h"

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <io.h>
#include <mutex>
#include <string>
#include <unordered_map>

namespace {

struct ArchiveMetadata {
    std::string name;
    int priority;
    bool enabled;
};

std::mutex s_apiLock;
std::unordered_map<SArchive*, ArchiveMetadata> s_archiveMetadata;
SFILE_LOAD_NOTIFY s_loadNotify = nullptr;
void* s_loadNotifyData = nullptr;
SFILE_IO_ERROR_PROC s_errorProc = nullptr;
unsigned int s_errorMode = 0;
unsigned int s_asyncBudget = 0;
unsigned int s_dataChunkSize = 0x10000;
unsigned short s_locale = 0;
unsigned char s_platform = 0;
bool s_seekOptimization = true;

SArchive* Archive(HSARCHIVE__* handle) { return reinterpret_cast<SArchive*>(handle); }
SFile* File(HSFILE__* handle) { return reinterpret_cast<SFile*>(handle); }
HSARCHIVE__* Public(SArchive* archive) { return reinterpret_cast<HSARCHIVE__*>(archive); }
HSFILE__* Public(SFile* file) { return reinterpret_cast<HSFILE__*>(file); }

bool CopyString(char* buffer, unsigned int bufferChars, const char* source) {
    if (!buffer || !bufferChars || !source) return false;
    return strcpy_s(buffer, bufferChars, source) == 0;
}

} // namespace

int __stdcall SFileAuthenticateArchive(HSARCHIVE__* handle, unsigned int* extendedResult) {
    if (extendedResult) *extendedResult = handle ? 1U : 0U;
    SetLastError(ERROR_NOT_SUPPORTED);
    return 0;
}

BOOL __stdcall SFileAuthenticateArchiveEx(HSARCHIVE__* handle, unsigned int* extendedResult,
    unsigned char*, LONG, const unsigned char*, unsigned int) {
    return SFileAuthenticateArchive(handle, extendedResult) ? TRUE : FALSE;
}

unsigned int __stdcall SFileCalcFileCrc(const char* filename) {
    if (!filename) return 0;
    SFile* file = nullptr;
    if (!SFile::Open(filename, &file)) return 0;
    unsigned long crc = 0;
    CrcBuffer(nullptr, 0, &crc, 1U);
    unsigned char data[32768];
    unsigned int actual = 0;
    do {
        if (!SFile::Read(file, data, sizeof(data), &actual, nullptr, nullptr)) { crc = 0; break; }
        if (actual) CrcBuffer(data, actual, &crc, 2U);
    } while (actual == sizeof(data));
    SFile::Close(file);
    return static_cast<unsigned int>(CrcBuffer(nullptr, 0, &crc, 4U));
}

int __stdcall SFileCancelRequestEx(IDirectSoundBuffer*) { return 0; }
int __stdcall SFileCloseArchive(HSARCHIVE__* handle) {
    SArchive* archive = Archive(handle);
    if (!archive) return 0;
    { std::lock_guard lock(s_apiLock); s_archiveMetadata.erase(archive); }
    return SFile::CloseArchive(archive);
}
int __stdcall SFileCloseFile(HSFILE__* handle) { return SFile::Close(File(handle)); }

void __stdcall SFileRegisterLoadNotifyProc(SFILE_LOAD_NOTIFY callback, void* opaqueData) {
    s_loadNotify = callback;
    s_loadNotifyData = opaqueData;
}

int __stdcall SFileDdaBegin(HSFILE__* handle, unsigned int, unsigned int) { return handle ? 1 : 0; }
int __stdcall SFileDdaBeginEx(HSFILE__* handle, unsigned int, unsigned int, unsigned int offset,
    int, int, IDirectSoundBuffer*) {
    return handle && SFileSetFilePointer(handle, static_cast<int>(offset), nullptr, FILE_BEGIN) != INVALID_SET_FILE_POINTER;
}
int __stdcall SFileDdaDestroy() { return 1; }
int __stdcall SFileDdaEnd(HSFILE__* handle) { return handle ? 1 : 0; }
int __stdcall SFileDdaGetPos(HSFILE__* handle, unsigned int* position, unsigned int* maxPosition) {
    if (!handle) return 0;
    if (position) *position = SFileSetFilePointer(handle, 0, nullptr, FILE_CURRENT);
    if (maxPosition) *maxPosition = SFileGetFileSize(handle, nullptr);
    return 1;
}
int __stdcall SFileDdaGetVolume(HSFILE__* handle, int* volume, int* pan) {
    if (!handle) return 0;
    if (volume) *volume = 0;
    if (pan) *pan = 0;
    return 1;
}
int __stdcall SFileDdaInitialize(IDirectSound*) { return 1; }
int __stdcall SFileDdaSetVolume(HSFILE__* handle, int, int) { return handle ? 1 : 0; }

int __stdcall SFileDestroy() {
    std::lock_guard lock(s_apiLock);
    if (!s_archiveMetadata.empty()) return 0;
    SFile::Destroy();
    return 1;
}
int __stdcall SFileEnableArchive(HSARCHIVE__* handle, int enable) {
    std::lock_guard lock(s_apiLock);
    auto found = s_archiveMetadata.find(Archive(handle));
    if (found == s_archiveMetadata.end()) return 0;
    found->second.enabled = enable != 0;
    return 1;
}
int __stdcall SFileEnableDirectAccess(unsigned int access) { return SFile::EnableDirectAccess(access); }
void __stdcall SFileEnableSeekOptimization(int enable) { s_seekOptimization = enable != 0; }
unsigned int __stdcall SFileFileExists(const char* filename) { return SFile::FileExists(filename); }
unsigned int __stdcall SFileFileExistsEx(HSARCHIVE__* archive, const char* filename, unsigned int flags) {
    SFile* file = nullptr;
    const int found = SFile::OpenEx(Archive(archive), filename, flags, &file);
    SFile::Close(file);
    return found ? 1U : 0U;
}

int __stdcall SFileGetArchiveInfo(HSARCHIVE__* handle, int* priority, int* cdrom) {
    std::lock_guard lock(s_apiLock);
    const auto found = s_archiveMetadata.find(Archive(handle));
    if (found == s_archiveMetadata.end()) return 0;
    if (priority) *priority = found->second.priority;
    if (cdrom) *cdrom = 0;
    return 1;
}
int __stdcall SFileGetArchiveName(HSARCHIVE__* handle, char* buffer, unsigned int bufferChars) {
    std::lock_guard lock(s_apiLock);
    const auto found = s_archiveMetadata.find(Archive(handle));
    return found != s_archiveMetadata.end() && CopyString(buffer, bufferChars, found->second.name.c_str());
}
int __stdcall SFileGetBasePath(char* buffer, unsigned int bufferChars) { return SFile::GetBasePath(buffer, bufferChars); }
int __stdcall SFileGetFileArchive(HSFILE__* handle, HSARCHIVE__** archive) {
    if (!handle || !archive) return 0;
    *archive = Public(File(handle)->m_archive);
    return 1;
}
unsigned int __stdcall SFileGetFileCrc(HSFILE__*) { return 0; }
int __stdcall SFileGetFileMD5(HSFILE__* handle, unsigned char* md5) {
    return md5 && SFile::GetMD5(File(handle), reinterpret_cast<MD5*>(md5));
}
int __stdcall SFileGetFileName(HSFILE__* handle, char* buffer, unsigned int bufferChars) {
    SFile* file = File(handle);
    return file && CopyString(buffer, bufferChars, file->m_filename);
}
int __stdcall SFileGetActualFileName(HSFILE__* handle, char* buffer, unsigned int bufferChars) {
    return SFile::GetActualFileName(File(handle), buffer, bufferChars);
}
unsigned int __stdcall SFileGetFileCompressedSize(HSFILE__* handle, unsigned int* fileSizeHigh) {
    return SFile::GetFileSize(File(handle), fileSizeHigh);
}
unsigned int __stdcall SFileGetFileSize(HSFILE__* handle, unsigned int* fileSizeHigh) {
    return SFile::GetFileSize(File(handle), fileSizeHigh);
}
BOOL __stdcall SFileGetFileTime(HSFILE__* handle, FILETIME* fileTime) {
    SFile* file = File(handle);
    if (!file || !fileTime || !file->m_fileptr) return FALSE;
    const intptr_t osHandle = _get_osfhandle(_fileno(static_cast<FILE*>(file->m_fileptr)));
    return osHandle == -1 ? FALSE : GetFileTime(reinterpret_cast<HANDLE>(osHandle), nullptr, nullptr, fileTime);
}

int __stdcall SFileLoadFile(const char* filename, void** buffer, unsigned int* bytes,
    unsigned int extraBytes, OVERLAPPED* overlapped) {
    return SFileLoadFileEx2(nullptr, filename, buffer, bytes, extraBytes, 0, overlapped, 0);
}
int __stdcall SFileLoadFileEx(HSARCHIVE__* archive, const char* filename, void** buffer,
    unsigned int* bytes, unsigned int extraBytes, unsigned int flags, OVERLAPPED* overlapped) {
    return SFileLoadFileEx2(archive, filename, buffer, bytes, extraBytes, flags, overlapped, 0);
}
int __stdcall SFileLoadFileEx2(HSARCHIVE__* archive, const char* filename, void** buffer,
    unsigned int* bytes, unsigned int extraBytes, unsigned int flags, OVERLAPPED*, int) {
    const int result = SFile::Load(Archive(archive), filename, buffer, bytes, extraBytes, flags, nullptr);
    if (result && s_loadNotify) s_loadNotify(filename, s_loadNotifyData);
    return result;
}

int __stdcall SFileOpenArchive(char* archiveName, int priority, char flags, HSARCHIVE__** handle) {
    if (!handle) return 0;
    SArchive* archive = nullptr;
    const int result = SFile::OpenArchive(archiveName, priority, flags, &archive);
    *handle = Public(archive);
    if (result) {
        std::lock_guard lock(s_apiLock);
        s_archiveMetadata.emplace(archive, ArchiveMetadata{archiveName, priority, true});
    }
    return result;
}
int __stdcall SFileOpenPathAsArchive(HSARCHIVE__*, const char* pathPrefix, int priority,
    unsigned int flags, HSARCHIVE__** handle) {
    return SFileOpenArchive(const_cast<char*>(pathPrefix), priority, static_cast<char>(flags), handle);
}
unsigned int __stdcall SFileOpenFile(const char* filename, HSFILE__** handle) {
    if (!handle) return 0;
    SFile* file = nullptr;
    const unsigned int result = SFile::Open(filename, &file);
    *handle = Public(file);
    return result;
}
int __stdcall SFileOpenFileAsArchive(HSARCHIVE__*, char* filename, int priority, char flags, HSARCHIVE__** handle) {
    return SFileOpenArchive(filename, priority, flags, handle);
}
int __stdcall SFileOpenFileEx(HSARCHIVE__* archive, char* filename, unsigned int flags, HSFILE__** handle) {
    if (!handle) return 0;
    SFile* file = nullptr;
    const int result = SFile::OpenEx(Archive(archive), filename, flags, &file);
    *handle = Public(file);
    return result;
}

int __stdcall SFileReadFile(HSFILE__* handle, void* buffer, unsigned int bytesToRead,
    unsigned int* bytesRead, OVERLAPPED* overlapped) {
    return SFileReadFileEx2(handle, buffer, bytesToRead, bytesRead, overlapped, 0, nullptr);
}
int __stdcall SFileReadFileEx(HSFILE__* handle, void* buffer, unsigned int bytesToRead,
    unsigned int* bytesRead, OVERLAPPED* overlapped, void* asyncParam) {
    return SFileReadFileEx2(handle, buffer, bytesToRead, bytesRead, overlapped, 0, asyncParam);
}
int __stdcall SFileReadFileEx2(HSFILE__* handle, void* buffer, unsigned int bytesToRead,
    unsigned int* bytesRead, OVERLAPPED* overlapped, int, void* asyncParam) {
    SFile* file = File(handle);
    if (overlapped) {
        const std::uint64_t offset = (static_cast<std::uint64_t>(overlapped->OffsetHigh) << 32) | overlapped->Offset;
        int high = static_cast<int>(offset >> 32);
        if (SFile::SetFilePointer(file, static_cast<int>(offset), &high, FILE_BEGIN) == INVALID_SET_FILE_POINTER) return 0;
    }
    const int result = SFile::Read(file, buffer, bytesToRead, bytesRead, nullptr, asyncParam);
    if (overlapped && overlapped->hEvent) SetEvent(overlapped->hEvent);
    if (!result && s_errorProc && file) s_errorProc(file->m_filename, s_errorMode, GetLastError());
    return result;
}

void __stdcall SFileSetAsyncBudget(unsigned int bytesPerSecond) { s_asyncBudget = bytesPerSecond; }
int __stdcall SFileSetBasePath(const char* path) { return SFile::SetBasePath(path); }
void __stdcall SFileSetDataChunkSize(unsigned int bytes) { if (bytes) s_dataChunkSize = bytes; }
DWORD __stdcall SFileSetFilePointer(HSFILE__* handle, int distanceToMove,
    int* distanceToMoveHigh, DWORD moveMethod) {
    return SFile::SetFilePointer(File(handle), distanceToMove, distanceToMoveHigh, moveMethod);
}
void __stdcall SFileSetIoErrorMode(unsigned int errorMode, SFILE_IO_ERROR_PROC errorProc) {
    s_errorMode = errorMode;
    s_errorProc = errorProc;
}
void __stdcall SFileSetLocale(unsigned short localeId) { s_locale = localeId; }
unsigned short __stdcall SFileGetLocale() { return s_locale; }
void __stdcall SFileSetPlatform(unsigned char platformId) { s_platform = platformId; }
int __stdcall SFileUnloadFile(void* buffer) { return SFile::Unload(buffer); }
