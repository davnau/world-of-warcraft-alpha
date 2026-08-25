#include "WoW/Source/WorldClient/mapobj.h"
#include "ENGINE/Source/Services/texture.h"

CMapObj::CMapObj() { }
void CMapObj::Clear() { Destroy(); }
void CMapObj::ClearCache() { }
void CMapObj::Create() { }
void CMapObj::Delete() { }
void CMapObj::Destroy() {
    for (CMapObjMaterial& material : materials) {
        if (material.diffuse) TextureRelease(material.diffuse);
        if (material.environment) TextureRelease(material.environment);
        material.diffuse = nullptr;
        material.environment = nullptr;
    }
    materials.clear();
    groups.clear();
    name.clear();
    textureNames.clear();
    groupNames.clear();
    forceTwoSided = false;
    loaded = false;
}
void CMapObj::GetBounds() { }
void CMapObj::GetDoodadSet() { }
void CMapObj::GetGroup() { }
void CMapObj::GetGroupBounds() { }
void CMapObj::GetGroupFlags() { }
void CMapObj::GetGroupInfo() { }
void CMapObj::GetGroupName() { }
void CMapObj::GetTris() { }
void CMapObj::Init() { }
void CMapObj::InitPtrs() { }
void CMapObj::Initialize() { }
void CMapObj::IsGroupLoaded() { }
void CMapObj::IsGroupLoading() { }
void CMapObj::QueryLightmap() { }
void CMapObj::QueryLiquidFishable() { }
void CMapObj::QueryLiquidStatus() { }
void CMapObj::QueryMapObjMinimap() { }
void CMapObj::QueryMapObjMinimapGroup() { }
void CMapObj::TestBounds() { }
void CMapObj::TestConvexVolume() { }
void CMapObj::TestGroupBounds() { }
void CMapObj::VectorIntersect() { }
void CMapObj::VectorIntersectPortal() { }
void CMapObj::VectorIntersectPortals() { }
void CMapObj::WaitLoad() { }
void CMapObj::WaitLoadGroup() { }
CMapObj::~CMapObj() { Destroy(); }
void CMapObjGroup::QueryMinimap() { }
void NearestPow2() { }
