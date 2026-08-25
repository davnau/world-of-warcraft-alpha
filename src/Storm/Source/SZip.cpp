#include "Storm/h/SZip.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstring>
#include <memory>
#include <mutex>
#include <new>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

std::mutex s_zipLock;
std::vector<ZipFileArchive*> s_archives;
std::vector<std::unique_ptr<ZipFileDirEntry>> s_directory;
std::unordered_map<ZipFileFCB*, std::vector<unsigned char>> s_openData;

std::string NormalizeName(const char* name) {
    std::string result = name ? name : "";
    for (char& ch : result) {
        if (ch == '/') ch = '\\';
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

bool Signature(const char value[4], const char (&expected)[5]) {
    return std::memcmp(value, expected, 4) == 0;
}

ZipFileDirEntry* FindEntry(const char* filename, ZipFileArchive* archive = nullptr) {
    const std::string wanted = NormalizeName(filename);
    for (auto it = s_directory.rbegin(); it != s_directory.rend(); ++it) {
        if ((!archive || (*it)->archive == archive) && NormalizeName((*it)->filename) == wanted) {
            return it->get();
        }
    }
    return nullptr;
}

bool ReadAt(FILE* file, unsigned int offset, void* output, std::size_t bytes) {
    return file && _fseeki64(file, offset, SEEK_SET) == 0 &&
        (bytes == 0 || std::fread(output, 1, bytes, file) == bytes);
}

class BitReader {
public:
    BitReader(const unsigned char* data, std::size_t size) : m_data(data), m_size(size) {}

    bool Read(unsigned int count, unsigned int& value) {
        value = 0;
        for (unsigned int bit = 0; bit < count; ++bit) {
            if (m_byte >= m_size) return false;
            value |= ((m_data[m_byte] >> m_bit) & 1U) << bit;
            if (++m_bit == 8) { m_bit = 0; ++m_byte; }
        }
        return true;
    }

    void Align() { if (m_bit) { m_bit = 0; ++m_byte; } }
    std::size_t Position() const { return m_byte; }
    void Advance(std::size_t bytes) { m_byte += bytes; }
    bool Available(std::size_t bytes) const { return m_byte + bytes <= m_size; }
    const unsigned char* Current() const { return m_data + m_byte; }

private:
    const unsigned char* m_data;
    std::size_t m_size;
    std::size_t m_byte = 0;
    unsigned int m_bit = 0;
};

unsigned int ReverseBits(unsigned int value, unsigned int count) {
    unsigned int result = 0;
    while (count--) { result = (result << 1) | (value & 1U); value >>= 1; }
    return result;
}

class Huffman {
public:
    bool Build(const std::vector<unsigned int>& lengths) {
        std::array<unsigned int, 16> counts{};
        unsigned int symbols = 0;
        for (unsigned int length : lengths) {
            if (length > 15) return false;
            if (length) {
                ++counts[length];
                ++symbols;
            }
        }
        if (!symbols) return false;

        // RFC 1951 canonical Huffman trees must not be over-subscribed.  An
        // incomplete tree is legal (the one-distance-code case is common).
        int available = 1;
        for (unsigned int bits = 1; bits <= 15; ++bits) {
            available = (available << 1) - static_cast<int>(counts[bits]);
            if (available < 0) return false;
        }

        std::array<unsigned int, 16> next{};
        unsigned int code = 0;
        for (unsigned int bits = 1; bits <= 15; ++bits) {
            code = (code + counts[bits - 1]) << 1;
            next[bits] = code;
        }

        m_nodes.clear();
        m_nodes.reserve(static_cast<std::size_t>(symbols) * 2U + 1U);
        m_nodes.emplace_back();
        for (std::size_t symbol = 0; symbol < lengths.size(); ++symbol) {
            const unsigned int length = lengths[symbol];
            if (!length) continue;

            const unsigned int reversed = ReverseBits(next[length]++, length);
            unsigned int node = 0;
            for (unsigned int depth = 0; depth < length; ++depth) {
                if (m_nodes[node].symbol >= 0) return false;
                const unsigned int bit = (reversed >> depth) & 1U;
                int child = m_nodes[node].child[bit];
                if (child < 0) {
                    child = static_cast<int>(m_nodes.size());
                    m_nodes[node].child[bit] = child;
                    m_nodes.emplace_back();
                }
                node = static_cast<unsigned int>(child);
            }
            if (m_nodes[node].symbol >= 0 || m_nodes[node].child[0] >= 0 ||
                m_nodes[node].child[1] >= 0) return false;
            m_nodes[node].symbol = static_cast<int>(symbol);
        }
        return true;
    }

    bool Decode(BitReader& bits, unsigned int& symbol) const {
        if (m_nodes.empty()) return false;
        unsigned int node = 0;
        for (unsigned int length = 0; length < 15; ++length) {
            unsigned int bit = 0;
            if (!bits.Read(1, bit)) return false;
            const int child = m_nodes[node].child[bit];
            if (child < 0) return false;
            node = static_cast<unsigned int>(child);
            if (m_nodes[node].symbol < 0) continue;
            symbol = static_cast<unsigned int>(m_nodes[node].symbol);
            return true;
        }
        return false;
    }

private:
    struct Node {
        int child[2] = {-1, -1};
        int symbol = -1;
    };
    std::vector<Node> m_nodes;
};

bool BuildFixed(Huffman& literal, Huffman& distance) {
    std::vector<unsigned int> lengths(288, 8);
    std::fill(lengths.begin() + 144, lengths.begin() + 256, 9);
    std::fill(lengths.begin() + 256, lengths.begin() + 280, 7);
    std::fill(lengths.begin() + 280, lengths.end(), 8);
    return literal.Build(lengths) && distance.Build(std::vector<unsigned int>(32, 5));
}

bool BuildDynamic(BitReader& bits, Huffman& literal, Huffman& distance) {
    unsigned int hlit = 0, hdist = 0, hclen = 0;
    if (!bits.Read(5, hlit) || !bits.Read(5, hdist) || !bits.Read(4, hclen)) return false;
    hlit += 257; hdist += 1; hclen += 4;
    constexpr unsigned int order[] = {16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15};
    std::vector<unsigned int> codeLengths(19, 0);
    for (unsigned int index = 0; index < hclen; ++index) {
        if (!bits.Read(3, codeLengths[order[index]])) return false;
    }
    Huffman codeTree;
    if (!codeTree.Build(codeLengths)) return false;
    std::vector<unsigned int> lengths;
    lengths.reserve(hlit + hdist);
    while (lengths.size() < hlit + hdist) {
        unsigned int symbol = 0;
        if (!codeTree.Decode(bits, symbol)) return false;
        if (symbol <= 15) {
            lengths.push_back(symbol);
            continue;
        }
        unsigned int extra = 0, repeat = 0, value = 0;
        if (symbol == 16) {
            if (lengths.empty() || !bits.Read(2, extra)) return false;
            repeat = extra + 3; value = lengths.back();
        } else if (symbol == 17) {
            if (!bits.Read(3, extra)) return false;
            repeat = extra + 3;
        } else if (symbol == 18) {
            if (!bits.Read(7, extra)) return false;
            repeat = extra + 11;
        } else return false;
        if (lengths.size() + repeat > hlit + hdist) return false;
        lengths.insert(lengths.end(), repeat, value);
    }
    return literal.Build({lengths.begin(), lengths.begin() + hlit}) &&
        distance.Build({lengths.begin() + hlit, lengths.end()});
}

bool InflateRaw(const unsigned char* input, std::size_t inputSize,
                 std::vector<unsigned char>& output, std::size_t expectedSize,
                 std::size_t outputLimit) {
    static constexpr unsigned int lengthBase[] = {
        3,4,5,6,7,8,9,10,11,13,15,17,19,23,27,31,35,43,51,59,67,83,99,115,
        131,163,195,227,258
    };
    static constexpr unsigned int lengthExtra[] = {
        0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0
    };
    static constexpr unsigned int distanceBase[] = {
        1,2,3,4,5,7,9,13,17,25,33,49,65,97,129,193,257,385,513,769,1025,
        1537,2049,3073,4097,6145,8193,12289,16385,24577
    };
    static constexpr unsigned int distanceExtra[] = {
        0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13
    };

    BitReader bits(input, inputSize);
    output.clear();
    if (expectedSize && expectedSize > outputLimit) return false;
    output.reserve(expectedSize ? expectedSize : outputLimit);
    bool finalBlock = false;
    while (!finalBlock) {
        unsigned int final = 0, type = 0;
        if (!bits.Read(1, final) || !bits.Read(2, type)) return false;
        finalBlock = final != 0;
        if (type == 0) {
            bits.Align();
            if (!bits.Available(4)) return false;
            const unsigned char* header = bits.Current();
            const unsigned int length = header[0] | (header[1] << 8);
            const unsigned int inverse = header[2] | (header[3] << 8);
            bits.Advance(4);
            if ((length ^ 0xFFFFU) != inverse || !bits.Available(length)) return false;
            if (length > outputLimit - output.size()) return false;
            output.insert(output.end(), bits.Current(), bits.Current() + length);
            bits.Advance(length);
            continue;
        }
        if (type == 3) return false;
        Huffman literalTree, distanceTree;
        if ((type == 1 && !BuildFixed(literalTree, distanceTree)) ||
            (type == 2 && !BuildDynamic(bits, literalTree, distanceTree))) return false;
        for (;;) {
            unsigned int symbol = 0;
            if (!literalTree.Decode(bits, symbol)) return false;
            if (symbol < 256) {
                if (output.size() == outputLimit) return false;
                output.push_back(static_cast<unsigned char>(symbol));
            } else if (symbol == 256) {
                break;
            } else {
                if (symbol < 257 || symbol > 285) return false;
                const unsigned int lengthIndex = symbol - 257;
                unsigned int extra = 0;
                if (!bits.Read(lengthExtra[lengthIndex], extra)) return false;
                const unsigned int length = lengthBase[lengthIndex] + extra;
                unsigned int distanceSymbol = 0;
                if (!distanceTree.Decode(bits, distanceSymbol) || distanceSymbol >= 30) return false;
                if (!bits.Read(distanceExtra[distanceSymbol], extra)) return false;
                const unsigned int distance = distanceBase[distanceSymbol] + extra;
                if (distance == 0 || distance > output.size()) return false;
                if (length > outputLimit - output.size()) return false;
                for (unsigned int index = 0; index < length; ++index) {
                    output.push_back(output[output.size() - distance]);
                }
            }
        }
    }
    return !expectedSize || output.size() == expectedSize;
}

bool Extract(ZipFileDirEntry* entry, std::vector<unsigned char>& output) {
    if (!entry || !entry->archive || !entry->archive->file) return false;
    std::vector<unsigned char> compressed(entry->compressedSize);
    if (!ReadAt(entry->archive->file, entry->startOffset, compressed.data(), compressed.size())) return false;
    if (entry->compressionMethod == 0) {
        output = std::move(compressed);
        return output.size() == entry->uncompressedSize;
    }
    return entry->compressionMethod == 8 &&
        InflateRaw(compressed.data(), compressed.size(), output, entry->uncompressedSize,
                   entry->uncompressedSize);
}

} // namespace

int StormInflateRaw(const void* source, unsigned int sourceBytes, void* destination, unsigned int* destinationBytes) {
    if (!source || !destination || !destinationBytes) return 0;
    const unsigned int capacity = *destinationBytes;
    std::vector<unsigned char> output;
    if (!InflateRaw(static_cast<const unsigned char*>(source), sourceBytes, output, 0, capacity)) return 0;
    std::memcpy(destination, output.data(), output.size());
    *destinationBytes = static_cast<unsigned int>(output.size());
    return 1;
}

void CentralDirectoryHeader::EndianCorrect() {}
void CentralDirectoryFileHeader::EndianCorrect() {}
void LocalFileHeader::EndianCorrect() {}
void DataDescriptor::EndianCorrect() {}

Flags::Flags() : m_value(0) {}
void Flags::Set(unsigned int bit) { m_value |= bit; }
void Flags::Clear(unsigned int bit) { m_value &= ~bit; }
unsigned int Flags::IsSet(unsigned int bit) const { return m_value & bit; }
BOOL Flags::IsClear(unsigned int bit) const { return (m_value & bit) == 0; }

ZipFileArchive::ZipFileArchive()
    : m_previousLink(nullptr), m_nextLink(nullptr), file(nullptr), filename{}, openFileCount(0) {}

ZipFileArchive::~ZipFileArchive() {
    if (file) std::fclose(file);
    file = nullptr;
    s_directory.erase(std::remove_if(s_directory.begin(), s_directory.end(),
        [this](const auto& entry) { return entry->archive == this; }), s_directory.end());
}

FILE* ZipFileArchive::Open(const char* archiveName) {
    if (!archiveName || fopen_s(&file, archiveName, "rb") != 0) return nullptr;
    strncpy_s(filename, archiveName, _TRUNCATE);
    return file;
}

BOOL ZipFileArchive::GetCentralDirectoryHeader(CentralDirectoryHeader* header) {
    if (!file || !header || _fseeki64(file, 0, SEEK_END) != 0) return FALSE;
    const auto fileSize = _ftelli64(file);
    if (fileSize < static_cast<long long>(sizeof(*header))) return FALSE;
    const auto searchStart = std::max<long long>(0, fileSize - 65557);
    std::vector<unsigned char> tail(static_cast<std::size_t>(fileSize - searchStart));
    if (_fseeki64(file, searchStart, SEEK_SET) != 0 ||
        std::fread(tail.data(), 1, tail.size(), file) != tail.size()) return FALSE;
    for (std::size_t pos = tail.size() - sizeof(*header) + 1; pos-- > 0;) {
        if (std::memcmp(tail.data() + pos, "PK\x05\x06", 4) != 0) continue;
        std::memcpy(header, tail.data() + pos, sizeof(*header));
        header->EndianCorrect();
        return header->thisDiskNumber == header->directoryStartDiskNumber &&
            header->directoryEntriesThisDisk == header->directoryEntriesTotal;
    }
    return FALSE;
}

int ZipFileArchive::ProcessCentralDirectory(CentralDirectoryHeader* header) {
    if (!header || _fseeki64(file, header->centralDirectoryOffset, SEEK_SET) != 0) return 0;
    for (unsigned int index = 0; index < header->directoryEntriesTotal; ++index) {
        if (!ReadCentralDirectoryFileHeader()) return 0;
    }
    return 1;
}

int ZipFileArchive::ReadCentralDirectoryFileHeader() {
    CentralDirectoryFileHeader central{};
    if (std::fread(&central, sizeof(central), 1, file) != 1 || !Signature(central.signature, "PK\x01\x02")) return 0;
    if (central.filenameSize >= 256 || (central.generalFlags & 1U) ||
        (central.compressionMethod != 0 && central.compressionMethod != 8)) return 0;
    std::array<char, 256> centralName{};
    if (central.filenameSize && std::fread(centralName.data(), 1, central.filenameSize, file) != central.filenameSize) return 0;
    ConvertFromZip(centralName.data());
    if (_fseeki64(file, central.extraFieldSize + central.commentSize, SEEK_CUR) != 0) return 0;
    const auto savedOffset = _ftelli64(file);
    LocalFileHeader local{};
    if (!ReadAt(file, central.localHeaderOffset, &local, sizeof(local)) || !Signature(local.signature, "PK\x03\x04") ||
        local.filenameSize >= 256) return 0;
    std::array<char, 256> localName{};
    if (local.filenameSize && std::fread(localName.data(), 1, local.filenameSize, file) != local.filenameSize) return 0;
    ConvertFromZip(localName.data());
    if (NormalizeName(centralName.data()) != NormalizeName(localName.data()) ||
        _fseeki64(file, local.extraFieldSize, SEEK_CUR) != 0) return 0;
    const auto dataOffset = _ftelli64(file);
    if (dataOffset < 0 || dataOffset > UINT_MAX) return 0;
    auto entry = std::make_unique<ZipFileDirEntry>();
    entry->archive = this;
    strcpy_s(entry->filename, centralName.data());
    entry->startOffset = static_cast<unsigned int>(dataOffset);
    entry->compressedSize = central.compressedSize;
    entry->uncompressedSize = central.uncompressedSize;
    entry->compressionMethod = central.compressionMethod;
    if (FindEntry(entry->filename, this)) return 0;
    s_directory.push_back(std::move(entry));
    return _fseeki64(file, savedOffset, SEEK_SET) == 0;
}

ZipFileDirEntry::ZipFileDirEntry()
    : m_hashLinks{}, archive(nullptr), filename{}, startOffset(0), compressedSize(0),
      uncompressedSize(0), compressionMethod(0) {}
ZipFileDirEntry::~ZipFileDirEntry() = default;

ZipFileFCB::ZipFileFCB()
    : dirEntry(nullptr), flags(), targetPosition(0), compressedPosition(0),
      uncompressedPosition(0), zlibStream{}, compressedData{} {}
ZipFileFCB::~ZipFileFCB() = default;
int ZipFileFCB::SetFault() { flags.Set(1U); return 0; }

void ConvertFromZip(char* filename) {
    if (!filename) return;
    for (; *filename; ++filename) if (*filename == '/') *filename = '\\';
}

ZipFileArchive* ZipFileOpenArchive(const char* archiveName) {
    std::lock_guard lock(s_zipLock);
    auto archive = std::make_unique<ZipFileArchive>();
    CentralDirectoryHeader header{};
    if (!archive->Open(archiveName) || !archive->GetCentralDirectoryHeader(&header) ||
        !archive->ProcessCentralDirectory(&header)) return nullptr;
    ZipFileArchive* result = archive.release();
    s_archives.push_back(result);
    return result;
}

int ZipFileCloseArchive(ZipFileArchive* archive) {
    if (!archive) return 0;
    std::lock_guard lock(s_zipLock);
    if (archive->openFileCount) return 0;
    s_archives.erase(std::remove(s_archives.begin(), s_archives.end(), archive), s_archives.end());
    delete archive;
    return 1;
}

int __fastcall ZipFileFileExists(const char* filename, ZipFileDirEntry** entry) {
    std::lock_guard lock(s_zipLock);
    ZipFileDirEntry* found = FindEntry(filename);
    if (entry) *entry = found;
    return found ? 1 : 0;
}

ZipFileFCB* __fastcall ZipFileOpenFile(const char* filename, ZipFileArchive* archive) {
    if (!filename) return nullptr;
    std::lock_guard lock(s_zipLock);
    ZipFileDirEntry* entry = FindEntry(filename, archive);
    if (!entry) return nullptr;
    std::vector<unsigned char> data;
    if (!Extract(entry, data)) return nullptr;
    auto file = std::make_unique<ZipFileFCB>();
    file->dirEntry = entry;
    file->compressedPosition = entry->compressedSize;
    file->flags.Set(2U | 4U);
    ZipFileFCB* result = file.release();
    s_openData.emplace(result, std::move(data));
    ++entry->archive->openFileCount;
    return result;
}

int ZipFileCloseFile(ZipFileFCB* file) {
    if (!file) return 0;
    std::lock_guard lock(s_zipLock);
    if (file->dirEntry && file->dirEntry->archive->openFileCount) --file->dirEntry->archive->openFileCount;
    s_openData.erase(file);
    delete file;
    return 1;
}

int __fastcall ZipFileSetFilePointer(ZipFileFCB* file, unsigned int offset, unsigned int origin) {
    if (!file || file->flags.IsSet(1U) || origin > 2) return 0;
    std::uint64_t position = offset;
    if (origin == 1) position += file->targetPosition;
    else if (origin == 2) position += file->dirEntry->uncompressedSize;
    if (position > file->dirEntry->uncompressedSize) return file->SetFault();
    file->targetPosition = static_cast<unsigned int>(position);
    file->uncompressedPosition = file->targetPosition;
    return 1;
}

unsigned int ZipFileGetFilePointer(ZipFileFCB* file) { return file ? file->targetPosition : 0; }
unsigned int ZipFileGetFileSize(ZipFileFCB* file) { return file && file->dirEntry ? file->dirEntry->uncompressedSize : 0; }

int __fastcall ZipFileReadFile(ZipFileFCB* file, void* buffer, std::size_t bytesToRead, unsigned int* bytesRead) {
    if (bytesRead) *bytesRead = 0;
    if (!file || (!buffer && bytesToRead) || file->flags.IsSet(1U)) return 0;
    std::lock_guard lock(s_zipLock);
    const auto found = s_openData.find(file);
    if (found == s_openData.end()) return file->SetFault();
    const std::size_t available = found->second.size() - std::min<std::size_t>(file->targetPosition, found->second.size());
    const std::size_t count = std::min(bytesToRead, available);
    if (count) std::memcpy(buffer, found->second.data() + file->targetPosition, count);
    file->targetPosition += static_cast<unsigned int>(count);
    file->uncompressedPosition = file->targetPosition;
    if (bytesRead) *bytesRead = static_cast<unsigned int>(count);
    return 1;
}

int __fastcall ZipFileLoadFile(const char* filename, unsigned char** buffer, unsigned int* bytes) {
    if (!filename || !buffer) return 0;
    *buffer = nullptr;
    ZipFileFCB* file = ZipFileOpenFile(filename, nullptr);
    if (!file) return 0;
    const unsigned int size = ZipFileGetFileSize(file);
    auto data = new (std::nothrow) unsigned char[size];
    unsigned int read = 0;
    const int result = data && ZipFileReadFile(file, data, size, &read) && read == size;
    ZipFileCloseFile(file);
    if (!result) { delete[] data; return 0; }
    *buffer = data;
    if (bytes) *bytes = size;
    return 1;
}

void ZipFileUnloadFile(void* buffer) { delete[] static_cast<unsigned char*>(buffer); }

int __fastcall ZipFileList(ZipFileArchive* archive, int (__fastcall* callback)(char*, void*), void* parameter) {
    if (!archive || !callback) return 0;
    std::lock_guard lock(s_zipLock);
    for (const auto& entry : s_directory) {
        if (entry->archive == archive && !callback(entry->filename, parameter)) break;
    }
    return 1;
}

WowFileSystemProvider::~WowFileSystemProvider() = default;
WowFile::WowFile(WowFileSystemProvider* provider) : m_provider(provider) {}
WowFile::~WowFile() = default;
unsigned char WowFile::Close() { return m_provider ? m_provider->Close(this) : 0; }
TestFile::TestFile(WowFileSystemProvider* provider, FILE* file) : WowFile(provider), m_f(file) {}
WowFile* TestFileSystemProvider::Open(const char* filename) {
    FILE* file = nullptr;
    return filename && fopen_s(&file, filename, "rb") == 0 ? new (std::nothrow) TestFile(this, file) : nullptr;
}
unsigned char TestFileSystemProvider::Close(WowFile* file) {
    auto testFile = static_cast<TestFile*>(file);
    if (!testFile) return 0;
    if (testFile->m_f) std::fclose(testFile->m_f);
    delete testFile;
    return 1;
}
WowFileSystem::WowFileSystem() : m_providerList(nullptr) {}
WowFileSystem::~WowFileSystem() = default;
void WowFileSystem::RegisterProvider(WowFileSystemProvider* provider) { if (!m_providerList) m_providerList = provider; }
void WowFileSystem::UnregisterProvider(WowFileSystemProvider* provider) { if (m_providerList == provider) m_providerList = nullptr; }
WowFile* WowFileSystem::Open(const char* filename) { return m_providerList ? m_providerList->Open(filename) : nullptr; }

void FSTest() {}
