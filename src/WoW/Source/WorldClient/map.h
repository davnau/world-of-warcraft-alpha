#pragma once

class CGxVertexPC {
public:
    CGxVertexPC();
};

class CMap {
public:
    void CalcMem();
    void ClearDetailDoodads();
    void CreateImpassableFacets();
    void Destroy();
    void GetChunkFacets();
    void GetCounts();
    void GetFacet();
    void GetFacetMapObjs();
    void GetFacetSubchunks();
    void GetFacetTerrain();
    void GetFacets();
    void GetFacetsMapObjs();
    void GetPlane();
    void GetTextureUseage();
    void GetTris();
    void GetTrisChunk();
    void GetTrisMapObjs();
    void GetTrisTerrain();
    void Initialize();
    void LocateViewerMapObjs();
    void PointIntersect();
    void TestQueryAdd();
    void VectorIntersect();
    void VectorIntersectDX();
    void VectorIntersectDY();
    void VectorIntersectDoodadDefLinkList();
    void VectorIntersectGameObjLinkList();
    void VectorIntersectMapObjs();
    void VectorIntersectSX();
    void VectorIntersectSY();
    void VectorIntersectSubchunks();
    void VectorIntersectTerrain();
    void VectorIntersectTri();
};

class CMapBaseObjLink {
public:
    ~CMapBaseObjLink();
};

class CMapCacheLight {
public:
    ~CMapCacheLight();
};

// Recovered symbol declarations for map.cpp

