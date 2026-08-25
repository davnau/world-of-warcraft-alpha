#pragma once

class CMapDoodadDef {
public:
    CMapDoodadDef();
    void GetBounds();
    void GetCollideExt();
    void QueryLightmap();
    void SelectLights();
    void Update();
    ~CMapDoodadDef();
};

class CMapStaticEntity {
public:
    void AdjustLightmap();
    void CreateCacheLight();
    void FindLights();
    void SelectLights();
};

// Recovered symbol declarations for mapdoodaddef.cpp

