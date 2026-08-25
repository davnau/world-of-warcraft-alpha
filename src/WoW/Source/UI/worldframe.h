#pragma once

#include "ENGINE/Source/Frame/frame.h"
#include "WoW/Source/UIUtil/camera.h"

#include <array>
#include <cstdint>
#include <vector>

class CGWorldFrame : public CSimpleFrame {
public:
    static CGWorldFrame* s_currentWorldFrame;
    static CSimpleFrame* Create(CSimpleFrame* parent);

    explicit CGWorldFrame(CSimpleFrame* parent = nullptr);
    ~CGWorldFrame() override;

    void OnLayerUpdate(float elapsedSec) override;
    void OnFrameRender(CRenderBatch* batch, unsigned layer) override;
    int OnLayerKeyDown(CKeyEvent* event) override;
    int OnLayerKeyUp(CKeyEvent* event) override;
    int OnLayerMouseDown(CMouseEvent* event) override;
    int OnLayerMouseUp(CMouseEvent* event) override;
    int OnLayerMouseMoveRelative(CMouseEvent* event) override;
    int OnLayerMouseWheel(CMouseEvent* event) override;
    void RenderWorld();
    void OnWorldUpdate();
    void OnWorldRender();

    void AddModelToScene();
    void CursorTrackObject();
    void CursorTrackUnit();
    void FindClosestModel();
    void GeometryTestModels();
    CGCamera* GetActiveCamera();
    NTempest::C3Vector GetCameraFacing();
    NTempest::C3Vector GetCameraPosition();
    void GetHitTestFilterFlags();
    void GetLineSegment();
    void GetObjectUnderMouse();
    void GetScreenCoordinates();
    void GetSkyProgress();
    void HandleUnitFade();
    void HideObstructingModels();
    void HigherPriorityModel();
    void HitTest();
    void HitTestPoint();
    void IsLegalSelection();
    void IsUnitLegalSelection();
    void MoveToFreeList();
    void OnLayerTrackObject();
    void OnLayerTrackTerrain();
    void OnLayerTrackUpdate(float elapsedSec) override;
    void OnMouseModeNormal();
    void OnMouseModeRelative();
    void PerformDefaultAction(unsigned action, unsigned timestamp);
    void ReduceToClosestModel();
    void RefreshPlayerAlpha();
    void RegisterObjectFadeoutModel();
    void SendObjectTrackEvent();
    void SendUnitFadeEvent();
    void SetCameraTarget();
    void SetNamePlateUpdate();
    void SetPlayerFadeCameraValue();
    void SetPlayerRender();
    void SetSpriteClickButtons();
    void SetTerrainClickButtons();
    void SphereTestModels();
    void TogglePlayerRender();
    void UnitUpdate();
    void UpdateDayNightInfo();
    void UpdateObject();
    void UpdatePlayerAlpha();
    void VolumeTestModels();

private:
    struct PickTarget {
        std::uint64_t guid = 0U;
        float x = 0.0F;
        float y = 0.0F;
        float halfWidth = 0.0F;
        float halfHeight = 0.0F;
        float distanceSq = 0.0F;
    };

    void DrawSkyBackground();
    void UpdatePickTargets();
    bool PickUnitAtCursor();
    bool PickUnitAt(const NTempest::C2Vector& cursor);

    std::array<std::array<char, 32>, KEY_LAST> m_lastKey{};
    CGCamera* m_camera = nullptr;
    float m_elapsedSec = 0.0F;
    NTempest::C2Vector m_actionCursor{};
    bool m_actionCursorValid = false;
    std::uint64_t m_lastPickedGuid = 0U;
    std::vector<PickTarget> m_pickTargets;
};
