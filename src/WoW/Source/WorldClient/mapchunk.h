#pragma once

class CChunkLayer {
public:
    CChunkLayer();
    ~CChunkLayer();
};

class CChunkTex {
public:
    CChunkTex();
    ~CChunkTex();
};

class CMapChunk {
public:
    void AllocAlphaGxTex();
    void AllocAsyncLoadBuffer();
    void AllocGxBuf();
    void AllocShadowGxTex();
    void AsyncCallback();
    void AsyncPollHandler();
    CMapChunk();
    void Create();
    void CreateAlphaShadow();
    void CreateChunkLayerTex();
    void CreateChunkShaderTex();
    void CreateChunkShadowTex();
    void CreateFacePlanes();
    void CreateLayer();
    void CreateNormals();
    void CreateRefs();
    void CreateRenderLists();
    void CreateShadow();
    void CreateVertices();
    void Destroy();
    void FindLights();
    void FreeAlphaGxTex();
    void FreeAsyncLoadBuffer();
    void FreeGxBuf();
    void FreeLists();
    void FreeShadowGxTex();
    void InitAsyncLoadBuffers();
    void Initialize();
    void Load();
    void SelectLights();
    void SyncLoad();
    void SyncLoadLayer();
    void SyncLoadShader();
    void SyncLoadShadow();
    void UnpackAlphaBits();
    void UnpackAlphaShadowBits();
    void UnpackShadowBits();
    void Update();
    void UpdateLayerGxTexture();
    void UpdateLights();
    void UpdateShaderGxTexture();
    void UpdateShadowGxTexture();
    void UpdateTextureDefault();
    ~CMapChunk();
};

// Recovered symbol declarations for mapchunk.cpp

