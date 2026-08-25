#pragma once

class CMap {
public:
    void LinkEntity();
    void LinkEntityToChunk();
    void LinkEntityToMapObj();
    void LinkIntersectMapObjs();
    void UpdateEntity();
};

class CMapEntity {
public:
    CMapEntity();
    void QueryCameraFog();
    void QueryLightmap();
    void QueryLiquidSounds();
    void QueryMapObjAreaTable();
    void QueryMapObjFileName();
    void QueryMapObjFog();
    void QueryMapObjIDs();
    void QueryMapObjListenerId();
    void QueryMapObjMatrix();
    void QueryMapObjMinimap();
    void QueryMapObjSubzoneName();
    void QueryMapObjZoneName();
    void Tick();
    void UpdateMapObjLiquid();
    ~CMapEntity();
};

class CMapObj {
public:
    void GetFog();
};

class CMapStaticEntity {
public:
    void GetMapObjAndGroup();
    void GetMapObjDef();
};

// Recovered symbol declarations for mapentity.cpp

