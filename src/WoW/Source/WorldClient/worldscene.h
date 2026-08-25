#pragma once

class CGxVertexPC {
public:
    ~CGxVertexPC();
};

class CSortEntry {
public:
    CSortEntry();
    ~CSortEntry();
};

class CSortTable {
public:
    void Clear();
    void Destroy();
    void Initialize();
};

class CWFrustum {
public:
    CWFrustum();
    void CalcPlanesFromCorners();
    void Cull();
    void Transform();
    void Translate();
};

class CWorld {
public:
    void NDCClip();
    void NDCXform();
};

class CWorldScene {
public:
    void AddChunkLiquid();
    void AddDoodadDef();
    void AddMapChunk();
    void AddMapEntity();
    void AddMapObjDef();
    void AddViewerGroup2();
    void AllocFrustum();
    void CalcFrustumCorners();
    void ClipBufferClear();
    void ClipBufferCull();
    void ClipBufferUpdate();
    void ClipPortal();
    void CullChunkLiquid();
    void CullChunks();
    void CullDoodads();
    void CullEntitys();
    void CullHorizon();
    void CullMapObjDef();
    void CullMapObjDefGroup();
    void CullMapObjDefs();
    void CullSortTable();
    void Destroy();
    void FreeFrustum();
    void FrustumCull();
    void FrustumGet();
    void FrustumPop();
    void FrustumPush();
    void FrustumSet();
    void FrustumXform();
    void Initialize();
    void LocateViewer();
    void LocateViewer2();
    void LocateViewer3();
    void PrepareRender();
    void PrepareRenderLiquid();
    void Render();
    void RenderAlpha();
    void RenderChunks();
    void RenderDoodads();
    void RenderHorizon();
    void RenderMagma();
    void RenderMapObjDefGroups();
    void RenderObjects();
    void RenderOcean();
    void RenderWater();
    void Update();
};

class ClipInfo {
public:
    void Set();
};

// Recovered symbol declarations for worldscene.cpp

