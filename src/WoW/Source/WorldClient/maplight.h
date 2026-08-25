#pragma once

class CMap {
public:
    void CreateLight();
    void DestroyLight();
    void DisableLight();
    void EnableLight();
    void GxuLightBucketSize();
    void GxuLightBucketSizeSet();
    void GxuLightCreate();
    void GxuLightDestroy();
    void GxuLightEnable();
    void GxuLightEnableSet();
    void GxuLightInitialize();
    void GxuLightLock();
    void GxuLightResetCache();
    void GxuLightSelect();
    void GxuLightSetMaxLights();
    void GxuLightShutdown();
    void GxuLightUnlock();
    void LinkLightToChunks();
    void LinkLightToMapObjDefs();
    void ProjectLights();
    void SelectLight();
    void SetLightFuncs();
    void UpdateLight();
    void UpdateLightBounds();
};

class CMapLight {
public:
    CMapLight();
    void CreatePointAtten();
    void DestroyPointAtten();
    void GetPointAttenTex();
    void Project();
    void ProjectLightRenderPN();
    ~CMapLight();
};

class CWTriData {
public:
    void GetBatch();
};

// Recovered symbol declarations for maplight.cpp

