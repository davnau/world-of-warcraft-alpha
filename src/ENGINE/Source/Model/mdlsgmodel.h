#pragma once
#include "ENGINE/Source/Model/model.h"
#include <span>
#include <string>
#include <utility>

std::uint32_t GetPrimitiveType(std::uint8_t mdlType);
std::uint32_t CountNumPrimLists(std::span<const std::uint8_t> primitiveTypes);
void LoadGeosetPrimitiveTypes(CGeosetShared& geoset, std::span<const std::uint8_t> types,
                              std::span<const std::uint32_t> counts);
bool LoadGeosetPrimitiveData(CGeosetShared& geoset, std::span<const std::uint8_t> bytes);
bool LoadGeosetData(CGeosetShared& geoset, std::span<const std::uint8_t> bytes);
bool MdxReadGeosets(std::span<const std::uint8_t> bytes, CModelComplex& model, CModelShared& shared);
bool MdlReadLoadGlobalProperties(CModelShared& shared, std::span<const std::uint8_t> bytes,
                                 std::uint32_t* loadFlags = nullptr);
bool MdlReadLoadModel(CModelComplex& model, CModelShared& shared,
                      std::span<const std::uint8_t> bytes, std::uint32_t loadFlags = 0);
void BuildAllGeosets(CModelComplex& model, const CModelShared& shared);
void CreateGeoset(CModelComplex& model, const CGeosetShared& shared);
void CreateGeosetWithNormals(CModelComplex& model, CGeosetShared& shared);
std::uint32_t GetTmuPassFlags(std::uint32_t layerFlags, bool firstLayer);
bool GetTextureShader(std::uint32_t createFlags, bool environmentMapped);
std::uint32_t GetTextureFlags(std::uint32_t mdlFlags);
void ProcessTextures(CModelComplex& model, std::span<const std::string> names,
                     std::span<const std::uint32_t> replaceableIds = {});
void ProcessMaterials(CModelComplex& model, std::span<const CMaterial> materials);
void ProcessTexLayers(CMaterial& material, std::span<const CTexLayer> layers);
void ProcessLayerAlpha(CMaterial& material, float alpha);
bool MdxReadTextures(std::span<const std::uint8_t> bytes, CModelComplex& model);
bool MdxReadMaterials(std::span<const std::uint8_t> bytes, CModelComplex& model,
                      std::uint32_t createFlags = 0);
bool MdxLoadGlobalProperties(std::span<const std::uint8_t> bytes, CModelShared& shared,
                             std::uint32_t* loadFlags = nullptr);
bool LoadMaterialData(CMaterial& material, std::span<const std::uint8_t> bytes,
                      std::uint32_t createFlags = 0);
bool LoadLayerData(CTexLayer& layer, std::span<const std::uint8_t> bytes,
                   std::uint32_t createFlags = 0);
bool LoadGeosetTransformGroups(CGeosetShared& geoset,
                               std::span<const std::uint32_t> groupCounts,
                               std::span<const std::uint32_t> matrices);
bool LoadAttachment(CModelShared& shared, std::uint32_t id, std::uint32_t index);
bool ProcessAttachments(CModelShared& shared,
                        std::span<const std::pair<std::uint32_t, std::uint32_t>> attachments);
bool MdxReadAttachments(std::span<const std::uint8_t> bytes, CModelShared& shared);
struct MDLPRIMITIVES {
    std::vector<std::uint8_t> types;
    std::vector<std::uint32_t> counts;
    MDLPRIMITIVES() = default;
    void SetCount(std::size_t count) { types.resize(count); counts.resize(count); }
};
