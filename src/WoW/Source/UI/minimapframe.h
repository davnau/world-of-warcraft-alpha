#pragma once

#include "ENGINE/Source/Frame/frame.h"

#include <string>

class CGMinimapFrame : public CSimpleFrame {
public:
    static CSimpleFrame* Create(CSimpleFrame* parent);
    static void Initialize(int continentID);
    static void Shutdown();
    static void RegisterScriptMethods();
    static void UnregisterScriptMethods();

    explicit CGMinimapFrame(CSimpleFrame* parent = nullptr);
    ~CGMinimapFrame() override;

    void PostLoadXML(const XMLNode* node, CStatus* status);
    void OnLayerUpdate(float elapsedSec) override;
    void OnFrameRender(CRenderBatch* batch, unsigned layer) override;
    int LookupScriptMethod(lua_State* state, const char* name) override;

    void MinimapTextureCallback();
    void ObjectEnumProc();
    void OnLayerCursorExit();
    void OnLayerTrackUpdate(float elapsedSec) override;
    void Render();
    void RenderCallback();
    void RenderInside();
    void RenderInsideQuad();
    void RenderInsideSortQuads();
    void RenderInsideTexture();
    void RenderObjectBlips();
    void SetPingPosition(float x, float y);
    void SetPlayerArrowPosition();
    void UpdateArrowRotation();
    void UpdateGeometry();
    void WorldPosToMinimapFrameCoords();

    static inline std::vector<FrameScript_Method> s_scriptMethods{};

private:
    void UpdateMapTexture();

    CSimpleTexture* m_mapTexture = nullptr;
    int m_areaX = -1;
    int m_areaY = -1;
    unsigned m_zoom = 0U;
    float m_pingX = 0.0F;
    float m_pingY = 0.0F;
    std::string m_loadedPath;
    bool m_layoutReported = false;
};
