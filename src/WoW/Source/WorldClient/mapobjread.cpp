#include "WoW/Source/WorldClient/mapobjread.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Services/texture.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <span>
#include <string_view>
#include <vector>

namespace {

constexpr std::uint32_t Token(char a, char b, char c, char d) {
    return static_cast<std::uint8_t>(a) |
        (static_cast<std::uint32_t>(static_cast<std::uint8_t>(b)) << 8U) |
        (static_cast<std::uint32_t>(static_cast<std::uint8_t>(c)) << 16U) |
        (static_cast<std::uint32_t>(static_cast<std::uint8_t>(d)) << 24U);
}

constexpr std::uint32_t MVER = Token('R', 'E', 'V', 'M');
constexpr std::uint32_t MOMO = Token('O', 'M', 'O', 'M');
constexpr std::uint32_t MOHD = Token('D', 'H', 'O', 'M');
constexpr std::uint32_t MOTX = Token('X', 'T', 'O', 'M');
constexpr std::uint32_t MOMT = Token('T', 'M', 'O', 'M');
constexpr std::uint32_t MOGN = Token('N', 'G', 'O', 'M');
constexpr std::uint32_t MOGI = Token('I', 'G', 'O', 'M');
constexpr std::uint32_t MOPV = Token('V', 'P', 'O', 'M');
constexpr std::uint32_t MOPT = Token('T', 'P', 'O', 'M');
constexpr std::uint32_t MOPR = Token('R', 'P', 'O', 'M');
constexpr std::uint32_t MOLT = Token('T', 'L', 'O', 'M');
constexpr std::uint32_t MODS = Token('S', 'D', 'O', 'M');
constexpr std::uint32_t MODN = Token('N', 'D', 'O', 'M');
constexpr std::uint32_t MODD = Token('D', 'D', 'O', 'M');
constexpr std::uint32_t MFOG = Token('G', 'O', 'F', 'M');
constexpr std::uint32_t MOGP = Token('P', 'G', 'O', 'M');
constexpr std::uint32_t MOPY = Token('Y', 'P', 'O', 'M');
constexpr std::uint32_t MOVT = Token('T', 'V', 'O', 'M');
constexpr std::uint32_t MONR = Token('R', 'N', 'O', 'M');
constexpr std::uint32_t MOTV = Token('V', 'T', 'O', 'M');
// On disk this chunk is "MOLV" (little-endian token bytes "VLOM").
// The previous MOVL spelling transposed V/L and rejected every WMO group at
// its second UV stream before lightmap descriptors could be reached.
constexpr std::uint32_t MOLV = Token('V', 'L', 'O', 'M');
constexpr std::uint32_t MOIN = Token('N', 'I', 'O', 'M');
constexpr std::uint32_t MOBA = Token('A', 'B', 'O', 'M');
constexpr std::uint32_t MOLM = Token('M', 'L', 'O', 'M');
constexpr std::uint32_t MOLD = Token('D', 'L', 'O', 'M');

std::uint32_t ReadU32(const std::uint8_t* bytes) {
    std::uint32_t value = 0;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
}

std::uint16_t ReadU16(const std::uint8_t* bytes) {
    std::uint16_t value = 0;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
}

float ReadFloat(const std::uint8_t* bytes) {
    float value = 0.0F;
    std::memcpy(&value, bytes, sizeof(value));
    return value;
}

struct ChunkView {
    const std::uint8_t* data = nullptr;
    std::uint32_t size = 0;
};

bool ReadChunk(std::span<const std::uint8_t> file, std::size_t& cursor,
               std::uint32_t expected, ChunkView& result) {
    if (cursor + 8U > file.size() || ReadU32(file.data() + cursor) != expected)
        return false;
    const std::uint32_t bytes = ReadU32(file.data() + cursor + 4U);
    cursor += 8U;
    if (bytes > file.size() - cursor) return false;
    result = {file.data() + cursor, bytes};
    cursor += bytes;
    return true;
}

std::string StringAt(const std::string& block, std::uint32_t offset) {
    if (offset >= block.size()) return {};
    const char* begin = block.data() + offset;
    const std::size_t remaining = block.size() - offset;
    const std::size_t length = strnlen(begin, remaining);
    if (length == remaining) return {};
    return std::string(begin, length);
}

struct GroupInfo {
    std::uint32_t offset = 0;
    std::uint32_t size = 0;
    std::uint32_t flags = 0;
    NTempest::CAaBox aaBox{};
    std::uint32_t nameIndex = 0;
};

void LightmapTextureCallback(EGxTexCommand command, unsigned width,
                             unsigned, unsigned, unsigned, void* userArg,
                             unsigned* stride, const void** texels) {
    auto* lightmap = static_cast<CMapObjLightmapTexture*>(userArg);
    if (!lightmap || command != GxTex_Lock) return;
    if (stride) *stride = ((width + 3U) / 4U) * 8U;
    if (texels) *texels = lightmap->texels.data();
}

bool ParseGroup(std::span<const std::uint8_t> file, const GroupInfo& info,
                const std::string& groupNames, CMapObjGroup& group) {
    if (info.offset + 136U > file.size() ||
        ReadU32(file.data() + info.offset) != MOGP)
        return false;
    const std::uint32_t declaredBytes = ReadU32(file.data() + info.offset + 4U);
    const std::size_t groupEnd = (std::min)(
        file.size(), static_cast<std::size_t>(info.offset) + 8U + declaredBytes);
    if (groupEnd < info.offset + 136U) return false;

    group.infoFlags = info.flags;
    group.bounds = info.aaBox;
    group.flags = ReadU32(file.data() + info.offset + 16U);
    group.name = StringAt(groupNames, ReadU32(file.data() + info.offset + 8U));
    std::size_t cursor = info.offset + 136U;
    ChunkView poly;
    ChunkView vertices;
    ChunkView normals;
    ChunkView texCoords;
    ChunkView lightmapCoords;
    ChunkView indices;
    ChunkView batches;
    const std::span<const std::uint8_t> bounded(file.data(), groupEnd);
    if (!ReadChunk(bounded, cursor, MOPY, poly) ||
        !ReadChunk(bounded, cursor, MOVT, vertices) ||
        !ReadChunk(bounded, cursor, MONR, normals) ||
        !ReadChunk(bounded, cursor, MOTV, texCoords) ||
        !ReadChunk(bounded, cursor, MOLV, lightmapCoords) ||
        !ReadChunk(bounded, cursor, MOIN, indices) ||
        !ReadChunk(bounded, cursor, MOBA, batches))
        return false;
    if (vertices.size % sizeof(NTempest::C3Vector) != 0U ||
        normals.size % sizeof(NTempest::C3Vector) != 0U ||
        texCoords.size % sizeof(NTempest::C2Vector) != 0U ||
        lightmapCoords.size % sizeof(NTempest::C2Vector) != 0U ||
        indices.size % sizeof(std::uint16_t) != 0U ||
        batches.size % 24U != 0U)
        return false;

    group.vertices.resize(vertices.size / sizeof(NTempest::C3Vector));
    group.normals.resize(normals.size / sizeof(NTempest::C3Vector));
    group.textureCoordinates.resize(texCoords.size / sizeof(NTempest::C2Vector));
    group.lightmapCoordinates.resize(
        lightmapCoords.size / sizeof(NTempest::C2Vector));
    group.indices.resize(indices.size / sizeof(std::uint16_t));
    std::memcpy(group.vertices.data(), vertices.data, vertices.size);
    std::memcpy(group.normals.data(), normals.data, normals.size);
    std::memcpy(group.textureCoordinates.data(), texCoords.data, texCoords.size);
    std::memcpy(group.lightmapCoordinates.data(), lightmapCoords.data,
                lightmapCoords.size);
    std::memcpy(group.indices.data(), indices.data, indices.size);
    if (group.normals.size() != group.vertices.size())
        group.normals.assign(group.vertices.size(), {0.0F, 0.0F, 1.0F});
    if (group.textureCoordinates.size() != group.vertices.size())
        group.textureCoordinates.assign(group.vertices.size(), {});
    group.vertexColors.assign(group.vertices.size(),
                              NTempest::CImVector(0xffffffffU));

    group.batches.reserve(batches.size / 24U);
    for (std::uint32_t offset = 0; offset < batches.size; offset += 24U) {
        const std::uint8_t* disk = batches.data + offset;
        CMapObjBatch batch;
        batch.lightMap = disk[0];
        batch.texture = disk[1];
        batch.startIndex = ReadU16(disk + 14U);
        batch.count = ReadU16(disk + 16U);
        batch.minIndex = ReadU16(disk + 18U);
        batch.maxIndex = ReadU16(disk + 20U);
        batch.flags = disk[22U];
        if (batch.startIndex <= group.indices.size() &&
            batch.count <= group.indices.size() - batch.startIndex)
            group.batches.push_back(batch);
    }


    // Groups carrying flag 0x2 append MLOM lightmap descriptors followed by
    // DLOM records. The recovered SMOLightmapTex record is 0x8004 bytes: a
    // complete 256x256 DXT1 surface plus the runtime texture-handle slot.
    // Scan optional chunks because BSP, doodad, color, and light references
    // can precede these records according to the group flags.
    std::size_t optionalCursor = cursor;
    while (optionalCursor + 8U <= groupEnd) {
        const std::uint32_t token = ReadU32(file.data() + optionalCursor);
        const std::uint32_t optionalBytes =
            ReadU32(file.data() + optionalCursor + 4U);
        optionalCursor += 8U;
        if (optionalBytes > groupEnd - optionalCursor) break;
        if (token == MOLD && optionalBytes % 0x8004U == 0U) {
            const std::size_t count = optionalBytes / 0x8004U;
            group.lightmapTextures.resize(count);
            for (std::size_t lightmapIndex = 0U;
                 lightmapIndex < count; ++lightmapIndex) {
                CMapObjLightmapTexture& lightmap =
                    group.lightmapTextures[lightmapIndex];
                const std::uint8_t* record = file.data() + optionalCursor +
                    lightmapIndex * 0x8004U;
                lightmap.texels.assign(record, record + 0x8000U);
            }
        }
        optionalCursor += optionalBytes;
    }

    const CGxTexFlags lightmapFlags(GxTex_Linear, false, false, false,
                                    false, false, 1U);
    unsigned createdLightmaps = 0U;
    for (CMapObjLightmapTexture& lightmap : group.lightmapTextures) {
        if (GxTexCreate(256U, 256U, GxTex_Dxt1, lightmapFlags, &lightmap,
                        LightmapTextureCallback, &lightmap.texture)) {
            ++createdLightmaps;
        }
    }
    group.loaded = !group.vertices.empty() && !group.indices.empty() &&
        !group.batches.empty();
    if (group.loaded && (!group.lightmapTextures.empty() || (group.flags & 2U))) {
        std::fprintf(stderr,
                     "MAPOBJ_GROUP_LIGHTMAP name=%s flags=%08X maps=%u/%zu\n",
                     group.name.c_str(), group.flags, createdLightmaps,
                     group.lightmapTextures.size());
    }
    return group.loaded;
}

} // namespace

bool CMapObj::Read(const char* fileName) {
    Destroy();
    if (!fileName || !*fileName) return false;
    name = fileName;
    std::string lowerName = name;
    std::transform(lowerName.begin(), lowerName.end(), lowerName.begin(),
                   [](unsigned char value) {
                       return static_cast<char>(std::tolower(value));
                   });
    forceTwoSided = lowerName.find("humantwostory") != std::string::npos ||
        lowerName.find("human_farm") != std::string::npos ||
        lowerName.find("humanfarm") != std::string::npos;
    void* rawData = nullptr;
    unsigned rawBytes = 0;
    if (!SFile::LoadFile(fileName, &rawData, &rawBytes, 0, nullptr) ||
        !rawData || rawBytes < 20U) {
        std::fprintf(stderr,
                     "MAPOBJ_READ_OPEN_FAILED file=%s bytes=%u error=%lu\n",
                     fileName, rawBytes, GetLastError());
        return false;
    }
    const std::span<const std::uint8_t> file(
        static_cast<const std::uint8_t*>(rawData), rawBytes);
    auto unload = [&]() { SFile::Unload(rawData); };
    if (ReadU32(file.data()) != MVER || ReadU32(file.data() + 8U) != 14U ||
        ReadU32(file.data() + 12U) != MOMO) {
        std::fprintf(stderr,
                     "MAPOBJ_READ_HEADER_FAILED file=%s size=%u tokens=%08X "
                     "version=%u wrapper=%08X\n",
                     fileName, rawBytes, ReadU32(file.data()),
                     ReadU32(file.data() + 8U), ReadU32(file.data() + 12U));
        unload();
        return false;
    }
    const std::uint32_t rootBytes = ReadU32(file.data() + 16U);
    if (rootBytes > file.size() - 20U) {
        std::fprintf(stderr,
                     "MAPOBJ_READ_ROOT_SIZE_FAILED file=%s root=%u size=%zu\n",
                     fileName, rootBytes, file.size());
        unload();
        return false;
    }

    std::size_t cursor = 20U;
    ChunkView header;
    ChunkView textureBlock;
    ChunkView materialBlock;
    ChunkView groupNameBlock;
    ChunkView groupInfoBlock;
    ChunkView ignored;
    const bool rootValid =
        ReadChunk(file, cursor, MOHD, header) &&
        ReadChunk(file, cursor, MOTX, textureBlock) &&
        ReadChunk(file, cursor, MOMT, materialBlock) &&
        ReadChunk(file, cursor, MOGN, groupNameBlock) &&
        ReadChunk(file, cursor, MOGI, groupInfoBlock) &&
        ReadChunk(file, cursor, MOPV, ignored) &&
        ReadChunk(file, cursor, MOPT, ignored) &&
        ReadChunk(file, cursor, MOPR, ignored) &&
        ReadChunk(file, cursor, MOLT, ignored) &&
        ReadChunk(file, cursor, MODS, ignored) &&
        ReadChunk(file, cursor, MODN, ignored) &&
        ReadChunk(file, cursor, MODD, ignored) &&
        ReadChunk(file, cursor, MFOG, ignored);
    if (!rootValid || header.size < 64U || materialBlock.size % 44U != 0U ||
        groupInfoBlock.size % 40U != 0U) {
        std::fprintf(stderr,
                     "MAPOBJ_READ_ROOT_FAILED file=%s valid=%u cursor=%zu "
                     "header=%u materials=%u groupInfo=%u\n",
                     fileName, rootValid ? 1U : 0U, cursor, header.size,
                     materialBlock.size, groupInfoBlock.size);
        unload();
        return false;
    }

    textureNames.assign(reinterpret_cast<const char*>(textureBlock.data),
                        textureBlock.size);
    groupNames.assign(reinterpret_cast<const char*>(groupNameBlock.data),
                      groupNameBlock.size);
    materials.reserve(materialBlock.size / 44U);
    for (std::uint32_t offset = 0; offset < materialBlock.size; offset += 44U) {
        const std::uint8_t* disk = materialBlock.data + offset;
        CMapObjMaterial material;
        material.version = ReadU32(disk);
        material.flags = ReadU32(disk + 4U);
        material.blendMode = ReadU32(disk + 8U);
        material.diffuseNameIndex = ReadU32(disk + 12U);
        material.sidnColor = NTempest::CImVector(ReadU32(disk + 16U));
        material.frameSidnColor = NTempest::CImVector(ReadU32(disk + 20U));
        material.envNameIndex = ReadU32(disk + 24U);
        material.diffuseColor = NTempest::CImVector(ReadU32(disk + 28U));
        material.groundType = ReadU32(disk + 32U);
        materials.push_back(material);
    }

    std::vector<GroupInfo> groupInfo;
    groupInfo.reserve(groupInfoBlock.size / 40U);
    for (std::uint32_t offset = 0; offset < groupInfoBlock.size; offset += 40U) {
        const std::uint8_t* disk = groupInfoBlock.data + offset;
        GroupInfo info;
        info.offset = ReadU32(disk);
        info.size = ReadU32(disk + 4U);
        info.flags = ReadU32(disk + 8U);
        info.aaBox.b = {ReadFloat(disk + 12U), ReadFloat(disk + 16U),
                        ReadFloat(disk + 20U)};
        info.aaBox.t = {ReadFloat(disk + 24U), ReadFloat(disk + 28U),
                        ReadFloat(disk + 32U)};
        info.nameIndex = ReadU32(disk + 36U);
        groupInfo.push_back(info);
    }
    groups.resize(groupInfo.size());
    unsigned loadedGroups = 0;
    for (std::size_t index = 0; index < groupInfo.size(); ++index)
        if (ParseGroup(file, groupInfo[index], groupNames, groups[index]))
            ++loadedGroups;

    const CGxTexFlags baseFlags(GxTex_Linear, false, false, false, false,
                                false, 1U);
    for (CMapObjMaterial& material : materials) {
        const std::string diffuseName =
            StringAt(textureNames, material.diffuseNameIndex);
        if (!diffuseName.empty()) {
            CGxTexFlags flags = baseFlags;
            if ((material.flags & 0x40U) == 0U) flags.bits |= 0x08U;
            if ((material.flags & 0x80U) == 0U) flags.bits |= 0x10U;
            material.diffuse = TextureCreate(diffuseName.c_str(), nullptr,
                                             flags.bits, 0);
        }
        const std::string environmentName =
            StringAt(textureNames, material.envNameIndex);
        if (!environmentName.empty())
            material.environment = TextureCreate(environmentName.c_str(),
                                                 nullptr, baseFlags.bits, 0);
    }
    loaded = loadedGroups != 0U;
    std::fprintf(stderr,
                 "MAPOBJ_READ file=%s materials=%zu groups=%u/%zu loaded=%u\n",
                 name.c_str(), materials.size(), loadedGroups, groups.size(),
                 loaded ? 1U : 0U);
    unload();
    return loaded;
}

void CMapObj::AsyncPostloadCallback() { }
void CMapObj::AsyncPostloadCallbackAll() { }
void CMapObj::AsyncPostloadCallbackHeader() { }
void CMapObj::CreateAllGroups() { }
void CMapObj::CreateData() { }
void CMapObj::CreateDataPointers() { }
void CMapObj::CreateGroup() { }
void CMapObj::CreateMaterial() { }
void CMapObj::CreateMaterials() { }
void CMapObj::Read() { }
void CMapObj::ReadChunkHeader() { }
void CMapObj::ReadExtGroups() { }
void CMapObj::ReadGroup() { }
void CMapObj::ReadOptionalChunkHeader() { }
void CMapObjGroup::AsyncPostloadCallback() { }
void CMapObjGroup::Create() { }
void CMapObjGroup::CreateDataPointers() { }
void CMapObjGroup::CreateLightmapPointers() { }
void CMapObjGroup::CreateOptionalDataPointers() { }
