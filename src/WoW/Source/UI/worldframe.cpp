#include "WoW/Source/UI/worldframe.h"

#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "WoW/common/daynight.h"
#include "WoW/Source/Game/GameClient/gametime_c.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/UI/gameui.h"
#include "WoW/Source/UI/uibindings.h"
#include "WoW/Source/UIUtil/inputcontrol.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowSvcs/wowsvcsclient/clientservices.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <vector>

CGWorldFrame* CGWorldFrame::s_currentWorldFrame = nullptr;

CSimpleFrame* CGWorldFrame::Create(CSimpleFrame* parent) {
    return new CGWorldFrame(parent);
}

CGWorldFrame::CGWorldFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent) {
    s_currentWorldFrame = this;
    m_camera = new CGCamera();
    if (m_top) m_top->m_keyboardFallback = this;
    // WorldFrame is a root-level frame in FrameXML.  The recovered constructor
    // anchors it to CSimpleFrame::m_top, not to its (normally null) XML parent.
    SetAllPoints(m_top);
    EnableEvent(SIMPLE_EVENT_KEYBOARD | SIMPLE_EVENT_MOUSE | SIMPLE_EVENT_UPDATE);
    m_strata = FRAME_STRATA_BACKGROUND;
    m_level = 0;

    NTempest::CRect rect{};
    const bool hasRect = GetRect(&rect);
    std::fprintf(stderr,
                 "WORLD_FRAME_CREATE parent=%p top=%p rect=%d (%.6f,%.6f)-(%.6f,%.6f)\n",
                 static_cast<void*>(parent), static_cast<void*>(m_top), hasRect ? 1 : 0,
                 rect.l, rect.t, rect.r, rect.b);
    std::fflush(stderr);
}

CGWorldFrame::~CGWorldFrame() {
    if (m_top && m_top->m_keyboardFallback == this)
        m_top->m_keyboardFallback = nullptr;
    delete m_camera;
    m_camera = nullptr;
    if (s_currentWorldFrame == this) s_currentWorldFrame = nullptr;
}

void CGWorldFrame::OnLayerUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    if (CGInputControl* input = CGInputControl::GetActive())
        input->OnUpdate(elapsedSec);
    m_elapsedSec = elapsedSec;
}

void CGWorldFrame::OnLayerTrackUpdate(float elapsedSec) {
    CSimpleFrame::OnLayerTrackUpdate(elapsedSec);
}

void CGWorldFrame::OnFrameRender(CRenderBatch* batch, unsigned layer) {
    CSimpleFrame::OnFrameRender(batch, layer);
    if (batch && layer == 0U)
        batch->QueueCallback([this] { RenderWorld(); });
}

void CGWorldFrame::DrawSkyBackground() {
    NTempest::CRect rect{};
    if (!GetRect(&rect) || rect.Width() <= 0.0F || rect.Height() <= 0.0F)
        return;
    const std::array<NTempest::C3Vector, 4> positions{
        NTempest::C3Vector(rect.l, rect.b, 0.0F),
        NTempest::C3Vector(rect.l, rect.t, 0.0F),
        NTempest::C3Vector(rect.r, rect.b, 0.0F),
        NTempest::C3Vector(rect.r, rect.t, 0.0F),
    };
    const std::array<NTempest::CImVector, 4> colors{
        NTempest::CImVector(0xff7ea3bdU),
        NTempest::CImVector(0xff172f55U),
        NTempest::CImVector(0xff7ea3bdU),
        NTempest::CImVector(0xff172f55U),
    };
    static constexpr std::uint16_t indices[] = {0, 1, 2, 3};
    float minX = 0.0F, maxX = 1.0F;
    float minY = 0.0F, maxY = 1.0F;
    float minZ = 0.0F, maxZ = 1.0F;
    NTempest::C44Matrix savedProjection;
    NTempest::C44Matrix savedView;
    GxXformViewport(&minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    GxXformProjection(&savedProjection);
    GxXformView(&savedView);
    const NTempest::C44Matrix identity;
    const float width = rect.r - rect.l;
    const float height = rect.t - rect.b;
    const NTempest::C44Matrix ortho(
        2.0F / width, 0.0F, 0.0F, 0.0F,
        0.0F, 2.0F / height, 0.0F, 0.0F,
        0.0F, 0.0F, 0.004F, 0.0F,
        -(rect.r + rect.l) / width,
        -(rect.t + rect.b) / height, -1.0F, 1.0F);
    GxXformSetViewport(0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F);
    GxXformSetView(identity);
    GxXformSetProjection(ortho);
    GxRsPush();
    GxVertexShaderSelect(GxVS_PassThru);
    GxRsSet(GxRs_Texture0, static_cast<CGxTex*>(nullptr));
    GxRsSet(GxRs_Blend, GxBlend_Opaque);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_DepthWrite, 0);
    GxRsSet(GxRs_Culling, 0);
    GxPrimLockVertexPtrs(4, positions.data(), sizeof(NTempest::C3Vector),
                         nullptr, 0, colors.data(), sizeof(NTempest::CImVector),
                         nullptr, 0, nullptr, 0, nullptr, 0);
    GxPrimDrawElements(GxPrim_TriangleStrip, 4, indices);
    GxPrimUnlockVertexPtrs();
    GxRsPop();
    GxXformSetProjection(savedProjection);
    GxXformSetView(savedView);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void CGWorldFrame::RenderWorld() {
    if (!CWorld::IsLoaded()) return;
    static bool loggedFirstRender = false;
    NTempest::C44Matrix projection;
    NTempest::C44Matrix view;
    GxXformProjection(&projection);
    GxXformView(&view);
    OnWorldUpdate();
    if (!loggedFirstRender) {
        NTempest::CRect rect{};
        const bool hasRect = GetRect(&rect);
        const NTempest::C3Vector& camera = m_camera->Position();
        std::fprintf(stderr,
                     "WORLD_FRAME_RENDER api=%u rect=%d (%.6f,%.6f)-(%.6f,%.6f) camera=(%.3f,%.3f,%.3f)\n",
                     static_cast<unsigned>(GxDevApi()), hasRect ? 1 : 0,
                     rect.l, rect.t, rect.r, rect.b, camera.x, camera.y, camera.z);
        std::fflush(stderr);
        loggedFirstRender = true;
    }
    OnWorldRender();
    GxXformSetProjection(projection);
    GxXformSetView(view);
}

void CGWorldFrame::OnWorldUpdate() {
    if (!m_camera) return;
    ClntObjMgrPollNameQueries();
    CWorld::UpdatePlayerMovement(m_elapsedSec);
    NTempest::C3Vector player = ClntObjMgrGetActivePlayer() != 0U
        ? ClntObjMgrGetActivePosition() : CWorld::PlayerPosition();
    m_camera->SetTarget(ClntObjMgrGetActivePlayer());
    m_camera->Update(player, ClntObjMgrGetActiveFacing(), OsGetAsyncTimeMs());
    NTempest::CRect projection{};
    if (GetRect(&projection)) m_camera->SetupWorldProjection(&projection);
    CWorld::SetCameraPosition(m_camera->Position());
    UpdateDayNightInfo();
    CWorld::Update(m_elapsedSec);
}

void CGWorldFrame::OnWorldRender() {
    DayNightRenderSky();
    CWorld::SetEnvironment();
    CWorld::Render();
    CWorld::RenderAlpha();
    static bool capturedWorldBuffer = false;
    if (!capturedWorldBuffer) {
        NTempest::CRect window{};
        GxCapsWindowSize(&window);
        const NTempest::CiRect pixelsRect(
            static_cast<int>(window.l), static_cast<int>(window.t),
            static_cast<int>(window.r), static_cast<int>(window.b));
        std::vector<NTempest::CImVector> pixels;
        std::vector<float> depths;
        GxDevReadPixels(pixelsRect, pixels);
        GxDevReadDepth(pixelsRect, depths);
        std::size_t colored = 0;
        std::size_t depthWritten = 0;
        std::uint8_t maxRed = 0, maxGreen = 0, maxBlue = 0;
        for (const auto& pixel : pixels) {
            colored += (pixel.r | pixel.g | pixel.b) != 0U;
            maxRed = (std::max)(maxRed, pixel.r);
            maxGreen = (std::max)(maxGreen, pixel.g);
            maxBlue = (std::max)(maxBlue, pixel.b);
        }
        for (float depth : depths)
            depthWritten += depth < 0.999999F;
        std::fprintf(stderr,
                     "WORLD_BUFFER size=%dx%d colored=%zu/%zu maxRGB=(%u,%u,%u) depthWritten=%zu/%zu\n",
                     pixelsRect.Width(), pixelsRect.Height(), colored, pixels.size(),
                     maxRed, maxGreen, maxBlue, depthWritten, depths.size());
        std::fflush(stderr);
        capturedWorldBuffer = true;
    }
}

CGCamera* CGWorldFrame::GetActiveCamera() { return m_camera; }
NTempest::C3Vector CGWorldFrame::GetCameraFacing() {
    NTempest::C3Vector result{};
    if (m_camera) m_camera->Forward(&result);
    return result;
}
NTempest::C3Vector CGWorldFrame::GetCameraPosition() {
    return m_camera ? m_camera->Position() : NTempest::C3Vector{};
}

int CGWorldFrame::OnLayerKeyDown(CKeyEvent* event) {
    if (CSimpleFrame::OnLayerKeyDown(event)) return 1;
    if (!event || event->key < 0 || event->key >= KEY_LAST) return 0;
    auto& keyName = m_lastKey[static_cast<unsigned>(event->key)];
    if (CGUIBindings::KeyEventToString(event, keyName.data(),
                                       static_cast<unsigned>(keyName.size())))
        return CGUIBindings::Get()->ExecKey(keyName.data(), event->time, 1);
    return 0;
}

int CGWorldFrame::OnLayerKeyUp(CKeyEvent* event) {
    if (CSimpleFrame::OnLayerKeyUp(event)) return 1;
    if (!event || event->key < 0 || event->key >= KEY_LAST) return 0;
    auto& keyName = m_lastKey[static_cast<unsigned>(event->key)];
    if (keyName[0] == 0)
        CGUIBindings::KeyEventToString(event, keyName.data(),
                                       static_cast<unsigned>(keyName.size()));
    if (keyName[0]) {
        const int handled = CGUIBindings::Get()->ExecKey(keyName.data(), event->time, 0);
        keyName[0] = 0;
        return handled;
    }
    return 0;
}

int CGWorldFrame::OnLayerMouseDown(CMouseEvent* event) {
    if (CSimpleFrame::OnLayerMouseDown(event)) return 1;
    if (CGGameUI::HandleMouseDown()) return 1;
    char keyName[32]{};
    if (event) {
        // Enabling free-look moves the physical cursor to the center of the
        // window. Keep the logical press location for the release hit test.
        m_actionCursor = NTempest::C2Vector(event->x, event->y);
        m_actionCursorValid = true;
    }
    if (event && CGUIBindings::MouseEventToString(event, keyName,
                                                  sizeof(keyName)))
        return CGUIBindings::Get()->ExecKey(keyName, event->time, 1);
    return 0;
}

int CGWorldFrame::OnLayerMouseUp(CMouseEvent* event) {
    if (CSimpleFrame::OnLayerMouseUp(event)) return 1;
    if (CGGameUI::HandleMouseUp()) return 1;
    char keyName[32]{};
    int handled = 0;
    if (event && CGUIBindings::MouseEventToString(event, keyName,
                                                  sizeof(keyName)))
        handled = CGUIBindings::Get()->ExecKey(keyName, event->time, 0);
    return handled;
}

int CGWorldFrame::OnLayerMouseMoveRelative(CMouseEvent* event) {
    if (CGInputControl* input = CGInputControl::GetActive())
        input->OnMouseMoveRel(event);
    return 1;
}

int CGWorldFrame::OnLayerMouseWheel(CMouseEvent* event) {
    if (CSimpleFrame::OnLayerMouseWheel(event)) return 1;
    char keyName[32]{};
    if (event && CGUIBindings::MouseEventToString(event, keyName,
                                                  sizeof(keyName)))
        return CGUIBindings::Get()->ExecKey(keyName, event->time, 1);
    return 0;
}

void CGWorldFrame::AddModelToScene() { }
void CGWorldFrame::CursorTrackObject() { }
void CGWorldFrame::CursorTrackUnit() { }
void CGWorldFrame::FindClosestModel() { }
void CGWorldFrame::GeometryTestModels() { }
void CGWorldFrame::GetHitTestFilterFlags() { }
void CGWorldFrame::GetLineSegment() { }
void CGWorldFrame::GetObjectUnderMouse() { }
void CGWorldFrame::GetScreenCoordinates() { }
void CGWorldFrame::GetSkyProgress() { }
void CGWorldFrame::HandleUnitFade() { }
void CGWorldFrame::HideObstructingModels() { }
void CGWorldFrame::HigherPriorityModel() { }
void CGWorldFrame::HitTest() { }
void CGWorldFrame::HitTestPoint() { }
void CGWorldFrame::IsLegalSelection() { }
void CGWorldFrame::IsUnitLegalSelection() { }
void CGWorldFrame::MoveToFreeList() { }
void CGWorldFrame::OnLayerTrackObject() { }
void CGWorldFrame::OnLayerTrackTerrain() { }
void CGWorldFrame::OnMouseModeNormal() { }
void CGWorldFrame::OnMouseModeRelative() { }
void CGWorldFrame::PerformDefaultAction(unsigned action, unsigned timestamp) {
    // The recovered input-control release path invokes this with button 1 for
    // CAMERAORSELECTORMOVE and button 4 for TURNORACTION. A dragged mouse is
    // filtered by CGInputControl::UnsetControlBit before this point.
    const bool hit = m_actionCursorValid ? PickUnitAt(m_actionCursor)
                                         : PickUnitAtCursor();
    m_actionCursorValid = false;
    if (hit && action == static_cast<unsigned>(MOUSE_BUTTON_RIGHT)) {
        const CLNT_VISIBLE_OBJECT* object =
            ClntObjMgrGetVisibleObject(m_lastPickedGuid);
        if (object && object->objectType == 7U &&
            object->ownerGuid == ClntObjMgrGetActivePlayer()) {
            CDataStore message;
            message.Put<std::uint32_t>(0x01C3U)
                ->Put<std::uint64_t>(object->guid);
            ClientServices_Send(&message);
            std::fprintf(stderr, "WORLD_CORPSE_RECLAIM guid=%llu\n",
                         static_cast<unsigned long long>(object->guid));
        }
    }
    if (!hit && action == static_cast<unsigned>(MOUSE_BUTTON_LEFT))
        CGGameUI::Target(0U);
    std::fprintf(stderr, "WORLD_DEFAULT_ACTION button=%u time=%u hit=%d\n",
                 action, timestamp, hit ? 1 : 0);
}
void CGWorldFrame::ReduceToClosestModel() { }
void CGWorldFrame::RefreshPlayerAlpha() { }
void CGWorldFrame::RegisterObjectFadeoutModel() { }
void CGWorldFrame::SendObjectTrackEvent() { }
void CGWorldFrame::SendUnitFadeEvent() { }
void CGWorldFrame::SetCameraTarget() { }
void CGWorldFrame::SetNamePlateUpdate() { }
void CGWorldFrame::SetPlayerFadeCameraValue() { }
void CGWorldFrame::SetPlayerRender() { }
void CGWorldFrame::SetSpriteClickButtons() { }
void CGWorldFrame::SetTerrainClickButtons() { }
void CGWorldFrame::SphereTestModels() { }
void CGWorldFrame::TogglePlayerRender() { }
void CGWorldFrame::UnitUpdate() { }
void CGWorldFrame::UpdateDayNightInfo() {
    if (!m_camera) return;

    // CGWorldFrame::UpdateDayNightInfo (0x4F2810) is the per-frame bridge
    // between the active camera/game clock and the preserved day/night system.
    DNInfo* info = DayNightGetInfo();
    info->farClip = m_camera->FarZ();
    info->elapsedSec = m_elapsedSec;
    info->nearClipScaled = m_camera->NearZ() * 3.0F;
    info->cameraPos = m_camera->Position();
    m_camera->Up(&info->cameraDir);
    if (info->cameraDir.SquaredMag() > 0.0F)
        info->cameraDir.Normalize();

    if (ClntObjMgrGetActivePlayer() != 0U)
        info->playerPos = ClntObjMgrGetActivePosition();

    info->time = static_cast<unsigned>(
        g_clientGameTime.WowTime.GetHourAndMinutes());
    info->dayProgression = static_cast<float>(
        g_clientGameTime.GameTimeGetDayProgression());
    info->day = static_cast<float>(
        g_clientGameTime.WowTime.GetDaysSinceEpoch());
}
void CGWorldFrame::UpdateObject() { }
void CGWorldFrame::UpdatePlayerAlpha() { }
void CGWorldFrame::VolumeTestModels() { }

void CGWorldFrame::UpdatePickTargets() {
    m_pickTargets.clear();
    if (!m_camera) return;

    NTempest::C44Matrix view;
    NTempest::C44Matrix projection;
    GxXformView(&view);
    GxXformProjection(&projection);
    projection.c0 = -projection.c0;
    projection.c1 = -projection.c1;
    projection.c2 = -projection.c2;
    projection.c3 = -projection.c3;
    if (!GxMasterEnable(GxMasterEnable_NormalProjection) &&
        projection.d3 != 1.0F) {
        projection.a0 *= 0.2F; projection.a1 *= 0.2F; projection.a2 *= 0.2F;
        projection.b0 *= 0.2F; projection.b1 *= 0.2F; projection.b2 *= 0.2F;
        projection.c0 *= 0.2F; projection.c1 *= 0.2F; projection.c2 *= 0.2F;
        projection.d0 *= 0.2F; projection.d1 *= 0.2F; projection.d2 *= 0.2F;
    }

    const NTempest::C3Vector camera = m_camera->Position();
    const std::uint64_t player = ClntObjMgrGetActivePlayer();
    for (const CLNT_VISIBLE_OBJECT& object : ClntObjMgrGetVisibleObjects()) {
        if ((object.objectType != 3U && object.objectType != 4U &&
             object.objectType != 7U) ||
            object.guid == player) {
            continue;
        }
        const NTempest::C3Vector relative = object.position - camera;
        const float distanceSq = relative.SquaredMag();
        if (distanceSq > 10000.0F) continue;

        NTempest::C44Matrix world;
        world.Translate(relative);
        NTempest::C44Matrix modelView = world * view;
        modelView.a2 = -modelView.a2;
        modelView.b2 = -modelView.b2;
        modelView.c2 = -modelView.c2;
        modelView.d2 = -modelView.d2;

        const auto project = [&modelView, &projection](float z, float& x, float& y) {
            const NTempest::C4Vector eye = modelView *
                NTempest::C4Vector(0.0F, 0.0F, z, 1.0F);
            const NTempest::C4Vector clip = projection * eye;
            if (clip.w <= 0.00001F) return false;
            const float ndcX = (clip.x / clip.w + 1.0F) * 0.5F;
            // D3D clip space is top-positive here while mouse DDC is
            // bottom-positive. The recovered HitTestPoint unprojection makes
            // this inversion implicitly when it constructs the world ray.
            const float ndcY = (1.0F - clip.y / clip.w) * 0.5F;
            if (ndcX < 0.0F || ndcX > 1.0F ||
                ndcY < 0.0F || ndcY > 1.0F) return false;
            NDCToDDC(&x, &y, ndcX, ndcY);
            return true;
        };

        float bottomX = 0.0F, bottomY = 0.0F;
        float topX = 0.0F, topY = 0.0F;
        const float height = (object.objectType == 3U ? 2.5F : 2.25F) *
            std::max(object.scale, 0.25F);
        if (!project(0.15F, bottomX, bottomY) ||
            !project(height, topX, topY)) continue;
        const float screenHeight = std::abs(topY - bottomY);
        PickTarget target;
        target.guid = object.guid;
        target.x = (bottomX + topX) * 0.5F;
        target.y = (bottomY + topY) * 0.5F;
        // The original selects against model hit-test volumes, which extend
        // beyond a centerline projection. Approximate that body volume from
        // feet to head while object-side hit volumes are reconstructed.
        target.halfHeight = std::max(screenHeight, 0.024F);
        target.halfWidth = std::max(screenHeight * 0.55F, 0.020F);
        target.distanceSq = distanceSq;
        m_pickTargets.push_back(target);
    }
}

bool CGWorldFrame::PickUnitAtCursor() {
    if (!m_top) return false;
    return PickUnitAt(m_top->m_cursor);
}

bool CGWorldFrame::PickUnitAt(const NTempest::C2Vector& cursor) {
    m_lastPickedGuid = 0U;
    if (!m_camera) return false;

    float ndcX = 0.0F;
    float ndcY = 0.0F;
    DDCToNDC(&ndcX, &ndcY, cursor.x, cursor.y);
    const float screenX = ndcX * 2.0F - 1.0F;
    const float screenY = ndcY * 2.0F - 1.0F;
    const float aspect = std::max(std::abs(m_camera->Aspect()), 0.001F);
    const float halfHeight = std::tan(
        (m_camera->Fov() / std::sqrt(aspect * aspect + 1.0F)) * 0.5F);

    NTempest::C3Vector forward;
    NTempest::C3Vector right;
    NTempest::C3Vector up;
    m_camera->Forward(&forward);
    m_camera->Right(&right);
    m_camera->Up(&up);
    NTempest::C3Vector ray = forward
        + (screenX * aspect * halfHeight) * right
        + (screenY * halfHeight) * up;
    if (ray.SquaredMag() <= 0.000001F) return false;
    ray.Normalize();

    const NTempest::C3Vector origin = m_camera->Position();
    const std::uint64_t player = ClntObjMgrGetActivePlayer();
    const CLNT_VISIBLE_OBJECT* best = nullptr;
    const CLNT_VISIBLE_OBJECT* nearest = nullptr;
    float bestDepth = 1000000.0F;
    float nearestScore = 1000000.0F;
    for (const CLNT_VISIBLE_OBJECT& object : ClntObjMgrGetVisibleObjects()) {
        if ((object.objectType != 3U && object.objectType != 4U &&
             object.objectType != 7U) ||
            object.guid == player) {
            continue;
        }

        const float scale = std::clamp(object.scale, 0.5F, 3.0F);
        const float height = (object.objectType == 3U ? 2.5F : 2.25F) * scale;
        const float radius = std::max(0.7F * scale, 0.5F);
        float objectScore = 1000000.0F;
        float objectDepth = 1000000.0F;
        // ModelHitTestVolumes in the recovered client tests all collision
        // volumes. Sample a vertical capsule until those model volumes are
        // wired back into the object instances.
        for (unsigned sample = 0; sample < 9U; ++sample) {
            NTempest::C3Vector point = object.position;
            point.z += height * (static_cast<float>(sample) + 0.5F) / 9.0F;
            const NTempest::C3Vector offset = point - origin;
            const float depth = NTempest::C3Vector::Dot(offset, ray);
            if (depth <= m_camera->NearZ() || depth > 100.0F) continue;
            const float perpendicularSq = std::max(
                offset.SquaredMag() - depth * depth, 0.0F);
            const float score = perpendicularSq / (radius * radius);
            if (score < objectScore) {
                objectScore = score;
                objectDepth = depth;
            }
        }
        if (objectScore < nearestScore) {
            nearest = &object;
            nearestScore = objectScore;
        }
        if (objectScore <= 1.0F && objectDepth < bestDepth) {
            best = &object;
            bestDepth = objectDepth;
        }
    }

    static unsigned reports = 0U;
    if (!best) {
        if (reports < 12U) {
            if (nearest) {
                std::fprintf(stderr,
                             "WORLD_PICK miss cursor=(%.6f,%.6f) nearest=%llu "
                             "rayScore=%.4f ray=(%.4f,%.4f,%.4f)\n",
                             cursor.x, cursor.y,
                             static_cast<unsigned long long>(nearest->guid),
                             nearestScore, ray.x, ray.y, ray.z);
            } else {
                std::fprintf(stderr,
                             "WORLD_PICK miss cursor=(%.6f,%.6f) units=0\n",
                             cursor.x, cursor.y);
            }
            ++reports;
        }
        return false;
    }
    std::fprintf(stderr,
                 "WORLD_PICK hit guid=%llu cursor=(%.6f,%.6f) depth=%.3f\n",
                 static_cast<unsigned long long>(best->guid), cursor.x, cursor.y,
                 bestDepth);
    m_lastPickedGuid = best->guid;
    if (best->objectType != 7U)
        CGGameUI::Target(best->guid);
    return true;
}
