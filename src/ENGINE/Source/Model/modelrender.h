#pragma once
#include "ENGINE/Source/Model/model.h"
#include "ENGINE/Source/Tempest/c4plane.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include <span>

struct CModelRenderData {
    CModel* model = nullptr;
    CModelShared* shared = nullptr;
    std::uint32_t renderFlags = 0;
    std::vector<std::uint32_t> visibleGeosets;
};

// D3D11 needs the opposite front-face convention for the skinned world
// character.  Keep this in render submission state so it cannot affect
// terrain, map objects, or ordinary doodad models.
constexpr std::uint32_t MODEL_RENDER_REVERSE_CULL = 0x80000000U;

void ModelRenderInitialize();
void ModelRenderDestroy();
void ModelAddToScene(CModel& model, std::uint32_t renderFlags = 0);
void ModelRenderScene();
void ModelRenderSceneOpaque();
void ModelRenderSceneTransparent();
void ModelRender(CModel& model, std::uint32_t renderFlags = 0);
void ModelBaseRender(CModel& model, std::uint32_t renderFlags);
void ModelComplexRender(CModel& model, std::uint32_t renderFlags);
void ModelSimpleRender(CModel& model, std::uint32_t renderFlags);
void FillInRenderData(CModel& model, std::uint32_t renderFlags, CModelRenderData& data);
bool ModelGetBounds(const CModel& model, CBoundsData& bounds);
bool ModelGetExtents(const CModel& model, NTempest::CAaBox& extents);
bool ModelGetSeqExtents(const CModel& model, std::uint32_t sequence, NTempest::CAaBox& extents);
void TransformBounds(const NTempest::CAaBox& source, const NTempest::C34Matrix& transform,
                     NTempest::CAaBox& destination);
bool ModelTestSphere(const CModel& model, const NTempest::CAaSphere& sphere);
bool ModelIntersectLineSegment(const CModel& model, const NTempest::C3Vector& start,
                               const NTempest::C3Vector& end, float& distance);
ModelIntersectResult ModelIntersectLineSegmentEx(const CModel& model,
                                                 const NTempest::C3Vector& start,
                                                 const NTempest::C3Vector& end);
bool ModelHitTestSphere(const CModel& model, const NTempest::CAaSphere& sphere);
bool ModelHasHitTestVolumes(const CModel& model);
bool ModelHitTestVolumes(const CModel& model, const NTempest::C3Vector& start,
                         const NTempest::C3Vector& end, ModelIntersectResult& result);
bool LineSegmentIntersectBox(const NTempest::C3Vector& start, const NTempest::C3Vector& end,
                             const NTempest::CAaBox& box, float& distance);
bool LineSegmentIntersectSphere(const NTempest::C3Vector& start, const NTempest::C3Vector& end,
                                const NTempest::CAaSphere& sphere, float& distance);
bool LineSegmentIntersectCylinder(const NTempest::C3Vector& start, const NTempest::C3Vector& end,
                                  const NTempest::C3Vector& base, float radius, float height,
                                  float& distance);
void GenerateSphereVerts(float radius, std::uint32_t rings, std::uint32_t segments,
                         std::vector<NTempest::C3Vector>& vertices,
                         std::vector<std::uint16_t>& indices);
void GenerateCylinderVerts(float radius, float height, std::uint32_t segments,
                           std::vector<NTempest::C3Vector>& vertices,
                           std::vector<std::uint16_t>& indices);
void CreateBoxGeometry(const NTempest::CAaBox& box, std::vector<NTempest::C3Vector>& vertices,
                       std::vector<std::uint16_t>& indices);
void AddAllGeosetsToScene(CModel& model, std::uint32_t renderFlags);
void AddGeosetToScene(CModel& model, std::uint32_t geoset, std::uint32_t renderFlags);
void AddEmitters2ToScene(CModel& model);
void AddRibbonsToScene(CModel& model);
void IModelBaseAddToScene(CModel& model, std::uint32_t renderFlags);
void IModelComplexAddToScene(CModel& model, std::uint32_t renderFlags);
void IModelSimpleAddToScene(CModel& model, std::uint32_t renderFlags);
bool IModelTestRay(const CModel& model, const NTempest::C3Vector& start,
                   const NTempest::C3Vector& end, ModelIntersectResult& result);
bool IModelTestCollisionVolumes(const CModel& model, const NTempest::C3Vector& start,
                                const NTempest::C3Vector& end, ModelIntersectResult& result);
bool GeosetTestRay(const CGeosetShared& geoset, const NTempest::C3Vector& start,
                   const NTempest::C3Vector& end, ModelIntersectResult& result);
std::shared_ptr<CModel> ModelCreateBox(const NTempest::CAaBox& box);
std::shared_ptr<CModel> ModelCreateSolidSphere(const NTempest::CAaSphere& sphere,
                                               std::uint32_t rings = 8, std::uint32_t segments = 12);
void CreateSphereGeometry(const NTempest::CAaSphere& sphere, CGeosetShared& geoset,
                          std::uint32_t rings = 8, std::uint32_t segments = 12);
void CreateCylinderGeometry(float radius, float height, CGeosetShared& geoset,
                            std::uint32_t segments = 12);
void CreatePlanarQuadGeometry(float width, float height, CGeosetShared& geoset);
void ModelShowBoundingBox(CModel& model);
void ModelShowBoundingSphere(CModel& model);
void ModelShowHitTestGeometry(CModel& model);
void ModelHideBounds(CModel& model);
void ModelHideHitTestGeometry(CModel& model);
bool ModelHitTestGeometry(const CModel& model, const NTempest::C3Vector& start,
                          const NTempest::C3Vector& end, ModelIntersectResult& result);
void ModelRenderSceneLogStart();
void ModelRenderSceneLogStop();
void ModelRenderSceneLogToggle();
void ModelSceneSetSharpness(float sharpness);
NTempest::C2Vector Project2d(const NTempest::C3Vector& world);
bool IsOpaque(const CMaterial& material);
std::uint32_t GetNumTexCoordLayers(const CGeosetShared& geoset);
std::span<CModelTexture> GetModelTextures(CModel& model);
std::span<CModelTexture> GetTextureList(CModel& model);
std::uint32_t GetInvalidMatrixId();
std::uint32_t MatrixAlloc(std::uint32_t count);
NTempest::C34Matrix* MatrixDeref(std::uint32_t matrix);
void SetVertexMatrixIndices(CGeosetShared& geoset, std::span<const std::uint32_t> indices);
struct COpaqueLayer { float priority = 0.0F; bool HasHigherPriority(const COpaqueLayer& other) const; };
struct CTransparentObject { float depth = 0.0F; bool HasHigherPriority(const CTransparentObject& other) const; };
int CompareTexLayers(const CTexLayer& left, const CTexLayer& right);
void AddHitTestGeometryGeoset(CModel& model, const CGeosetShared& geoset);
CGeosetShared BuildComplexGeoset(std::span<const NTempest::C3Vector> vertices,
                                 std::span<const std::uint16_t> indices);
std::shared_ptr<CModel> CreateModelBoundingBox(const CModel& model);
std::shared_ptr<CModel> CreateModelBoundingSphere(const CModel& model);
void EnqueueSimpleObject(CModel& model, std::uint32_t renderFlags);
void EnqueueTransparentGeoset(CModel& model, std::uint32_t geoset);
void GeosetComplexRender(CModel& model, std::uint32_t geoset, std::uint32_t renderFlags);
NTempest::C3Vector GetGeosetSortPos(const CModel& model, std::uint32_t geoset);
bool IModelGeosetAdd(CModel& model, const CGeosetShared& geoset);
bool IModelHandleGeosetAdd(CModel& model, const CGeosetShared& geoset);
bool IModelGetBoundingSphere(const CModel& model, NTempest::CAaSphere& sphere);
bool IModelGetExtents(const CModel& model, NTempest::CAaBox& extents);
void IModelRenderSceneOpaque();
void IModelRenderSceneTransparent();
std::span<NTempest::C3Vector> LockVertices(CGeosetShared& geoset, std::size_t count);
bool LockVertsAndIndices(CGeosetShared& geoset, std::size_t vertexCount,
                         std::size_t indexCount);
void ModelSceneCalcFrustumPlanes(const NTempest::C44Matrix& viewProjection);
std::span<const NTempest::C4Plane> ModelSceneGetFrustumPlanes();
void ModelSceneSetFrustumPlanes(std::span<const NTempest::C4Plane> planes);
void ModelScenePlaceCamera(const NTempest::C3Vector& position);
void ModelScenePlaceCamera(const NTempest::C3Vector& position,
                           const NTempest::C3Vector& direction);
void ModelSetProject2dCallback(std::function<NTempest::C2Vector(const NTempest::C3Vector&)> callback);
void ClearTransformedUVLayer(std::uint32_t layer);
void ClearUvTransforms();
std::span<const NTempest::C2Vector> GetTransformedUVLayer(std::uint32_t layer);
void SetUvTransforms(std::uint32_t layer, std::span<const NTempest::C2Vector> coordinates);
void RenderCustomGeoset(CModel& model, std::uint32_t geoset);
void RenderGeoset(CModel& model, std::uint32_t geoset);
bool RenderGeosetCheckVis(const CModel& model, std::uint32_t geoset);
void RenderGeosetLayers(CModel& model, std::uint32_t geoset);
void RenderGeosetMultiUvMapping(CModel& model, std::uint32_t geoset);
void RenderGeosetOneUvMapping(CModel& model, std::uint32_t geoset);
bool RenderGeosetPrep(const CModel& model, std::uint32_t geoset);
void RenderGeosetSingleLayer(CModel& model, std::uint32_t geoset, std::uint32_t layer);
void RenderSingleUVMapPrep(CModel& model, std::uint32_t geoset);
void RenderSortedGeoset(CModel& model, std::uint32_t geoset);
void RenderUniformUVMapLayers(CModel& model, std::uint32_t geoset);
void SingleUvMapping(CModel& model, std::uint32_t geoset);
std::uint32_t SaveFog();
void RestoreFog(std::uint32_t state);
