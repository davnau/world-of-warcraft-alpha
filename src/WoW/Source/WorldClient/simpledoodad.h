#pragma once

class CSimpleDoodad {
public:
    void AddToScene();
    void ClearCache();
    void Create();
    void CreateIndices();
    void CreateVertices();
    void Delete();
    void Destroy();
    void GxBufDynCallback();
    void Initialize();
    void MdlReadCallback();
    void PrepareUpdate();
    void Read();
    void RenderScene();
};

class CSimpleDoodadGeoset {
public:
    CSimpleDoodadGeoset();
    ~CSimpleDoodadGeoset();
};

// Recovered symbol declarations for simpledoodad.cpp

