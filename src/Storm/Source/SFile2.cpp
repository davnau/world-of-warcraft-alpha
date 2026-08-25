#include "Storm/h/SFile2.h"
#include "Storm/h/SComp.h"
#include "Storm/h/SZip.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <vector>

namespace {

std::filesystem::path s_basePath;
std::filesystem::path s_dataPath;
std::filesystem::path s_alternateDataPath;
bool s_directAccess = true;
bool s_checkDisk = true;
bool s_hashEnabled = true;

constexpr std::uint32_t MPQ_HEADER_MAGIC = 0x1A51504DU;
constexpr std::uint32_t MPQ_FILE_IMPLODE = 0x00000100U;
constexpr std::uint32_t MPQ_FILE_COMPRESS = 0x00000200U;
constexpr std::uint32_t MPQ_FILE_ENCRYPTED = 0x00010000U;
constexpr std::uint32_t MPQ_FILE_FIX_KEY = 0x00020000U;
constexpr std::uint32_t MPQ_FILE_SINGLE_UNIT = 0x01000000U;
constexpr std::uint32_t MPQ_FILE_SECTOR_CRC = 0x04000000U;
constexpr std::uint32_t MPQ_FILE_EXISTS = 0x80000000U;
constexpr std::uint32_t MPQ_HASH_EMPTY = 0xFFFFFFFFU;
constexpr std::uint32_t MPQ_HASH_DELETED = 0xFFFFFFFEU;

#pragma pack(push, 1)
struct MpqHeader {
    std::uint32_t magic;
    std::uint32_t headerSize;
    std::uint32_t archiveSize;
    std::uint16_t formatVersion;
    std::uint16_t sectorSizeShift;
    std::uint32_t hashTablePosition;
    std::uint32_t blockTablePosition;
    std::uint32_t hashTableEntries;
    std::uint32_t blockTableEntries;
};

struct MpqHashEntry {
    std::uint32_t nameHashA;
    std::uint32_t nameHashB;
    std::uint16_t locale;
    std::uint16_t platform;
    std::uint32_t blockIndex;
};

struct MpqBlockEntry {
    std::uint32_t filePosition;
    std::uint32_t compressedSize;
    std::uint32_t fileSize;
    std::uint32_t flags;
};
#pragma pack(pop)

struct ArchiveStorage {
    std::filesystem::path path;
    FILE* stream = nullptr;
    std::uint64_t archiveOffset = 0;
    std::uint32_t sectorSize = 0;
    std::vector<MpqHashEntry> hashTable;
    std::vector<MpqBlockEntry> blockTable;
    bool directory = false;
    std::mutex streamMutex;
};

struct MemoryFile {
    std::vector<unsigned char> bytes;
};

struct MpqFileStream {
    ArchiveStorage* storage = nullptr;
    MpqBlockEntry block{};
    std::uint32_t key = 0;
    std::vector<std::uint32_t> sectorOffsets;
};

struct OpenArchiveEntry {
    SArchive* archive;
    int priority;
    std::uint64_t sequence;
};

std::vector<OpenArchiveEntry> s_openArchives;
std::uint64_t s_archiveSequence = 0;

const std::array<std::uint32_t, 0x500>& MpqCryptTable() {
    static const std::array<std::uint32_t, 0x500> table = [] {
        std::array<std::uint32_t, 0x500> result{};
        std::uint32_t seed = 0x00100001U;
        for (std::uint32_t index1 = 0; index1 < 0x100; ++index1) {
            std::uint32_t index2 = index1;
            for (unsigned iteration = 0; iteration < 5; ++iteration, index2 += 0x100) {
                seed = (seed * 125U + 3U) % 0x2AAAABU;
                const std::uint32_t high = (seed & 0xFFFFU) << 16;
                seed = (seed * 125U + 3U) % 0x2AAAABU;
                result[index2] = high | (seed & 0xFFFFU);
            }
        }
        return result;
    }();
    return table;
}

std::string NormalizeMpqName(const char* filename) {
    std::string result = filename ? filename : "";
    for (char& value : result) {
        if (value == '/') value = '\\';
        if (value >= 'a' && value <= 'z') value = static_cast<char>(value - 'a' + 'A');
    }
    return result;
}

std::uint32_t MpqHashString(const char* filename, std::uint32_t hashType) {
    const auto& crypt = MpqCryptTable();
    const std::string normalized = NormalizeMpqName(filename);
    std::uint32_t seed1 = 0x7FED7FEDU;
    std::uint32_t seed2 = 0xEEEEEEEEU;
    for (const unsigned char value : normalized) {
        seed1 = crypt[(hashType << 8) + value] ^ (seed1 + seed2);
        seed2 = value + seed1 + seed2 + (seed2 << 5) + 3U;
    }
    return seed1;
}

void MpqDecryptBlock(void* data, std::size_t bytes, std::uint32_t key) {
    auto* values = static_cast<std::uint32_t*>(data);
    const std::size_t count = bytes / sizeof(std::uint32_t);
    const auto& crypt = MpqCryptTable();
    std::uint32_t seed = 0xEEEEEEEEU;
    for (std::size_t index = 0; index < count; ++index) {
        seed += crypt[0x400U + (key & 0xFFU)];
        const std::uint32_t decoded = values[index] ^ (key + seed);
        key = ((~key << 21) + 0x11111111U) | (key >> 11);
        seed = decoded + seed + (seed << 5) + 3U;
        values[index] = decoded;
    }
}

bool ReadAt(FILE* stream, std::uint64_t position, void* destination, std::size_t bytes) {
    if (!stream || position > static_cast<std::uint64_t>(std::numeric_limits<__int64>::max()) ||
        _fseeki64(stream, static_cast<__int64>(position), SEEK_SET) != 0) {
        return false;
    }
    return bytes == 0 || std::fread(destination, 1, bytes, stream) == bytes;
}

bool LocateMpqHeader(FILE* stream, std::uint64_t& archiveOffset, MpqHeader& header) {
    if (!stream || _fseeki64(stream, 0, SEEK_END) != 0) return false;
    const __int64 fileSize = _ftelli64(stream);
    if (fileSize < static_cast<__int64>(sizeof(MpqHeader))) return false;
    const std::uint64_t searchLimit = std::min<std::uint64_t>(static_cast<std::uint64_t>(fileSize), 0x1000000U);
    for (std::uint64_t offset = 0; offset + sizeof(MpqHeader) <= searchLimit; offset += 0x200U) {
        if (!ReadAt(stream, offset, &header, sizeof(header))) return false;
        if (header.magic == MPQ_HEADER_MAGIC && header.headerSize >= sizeof(MpqHeader) &&
            header.formatVersion == 0 && header.hashTableEntries && header.blockTableEntries) {
            archiveOffset = offset;
            return true;
        }
    }
    return false;
}

bool LoadMpqArchive(ArchiveStorage& storage) {
    if (_wfopen_s(&storage.stream, storage.path.c_str(), L"rb") != 0 || !storage.stream) return false;
    MpqHeader header{};
    if (!LocateMpqHeader(storage.stream, storage.archiveOffset, header) || header.sectorSizeShift > 15U ||
        header.hashTableEntries > 0x1000000U || header.blockTableEntries > 0x1000000U) {
        std::fclose(storage.stream);
        storage.stream = nullptr;
        return false;
    }
    storage.sectorSize = 512U << header.sectorSizeShift;
    storage.hashTable.resize(header.hashTableEntries);
    storage.blockTable.resize(header.blockTableEntries);
    const std::size_t hashBytes = storage.hashTable.size() * sizeof(MpqHashEntry);
    const std::size_t blockBytes = storage.blockTable.size() * sizeof(MpqBlockEntry);
    if (!ReadAt(storage.stream, storage.archiveOffset + header.hashTablePosition,
                storage.hashTable.data(), hashBytes) ||
        !ReadAt(storage.stream, storage.archiveOffset + header.blockTablePosition,
                storage.blockTable.data(), blockBytes)) {
        std::fclose(storage.stream);
        storage.stream = nullptr;
        return false;
    }
    MpqDecryptBlock(storage.hashTable.data(), hashBytes, MpqHashString("(hash table)", 3));
    MpqDecryptBlock(storage.blockTable.data(), blockBytes, MpqHashString("(block table)", 3));
    return true;
}

const MpqBlockEntry* FindMpqBlock(const ArchiveStorage& storage, const char* filename) {
    if (storage.hashTable.empty() || !filename || !*filename) return nullptr;
    const std::uint32_t hashA = MpqHashString(filename, 1);
    const std::uint32_t hashB = MpqHashString(filename, 2);
    const std::size_t start = MpqHashString(filename, 0) % storage.hashTable.size();
    const MpqBlockEntry* fallback = nullptr;
    for (std::size_t probe = 0; probe < storage.hashTable.size(); ++probe) {
        const MpqHashEntry& entry = storage.hashTable[(start + probe) % storage.hashTable.size()];
        if (entry.blockIndex == MPQ_HASH_EMPTY) break;
        if (entry.blockIndex == MPQ_HASH_DELETED || entry.blockIndex >= storage.blockTable.size() ||
            entry.nameHashA != hashA || entry.nameHashB != hashB) continue;
        const MpqBlockEntry& block = storage.blockTable[entry.blockIndex];
        if (!(block.flags & MPQ_FILE_EXISTS)) continue;
        if (entry.locale == 0 && entry.platform == 0) return &block;
        if (!fallback) fallback = &block;
    }
    return fallback;
}

std::uint32_t MpqFileKey(const char* filename, const MpqBlockEntry& block) {
    std::string name = filename ? filename : "";
    const std::size_t slash = name.find_last_of("\\/");
    if (slash != std::string::npos) name.erase(0, slash + 1);
    std::uint32_t key = MpqHashString(name.c_str(), 3);
    if (block.flags & MPQ_FILE_FIX_KEY) key = (key + block.filePosition) ^ block.fileSize;
    return key;
}

bool DecompressMpqSector(const unsigned char* source, std::uint32_t sourceBytes,
                         unsigned char* destination, std::uint32_t destinationBytes,
                         std::uint32_t flags) {
    if (sourceBytes == destinationBytes) {
        if (sourceBytes) std::memcpy(destination, source, sourceBytes);
        return true;
    }
    if (flags & MPQ_FILE_COMPRESS) {
        unsigned int actual = destinationBytes;
        return SCompDecompress(destination, &actual, source, sourceBytes) && actual == destinationBytes;
    }
    // PKWARE implode is represented separately from multi-compression.  The
    // preserved WoW archives use zlib sectors, so fail explicitly if an older
    // imploded member is encountered instead of returning corrupt bytes.
    if (flags & MPQ_FILE_IMPLODE) return false;
    return false;
}

bool ExtractMpqFile(ArchiveStorage& storage, const char* filename,
                    const MpqBlockEntry& block, std::vector<unsigned char>& output) {
    output.clear();
    if (block.fileSize == 0) return true;
    if (!storage.stream || !storage.sectorSize || block.compressedSize == 0) return false;
    std::vector<unsigned char> packed(block.compressedSize);
    {
        std::lock_guard guard(storage.streamMutex);
        if (!ReadAt(storage.stream, storage.archiveOffset + block.filePosition,
                    packed.data(), packed.size())) return false;
    }

    const std::uint32_t key = (block.flags & MPQ_FILE_ENCRYPTED) ? MpqFileKey(filename, block) : 0;
    output.resize(block.fileSize);
    if (block.flags & MPQ_FILE_SINGLE_UNIT) {
        if (key) MpqDecryptBlock(packed.data(), packed.size(), key);
        if ((block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE)) && block.compressedSize < block.fileSize)
            return DecompressMpqSector(packed.data(), block.compressedSize, output.data(), block.fileSize, block.flags);
        if (block.compressedSize < block.fileSize) return false;
        std::memcpy(output.data(), packed.data(), block.fileSize);
        return true;
    }

    const std::uint32_t sectorCount = (block.fileSize + storage.sectorSize - 1U) / storage.sectorSize;
    if (!(block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE))) {
        if (packed.size() < block.fileSize) return false;
        for (std::uint32_t sector = 0, outputOffset = 0; sector < sectorCount; ++sector) {
            const std::uint32_t bytes = std::min(storage.sectorSize, block.fileSize - outputOffset);
            if (key) MpqDecryptBlock(packed.data() + outputOffset, bytes, key + sector);
            std::memcpy(output.data() + outputOffset, packed.data() + outputOffset, bytes);
            outputOffset += bytes;
        }
        return true;
    }

    const std::uint32_t offsetCount = sectorCount + 1U + ((block.flags & MPQ_FILE_SECTOR_CRC) ? 1U : 0U);
    const std::size_t tableBytes = static_cast<std::size_t>(offsetCount) * sizeof(std::uint32_t);
    if (tableBytes > packed.size()) return false;
    if (key) MpqDecryptBlock(packed.data(), tableBytes, key - 1U);
    const auto* offsets = reinterpret_cast<const std::uint32_t*>(packed.data());
    std::uint32_t outputOffset = 0;
    for (std::uint32_t sector = 0; sector < sectorCount; ++sector) {
        const std::uint32_t begin = offsets[sector];
        const std::uint32_t end = offsets[sector + 1U];
        if (begin > end || end > packed.size()) return false;
        const std::uint32_t packedBytes = end - begin;
        const std::uint32_t unpackedBytes = std::min(storage.sectorSize, block.fileSize - outputOffset);
        unsigned char* sectorData = packed.data() + begin;
        if (key) MpqDecryptBlock(sectorData, packedBytes, key + sector);
        if (!DecompressMpqSector(sectorData, packedBytes, output.data() + outputOffset,
                                 unpackedBytes, block.flags)) return false;
        outputOffset += unpackedBytes;
    }
    return outputOffset == block.fileSize;
}

bool InitializeMpqFileStream(ArchiveStorage& storage, const char* filename,
                             const MpqBlockEntry& block, MpqFileStream& stream) {
    stream.storage = &storage;
    stream.block = block;
    stream.key = (block.flags & MPQ_FILE_ENCRYPTED) ? MpqFileKey(filename, block) : 0U;
    if (!(block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE))) return true;

    const std::uint32_t sectorCount =
        (block.fileSize + storage.sectorSize - 1U) / storage.sectorSize;
    const std::uint32_t offsetCount = sectorCount + 1U +
        ((block.flags & MPQ_FILE_SECTOR_CRC) ? 1U : 0U);
    stream.sectorOffsets.resize(offsetCount);
    const std::size_t tableBytes = stream.sectorOffsets.size() * sizeof(std::uint32_t);
    if (tableBytes > block.compressedSize) return false;
    {
        std::lock_guard guard(storage.streamMutex);
        if (!ReadAt(storage.stream, storage.archiveOffset + block.filePosition,
                    stream.sectorOffsets.data(), tableBytes)) return false;
    }
    if (stream.key)
        MpqDecryptBlock(stream.sectorOffsets.data(), tableBytes, stream.key - 1U);
    for (std::uint32_t sector = 0; sector < sectorCount; ++sector) {
        if (stream.sectorOffsets[sector] > stream.sectorOffsets[sector + 1U] ||
            stream.sectorOffsets[sector + 1U] > block.compressedSize)
            return false;
    }
    return true;
}

bool ReadMpqFileStream(MpqFileStream& stream, std::uint32_t offset,
                       void* destination, std::uint32_t bytes) {
    if (!stream.storage || offset > stream.block.fileSize ||
        bytes > stream.block.fileSize - offset) return false;
    if (bytes == 0) return true;

    ArchiveStorage& storage = *stream.storage;
    auto* output = static_cast<unsigned char*>(destination);
    if (!(stream.block.flags & (MPQ_FILE_COMPRESS | MPQ_FILE_IMPLODE |
                                MPQ_FILE_ENCRYPTED))) {
        std::lock_guard guard(storage.streamMutex);
        return ReadAt(storage.stream,
                      storage.archiveOffset + stream.block.filePosition + offset,
                      output, bytes);
    }

    const std::uint32_t firstSector = offset / storage.sectorSize;
    const std::uint32_t lastSector = (offset + bytes - 1U) / storage.sectorSize;
    std::uint32_t copied = 0;
    for (std::uint32_t sector = firstSector; sector <= lastSector; ++sector) {
        const std::uint32_t sectorFileOffset = sector * storage.sectorSize;
        const std::uint32_t unpackedBytes =
            std::min(storage.sectorSize, stream.block.fileSize - sectorFileOffset);
        std::uint32_t packedBegin = sectorFileOffset;
        std::uint32_t packedEnd = packedBegin + unpackedBytes;
        if (!stream.sectorOffsets.empty()) {
            packedBegin = stream.sectorOffsets[sector];
            packedEnd = stream.sectorOffsets[sector + 1U];
        }
        if (packedBegin > packedEnd || packedEnd > stream.block.compressedSize)
            return false;

        std::vector<unsigned char> packed(packedEnd - packedBegin);
        {
            std::lock_guard guard(storage.streamMutex);
            if (!ReadAt(storage.stream,
                        storage.archiveOffset + stream.block.filePosition + packedBegin,
                        packed.data(), packed.size())) return false;
        }
        if (stream.key)
            MpqDecryptBlock(packed.data(), packed.size(), stream.key + sector);

        std::vector<unsigned char> unpacked(unpackedBytes);
        if (stream.sectorOffsets.empty()) {
            if (packed.size() != unpacked.size()) return false;
            std::memcpy(unpacked.data(), packed.data(), unpacked.size());
        } else if (!DecompressMpqSector(packed.data(),
                                        static_cast<std::uint32_t>(packed.size()),
                                        unpacked.data(), unpackedBytes,
                                        stream.block.flags)) {
            return false;
        }

        const std::uint32_t copyBegin = std::max(offset, sectorFileOffset);
        const std::uint32_t copyEnd = std::min(offset + bytes,
                                               sectorFileOffset + unpackedBytes);
        const std::uint32_t copyBytes = copyEnd - copyBegin;
        std::memcpy(output + copied,
                    unpacked.data() + (copyBegin - sectorFileOffset), copyBytes);
        copied += copyBytes;
    }
    return copied == bytes;
}

ArchiveStorage* Storage(const SArchive* archive) {
    return archive && archive->m_type == SARCHIVE_MPQ
        ? static_cast<ArchiveStorage*>(archive->m_archive) : nullptr;
}

char* DuplicateString(const std::string& value) {
    const auto bytes = value.size() + 1;
    char* copy = new (std::nothrow) char[bytes];
    if (copy) {
        std::memcpy(copy, value.c_str(), bytes);
    }
    return copy;
}

bool CopyPath(char* buffer, unsigned int bufferChars, const std::filesystem::path& path) {
    if (!buffer || bufferChars == 0) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return false;
    }
    const std::string value = path.string();
    if (value.size() + 1 > bufferChars) {
        buffer[0] = '\0';
        SetLastError(ERROR_INSUFFICIENT_BUFFER);
        return false;
    }
    std::memcpy(buffer, value.c_str(), value.size() + 1);
    return true;
}

std::filesystem::path ArchiveRoot(const SArchive* archive) {
    const ArchiveStorage* storage = Storage(archive);
    return storage && storage->directory ? storage->path : std::filesystem::path();
}

std::filesystem::path ResolveFile(const SArchive* archive, const char* filename) {
    if (!filename || !*filename) {
        return {};
    }
    std::filesystem::path requested(filename);
    if (requested.is_absolute()) {
        return requested;
    }
    if (archive) {
        const auto root = ArchiveRoot(archive);
        if (!root.empty() && std::filesystem::is_directory(root)) {
            return root / requested;
        }
    }
    const std::filesystem::path base = s_basePath.empty() ? std::filesystem::current_path() : s_basePath;
    const std::filesystem::path candidates[] = {
        base / requested,
        s_dataPath.empty() ? std::filesystem::path() : base / s_dataPath / requested,
        s_alternateDataPath.empty() ? std::filesystem::path() : base / s_alternateDataPath / requested,
    };
    for (const auto& candidate : candidates) {
        if (!candidate.empty() && std::filesystem::is_regular_file(candidate)) {
            return candidate;
        }
    }
    return candidates[0];
}

int SeekOrigin(DWORD moveMethod) {
    switch (moveMethod) {
    case FILE_BEGIN: return SEEK_SET;
    case FILE_CURRENT: return SEEK_CUR;
    case FILE_END: return SEEK_END;
    default: return -1;
    }
}

} // namespace

SFile::SFile(SFILE_TYPE type)
    : m_type(type), m_fileptr(nullptr), m_archive(nullptr), m_filename(nullptr),
      m_actualname(nullptr), m_size(0), m_zbuffer(nullptr), m_zstream(nullptr),
      m_curOffset(0), m_lock{}, m_hsfile(nullptr), m_zipFile(nullptr), m_md5{},
      m_haveMD5(0), m_closeAfterLoad(0), m_asyncCount(0) {}

SFile::~SFile() {
    delete[] m_filename;
    delete[] m_actualname;
}

void __stdcall SFile::EnableHash(unsigned char enable) { s_hashEnabled = enable != 0; }
void __stdcall SFile::RebuildHash() {}
void SFile::DoAsyncRead() {}
unsigned int __stdcall SFile::ReadProc(void* parameter) {
    if (parameter) static_cast<SFile*>(parameter)->DoAsyncRead();
    return 0;
}
void SFile::InitializeReadThread() {}
void __fastcall SFile::QueueReadRequest(SFile* file, void*, unsigned int, SOVERLAPPED* overlapped) {
    if (file) ++file->m_asyncCount;
    if (overlapped && overlapped->hEvent) overlapped->hEvent->Set();
    if (file && file->m_asyncCount) --file->m_asyncCount;
}
BOOL __fastcall SFile::DoZRead(void*, unsigned int, unsigned int* bytesRead) {
    if (bytesRead) *bytesRead = 0;
    SetLastError(ERROR_NOT_SUPPORTED);
    return FALSE;
}

unsigned int __stdcall SFile::Open(const char* filename, SFile** file) {
    return OpenEx(nullptr, filename, 0, file) ? 2U : 0U;
}

int __stdcall SFile::OpenEx(SArchive* archive, const char* filename, unsigned int, SFile** file) {
    if (!file || !filename) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    *file = nullptr;
    if (archive && archive->m_type == SARCHIVE_ZIP) {
        ZipFileFCB* zipFile = ZipFileOpenFile(filename, static_cast<ZipFileArchive*>(archive->m_archive));
        if (!zipFile) { SetLastError(ERROR_FILE_NOT_FOUND); return 0; }
        auto result = new (std::nothrow) SFile(SFILE_ZIP_FILE);
        if (!result) { ZipFileCloseFile(zipFile); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
        result->m_archive = archive;
        result->m_zipFile = zipFile;
        result->m_filename = DuplicateString(filename);
        result->m_actualname = DuplicateString(filename);
        result->m_size = ZipFileGetFileSize(zipFile);
        *file = result;
        SetLastError(ERROR_SUCCESS);
        return 1;
    }

    if (archive) {
        ArchiveStorage* storage = Storage(archive);
        if (storage && !storage->directory) {
            const MpqBlockEntry* block = FindMpqBlock(*storage, filename);
            if (!block) { SetLastError(ERROR_FILE_NOT_FOUND); return 0; }
            if (!(block->flags & MPQ_FILE_SINGLE_UNIT)) {
                auto stream = std::make_unique<MpqFileStream>();
                if (!InitializeMpqFileStream(*storage, filename, *block, *stream)) {
                    SetLastError(ERROR_BAD_COMPRESSION_BUFFER);
                    return 0;
                }
                auto result = new (std::nothrow) SFile(SFILE_COMPRESSED);
                if (!result) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
                result->m_fileptr = stream.release();
                result->m_archive = archive;
                result->m_filename = DuplicateString(filename);
                result->m_actualname = DuplicateString(filename);
                result->m_size = block->fileSize;
                *file = result;
                SetLastError(ERROR_SUCCESS);
                return 1;
            }
            auto memory = new (std::nothrow) MemoryFile;
            if (!memory) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
            if (!ExtractMpqFile(*storage, filename, *block, memory->bytes)) {
                delete memory;
                SetLastError(ERROR_BAD_COMPRESSION_BUFFER);
                return 0;
            }
            auto result = new (std::nothrow) SFile(SFILE_PAQ);
            if (!result) { delete memory; SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
            result->m_fileptr = memory;
            result->m_archive = archive;
            result->m_filename = DuplicateString(filename);
            result->m_actualname = DuplicateString(filename);
            result->m_size = static_cast<unsigned int>(memory->bytes.size());
            *file = result;
            SetLastError(ERROR_SUCCESS);
            return 1;
        }
    }

    if (!archive) {
        const auto resolved = ResolveFile(nullptr, filename);
        FILE* stream = nullptr;
        if (s_directAccess && _wfopen_s(&stream, resolved.c_str(), L"rb") == 0 && stream) {
            auto result = new (std::nothrow) SFile(SFILE_PLAIN);
            if (!result) { std::fclose(stream); SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
            result->m_fileptr = stream;
            result->m_filename = DuplicateString(filename);
            result->m_actualname = DuplicateString(resolved.string());
            std::error_code error;
            const auto length = std::filesystem::file_size(resolved, error);
            result->m_size = error || length > UINT_MAX ? UINT_MAX : static_cast<unsigned int>(length);
            *file = result;
            SetLastError(ERROR_SUCCESS);
            return 1;
        }

        // The legacy data tree stores many individual assets as PAQ files:
        // requesting "World\\...\\Object.wmo" resolves the sibling
        // "World\\...\\Object.wmo.MPQ", whose payload is named Object.wmo.
        // This is the SFILE_PAQ branch in the recovered FindFile/OpenEx path.
        // Keep the extracted payload in memory so the temporary archive can be
        // closed immediately while preserving ordinary SFile lifetime rules.
        std::string paqName(filename);
        paqName += ".MPQ";
        const std::filesystem::path paqPath = ResolveFile(nullptr, paqName.c_str());
        std::error_code paqError;
        if (s_directAccess && std::filesystem::is_regular_file(paqPath, paqError)) {
            ArchiveStorage paqStorage;
            paqStorage.path = paqPath;
            if (LoadMpqArchive(paqStorage)) {
                const char* internalName = filename;
                if (const char* slash = std::strrchr(filename, '\\'))
                    internalName = slash + 1;
                if (const char* slash = std::strrchr(internalName, '/'))
                    internalName = slash + 1;
                const MpqBlockEntry* block = FindMpqBlock(paqStorage, internalName);
                auto memory = std::make_unique<MemoryFile>();
                const bool extracted = block &&
                    ExtractMpqFile(paqStorage, internalName, *block,
                                   memory->bytes);
                if (paqStorage.stream) std::fclose(paqStorage.stream);
                paqStorage.stream = nullptr;
                if (extracted) {
                    auto result = new (std::nothrow) SFile(SFILE_PAQ);
                    if (!result) {
                        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
                        return 0;
                    }
                    result->m_fileptr = memory.release();
                    result->m_filename = DuplicateString(filename);
                    result->m_actualname = DuplicateString(paqPath.string());
                    result->m_size = static_cast<unsigned int>(
                        static_cast<MemoryFile*>(result->m_fileptr)->bytes.size());
                    *file = result;
                    SetLastError(ERROR_SUCCESS);
                    return 1;
                }
            }
        }

        std::vector<OpenArchiveEntry> ordered = s_openArchives;
        std::stable_sort(ordered.begin(), ordered.end(), [](const OpenArchiveEntry& left, const OpenArchiveEntry& right) {
            if (left.priority != right.priority) return left.priority > right.priority;
            return left.sequence > right.sequence;
        });
        for (const OpenArchiveEntry& entry : ordered) {
            if (OpenEx(entry.archive, filename, 0, file)) return 1;
        }
        SetLastError(ERROR_FILE_NOT_FOUND);
        return 0;
    }

    const auto resolved = ResolveFile(archive, filename);
    FILE* stream = nullptr;
    if (_wfopen_s(&stream, resolved.c_str(), L"rb") != 0 || !stream) {
        SetLastError(ERROR_FILE_NOT_FOUND);
        return 0;
    }
    auto result = new (std::nothrow) SFile(SFILE_PLAIN);
    if (!result) {
        std::fclose(stream);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    result->m_fileptr = stream;
    result->m_archive = archive;
    result->m_filename = DuplicateString(filename);
    result->m_actualname = DuplicateString(resolved.string());
    std::error_code error;
    const auto length = std::filesystem::file_size(resolved, error);
    result->m_size = error || length > UINT_MAX ? UINT_MAX : static_cast<unsigned int>(length);
    *file = result;
    SetLastError(ERROR_SUCCESS);
    return 1;
}

BOOL __stdcall SFile::Read(SFile* file, void* buffer, unsigned int bytesToRead,
                           unsigned int* bytesRead, SOVERLAPPED* overlapped, void*) {
    if (bytesRead) *bytesRead = 0;
    if (!file || !file->m_fileptr || (!buffer && bytesToRead)) {
        if (file && file->m_zipFile && (!buffer && bytesToRead) == 0) {
            return ZipFileReadFile(file->m_zipFile, buffer, bytesToRead, bytesRead) ? TRUE : FALSE;
        }
        SetLastError(ERROR_INVALID_PARAMETER);
        return FALSE;
    }
    if (file->m_type == SFILE_PAQ) {
        auto* memory = static_cast<MemoryFile*>(file->m_fileptr);
        if (overlapped) {
            file->m_curOffset = std::min<unsigned int>(overlapped->Offset, file->m_size);
            if (overlapped->hEvent) overlapped->hEvent->Reset();
        }
        const unsigned int available = file->m_curOffset < file->m_size ? file->m_size - file->m_curOffset : 0;
        const unsigned int count = std::min(bytesToRead, available);
        if (count) std::memcpy(buffer, memory->bytes.data() + file->m_curOffset, count);
        file->m_curOffset += count;
        if (bytesRead) *bytesRead = count;
        if (overlapped && overlapped->hEvent) overlapped->hEvent->Set();
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    if (file->m_type == SFILE_COMPRESSED) {
        auto* stream = static_cast<MpqFileStream*>(file->m_fileptr);
        const std::uint32_t readOffset = overlapped ? overlapped->Offset : file->m_curOffset;
        const std::uint32_t available = readOffset < file->m_size
            ? file->m_size - readOffset : 0U;
        const std::uint32_t count = std::min(bytesToRead, available);
        if (overlapped && overlapped->hEvent) overlapped->hEvent->Reset();
        if (!ReadMpqFileStream(*stream, readOffset, buffer, count)) {
            if (overlapped && overlapped->hEvent) overlapped->hEvent->Set();
            SetLastError(ERROR_READ_FAULT);
            return FALSE;
        }
        file->m_curOffset = readOffset + count;
        if (bytesRead) *bytesRead = count;
        if (overlapped && overlapped->hEvent) overlapped->hEvent->Set();
        SetLastError(ERROR_SUCCESS);
        return TRUE;
    }
    FILE* stream = static_cast<FILE*>(file->m_fileptr);
    if (overlapped) {
        if (_fseeki64(stream, overlapped->Offset, SEEK_SET) != 0) {
            SetLastError(ERROR_SEEK);
            return FALSE;
        }
        if (overlapped->hEvent) overlapped->hEvent->Reset();
    }
    const std::size_t count = std::fread(buffer, 1, bytesToRead, stream);
    file->m_curOffset = static_cast<unsigned int>(_ftelli64(stream));
    if (bytesRead) *bytesRead = static_cast<unsigned int>(count);
    if (overlapped && overlapped->hEvent) overlapped->hEvent->Set();
    if (count != bytesToRead && std::ferror(stream)) {
        SetLastError(ERROR_READ_FAULT);
        return FALSE;
    }
    SetLastError(ERROR_SUCCESS);
    return TRUE;
}

unsigned int __stdcall SFile::LoadFile(const char* filename, void** buffer, unsigned int* bytes,
                                       unsigned int extraBytes, SOVERLAPPED* overlapped) {
    return Load(nullptr, filename, buffer, bytes, extraBytes, 0, overlapped) ? 1U : 0U;
}

int __stdcall SFile::Load(SArchive* archive, const char* filename, void** buffer, unsigned int* bytes,
                          unsigned int extraBytes, unsigned int flags, SOVERLAPPED* overlapped) {
    if (!buffer) {
        SetLastError(ERROR_INVALID_PARAMETER);
        return 0;
    }
    *buffer = nullptr;
    SFile* file = nullptr;
    if (!OpenEx(archive, filename, flags, &file)) return 0;
    const unsigned int size = GetFileSize(file, nullptr);
    if (size > UINT_MAX - extraBytes) {
        Close(file);
        SetLastError(ERROR_ARITHMETIC_OVERFLOW);
        return 0;
    }
    auto data = new (std::nothrow) unsigned char[size + extraBytes];
    if (!data) {
        Close(file);
        SetLastError(ERROR_NOT_ENOUGH_MEMORY);
        return 0;
    }
    unsigned int read = 0;
    const BOOL ok = Read(file, data, size, &read, overlapped, nullptr);
    Close(file);
    if (!ok || read != size) {
        delete[] data;
        return 0;
    }
    std::memset(data + size, 0, extraBytes);
    *buffer = data;
    if (bytes) *bytes = size;
    return 1;
}

int __stdcall SFile::Unload(void* buffer) {
    delete[] static_cast<unsigned char*>(buffer);
    return 1;
}

unsigned int __stdcall SFile::Close(SFile* file) {
    if (!file) return 0;
    if (file->m_zipFile) ZipFileCloseFile(file->m_zipFile);
    if (file->m_fileptr) {
        if (file->m_type == SFILE_PAQ) delete static_cast<MemoryFile*>(file->m_fileptr);
        else if (file->m_type == SFILE_COMPRESSED)
            delete static_cast<MpqFileStream*>(file->m_fileptr);
        else std::fclose(static_cast<FILE*>(file->m_fileptr));
    }
    file->m_zipFile = nullptr;
    file->m_fileptr = nullptr;
    delete file;
    return 1;
}

int __stdcall SFile::GetActualFileName(SFile* file, char* buffer, unsigned int bufferChars) {
    return file && file->m_actualname && CopyPath(buffer, bufferChars, file->m_actualname) ? 1 : 0;
}

unsigned int __stdcall SFile::GetFileSize(SFile* file, unsigned int* fileSizeHigh) {
    if (fileSizeHigh) *fileSizeHigh = 0;
    if (!file) {
        SetLastError(ERROR_INVALID_HANDLE);
        return UINT_MAX;
    }
    return file->m_zipFile ? ZipFileGetFileSize(file->m_zipFile) : file->m_size;
}

int __stdcall SFile::GetBasePath(char* buffer, unsigned int bufferChars) {
    return CopyPath(buffer, bufferChars, s_basePath) ? 1 : 0;
}

int __stdcall SFile::SetBasePath(const char* path) {
    if (!path) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    s_basePath = std::filesystem::path(path);
    return 1;
}

int __stdcall SFile::SetDataPath(const char* path) {
    if (!path) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    s_dataPath = std::filesystem::path(path);
    return 1;
}

int __stdcall SFile::SetDataPathAlternate(const char* path) {
    if (!path) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    s_alternateDataPath = std::filesystem::path(path);
    return 1;
}

int __stdcall SFile::FileExists(const char* filename) {
    std::error_code error;
    if (s_directAccess && std::filesystem::is_regular_file(ResolveFile(nullptr, filename), error)) return 1;
    for (const OpenArchiveEntry& entry : s_openArchives) {
        const ArchiveStorage* storage = Storage(entry.archive);
        if (storage && !storage->directory && FindMpqBlock(*storage, filename)) return 1;
        if (storage && storage->directory &&
            std::filesystem::is_regular_file(storage->path / std::filesystem::path(filename), error)) return 1;
    }
    return 0;
}

DWORD __stdcall SFile::SetFilePointer(SFile* file, int distanceToMove, int* distanceToMoveHigh, DWORD moveMethod) {
    if (!file) { SetLastError(ERROR_INVALID_HANDLE); return INVALID_SET_FILE_POINTER; }
    if (file->m_zipFile) {
        if (distanceToMoveHigh && *distanceToMoveHigh != 0) { SetLastError(ERROR_INVALID_PARAMETER); return INVALID_SET_FILE_POINTER; }
        if (!ZipFileSetFilePointer(file->m_zipFile, static_cast<unsigned int>(distanceToMove), moveMethod)) return INVALID_SET_FILE_POINTER;
        return ZipFileGetFilePointer(file->m_zipFile);
    }
    if (file->m_type == SFILE_PAQ) {
        if (distanceToMoveHigh && *distanceToMoveHigh != 0) {
            SetLastError(ERROR_INVALID_PARAMETER);
            return INVALID_SET_FILE_POINTER;
        }
        std::int64_t base = 0;
        switch (moveMethod) {
        case FILE_BEGIN: base = 0; break;
        case FILE_CURRENT: base = file->m_curOffset; break;
        case FILE_END: base = file->m_size; break;
        default: SetLastError(ERROR_INVALID_PARAMETER); return INVALID_SET_FILE_POINTER;
        }
        const std::int64_t position = base + distanceToMove;
        if (position < 0 || position > file->m_size) { SetLastError(ERROR_SEEK); return INVALID_SET_FILE_POINTER; }
        file->m_curOffset = static_cast<unsigned int>(position);
        if (distanceToMoveHigh) *distanceToMoveHigh = 0;
        SetLastError(ERROR_SUCCESS);
        return file->m_curOffset;
    }
    if (file->m_type == SFILE_COMPRESSED) {
        if (distanceToMoveHigh && *distanceToMoveHigh != 0) {
            SetLastError(ERROR_INVALID_PARAMETER);
            return INVALID_SET_FILE_POINTER;
        }
        std::int64_t base = 0;
        switch (moveMethod) {
        case FILE_BEGIN: base = 0; break;
        case FILE_CURRENT: base = file->m_curOffset; break;
        case FILE_END: base = file->m_size; break;
        default: SetLastError(ERROR_INVALID_PARAMETER); return INVALID_SET_FILE_POINTER;
        }
        const std::int64_t position = base + distanceToMove;
        if (position < 0 || position > file->m_size) {
            SetLastError(ERROR_SEEK);
            return INVALID_SET_FILE_POINTER;
        }
        file->m_curOffset = static_cast<unsigned int>(position);
        if (distanceToMoveHigh) *distanceToMoveHigh = 0;
        SetLastError(ERROR_SUCCESS);
        return file->m_curOffset;
    }
    if (!file->m_fileptr) { SetLastError(ERROR_INVALID_HANDLE); return INVALID_SET_FILE_POINTER; }
    const int origin = SeekOrigin(moveMethod);
    if (origin < 0) { SetLastError(ERROR_INVALID_PARAMETER); return INVALID_SET_FILE_POINTER; }
    std::int64_t distance = distanceToMove;
    if (distanceToMoveHigh) {
        distance = (static_cast<std::int64_t>(*distanceToMoveHigh) << 32)
                 | static_cast<std::uint32_t>(distanceToMove);
    }
    FILE* stream = static_cast<FILE*>(file->m_fileptr);
    if (_fseeki64(stream, distance, origin) != 0) { SetLastError(ERROR_SEEK); return INVALID_SET_FILE_POINTER; }
    const std::int64_t position = _ftelli64(stream);
    file->m_curOffset = static_cast<unsigned int>(position);
    if (distanceToMoveHigh) *distanceToMoveHigh = static_cast<int>(position >> 32);
    SetLastError(ERROR_SUCCESS);
    return static_cast<DWORD>(position);
}

int __stdcall SFile::EnableDirectAccess(unsigned int access) {
    const bool previous = s_directAccess;
    s_directAccess = access != 0;
    return previous ? 1 : 0;
}

void __stdcall SFile::DisableSFileCheckDisk() { s_checkDisk = false; }
void __stdcall SFile::CreateOverlapped(SOVERLAPPED* overlapped) {
    if (overlapped) { overlapped->Offset = 0; overlapped->hEvent = new (std::nothrow) SEvent; }
}
void __stdcall SFile::DestroyOverlapped(SOVERLAPPED* overlapped) {
    if (overlapped) { delete overlapped->hEvent; overlapped->hEvent = nullptr; }
}
void __stdcall SFile::ResetOverlapped(SOVERLAPPED* overlapped) {
    if (overlapped && overlapped->hEvent) overlapped->hEvent->Reset();
}
void __stdcall SFile::WaitOverlapped(SOVERLAPPED* overlapped) {
    if (overlapped && overlapped->hEvent) overlapped->hEvent->Wait(INFINITE);
}
BOOL __stdcall SFile::PollOverlapped(SOVERLAPPED* overlapped) {
    return overlapped && overlapped->hEvent && overlapped->hEvent->Wait(0) == WAIT_OBJECT_0;
}

void SFile::Destroy() {
    s_basePath.clear(); s_dataPath.clear(); s_alternateDataPath.clear();
    s_directAccess = true; s_checkDisk = true; s_hashEnabled = true;
    s_openArchives.clear();
}

int __stdcall SFile::OpenArchive(const char* archiveName, int priority, char, SArchive** handle) {
    if (!archiveName || !handle) { SetLastError(ERROR_INVALID_PARAMETER); return 0; }
    *handle = nullptr;
    std::filesystem::path path = ResolveFile(nullptr, archiveName);
    std::error_code error;
    if (!std::filesystem::exists(path, error)) { SetLastError(ERROR_FILE_NOT_FOUND); return 0; }
    auto archive = new (std::nothrow) SArchive{};
    if (!archive) { SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
    archive->m_type = path.extension() == ".zip" ? SARCHIVE_ZIP : SARCHIVE_MPQ;
    if (archive->m_type == SARCHIVE_ZIP) {
        archive->m_archive = ZipFileOpenArchive(path.string().c_str());
    } else {
        auto* storage = new (std::nothrow) ArchiveStorage;
        if (!storage) { delete archive; SetLastError(ERROR_NOT_ENOUGH_MEMORY); return 0; }
        storage->path = path;
        storage->directory = std::filesystem::is_directory(path, error);
        if (!storage->directory && !LoadMpqArchive(*storage)) {
            delete storage;
            delete archive;
            SetLastError(ERROR_BAD_FORMAT);
            return 0;
        }
        archive->m_archive = storage;
    }
    if (!archive->m_archive) { delete archive; SetLastError(ERROR_BAD_FORMAT); return 0; }
    s_openArchives.push_back(OpenArchiveEntry{archive, priority, ++s_archiveSequence});
    *handle = archive;
    SetLastError(ERROR_SUCCESS);
    return 1;
}

int __stdcall SFile::CloseArchive(SArchive* archive) {
    if (!archive) return 0;
    s_openArchives.erase(std::remove_if(s_openArchives.begin(), s_openArchives.end(),
        [archive](const OpenArchiveEntry& entry) { return entry.archive == archive; }), s_openArchives.end());
    if (archive->m_type == SARCHIVE_ZIP) ZipFileCloseArchive(static_cast<ZipFileArchive*>(archive->m_archive));
    else {
        auto* storage = Storage(archive);
        if (storage && storage->stream) std::fclose(storage->stream);
        delete storage;
    }
    delete archive;
    return 1;
}

int __stdcall SFile::GetMD5(SFile* file, MD5* sum) {
    if (!file || !sum || !file->m_haveMD5) return 0;
    *sum = file->m_md5;
    return 1;
}

int __stdcall SFile::List(SArchive* archive, int (__fastcall* callback)(char*, void*), void* parameter) {
    if (!archive || !callback) return 0;
    if (archive->m_type == SARCHIVE_ZIP) {
        return ZipFileList(static_cast<ZipFileArchive*>(archive->m_archive), callback, parameter);
    }
    ArchiveStorage* storage = Storage(archive);
    if (storage && !storage->directory) {
        const MpqBlockEntry* listBlock = FindMpqBlock(*storage, "(listfile)");
        if (!listBlock) return 0;
        std::vector<unsigned char> data;
        if (!ExtractMpqFile(*storage, "(listfile)", *listBlock, data)) return 0;
        int count = 0;
        std::size_t begin = 0;
        while (begin < data.size()) {
            std::size_t end = begin;
            while (end < data.size() && data[end] != '\r' && data[end] != '\n' && data[end] != 0) ++end;
            if (end > begin) {
                std::vector<char> name(data.begin() + begin, data.begin() + end);
                name.push_back('\0');
                ++count;
                if (!callback(name.data(), parameter)) break;
            }
            begin = end;
            while (begin < data.size() && (data[begin] == '\r' || data[begin] == '\n' || data[begin] == 0)) ++begin;
        }
        return count;
    }
    const auto root = ArchiveRoot(archive);
    std::error_code error;
    if (!std::filesystem::is_directory(root, error)) return 0;
    int count = 0;
    for (std::filesystem::recursive_directory_iterator it(root, error), end; it != end && !error; it.increment(error)) {
        if (!it->is_regular_file(error)) continue;
        std::string relative = std::filesystem::relative(it->path(), root, error).string();
        if (error) break;
        std::vector<char> mutableName(relative.begin(), relative.end());
        mutableName.push_back('\0');
        ++count;
        if (!callback(mutableName.data(), parameter)) break;
    }
    return count;
}
