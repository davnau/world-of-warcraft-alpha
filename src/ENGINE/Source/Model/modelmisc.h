#pragma once
#include "ENGINE/Source/Model/model.h"
#include <span>
struct CRibbonMat;

void ModelEnableEmitters(CModel& model, bool enable);
void ModelEnableRibbons(CModel& model, bool enable);
void ModelEnableLights(CModel& model, bool enable);
void ModelEnableFullAlpha(CModel& model, bool enable);
void ModelEnableAnimBlending(CModel& model, bool enable);
bool ModelUsesBlending(const CModel& model);
std::uint32_t ModelGetNumTextures(const CModel& model);
std::uint32_t ModelGetTextureReplaceableId(const CModel& model, std::uint32_t texture);
bool ModelReplaceTexture(CModel& model, std::uint32_t replaceableId,
                         HTEXTURE__* replacement, bool doLinkedModels = false);
std::uint32_t ModelGetNumCameras(const CModel& model);
HCAMERA__* ModelGetCamera(CModel& model, std::uint32_t camera);
std::uint32_t ModelGetNumLights(const CModel& model);
std::uintptr_t ModelGetLight(const CModel& model, std::uint32_t light);
void ModelHideGeosets(CModel& model, std::uint32_t selectionGroup, bool hide);
void ModelHideGeosetsRange(CModel& model, std::uint32_t selectionStart,
                           std::uint32_t selectionEnd, bool hide);
void GeosetSetVertexAlpha(CGeosetColor& color, float alpha);
float GeosetGetVertexAlpha(const CGeosetColor& color);
void GeosetSetVertexColor(CGeosetColor& target, const NTempest::C3Vector& color);
NTempest::C3Vector GeosetGetVertexColor(const CGeosetColor& color);
std::uint32_t ModelCustGeosetAdd(CModel& model, const CCustomGeoset& geoset);
bool ModelCustGeosetRemove(CModel& model, std::uint32_t index);
bool ModelCustGeosetMove(CModel& model, std::uint32_t from, std::uint32_t to);
bool ModelAddLink(CModel& model, std::uint32_t point, CModel& linked, float scale = 1.0F);
bool ModelRemoveLink(CModel& model, CModel& linked);
bool ModelClearLink(CModel& model, std::uint32_t point);
void ModelClearAllLinks(CModel& model);
std::uint32_t ModelGetNumLinkedAtPoint(const CModel& model, std::uint32_t point);
bool ModelHasLinkPoint(const CModel& model, std::uint32_t point);
std::uint32_t ModelGetNumLinkPoints(const CModel& model);
void ModelSetVertexAlpha(CModel& model, float alpha);
float ModelGetVertexAlpha(const CModel& model);
void ModelSetVertexColor(CModel& model, const NTempest::C3Vector& color);
NTempest::C3Vector ModelGetVertexColor(const CModel& model);
void ModelHideUnselectable(CModel& model);
void ModelShowUnselectable(CModel& model);
bool ModelIsShowingUnselectable(const CModel& model);
void GeosetHideUnselectable(CGeoset& geoset);
void GeosetShowUnselectable(CGeoset& geoset);
bool GeosetIsShowingUnselectable(const CGeoset& geoset);
bool ModelIsShowingBoundingBox(const CModel& model);
bool ModelIsShowingBoundingSphere(const CModel& model);
bool ModelIsShowingHitTestGeometry(const CModel& model);
bool ModelIsCameraEnabled(const CModel& model, std::uint32_t camera);
void ModelSetBlendMode(CModel& model, std::uint32_t material, std::uint32_t layer,
                       std::uint32_t blendMode);
void ModelRestoreBlendMode(CModel& model, std::uint32_t material, std::uint32_t layer);
void ModelSetEmissiveColor(CModel& model, std::uint32_t material,
                           const NTempest::C3Vector& color);
void ModelSetMaterialDisables(CModel& model, std::uint32_t material, std::uint32_t disables);
void ModelMaterialShowLayer(CModel& model, std::uint32_t material, std::uint32_t layer, bool show);
void ModelOptimizeVisibleGeosets(CModel& model);
void UpdateParticleEmitters(CModel& model, float elapsedSec);
void UpdateRibbonMaterial(CModel& model, std::uint32_t ribbon, const CRibbonMat& material);
bool ModelAnimHasObjectId(const CModel& model, std::uint32_t id);
std::uint32_t ModelGetMatrixCount(const CModel& model);
NTempest::C34Matrix ModelGetLinkPoint(const CModel& model, std::uint32_t point);
void AddMatrixGroupRangeToSet(std::vector<std::uint32_t>& set, std::uint32_t first,
                              std::uint32_t count);
void AddGeosetMatrixGroups(const CGeosetShared& geoset, std::vector<std::uint32_t>& set);
CGeosetShared BuildCompositeGeoset(std::span<const CGeosetShared> geosets);
std::uint32_t ComplexModelReplaceTexture(CModel& model, std::uint32_t replaceableId,
                                         HTEXTURE__* replacement, bool doLinkedModels);
void ComplexModelSetBlendMode(CModel& model, std::uint32_t material, std::uint32_t layer, std::uint32_t mode);
void ComplexModelRestoreBlendMode(CModel& model, std::uint32_t material, std::uint32_t layer);
void ComplexModelSetEmissiveColor(CModel& model, std::uint32_t material, const NTempest::C3Vector& color);
void ComplexModelSetMaterialDisables(CModel& model, std::uint32_t material, std::uint32_t disables);
void IModelEnableFullAlpha(CModel& model, bool enable);
void IModelOptimizeVisibleGeosets(CModel& model);
void IModelSetMaterialDisables(CModel& model, std::uint32_t material, std::uint32_t disables);
void IModelSetVertexAlpha(CModel& model, float alpha);
void IModelSetVertexColor(CModel& model, const NTempest::C3Vector& color);
CMaterial MaterialDuplicate(const CMaterial& material);
bool MaterialUsedOnce(const CModel& model, std::uint32_t material);
std::vector<std::uint32_t> ModelEnumAnimObjects(const CModel& model);
void ModelSetLightSelectCallback(CModel& model, CModelBase::PickLights callback, void* parameter);
void GxuLightSelectCallback(void* parameter, NTempest::C3Vector center,
                            const NTempest::C3Vector* bounds, unsigned flags);
