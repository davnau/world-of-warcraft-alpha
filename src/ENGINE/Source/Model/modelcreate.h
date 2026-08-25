#pragma once
#include "ENGINE/Source/Model/model.h"
#include <span>
void ModelInitialize();
void ModelCacheFlush();
void ModelCacheUpdate();
std::shared_ptr<CModel> ModelCreate(const char* fileName, const CModelCreate* create = nullptr);
std::shared_ptr<CModel> ModelCreate(std::span<const std::uint8_t> bytes, const char* name, const CModelCreate* create = nullptr);
std::shared_ptr<CModel> IModelCreateBlocking(std::span<const std::uint8_t> bytes, const char* name, const CModelCreate* create = nullptr);
std::shared_ptr<CModel> IModelCreateSimpleEmpty(const char* name = nullptr);
std::shared_ptr<CModel> ModelDuplicate(const CModel& source);
void ModelDestroy(std::shared_ptr<CModel>& model);
bool ModelIsLoaded(const CModel& model);
bool ModelIsUsed(const CModel& model);
bool IsBinaryFile(std::span<const std::uint8_t> bytes);
bool IsSimpleModel(const CModel& model);
void ComputeBoundingBox(std::span<const NTempest::C3Vector> positions, NTempest::CAaBox& box);
float ComputeBoundingRadius(std::span<const NTempest::C3Vector> positions, const NTempest::C3Vector& center = {});
void IModelComputeBounds(CModelShared& shared);
bool MdxReadPositions(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdxReadExtents(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdxReadHitTestData(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdxReadNumMatrices(std::span<const std::uint8_t> bytes, CModelShared& shared);
struct CMdlBounds { CBoundsData bounds{}; CMdlBounds() = default; };
struct MDLBASE { std::uint32_t flags = 0; MDLBASE() = default; virtual ~MDLBASE() = default; };
struct MDLCOLLISION { std::vector<NTempest::C3Vector> vertices; std::vector<std::uint16_t> indices; MDLCOLLISION() = default; };
struct CModelHash { std::string key; std::weak_ptr<CModel> model; CModelHash() = default; ~CModelHash() = default; };
std::shared_ptr<CModelShared> CreateSharedModelData(const char* name = nullptr);
std::shared_ptr<CModel> CreateDefaultModel(const char* name = nullptr);
std::shared_ptr<CModel> BuildModelFromMdxData(std::span<const std::uint8_t> bytes, const char* name,
                                              const CModelCreate* create = nullptr);
std::shared_ptr<CModel> BuildModelFromMdlData(std::span<const std::uint8_t> bytes, const char* name,
                                              const CModelCreate* create = nullptr);
std::shared_ptr<CModel> BuildSimpleModelFromMdxData(std::span<const std::uint8_t> bytes, const char* name);
std::shared_ptr<CModel> BuildSimpleModelFromMdlData(std::span<const std::uint8_t> bytes, const char* name);
std::shared_ptr<CModel> ModelCreateSimpleMesh(std::span<const NTempest::C3Vector> vertices,
                                              std::span<const std::uint16_t> indices,
                                              const char* name = nullptr);
bool ModelGeosetAdd(CModel& model, const CGeosetShared& geoset);
std::shared_ptr<CModel> ModelGetModel(const char* name);
std::shared_ptr<CModel> GetModel(const char* name);
void HashNewModel(const char* name, const std::shared_ptr<CModel>& model);
void ModelRemoveFromCache(const char* name);
void IModelDerefHandle(std::shared_ptr<CModel>& model);
std::uint32_t GetSectionCount(std::span<const std::uint8_t> bytes);
std::uint32_t GetTextureCount(const CModel& model);
std::uint32_t ConvertAnimCreateFlags(std::uint32_t flags);
bool LoadBoundsData(std::span<const std::uint8_t> bytes, CBoundsData& bounds);
bool MdlReadLoadPositions(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdlReadLoadExtents(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdlReadLoadHitTestData(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdlReadLoadNumMatrices(std::span<const std::uint8_t> bytes, CModelShared& shared);
bool MdlReadLoadAnim(std::span<const std::uint8_t> bytes, CModel& model);
bool MdxReadAnimation(std::span<const std::uint8_t> bytes, CModel& model);
void ProcessAnimReorders(CModel& model, std::span<const std::uint32_t> reorder);
void EnqueueModelCommand(std::function<void()> command);
void ExecuteQueuedActions();
void AsyncModelHandler();
void AsnycModelPostLoadCallback(CModel& model, bool success);
void BuildSimpleGeoset(CModel& model, const CGeosetShared& geoset);
void BuildSimpleMaterial(CModel& model, const CMaterial& material);
