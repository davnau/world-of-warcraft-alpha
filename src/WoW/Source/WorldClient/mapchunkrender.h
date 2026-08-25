#pragma once

class CMapChunk {
public:
    void CreateDetailDoodads();
    void FillGxBufDynIndex();
    void FillGxBufDynVertex();
    void FillGxBufIndex();
    void FillGxBufVertex();
    void GxBufDynFillCallback();
    void GxBufFillCallback();
    void Render();
    void RenderLayers();
    void RenderLayersColor();
    void RenderLayersColorDyn();
    void RenderLayersDyn();
};

// Recovered symbol declarations for mapchunkrender.cpp

