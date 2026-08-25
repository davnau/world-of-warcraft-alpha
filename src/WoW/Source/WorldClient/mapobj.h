#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/caabox.h"
#include "ENGINE/Source/Tempest/cimvector.h"

#include <cstdint>
#include <string>
#include <vector>

class CTexture;
class CGxTex;
using HTEXTURE__ = CTexture;

struct CMapObjLightmapTexture {
    std::vector<std::uint8_t> texels;
    CGxTex* texture = nullptr;
};

struct CMapObjMaterial {
    std::uint32_t version = 0;
    std::uint32_t flags = 0;
    std::uint32_t blendMode = 0;
    std::uint32_t diffuseNameIndex = 0;
    NTempest::CImVector sidnColor{0xffffffffU};
    NTempest::CImVector frameSidnColor{0xffffffffU};
    std::uint32_t envNameIndex = 0;
    NTempest::CImVector diffuseColor{0xffffffffU};
    std::uint32_t groundType = 0;
    HTEXTURE__* diffuse = nullptr;
    HTEXTURE__* environment = nullptr;
};

struct CMapObjBatch {
    std::uint8_t lightMap = 0;
    std::uint8_t texture = 0;
    std::uint16_t startIndex = 0;
    std::uint16_t count = 0;
    std::uint16_t minIndex = 0;
    std::uint16_t maxIndex = 0;
    std::uint8_t flags = 0;
};

class CMapObjGroup {
public:
    CMapObjGroup();
    ~CMapObjGroup();

    void AllocExtGxBuf();
    void AllocIntGxBuf();
    void AsyncPostloadCallback();
    void Clear();
    void Create();
    void CreateDataPointers();
    void CreateLightmapPointers();
    void CreateLightmaps();
    void CreateOptionalDataPointers();
    void Destroy();
    void ExtGxBufFill();
    void ExtGxBufFillVertex();
    void FreeExtGxBuf();
    void FreeIntGxBuf();
    void FreeLightmaps();
    void GetTris();
    void GetTrisFromQuery();
    void GxBufFillIndex();
    void Init();
    void InitPtrs();
    void IntGxBufFill();
    void IntGxBufFillVertex();
    void PointInPoly();
    void QueryLightmap();
    void QueryLiquidFishable();
    void QueryLiquidStatus();
    void QueryMinimap();
    void SphereIntersectPoly();
    void UpdateLightmapTex();

    // SMOGroupInfo metadata from the root MOGI chunk.  These flags and bounds
    // drive exterior/portal visibility; CMapObjGroup::flags is the separate
    // embedded MOGP group-header field used by the material render path.
    std::uint32_t infoFlags = 0;
    NTempest::CAaBox bounds{};
    std::uint32_t flags = 0;
    std::string name;
    std::vector<NTempest::C3Vector> vertices;
    std::vector<NTempest::C3Vector> normals;
    std::vector<NTempest::C2Vector> textureCoordinates;
    std::vector<NTempest::C2Vector> lightmapCoordinates;
    std::vector<NTempest::CImVector> vertexColors;
    std::vector<std::uint16_t> indices;
    std::vector<CMapObjBatch> batches;
    std::vector<CMapObjLightmapTexture> lightmapTextures;
    bool loaded = false;
    bool lightmapsCreated = false;
    unsigned lightmapRevision = 0;
};

class CMapObj {
public:
    CMapObj();
    ~CMapObj();

    bool Read(const char* fileName);
    void Render(const NTempest::C44Matrix& worldTransform) const;
    bool IsLoaded() const noexcept { return loaded; }

    void AsyncPostloadCallback();
    void AsyncPostloadCallbackAll();
    void AsyncPostloadCallbackHeader();
    void Clear();
    void ClearCache();
    void Create();
    void CreateAllGroups();
    void CreateData();
    void CreateDataPointers();
    void CreateGroup();
    void CreateMaterial();
    void CreateMaterials();
    void CullBatch();
    void Delete();
    void Destroy();
    void ExtRender();
    void GetBounds();
    void GetDoodadSet();
    void GetGroup();
    void GetGroupBounds();
    void GetGroupFlags();
    void GetGroupInfo();
    void GetGroupName();
    void GetTris();
    void Init();
    void InitPtrs();
    void Initialize();
    void IntRender();
    void IsGroupLoaded();
    void IsGroupLoading();
    void LocateViewer();
    void PrepareUpdate();
    void QueryLightmap();
    void QueryLiquidFishable();
    void QueryLiquidStatus();
    void QueryMapObjMinimap();
    void QueryMapObjMinimapGroup();
    void Read();
    void ReadChunkHeader();
    void ReadExtGroups();
    void ReadGroup();
    void ReadOptionalChunkHeader();
    void RenderAlways();
    void RenderExteriorWater_0();
    void RenderGroup();
    void RenderGroupBsp();
    void RenderGroupColorTex();
    void RenderGroupColorTex_Ext();
    void RenderGroupColorTex_Int();
    void RenderGroupLightTex();
    void RenderGroupLightmap();
    void RenderGroupLightmapTex();
    void RenderGroupLightmapTex_Ext();
    void RenderGroupLightmapTex_Int();
    void RenderGroupNormals();
    void RenderGroupTex();
    void RenderGroup_Ext();
    void RenderGroup_Int();
    void RenderInteriorWater_0();
    void RenderLiquid_0();
    void RenderMagma();
    void RenderPortals();
    void RenderWaterIndices_0();
    void RRenderThruPortals();
    void RTransformPortal();
    void SetGroupRenderCallback();
    void StabPortals();
    void TestBounds();
    void TestConvexVolume();
    void TestGroupBounds();
    void UpdateMaterials();
    void VectorIntersect();
    void VectorIntersectPortal();
    void VectorIntersectPortals();
    void WaitLoad();
    void WaitLoadGroup();

    std::string name;
    std::string textureNames;
    std::string groupNames;
    std::vector<CMapObjMaterial> materials;
    std::vector<CMapObjGroup> groups;
    bool forceTwoSided = false;
    bool loaded = false;
};
