#include "WoW/Source/WorldClient/mapobjgroup.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"

void CMapObjGroup::AllocExtGxBuf() { }
void CMapObjGroup::AllocIntGxBuf() { }
CMapObjGroup::CMapObjGroup() { }
void CMapObjGroup::Clear() { Destroy(); }
void CMapObjGroup::Destroy() {
    vertices.clear();
    normals.clear();
    textureCoordinates.clear();
    lightmapCoordinates.clear();
    vertexColors.clear();
    indices.clear();
    batches.clear();
    for (CMapObjLightmapTexture& lightmap : lightmapTextures) {
        if (lightmap.texture) GxTexDestroy(lightmap.texture);
        lightmap.texture = nullptr;
    }
    lightmapTextures.clear();
    name.clear();
    infoFlags = 0;
    bounds = {};
    flags = 0;
    loaded = false;
}
void CMapObjGroup::ExtGxBufFill() { }
void CMapObjGroup::FreeExtGxBuf() { }
void CMapObjGroup::FreeIntGxBuf() { }
void CMapObjGroup::GetTris() { }
void CMapObjGroup::GetTrisFromQuery() { }
void CMapObjGroup::Init() { }
void CMapObjGroup::InitPtrs() { }
void CMapObjGroup::IntGxBufFill() { }
void CMapObjGroup::PointInPoly() { }
void CMapObjGroup::QueryLightmap() { }
void CMapObjGroup::QueryLiquidFishable() { }
void CMapObjGroup::QueryLiquidStatus() { }
void CMapObjGroup::SphereIntersectPoly() { }
CMapObjGroup::~CMapObjGroup() { Destroy(); }
void QueryCull() { }
