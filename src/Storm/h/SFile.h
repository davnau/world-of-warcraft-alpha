#pragma once

#include "Storm/h/SFile2.h"

struct HSARCHIVE__ { int unused; };
struct HSFILE__ { int unused; };
struct IDirectSound;
struct IDirectSoundBuffer;

using SFILE_LOAD_NOTIFY = void (__stdcall*)(const char*, void*);
using SFILE_IO_ERROR_PROC = int (__stdcall*)(const char*, unsigned int, unsigned int);

int __stdcall SFileAuthenticateArchive(HSARCHIVE__* handle, unsigned int* extendedResult);
BOOL __stdcall SFileAuthenticateArchiveEx(HSARCHIVE__* handle, unsigned int* extendedResult,
    unsigned char* modulus, LONG modulusSize, const unsigned char* exponent, unsigned int exponentSize);
unsigned int __stdcall SFileCalcFileCrc(const char* filename);
int __stdcall SFileCancelRequestEx(IDirectSoundBuffer* buffer);
int __stdcall SFileCloseArchive(HSARCHIVE__* handle);
int __stdcall SFileCloseFile(HSFILE__* handle);
void __stdcall SFileRegisterLoadNotifyProc(SFILE_LOAD_NOTIFY callback, void* opaqueData);
int __stdcall SFileDdaBegin(HSFILE__* handle, unsigned int bufferSize, unsigned int flags);
int __stdcall SFileDdaBeginEx(HSFILE__* handle, unsigned int bufferSize, unsigned int flags,
    unsigned int offset, int volume, int pan, IDirectSoundBuffer* soundBuffer);
int __stdcall SFileDdaDestroy();
int __stdcall SFileDdaEnd(HSFILE__* handle);
int __stdcall SFileDdaGetPos(HSFILE__* handle, unsigned int* position, unsigned int* maxPosition);
int __stdcall SFileDdaGetVolume(HSFILE__* handle, int* volume, int* pan);
int __stdcall SFileDdaInitialize(IDirectSound* directSound);
int __stdcall SFileDdaSetVolume(HSFILE__* handle, int volume, int pan);
int __stdcall SFileDestroy();
int __stdcall SFileEnableArchive(HSARCHIVE__* archive, int enable);
int __stdcall SFileEnableDirectAccess(unsigned int access);
void __stdcall SFileEnableSeekOptimization(int enable);
unsigned int __stdcall SFileFileExists(const char* filename);
unsigned int __stdcall SFileFileExistsEx(HSARCHIVE__* archive, const char* filename, unsigned int flags);
int __stdcall SFileGetArchiveInfo(HSARCHIVE__* archive, int* priority, int* cdrom);
int __stdcall SFileGetArchiveName(HSARCHIVE__* archive, char* buffer, unsigned int bufferChars);
int __stdcall SFileGetBasePath(char* buffer, unsigned int bufferChars);
int __stdcall SFileGetFileArchive(HSFILE__* file, HSARCHIVE__** archive);
unsigned int __stdcall SFileGetFileCrc(HSFILE__* handle);
int __stdcall SFileGetFileMD5(HSFILE__* handle, unsigned char* md5);
int __stdcall SFileGetFileName(HSFILE__* file, char* buffer, unsigned int bufferChars);
int __stdcall SFileGetActualFileName(HSFILE__* file, char* buffer, unsigned int bufferChars);
unsigned int __stdcall SFileGetFileCompressedSize(HSFILE__* handle, unsigned int* fileSizeHigh);
unsigned int __stdcall SFileGetFileSize(HSFILE__* handle, unsigned int* fileSizeHigh);
BOOL __stdcall SFileGetFileTime(HSFILE__* handle, FILETIME* fileTime);
int __stdcall SFileLoadFile(const char* filename, void** buffer, unsigned int* bytes,
    unsigned int extraBytes, OVERLAPPED* overlapped);
int __stdcall SFileLoadFileEx(HSARCHIVE__* archive, const char* filename, void** buffer,
    unsigned int* bytes, unsigned int extraBytes, unsigned int flags, OVERLAPPED* overlapped);
int __stdcall SFileLoadFileEx2(HSARCHIVE__* archive, const char* filename, void** buffer,
    unsigned int* bytes, unsigned int extraBytes, unsigned int flags, OVERLAPPED* overlapped, int overlappedPriority);
int __stdcall SFileOpenArchive(char* archiveName, int priority, char flags, HSARCHIVE__** handle);
int __stdcall SFileOpenPathAsArchive(HSARCHIVE__* ownerArchive, const char* pathPrefix,
    int priority, unsigned int flags, HSARCHIVE__** handle);
unsigned int __stdcall SFileOpenFile(const char* filename, HSFILE__** handle);
int __stdcall SFileOpenFileAsArchive(HSARCHIVE__* ownerArchive, char* filename,
    int priority, char flags, HSARCHIVE__** handle);
int __stdcall SFileOpenFileEx(HSARCHIVE__* archive, char* filename, unsigned int flags, HSFILE__** handle);
int __stdcall SFileReadFile(HSFILE__* handle, void* buffer, unsigned int bytesToRead,
    unsigned int* bytesRead, OVERLAPPED* overlapped);
int __stdcall SFileReadFileEx(HSFILE__* handle, void* buffer, unsigned int bytesToRead,
    unsigned int* bytesRead, OVERLAPPED* overlapped, void* asyncParam);
int __stdcall SFileReadFileEx2(HSFILE__* handle, void* buffer, unsigned int bytesToRead,
    unsigned int* bytesRead, OVERLAPPED* overlapped, int overlappedPriority, void* asyncParam);
void __stdcall SFileSetAsyncBudget(unsigned int bytesPerSecond);
int __stdcall SFileSetBasePath(const char* path);
void __stdcall SFileSetDataChunkSize(unsigned int bytes);
DWORD __stdcall SFileSetFilePointer(HSFILE__* handle, int distanceToMove,
    int* distanceToMoveHigh, DWORD moveMethod);
void __stdcall SFileSetIoErrorMode(unsigned int errorMode, SFILE_IO_ERROR_PROC errorProc);
void __stdcall SFileSetLocale(unsigned short localeId);
unsigned short __stdcall SFileGetLocale();
void __stdcall SFileSetPlatform(unsigned char platformId);
int __stdcall SFileUnloadFile(void* buffer);
