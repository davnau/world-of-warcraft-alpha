#pragma once

#include "Storm/h/storm.h"

#include <cstddef>
#include <cstdio>

#pragma pack(push, 2)
struct CentralDirectoryHeader {
    char signature[4];
    std::uint16_t thisDiskNumber;
    std::uint16_t directoryStartDiskNumber;
    std::uint16_t directoryEntriesThisDisk;
    std::uint16_t directoryEntriesTotal;
    std::uint32_t centralDirectorySize;
    std::uint32_t centralDirectoryOffset;
    std::uint16_t commentLength;
    void EndianCorrect();
};

struct CentralDirectoryFileHeader {
    char signature[4];
    std::uint16_t versionMadeBy;
    std::uint16_t versionRequired;
    std::uint16_t generalFlags;
    std::uint16_t compressionMethod;
    std::uint16_t modifiedTime;
    std::uint16_t modifiedDate;
    std::uint32_t z_crc32;
    std::uint32_t compressedSize;
    std::uint32_t uncompressedSize;
    std::uint16_t filenameSize;
    std::uint16_t extraFieldSize;
    std::uint16_t commentSize;
    std::uint16_t diskNumberStart;
    std::uint16_t internalFileAttributes;
    std::uint32_t externalFileAttributes;
    std::uint32_t localHeaderOffset;
    void EndianCorrect();
};

struct LocalFileHeader {
    char signature[4];
    std::uint16_t versionRequired;
    std::uint16_t generalFlags;
    std::uint16_t compressionMethod;
    std::uint16_t modifiedTime;
    std::uint16_t modifiedDate;
    std::uint32_t z_crc32;
    std::uint32_t compressedSize;
    std::uint32_t uncompressedSize;
    std::uint16_t filenameSize;
    std::uint16_t extraFieldSize;
    void EndianCorrect();
};
#pragma pack(pop)

struct DataDescriptor {
    std::uint32_t z_crc32;
    std::uint32_t compressedSize;
    std::uint32_t uncompressedSize;
    void EndianCorrect();
};

struct Flags {
    unsigned int m_value;
    Flags();
    void Set(unsigned int bit);
    void Clear(unsigned int bit);
    unsigned int IsSet(unsigned int bit) const;
    BOOL IsClear(unsigned int bit) const;
};

struct ZipFileArchive {
    void* m_previousLink;
    void* m_nextLink;
    FILE* file;
    char filename[256];
    unsigned int openFileCount;

    ZipFileArchive();
    ~ZipFileArchive();
    FILE* Open(const char* archiveName);
    BOOL GetCentralDirectoryHeader(CentralDirectoryHeader* header);
    int ProcessCentralDirectory(CentralDirectoryHeader* header);
    int ReadCentralDirectoryFileHeader();
};

struct ZipFileDirEntry {
    std::uint8_t m_hashLinks[24];
    ZipFileArchive* archive;
    char filename[256];
    unsigned int startOffset;
    unsigned int compressedSize;
    unsigned int uncompressedSize;
    unsigned int compressionMethod;

    ZipFileDirEntry();
    ~ZipFileDirEntry();
};

struct internal_state;
struct z_stream_s {
    unsigned char* next_in;
    unsigned int avail_in;
    unsigned int total_in;
    unsigned char* next_out;
    unsigned int avail_out;
    unsigned int total_out;
    char* msg;
    internal_state* state;
    void* (__fastcall* zalloc)(void*, unsigned int, unsigned int);
    void (__fastcall* zfree)(void*, void*);
    void* opaque;
    int data_type;
    unsigned int adler;
    unsigned int reserved;
};

struct ZipFileFCB {
    ZipFileDirEntry* dirEntry;
    Flags flags;
    unsigned int targetPosition;
    unsigned int compressedPosition;
    unsigned int uncompressedPosition;
    z_stream_s zlibStream;
    unsigned char compressedData[4096];

    ZipFileFCB();
    ~ZipFileFCB();
    int SetFault();
};

class WowFile;
class WowFileSystemProvider {
public:
    virtual ~WowFileSystemProvider();
    virtual WowFile* Open(const char* filename) = 0;
    virtual unsigned char Close(WowFile* file) = 0;
};

class WowFile {
public:
    WowFileSystemProvider* m_provider;
    explicit WowFile(WowFileSystemProvider* provider);
    ~WowFile();
    unsigned char Close();
};

class TestFile final : public WowFile {
public:
    FILE* m_f;
    TestFile(WowFileSystemProvider* provider, FILE* file);
};

class TestFileSystemProvider final : public WowFileSystemProvider {
public:
    WowFile* Open(const char* filename) override;
    unsigned char Close(WowFile* file) override;
};

class WowFileSystem {
public:
    WowFileSystemProvider* m_providerList;
    WowFileSystem();
    ~WowFileSystem();
    void RegisterProvider(WowFileSystemProvider* provider);
    void UnregisterProvider(WowFileSystemProvider* provider);
    WowFile* Open(const char* filename);
};

ZipFileArchive* ZipFileOpenArchive(const char* archiveName);
int ZipFileCloseArchive(ZipFileArchive* archive);
int __fastcall ZipFileFileExists(const char* filename, ZipFileDirEntry** entry);
ZipFileFCB* __fastcall ZipFileOpenFile(const char* filename, ZipFileArchive* archive);
int ZipFileCloseFile(ZipFileFCB* file);
int __fastcall ZipFileSetFilePointer(ZipFileFCB* file, unsigned int offset, unsigned int origin);
unsigned int ZipFileGetFilePointer(ZipFileFCB* file);
unsigned int ZipFileGetFileSize(ZipFileFCB* file);
int __fastcall ZipFileReadFile(ZipFileFCB* file, void* buffer, std::size_t bytesToRead, unsigned int* bytesRead);
int __fastcall ZipFileLoadFile(const char* filename, unsigned char** buffer, unsigned int* bytes);
void ZipFileUnloadFile(void* buffer);
int __fastcall ZipFileList(ZipFileArchive* archive, int (__fastcall* callback)(char*, void*), void* parameter);
void ConvertFromZip(char* filename);
void FSTest();
int StormInflateRaw(const void* source, unsigned int sourceBytes, void* destination, unsigned int* destinationBytes);

static_assert(sizeof(CentralDirectoryHeader) == 22);
static_assert(sizeof(CentralDirectoryFileHeader) == 46);
static_assert(sizeof(LocalFileHeader) == 30);
static_assert(sizeof(DataDescriptor) == 12);
#if defined(_M_IX86)
static_assert(sizeof(ZipFileArchive) == 272, "ZipFileArchive x86 PDB layout mismatch.");
static_assert(sizeof(ZipFileDirEntry) == 300, "ZipFileDirEntry x86 PDB layout mismatch.");
static_assert(sizeof(z_stream_s) == 56, "z_stream_s x86 PDB layout mismatch.");
static_assert(sizeof(ZipFileFCB) == 4172, "ZipFileFCB x86 PDB layout mismatch.");
static_assert(sizeof(WowFileSystemProvider) == 4, "WowFileSystemProvider x86 PDB layout mismatch.");
static_assert(sizeof(WowFile) == 4, "WowFile x86 PDB layout mismatch.");
static_assert(sizeof(TestFile) == 8, "TestFile x86 PDB layout mismatch.");
static_assert(sizeof(WowFileSystem) == 4, "WowFileSystem x86 PDB layout mismatch.");
#endif
