#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/caabox.h"
#include "ENGINE/Source/Tempest/caasphere.h"
#include "Storm/h/stpl.h"

#include <array>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <vector>

class CParticleEmitter2;
class CRibbonEmitter;
class CTexture;
class CStatus;
using HTEXTURE__ = CTexture;

struct HMODEL__;
struct HMODELSHARED__;
struct CAnim;
struct CAnimData;
using HANIM__ = CAnim;
struct HCAMERA__;
struct HCOLLISIONDATA__;
struct CModel;

enum COLLIDE_TYPE : std::uint32_t {
    COLLIDE_SPHERE = 0,
    COLLIDE_BOX = 1,
    COLLIDE_CYLINDER = 2,
};

enum EModelLoad : std::uint32_t {
    MODEL_LOAD_PENDING,
    MODEL_LOAD_COMPLETE,
    MODEL_LOAD_FAILED,
};

struct CBoundsData {
    NTempest::CAaBox extent{};
    NTempest::CAaSphere sphere{};
};

struct CHitTest {
    COLLIDE_TYPE type = COLLIDE_SPHERE;
    NTempest::C3Vector extent[2]{};
    float radius = 0.0F;
};

struct CPrimitive {
    std::uint32_t type = 0;
    std::uint32_t start = 0;
    std::uint32_t count = 0;
};

struct CGeosetShared {
    std::vector<NTempest::C3Vector> position;
    std::vector<std::uint8_t> boneWeights;
    std::vector<NTempest::C3Vector> normal;
    std::vector<std::vector<NTempest::C2Vector>> texCoord;
    std::vector<CPrimitive> primitive;
    std::vector<std::uint16_t> primitiveVertices;
    std::vector<std::uint32_t> groupMatrixCounts;
    std::vector<std::uint32_t> matrices;
    std::vector<std::uint32_t> hwBoneIndices;
    std::vector<std::uint32_t> hwBoneWeights;
    std::uint32_t vertexShader = 0;
    std::uint32_t materialId = 0;
    NTempest::C3Vector centroid{};
    float radius = 0.0F;
    std::uint32_t selectionGroup = 0;
    std::uint32_t geosetId = 0;
    std::uint32_t flags = 0;
};

struct CGeoset {
    std::uint32_t weightedBones = ~0U;
    std::uint32_t flags = 0;
};

#ifndef WOW_CGEOSETCOLOR_DEFINED
#define WOW_CGEOSETCOLOR_DEFINED
struct CGeosetColor {
    CGeosetColor() = default;
    NTempest::C3Vector color{1.0F};
    float alpha = 1.0F;
};
#endif

struct CCustomGeoset {
    CGeosetShared shared;
    CGeoset unique;
};

struct CModelTexture {
    CModelTexture();
    ~CModelTexture();
    CModelTexture(const CModelTexture&);
    CModelTexture& operator=(const CModelTexture&);
    HTEXTURE__* handle = nullptr;
    std::uint32_t replaceableId = 0;
};

struct CTexLayer {
    CTexLayer() = default;
    static int Compare(const CModelTexture* textures, const CTexLayer& left, const CTexLayer& right);
    // These fields mirror the unique/shared CTexLayer state reconstructed from
    // the original client.  `flags` is the internal disables mask, not the raw
    // MDX shading-flags word.
    std::uint32_t vertexFormat = 6;
    std::uint32_t flags = 0;
    std::uint32_t blendMode = 0;
    std::uint32_t combiner = 1;
    std::uint32_t texture = ~0U;
    std::int32_t transformId = -1;
    std::int32_t coordId = 0;
    std::uint32_t textureShader = 0;
    std::uint32_t tmuFlags = 0;
    float alpha = 1.0F;
    std::uint32_t animationId = ~0U;
};

struct CMaterial {
    std::vector<CTexLayer> layers;
    std::uint32_t flags = 0;
    std::int32_t priorityPlane = 0;
    NTempest::C3Vector emissive{};
};

struct GROUND_TRACK {
    std::uint32_t bone = 0;
    float height = 0.0F;
};

struct CCollisionData {
    std::vector<NTempest::C3Vector> vertices;
    std::vector<std::uint16_t> indices;
    std::vector<NTempest::C3Vector> surfaceNormals;
    NTempest::CAaBox extents{};
};

struct CModelShared {
    const char* GetObjectName() const { return name.data(); }
    std::vector<CBoundsData> seqBounds;
    std::vector<std::uint32_t> attachIdToIndex;
    std::vector<NTempest::C3Vector> positions;
    std::vector<CHitTest> hitTest;
    std::vector<CGeosetShared> geosets;
    std::vector<std::uint32_t> emitter2Order;
    std::vector<std::uint32_t> ribbonOrder;
    std::uint32_t numBones = 0;
    std::uint32_t numTexBones = 0;
    GROUND_TRACK groundTrack{};
    std::shared_ptr<CCollisionData> collision;
    std::array<char, 260> name{};
    CBoundsData bounds{};
    std::uint8_t numGeosets = 0;
    std::uint8_t numLayers = 0;
};

struct CModelBase {
    using PickLights = void (*)(void*, NTempest::C3Vector, const NTempest::C3Vector*, unsigned);
    CModelBase();
    virtual ~CModelBase();
    PickLights m_PickLights = nullptr;
    void* m_pickLightsParm = nullptr;
    std::uint32_t m_flags = 0;
    NTempest::C34Matrix m_modelToWorld{};
    std::uint32_t m_texBones = 0;
    HANIM__* m_anim = nullptr;
    HMODEL__* m_boundsModel = nullptr;
    std::uint32_t m_aaBoxCustGeoId = 0;
    HMODEL__* m_collideModel = nullptr;
};

struct CModelComplex : CModelBase {
    CModelComplex();
    ~CModelComplex() override;
    void CopyAttachments(const CModelComplex& source);
    void CopyCameras(const CModelComplex& source);
    void CopyEmitters(const CModelComplex& source);
    void CopyLights(const CModelComplex& source);
    void CopyRibbons(const CModelComplex& source);
    std::vector<CGeoset> m_geosets;
    std::vector<CGeoset> m_addlGeosets;
    std::vector<CGeosetColor> m_geosetColor;
    std::vector<CCustomGeoset> m_customGeosets;
    std::vector<CModelTexture> m_textures;
    std::vector<CMaterial> m_materials;
    std::vector<std::uintptr_t> m_lights;
    std::vector<CParticleEmitter2*> m_emitters2;
    std::vector<CRibbonEmitter*> m_ribbons;
    std::vector<HCAMERA__*> m_cameras;
};

struct CModelSimple : CModelBase {
    CModelSimple();
    ~CModelSimple() override;
    void CopyMaterials(const CModelSimple& source);
    std::array<CGeoset, 16> m_geosets{};
    std::uint32_t m_numGeosets = 0;
};

struct CModelCreate {
    std::uint32_t flags = 0;
    const char** sequenceNames = nullptr;
    std::uint32_t numSequences = 0;
    const char** boneNames = nullptr;
    std::uint32_t numBones = 0;
    const char** cameraNames = nullptr;
    std::uint32_t numCameras = 0;
};

struct CModelSequence {
    std::uint32_t id = 0;
    std::string name;
    std::uint32_t durationMs = 0;
    float moveSpeed = 0.0F;
    std::uint32_t flags = 0;
};

struct CModelLink {
    std::uint32_t point = 0;
    CModel* model = nullptr;
    float scale = 1.0F;
};

struct CModel {
    CModel();
    ~CModel();
    CModel(CModel&&) noexcept;
    CModel& operator=(CModel&&) noexcept;
    CModel(const CModel&) = delete;
    CModel& operator=(const CModel&) = delete;
    void DeleteAsyncObj();
    void RemoveModelCommandsFromQueue();
    void FinishDuplication(const CModel& source);
    std::unique_ptr<CModelBase> data;
    std::shared_ptr<CModelShared> shared;
    std::shared_ptr<CAnimData> animData;
    std::unique_ptr<CAnim> anim;
    CModelCreate create{};
    EModelLoad load = MODEL_LOAD_PENDING;
    std::vector<CModelSequence> sequences;
    std::vector<CModelLink> links;
    std::uint32_t primarySequence = 0;
    std::uint32_t sequenceTimeMs = 0;
    float timeScale = 1.0F;
    bool paused = false;
    bool emittersEnabled = true;
    bool ribbonsEnabled = true;
    bool lightsEnabled = true;
    bool fullAlpha = false;
    std::function<void(CModel&)> sequenceFinished;
    std::function<void(const char*, const NTempest::C3Vector&)> eventCallback;
    std::vector<float> layerAlpha;
    std::vector<std::uint32_t> layerTextureIds;
    std::vector<NTempest::C34Matrix> transforms;
    std::uint32_t fidgetSequence = ~0U;
    std::uint32_t fidgetCount = 0;
};

struct ModelIntersectResult {
    bool hit = false;
    float distance = 0.0F;
    std::uint32_t index = 0;
    NTempest::C3Vector point{};
    NTempest::C3Vector normal{};
};
