#include "WoW/Source/UI/minimapframe.h"

#include "ENGINE/Source/Frame/framescript.h"
#include "WoW/Source/Game/GameClient/minimap.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/WorldClient/World.h"

extern "C" {
#include "lauxlib.h"
}

#include <array>
#include <cmath>
#include <cstdio>

namespace {
constexpr float kAreaWorldSize = 533.33331F;
constexpr float kHalfWorldSize = 17066.666F;

CGMinimapFrame* GetMinimap(lua_State* state) {
    return dynamic_cast<CGMinimapFrame*>(FrameScript_GetObject(state, 1));
}

int CGMinimapFrame_SetZoom(lua_State* state) {
    CGMinimapFrame* frame = GetMinimap(state);
    MinimapSetZoom(static_cast<unsigned>(luaL_checknumber(state, 2)));
    if (frame) frame->UpdateGeometry();
    return 0;
}

int CGMinimapFrame_GetZoom(lua_State* state) {
    lua_pushnumber(state, MinimapGetZoom());
    return 1;
}

int CGMinimapFrame_GetZoomLevels(lua_State* state) {
    lua_pushnumber(state, MinimapGetZoomLevels());
    return 1;
}

int CGMinimapFrame_PingLocation(lua_State* state) {
    if (CGMinimapFrame* frame = GetMinimap(state)) {
        frame->SetPingPosition(
            static_cast<float>(luaL_checknumber(state, 2)),
            static_cast<float>(luaL_checknumber(state, 3)));
    }
    return 0;
}

int CGMinimapFrame_GetPingPosition(lua_State* state) {
    lua_pushnumber(state, 0.0);
    lua_pushnumber(state, 0.0);
    return 2;
}
}

CSimpleFrame* CGMinimapFrame::Create(CSimpleFrame* parent) {
    return new CGMinimapFrame(parent);
}

void CGMinimapFrame::Initialize(int continentID) {
    MinimapInitialize(continentID);
}

void CGMinimapFrame::Shutdown() {
    MinimapShutdown();
}

CGMinimapFrame::CGMinimapFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent) {
    m_mapTexture = new CSimpleTexture(this);
    m_mapTexture->SetAllPoints(this);
    RegisterRegion(m_mapTexture, 0U);
    EnableEvent(SIMPLE_EVENT_UPDATE);
}

CGMinimapFrame::~CGMinimapFrame() = default;

void CGMinimapFrame::PostLoadXML(const XMLNode* node, CStatus* status) {
    CSimpleFrame::PostLoadXML(node, status);
    if (m_mapTexture) m_mapTexture->SetAllPoints(this);
    UpdateMapTexture();
}

void CGMinimapFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    if (!m_layoutReported) {
        NTempest::CRect rect{};
        NTempest::CRect parentRect{};
        NTempest::CRect textureRect{};
        const bool hasRect = GetRect(&rect);
        const bool hasParentRect = m_parent && m_parent->GetRect(&parentRect);
        const bool hasTextureRect = m_mapTexture && m_mapTexture->GetRect(&textureRect);
        std::fprintf(stderr,
                     "MINIMAP_LAYOUT name=%s shown=%d visible=%d strata=%d level=%d "
                     "rect=%d(%.6f,%.6f,%.6f,%.6f) parent=%s shown=%d visible=%d "
                     "rect=%d(%.6f,%.6f,%.6f,%.6f) texture=%p rect=%d(%.6f,%.6f,%.6f,%.6f)\n",
                     GetName(), m_shown ? 1 : 0, m_visible ? 1 : 0, m_strata, m_level,
                     hasRect ? 1 : 0, rect.l, rect.t, rect.r, rect.b,
                     m_parent && m_parent->GetName() ? m_parent->GetName() : "<none>",
                     m_parent && m_parent->m_shown ? 1 : 0,
                     m_parent && m_parent->m_visible ? 1 : 0,
                     hasParentRect ? 1 : 0, parentRect.l, parentRect.t,
                     parentRect.r, parentRect.b, static_cast<void*>(m_mapTexture),
                     hasTextureRect ? 1 : 0, textureRect.l, textureRect.t,
                     textureRect.r, textureRect.b);
        m_layoutReported = true;
    }
    if (ClntObjMgrGetActivePlayer() != 0U) {
        g_minimap.SetCurrentPosition(
            static_cast<int>(ClntObjMgrGetMapID()),
            ClntObjMgrGetActivePosition());
        UpdateMapTexture();
    }
}

void CGMinimapFrame::OnFrameRender(CRenderBatch* batch, unsigned layer) {
    CSimpleFrame::OnFrameRender(batch, layer);
}

void CGMinimapFrame::UpdateMapTexture() {
    if (!m_mapTexture || !CWorld::IsLoaded()) return;
    const NTempest::C3Vector& position = ClntObjMgrGetActivePosition();
    const int areaX = static_cast<int>(
        std::floor((kHalfWorldSize - position.x) / kAreaWorldSize));
    const int areaY = static_cast<int>(
        std::floor((kHalfWorldSize - position.y) / kAreaWorldSize));
    const unsigned zoom = MinimapGetZoom();
    if (areaX == m_areaX && areaY == m_areaY && zoom == m_zoom &&
        !m_loadedPath.empty()) return;

    m_areaX = areaX;
    m_areaY = areaY;
    m_zoom = zoom;
    const char* mapName = CWorld::GetMapName();
    if (!mapName || !*mapName) return;

    std::array<std::array<char, 260>, 6> candidates{};
    std::snprintf(candidates[0].data(), candidates[0].size(),
                  "World\\Minimaps\\%s\\map%d_%d.blp",
                  mapName, areaX, areaY);
    std::snprintf(candidates[1].data(), candidates[1].size(),
                  "World\\Minimaps\\%s\\map%d_%d.blp",
                  mapName, areaY, areaX);
    std::snprintf(candidates[2].data(), candidates[2].size(),
                  "World\\Minimaps\\%s\\%s_%d_%d.blp",
                  mapName, mapName, areaX, areaY);
    std::snprintf(candidates[3].data(), candidates[3].size(),
                  "World\\Minimaps\\%s\\%s_%d_%d.blp",
                  mapName, mapName, areaY, areaX);
    std::snprintf(candidates[4].data(), candidates[4].size(),
                  "World\\Minimaps\\%s_%d_%d.blp",
                  mapName, areaX, areaY);
    std::snprintf(candidates[5].data(), candidates[5].size(),
                  "World\\Minimaps\\%s_%d_%d.blp",
                  mapName, areaY, areaX);

    m_loadedPath.clear();
    for (const auto& candidate : candidates) {
        if (m_mapTexture->SetTexture(candidate.data(), 0)) {
            m_loadedPath = candidate.data();
            break;
        }
    }
    if (m_loadedPath.empty()) {
        m_mapTexture->SetTexture("Interface\\Minimap\\UI-Minimap-Background");
        m_loadedPath = "Interface\\Minimap\\UI-Minimap-Background";
    }
    m_mapTexture->SetTexCoord(0.0F, 1.0F, 0.0F, 1.0F);
    std::fprintf(stderr, "MINIMAP_TEXTURE area=%d,%d zoom=%u path=%s\n",
                 areaX, areaY, zoom, m_loadedPath.c_str());
}

void CGMinimapFrame::RegisterScriptMethods() {
    FrameScript_Object::FillScriptMethodTable(s_scriptMethods, {
        {"SetZoom", CGMinimapFrame_SetZoom},
        {"GetZoom", CGMinimapFrame_GetZoom},
        {"GetZoomLevels", CGMinimapFrame_GetZoomLevels},
        {"PingLocation", CGMinimapFrame_PingLocation},
        {"GetPingPosition", CGMinimapFrame_GetPingPosition},
    });
}

void CGMinimapFrame::UnregisterScriptMethods() { s_scriptMethods.clear(); }

int CGMinimapFrame::LookupScriptMethod(lua_State* state, const char* name) {
    return FrameScript_Object::LookupScriptMethod(state, name, s_scriptMethods);
}

void CGMinimapFrame::SetPingPosition(float x, float y) {
    m_pingX = x;
    m_pingY = y;
}

void CGMinimapFrame::MinimapTextureCallback() { UpdateMapTexture(); }
void CGMinimapFrame::ObjectEnumProc() { }
void CGMinimapFrame::OnLayerCursorExit() { }
void CGMinimapFrame::OnLayerTrackUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerTrackUpdate(elapsedSec);
}
void CGMinimapFrame::Render() { }
void CGMinimapFrame::RenderCallback() { }
void CGMinimapFrame::RenderInside() { }
void CGMinimapFrame::RenderInsideQuad() { }
void CGMinimapFrame::RenderInsideSortQuads() { }
void CGMinimapFrame::RenderInsideTexture() { }
void CGMinimapFrame::RenderObjectBlips() { }
void CGMinimapFrame::SetPlayerArrowPosition() { }
void CGMinimapFrame::UpdateArrowRotation() { }
void CGMinimapFrame::UpdateGeometry() { UpdateMapTexture(); }
void CGMinimapFrame::WorldPosToMinimapFrameCoords() { }
