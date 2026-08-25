#include "ENGINE/Source/Model/mdlsgmodel.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/modelparse.h"
#include "ENGINE/Source/Services/texture.h"

#include <algorithm>
#include <array>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>

namespace {
constexpr std::uint32_t FourCC(char a, char b, char c, char d)
{
    return std::uint32_t(std::uint8_t(a)) |
           (std::uint32_t(std::uint8_t(b)) << 8U) |
           (std::uint32_t(std::uint8_t(c)) << 16U) |
           (std::uint32_t(std::uint8_t(d)) << 24U);
}

bool ReadTag(ModelByteReader& reader, std::uint32_t expected)
{
    std::uint32_t tag = 0;
    return reader.Read(tag) && tag == expected;
}

template <class T>
bool ReadTaggedArray(ModelByteReader& reader, std::uint32_t tag, std::vector<T>& values)
{
    std::uint32_t count = 0;
    return ReadTag(reader, tag) && reader.Read(count) && reader.ReadArray(values, count);
}

std::uint32_t BlendFromMdxFilter(std::uint32_t filter)
{
    switch (filter) {
    case 0: return GxBlend_Opaque;
    case 1: return GxBlend_AlphaKey;
    case 2: return GxBlend_Alpha;
    case 3: return GxBlend_Add;
    case 4: return GxBlend_Mod;
    case 5: return GxBlend_Mod2x;
    default: return GxBlend_Opaque;
    }
}
}

std::uint32_t GetPrimitiveType(std::uint8_t mdlType)
{
    static constexpr std::array<std::uint32_t, 10> mdlToGxPrim{
        GxPrim_Points, GxPrim_Lines, GxPrims_Last, GxPrim_LineStrip,
        GxPrim_Triangles, GxPrim_TriangleStrip, GxPrim_TriangleFan,
        GxPrims_Last, GxPrims_Last, GxPrims_Last};
    return mdlType < mdlToGxPrim.size() ? mdlToGxPrim[mdlType] : GxPrims_Last;
}

std::uint32_t CountNumPrimLists(std::span<const std::uint8_t> primitiveTypes)
{
    static constexpr std::array<bool, 10> multiPrimType{
        true, true, false, false, true, false, false, true, false, false};
    std::uint32_t count = 0;
    std::uint8_t previous = 10U;
    for (const auto type : primitiveTypes) {
        if (type >= multiPrimType.size()) continue;
        if (type != previous || !multiPrimType[type]) ++count;
        previous = type;
    }
    return count;
}

void LoadGeosetPrimitiveTypes(CGeosetShared& geoset, std::span<const std::uint8_t> types,
                              std::span<const std::uint32_t> counts)
{
    static constexpr std::array<bool, 10> multiPrimType{
        true, true, false, false, true, false, false, true, false, false};
    geoset.primitive.clear();
    geoset.primitive.reserve(CountNumPrimLists(types));
    std::uint32_t start = 0;
    std::uint8_t previous = 10U;
    for (std::size_t i = 0; i < std::min(types.size(), counts.size()); ++i) {
        if (types[i] >= multiPrimType.size()) {
            start += counts[i];
            continue;
        }
        const auto type = GetPrimitiveType(types[i]);
        if (type < GxPrims_Last) {
            if (!geoset.primitive.empty() && types[i] == previous && multiPrimType[types[i]])
                geoset.primitive.back().count += counts[i];
            else
                geoset.primitive.push_back({type, start, counts[i]});
        }
        start += counts[i];
        previous = types[i];
    }
}

bool LoadGeosetPrimitiveData(CGeosetShared& geoset, std::span<const std::uint8_t> bytes)
{
    ModelByteReader reader(bytes); std::uint32_t count = 0;
    if (!reader.Read(count) || !reader.ReadArray(geoset.primitiveVertices, count)) return false;
    return std::all_of(geoset.primitiveVertices.begin(), geoset.primitiveVertices.end(),
                       [&geoset](std::uint16_t index) { return index < geoset.position.size(); });
}

bool LoadGeosetData(CGeosetShared& geoset, std::span<const std::uint8_t> bytes)
{
    ModelByteReader reader(bytes);
    if (!ReadTaggedArray(reader, FourCC('V','R','T','X'), geoset.position) ||
        geoset.position.size() > 0xFFFFU)
        return false;
    if (!ReadTaggedArray(reader, FourCC('N','R','M','S'), geoset.normal) ||
        geoset.normal.size() != geoset.position.size())
        return false;

    geoset.texCoord.clear();
    if (reader.Remaining() >= 8U) {
        std::uint32_t nextTag = 0;
        std::memcpy(&nextTag, bytes.data() + reader.Position(), sizeof(nextTag));
        if (nextTag == FourCC('U','V','A','S')) {
            std::uint32_t channelCount = 0;
            if (!ReadTag(reader, nextTag) || !reader.Read(channelCount) || channelCount > 8U)
                return false;
            geoset.texCoord.resize(channelCount);
            for (auto& coordinates : geoset.texCoord)
                if (!reader.ReadArray(coordinates, geoset.position.size())) return false;
        }
    }

    std::uint32_t primitiveTypeCount = 0;
    if (!ReadTag(reader, FourCC('P','T','Y','P')) || !reader.Read(primitiveTypeCount)) return false;
    std::vector<std::uint8_t> primitiveTypes;
    if (!reader.ReadArray(primitiveTypes, primitiveTypeCount)) return false;

    std::uint32_t primitiveCountCount = 0;
    std::vector<std::uint32_t> primitiveCounts;
    if (!ReadTag(reader, FourCC('P','C','N','T')) || !reader.Read(primitiveCountCount) ||
        primitiveCountCount != primitiveTypeCount || !reader.ReadArray(primitiveCounts, primitiveCountCount))
        return false;
    if (!ReadTaggedArray(reader, FourCC('P','V','T','X'), geoset.primitiveVertices)) return false;
    LoadGeosetPrimitiveTypes(geoset, primitiveTypes, primitiveCounts);

    if (!ReadTaggedArray(reader, FourCC('G','N','D','X'), geoset.boneWeights) ||
        !ReadTaggedArray(reader, FourCC('M','T','G','C'), geoset.groupMatrixCounts) ||
        !ReadTaggedArray(reader, FourCC('M','A','T','S'), geoset.matrices) ||
        !ReadTaggedArray(reader, FourCC('B','I','D','X'), geoset.hwBoneIndices) ||
        !ReadTaggedArray(reader, FourCC('B','W','G','T'), geoset.hwBoneWeights))
        return false;

    std::uint32_t extentCount = 0;
    NTempest::C3Vector extentMin{}, extentMax{};
    if (!reader.Read(geoset.materialId) || !reader.Read(geoset.selectionGroup) ||
        !reader.Read(geoset.flags) || !reader.Read(geoset.radius) ||
        !reader.Read(extentMin) || !reader.Read(extentMax) || !reader.Read(extentCount) ||
        extentCount > reader.Remaining() / (sizeof(float) * 7U) ||
        !reader.Skip(static_cast<std::size_t>(extentCount) * sizeof(float) * 7U))
        return false;

    geoset.centroid = (extentMin + extentMax) * 0.5F;
    geoset.vertexShader = geoset.groupMatrixCounts.size() > 1U;
    return reader.Remaining() == 0U &&
           std::all_of(geoset.primitiveVertices.begin(), geoset.primitiveVertices.end(),
                       [&geoset](std::uint16_t index) { return index < geoset.position.size(); });
}

bool MdxReadGeosets(std::span<const std::uint8_t> bytes, CModelComplex& model, CModelShared& shared)
{
    ModelByteReader reader(bytes); std::uint32_t count = 0;
    if (!reader.Read(count) || count > 255U) return false;
    shared.geosets.clear(); shared.geosets.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t size = 0;
        if (!reader.Read(size) || size < sizeof(size) || size - sizeof(size) > reader.Remaining()) return false;
        CGeosetShared geoset;
        geoset.geosetId = i;
        if (!LoadGeosetData(geoset, bytes.subspan(reader.Position(), size - sizeof(size)))) return false;
        shared.positions.insert(shared.positions.end(), geoset.position.begin(), geoset.position.end());
        shared.geosets.push_back(std::move(geoset));
        if (!reader.Skip(size - sizeof(size))) return false;
    }
    if (reader.Remaining() != 0U) return false;
    shared.numGeosets = static_cast<std::uint8_t>(count);
    BuildAllGeosets(model, shared);

    static bool dumpedMainMenu = false;
    if (!dumpedMainMenu && std::strstr(shared.name.data(), "UI_MainMenu") != nullptr) {
        dumpedMainMenu = true;
        std::fprintf(stderr, "GEODUMP model=%s geosets=%zu\n", shared.name.data(), shared.geosets.size());
        for (std::size_t geosetIndex = 0; geosetIndex < shared.geosets.size(); ++geosetIndex) {
            const auto& geoset = shared.geosets[geosetIndex];
            NTempest::C3Vector minimum{std::numeric_limits<float>::max()};
            NTempest::C3Vector maximum{-std::numeric_limits<float>::max()};
            for (const auto& position : geoset.position) {
                minimum.x = std::min(minimum.x, position.x); minimum.y = std::min(minimum.y, position.y); minimum.z = std::min(minimum.z, position.z);
                maximum.x = std::max(maximum.x, position.x); maximum.y = std::max(maximum.y, position.y); maximum.z = std::max(maximum.z, position.z);
            }
            std::uint16_t maxVertex = 0;
            for (const auto index : geoset.primitiveVertices) maxVertex = std::max(maxVertex, index);
            std::uint8_t maxGroup = 0;
            for (const auto group : geoset.boneWeights) maxGroup = std::max(maxGroup, group);
            std::uint32_t maxMatrix = 0;
            for (const auto matrix : geoset.matrices) maxMatrix = std::max(maxMatrix, matrix);
            std::uint64_t matrixTotal = 0;
            for (const auto groupCount : geoset.groupMatrixCounts) matrixTotal += groupCount;
            NTempest::C2Vector uvMinimum{std::numeric_limits<float>::max()};
            NTempest::C2Vector uvMaximum{-std::numeric_limits<float>::max()};
            if (!geoset.texCoord.empty()) {
                for (const auto& uv : geoset.texCoord[0]) {
                    uvMinimum.x = std::min(uvMinimum.x, uv.x); uvMinimum.y = std::min(uvMinimum.y, uv.y);
                    uvMaximum.x = std::max(uvMaximum.x, uv.x); uvMaximum.y = std::max(uvMaximum.y, uv.y);
                }
            }
            std::fprintf(stderr,
                         "GEODUMP g=%zu v=%zu n=%zu uv=%zu uv0=(%g,%g)-(%g,%g) idx=%zu maxidx=%u groups=%zu gndx=%zu maxgroup=%u matrices=%zu sum=%llu maxbone=%u shader=%u material=%u ext=(%g,%g,%g)-(%g,%g,%g) prims=",
                         geosetIndex, geoset.position.size(), geoset.normal.size(), geoset.texCoord.size(),
                         uvMinimum.x, uvMinimum.y, uvMaximum.x, uvMaximum.y,
                         geoset.primitiveVertices.size(), maxVertex, geoset.groupMatrixCounts.size(),
                         geoset.boneWeights.size(), maxGroup, geoset.matrices.size(), matrixTotal,
                         maxMatrix, geoset.vertexShader, geoset.materialId,
                         minimum.x, minimum.y, minimum.z, maximum.x, maximum.y, maximum.z);
            for (const auto& primitive : geoset.primitive)
                std::fprintf(stderr, "[%u,%u,%u]", primitive.type, primitive.start, primitive.count);
            std::fprintf(stderr, "\n");
        }
    }
    return true;
}

bool MdlReadLoadGlobalProperties(CModelShared& shared, std::span<const std::uint8_t> bytes,
                                 std::uint32_t* loadFlags)
{
    ModelByteReader reader(bytes); std::uint32_t flags = 0;
    if (!reader.Read(flags) || !reader.Read(shared.numBones) || !reader.Read(shared.numTexBones)) return false;
    if (loadFlags) *loadFlags = flags; return true;
}

bool MdlReadLoadModel(CModelComplex& model, CModelShared& shared,
                      std::span<const std::uint8_t> bytes, std::uint32_t)
{
    return MdxReadGeosets(bytes, model, shared);
}

void CreateGeoset(CModelComplex& model, const CGeosetShared& shared)
{
    model.m_geosets.push_back({~0U, shared.flags});
}

void CreateGeosetWithNormals(CModelComplex& model, CGeosetShared& shared)
{
    if (shared.normal.size() != shared.position.size()) {
        shared.normal.assign(shared.position.size(), {});
        for (std::size_t i = 0; i + 2U < shared.primitiveVertices.size(); i += 3U) {
            const auto ia = shared.primitiveVertices[i], ib = shared.primitiveVertices[i + 1U], ic = shared.primitiveVertices[i + 2U];
            auto n = NTempest::C3Vector::Cross(shared.position[ib] - shared.position[ia], shared.position[ic] - shared.position[ia]);
            shared.normal[ia] += n; shared.normal[ib] += n; shared.normal[ic] += n;
        }
        for (auto& normal : shared.normal) if (normal.SquaredMag() > std::numeric_limits<float>::epsilon()) normal.Normalize();
    }
    CreateGeoset(model, shared);
}

void BuildAllGeosets(CModelComplex& model, const CModelShared& shared)
{
    model.m_geosets.clear(); model.m_geosets.reserve(shared.geosets.size());
    for (const auto& geoset : shared.geosets) CreateGeoset(model, geoset);
    model.m_geosetColor.resize(shared.geosets.size());
}

std::uint32_t GetTmuPassFlags(std::uint32_t layerFlags, bool firstLayer)
{
    std::uint32_t result = firstLayer ? 1U : 0U;
    if ((layerFlags & 0x100000U) != 0U) result |= 2U;
    return result;
}
bool GetTextureShader(std::uint32_t createFlags, bool environmentMapped)
{
    return createFlags != ~0U && !environmentMapped;
}
std::uint32_t GetTextureFlags(std::uint32_t mdlFlags)
{
    std::uint32_t flags = 0; if (mdlFlags & 1U) flags |= 1U; if (mdlFlags & 2U) flags |= 4U; if (mdlFlags & 4U) flags |= 8U; return flags;
}

void ProcessTextures(CModelComplex& model, std::span<const std::string> names,
                     std::span<const std::uint32_t> replaceableIds)
{
    model.m_textures.clear(); model.m_textures.reserve(names.size());
    for (std::size_t i = 0; i < names.size(); ++i) {
        CModelTexture texture; texture.replaceableId = i < replaceableIds.size() ? replaceableIds[i] : 0U;
        if (!names[i].empty() && texture.replaceableId == 0) texture.handle = TextureCreate(names[i].c_str(), nullptr, 0, 0);
        model.m_textures.push_back(texture);
    }
}
void ProcessMaterials(CModelComplex& model, std::span<const CMaterial> materials) { model.m_materials.assign(materials.begin(), materials.end()); }
void ProcessTexLayers(CMaterial& material, std::span<const CTexLayer> layers) { material.layers.assign(layers.begin(), layers.end()); }
void ProcessLayerAlpha(CMaterial& material, float alpha) { for (auto& layer : material.layers) layer.alpha = std::clamp(alpha * layer.alpha, 0.0F, 1.0F); }

bool LoadLayerData(CTexLayer& layer, std::span<const std::uint8_t> bytes, std::uint32_t createFlags)
{
    ModelByteReader reader(bytes);
    std::uint32_t filterMode = 0;
    std::uint32_t shadingFlags = 0;
    if (!reader.Read(filterMode) || !reader.Read(shadingFlags) || !reader.Read(layer.texture) ||
        !reader.Read(layer.transformId) || !reader.Read(layer.coordId) || !reader.Read(layer.alpha))
        return false;
    layer.blendMode = BlendFromMdxFilter(filterMode);
    layer.vertexFormat = layer.coordId == -1 ? 0U : 2U;
    layer.combiner = 1U;
    layer.textureShader = GetTextureShader(static_cast<std::uint32_t>(layer.transformId),
                                           (createFlags & 0x100U) != 0U)
        ? 1U : 0U;
    layer.tmuFlags = GetTmuPassFlags(createFlags, (shadingFlags & 2U) != 0U);

    // LoadLayerData in the original client converts the MDX shading flags to
    // the compact renderer-disable bitfield used by ModelRender.
    layer.flags = 0U;
    if (layer.blendMode >= GxBlend_Alpha && layer.blendMode <= GxBlend_ModAdd)
        layer.flags |= 0x08U;
    if ((shadingFlags & 0x01U) != 0U) layer.flags |= 0x01U; // unlit
    if ((shadingFlags & 0x10U) != 0U) layer.flags |= 0x10U; // two-sided
    if ((shadingFlags & 0x20U) != 0U) layer.flags |= 0x02U; // unfogged
    if ((shadingFlags & 0x40U) != 0U) layer.flags |= 0x04U; // no depth test
    if ((shadingFlags & 0x80U) != 0U) layer.flags |= 0x08U; // no depth write

    // The unique layer stores an 8-bit alpha.  Quantize here so visibility and
    // opacity classification use the same value as the recovered executable.
    layer.alpha = std::clamp(layer.alpha, 0.0F, 1.0F);
    layer.alpha = std::floor(layer.alpha * 255.0F + 0.5F) / 255.0F;
    return true;
}
bool LoadMaterialData(CMaterial& material, std::span<const std::uint8_t> bytes,
                      std::uint32_t createFlags)
{
    ModelByteReader reader(bytes); std::uint32_t count = 0;
    if (!reader.Read(material.priorityPlane) || !reader.Read(count)) return false;
    material.layers.clear(); material.layers.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t layerBytes = 0;
        if (!reader.Read(layerBytes) || layerBytes < sizeof(layerBytes) ||
            layerBytes - sizeof(layerBytes) > reader.Remaining()) return false;
        CTexLayer layer;
        if (!LoadLayerData(layer, bytes.subspan(reader.Position(), layerBytes - sizeof(layerBytes)),
                           createFlags)) return false;
        material.layers.push_back(layer);
        if (!reader.Skip(layerBytes - sizeof(layerBytes))) return false;
    }
    return reader.Remaining() == 0U;
}
bool MdxReadTextures(std::span<const std::uint8_t> bytes, CModelComplex& model)
{
    constexpr std::size_t kTextureRecordBytes = 268U;
    if (bytes.size() % kTextureRecordBytes != 0U) return false;
    ModelByteReader reader(bytes);
    const auto count = static_cast<std::uint32_t>(bytes.size() / kTextureRecordBytes);
    std::vector<std::string> names; std::vector<std::uint32_t> replaces; names.reserve(count); replaces.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t replace = 0, flags = 0;
        std::array<char, 260> filename{};
        if (!reader.Read(replace) || !reader.Read(filename) || !reader.Read(flags)) return false;
        const auto end = std::find(filename.begin(), filename.end(), '\0');
        names.emplace_back(filename.begin(), end);
        replaces.push_back(replace);
    }
    ProcessTextures(model, names, replaces); return true;
}
bool MdxReadMaterials(std::span<const std::uint8_t> bytes, CModelComplex& model,
                      std::uint32_t createFlags)
{
    ModelByteReader reader(bytes); std::uint32_t count = 0, reserved = 0;
    if (!reader.Read(count) || !reader.Read(reserved)) return false;
    model.m_materials.clear(); model.m_materials.reserve(count);
    std::uint32_t layerId = 0;
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t size = 0;
        if (!reader.Read(size) || size < sizeof(size) || size - sizeof(size) > reader.Remaining()) return false;
        CMaterial material;
        if (!LoadMaterialData(material, bytes.subspan(reader.Position(), size - sizeof(size)), createFlags) ||
            !reader.Skip(size - sizeof(size))) return false;
        for (auto& layer : material.layers) {
            layer.animationId = layerId++;
        }
        model.m_materials.push_back(std::move(material));
    }
    return reader.Remaining() == 0U;
}
bool MdxLoadGlobalProperties(std::span<const std::uint8_t> bytes, CModelShared& shared, std::uint32_t* loadFlags) { return MdlReadLoadGlobalProperties(shared, bytes, loadFlags); }

bool LoadGeosetTransformGroups(CGeosetShared& geoset, std::span<const std::uint32_t> groupCounts,
                               std::span<const std::uint32_t> matrices)
{
    std::uint64_t total = 0; for (auto count : groupCounts) total += count; if (total != matrices.size()) return false;
    geoset.groupMatrixCounts.assign(groupCounts.begin(), groupCounts.end()); geoset.matrices.assign(matrices.begin(), matrices.end()); return true;
}
bool LoadAttachment(CModelShared& shared, std::uint32_t id, std::uint32_t index)
{
    if (shared.attachIdToIndex.size() <= id) shared.attachIdToIndex.resize(id + 1U, ~0U); shared.attachIdToIndex[id] = index; return true;
}
bool ProcessAttachments(CModelShared& shared, std::span<const std::pair<std::uint32_t, std::uint32_t>> attachments)
{
    shared.attachIdToIndex.clear(); for (const auto& attachment : attachments) LoadAttachment(shared, attachment.first, attachment.second); return true;
}
bool MdxReadAttachments(std::span<const std::uint8_t> bytes, CModelShared& shared)
{
    if (bytes.size() < 8U) return false;
    std::uint32_t count = 0, maxAttachmentId = 0;
    std::memcpy(&count, bytes.data(), sizeof(count));
    std::memcpy(&maxAttachmentId, bytes.data() + 4U, sizeof(maxAttachmentId));
    if (maxAttachmentId == ~0U) shared.attachIdToIndex.clear();
    else shared.attachIdToIndex.assign(static_cast<std::size_t>(maxAttachmentId) + 1U, ~0U);

    std::size_t position = 8U;
    for (std::uint32_t index = 0; index < count; ++index) {
        if (position + 8U > bytes.size()) return false;
        std::uint32_t recordSize = 0, objectSize = 0;
        std::memcpy(&recordSize, bytes.data() + position, sizeof(recordSize));
        std::memcpy(&objectSize, bytes.data() + position + 4U, sizeof(objectSize));
        if (recordSize < 365U || recordSize > bytes.size() - position || objectSize < 96U)
            return false;
        const std::size_t staticData = position + 4U + objectSize;
        if (staticData + 265U > position + recordSize) return false;
        std::uint32_t attachmentId = 0;
        std::memcpy(&attachmentId, bytes.data() + staticData, sizeof(attachmentId));
        if (attachmentId >= shared.attachIdToIndex.size()) return false;
        shared.attachIdToIndex[attachmentId] = index;
        position += recordSize;
    }
    return position == bytes.size();
}
