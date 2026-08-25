#pragma once

class CChunkLiquid {
public:
    void GetAaBox();
    void Render();
    void Render0I();
    void RenderMagma0();
    void RenderMagma0Callback();
    void RenderMagma0V();
    void RenderOcean0();
    void RenderOcean0Callback();
    void RenderOcean0V();
    void RenderRiver0();
    void RenderRiver0Callback();
    void RenderRiver0V();
};

class CMap {
public:
    void GetLiquidTexture();
    void OceanFFT();
    void QueryLiquidSounds();
    void UnloadLiquidTexture();
    void UpdateLiquidTextures();
    void WaterDestroy();
    void WaterDiffTexCallback();
    void WaterInitialize();
    void WaterRipple();
};

class CMapArea {
public:
    void InitWater();
    void QueryLiquidSounds();
};

class CMapObj {
public:
    void QueryLiquidSounds();
};

class CMapObjGroup {
public:
    void QueryLiquidSounds();
};

class CWorld {
public:
    void GetCurTimeSec();
};

class ChunkLodIdx {
public:
    void GenCenterIndices();
    void GenCenterIndicesRow();
    void GenEdgeIndices();
    void GenIndices();
    void GenLinkIndices();
};

class LODArrays {
public:
    void GenFixes();
    void GenVerts();
};

class Particulate {
public:
    void ComputeMovement();
    void CustomRenderCallback();
    void InitMovement();
    void InitParticles();
    Particulate();
    void Render();
    void SetPercentage();
    void SetScale();
    void SetSize();
    void SetTexture();
    void Update();
    ~Particulate();
};

class WaterRadWave {
public:
    void Init();
    void Update();
};

class WaveTrain {
public:
    void Contains();
    void Init();
    void Move();
    void Phase();
};

// Recovered symbol declarations for mapwater.cpp

