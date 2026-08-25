#pragma once

class CDetailDoodad {
public:
    void AllocGeom();
    void AllocGxBuf();
    void AllocInst();
    void Clear();
    void CreateAlphaRampTexture();
    void Destroy();
    void FreeGeom();
    void FreeGxBuf();
    void FreeInst();
    void GxBufFillCallback();
    void Initialize();
    void UpdateAlphaRampTexture();
};

class CDetailDoodadData {
public:
    CDetailDoodadData();
    void Load();
    void MdlReadCallback();
    ~CDetailDoodadData();
};

class CDetailDoodadGeom {
public:
    void FillGxBufIndex();
    void FillGxBufVertex();
};

class CDetailDoodadInst {
public:
    void AddDoodad();
    CDetailDoodadInst();
    void FreeBufs();
    void Render();
    void RenderAlpha();
    ~CDetailDoodadInst();
};

// Recovered symbol declarations for detaildoodad.cpp

