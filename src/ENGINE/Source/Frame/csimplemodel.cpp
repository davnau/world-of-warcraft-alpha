#include "ENGINE/Source/Frame/csimplemodel.h"
#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Model/mdlcamera.h"
#include "ENGINE/Source/Model/modelanim.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/modelmisc.h"
#include "ENGINE/Source/Model/modelrender.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>

namespace {
CModel* ModelFromFrame(const CSimpleModel& frame)
{
    return frame.m_modelOwner ? frame.m_modelOwner.get() : reinterpret_cast<CModel*>(frame.m_model);
}

std::uint8_t XmlColorChannel(const char* value, std::uint8_t fallback)
{
    if (!value || !*value) return fallback;
    return static_cast<std::uint8_t>(std::clamp(std::strtof(value, nullptr), 0.0F, 1.0F) * 255.0F + 0.5F);
}

NTempest::C44Matrix CameraProjection(const HCAMERA__& camera, float width, float height)
{
    // CCamera::SetupWorldProjection passes (bottom - top) / (right - left)
    // to GxuXformCreateProjection.
    const float aspect = width > 0.0F && height > 0.0F ? height / width : 1.0F;
    const float fov = std::clamp(camera.fieldOfView, 0.01F, 3.13159265F);
    const float nearClip = std::max(0.001F, camera.nearClip);
    const float farClip = std::max(nearClip + 0.001F, camera.farClip);
    const float tanHalfFov = std::tan((fov / std::sqrt(aspect * aspect + 1.0F)) * 0.5F);
    const float xScale = 1.0F / (aspect * tanHalfFov);
    const float yScale = 1.0F / tanHalfFov;
    const float range = farClip - nearClip;
    return {xScale,0,0,0, 0,yScale,0,0,
            0,0,(nearClip + farClip) / range,1,
            0,0,-2.0F * nearClip * farClip / range,0};
}

NTempest::C44Matrix CameraView(const HCAMERA__& camera)
{
    auto forward = camera.target - camera.position;
    if (forward.SquaredMag() <= 1.0e-12F) forward = {1.0F, 0.0F, 0.0F};
    else forward.Normalize();
    const float horizontal = std::sqrt(forward.x * forward.x + forward.y * forward.y);
    const float sinRoll = std::sin(camera.roll);
    NTempest::C3Vector up = horizontal > 1.0e-6F
        ? NTempest::C3Vector{-forward.y / horizontal * sinRoll,
                             forward.x / horizontal * sinRoll,
                             std::cos(camera.roll)}
        : NTempest::C3Vector{0.0F, sinRoll, std::cos(camera.roll)};

    auto right = NTempest::C3Vector::Cross(forward, up);
    if (right.SquaredMag() <= 1.0e-12F) right = {0.0F, 1.0F, 0.0F};
    else right.Normalize();
    up = NTempest::C3Vector::Cross(right, forward);
    up.Normalize();
    const float tx = -NTempest::C3Vector::Dot(right, camera.position);
    const float ty = -NTempest::C3Vector::Dot(up, camera.position);
    const float tz = -NTempest::C3Vector::Dot(forward, camera.position);
    return {right.x,up.x,forward.x,0, right.y,up.y,forward.y,0,
            right.z,up.z,forward.z,0, tx,ty,tz,1};
}
}

void AnimFinishedCallback(CSimpleModel* model)
{
    if (model) model->m_onAnimFinished = 1;
}

CSimpleModel::CSimpleModel(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
    m_light = {};
}

CSimpleModel::~CSimpleModel() = default;

void CSimpleModel::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        if (const auto* value = node->GetAttributeByName("file"); value && *value) SetModel(value);
        if (const auto* value = node->GetAttributeByName("facing")) SetFacing(std::strtof(value, nullptr));
        if (const auto* value = node->GetAttributeByName("scale")) SetScale(std::strtof(value, nullptr));
        if (const auto* value = node->GetAttributeByName("positionX")) m_position.x = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("positionY")) m_position.y = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("positionZ")) m_position.z = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("fogNear")) m_fogNear = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("fogFar")) m_fogFar = std::strtof(value, nullptr);
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "FogColor") != 0) continue;
            m_fogColor.r = XmlColorChannel(child->GetAttributeByName("r"), m_fogColor.r);
            m_fogColor.g = XmlColorChannel(child->GetAttributeByName("g"), m_fogColor.g);
            m_fogColor.b = XmlColorChannel(child->GetAttributeByName("b"), m_fogColor.b);
            m_fogColor.a = XmlColorChannel(child->GetAttributeByName("a"), 255);
            m_fogEnabled = true;
        }
    }
    CalculateBounds();
}

void CSimpleModel::LoadXML_Scripts(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML_Scripts(node, status);
}

void CSimpleModel::SetModel(HMODEL model)
{
    if (reinterpret_cast<CModel*>(model) != m_modelOwner.get()) m_modelOwner.reset();
    m_model = model;
    if (model) m_flags |= 1u;
    else m_flags &= ~1u;
    CalculateBounds();
}

void CSimpleModel::SetModel(const char* sourcefile)
{
    SetModel(sourcefile, nullptr, nullptr);
}

void CSimpleModel::SetModel(const char* sourcefile, const CModelCreate* createData,
                            CStatus* status)
{
    (void)status;
    m_flags |= 2U;
    m_onAnimFinished = 0;
    auto model = sourcefile && *sourcefile
        ? ModelCreate(sourcefile, createData) : std::shared_ptr<CModel>{};
    m_modelOwner = std::move(model);
    m_model = reinterpret_cast<HMODEL>(m_modelOwner.get());
    if (m_modelOwner && ModelIsLoaded(*m_modelOwner)) {
        m_flags = (m_flags | 1U) & ~2U;
        SetCameraByIndex(m_cameraIndex);
    } else {
        m_flags &= ~(1U | 2U);
        m_camera = nullptr;
    }
    CalculateBounds();
}

void CSimpleModel::SetModel(HOBJECT__* object, HMODEL model)
{
    (void)object;
    SetModel(model);
}

void CSimpleModel::SetCameraInternal(HOBJECT__* object, HCAMERA__* camera)
{
    (void)object;
    SetCamera(camera);
}

void CSimpleModel::FinishLoadingModel(HMODEL model)
{
    SetModel(model);
    m_flags &= ~2u;
}

void CSimpleModel::SetCamera(HCAMERA__* camera)
{
    m_camera = camera;
    m_flags &= ~4u;
}

void CSimpleModel::SetCameraByIndex(unsigned index)
{
    m_cameraIndex = index;
    if (auto* model = ModelFromFrame(*this)) {
        m_camera = ModelGetCamera(*model, index);
        m_flags &= ~4U;
    } else {
        m_camera = nullptr;
        m_flags |= 4U;
    }
}

void CSimpleModel::SetLight(const CGxLight* light)
{
    if (light) m_light = *light;
}

void CSimpleModel::SetSequence(unsigned index)
{
    m_sequenceIndex = index;
    m_sequenceTime = 0.0f;
    m_flags |= 8u;
    if (auto* model = ModelFromFrame(*this)) ModelSetSequence(*model, index);
}

int CSimpleModel::SetSequenceTime(unsigned index, float timeOffset)
{
    m_sequenceIndex = index;
    m_sequenceTime = std::max(0.0f, timeOffset);
    if (auto* model = ModelFromFrame(*this))
        return ModelForceSequenceTime(*model, index, static_cast<std::uint32_t>(m_sequenceTime * 1000.0F));
    return 0;
}

int CSimpleModel::AdvanceTime(float elapsedSec)
{
    if (!m_model) return 0;
    auto* model = ModelFromFrame(*this);
    if (!model || !ModelIsLoaded(*model)) return 0;
    const auto elapsedMs = static_cast<std::uint32_t>(
        std::lround(std::max(0.0F, elapsedSec) * 1000.0F));
    ModelAnimateAttached(*model, elapsedMs);
    m_sequenceIndex = ModelGetPrimarySequence(*model);
    m_sequenceTime = static_cast<float>(ModelGetSequenceTime(*model)) * 0.001F;
    return 1;
}

void CSimpleModel::ReplaceTexture(unsigned materialID, CStatus* textureName)
{
    (void)materialID;
    (void)textureName;
}

bool CSimpleModel::ModelJustLoaded() const
{
    return (m_flags & 2u) != 0;
}

void CSimpleModel::ClearModel()
{
    m_model = nullptr;
    m_modelOwner.reset();
    m_camera = nullptr;
    m_flags &= ~1u;
    m_cachedExtents = {};
}

void CSimpleModel::SetPosition(const NTempest::C3Vector& position) { m_position = position; }
void CSimpleModel::SetFacing(float facing) { m_facing = facing; }
void CSimpleModel::SetScale(float scale) { m_scale = std::max(0.0f, scale); }
float CSimpleModel::GetWidth() const { return CLayoutFrame::GetWidth(); }
float CSimpleModel::GetHeight() const { return CLayoutFrame::GetHeight(); }
void CSimpleModel::SetAlpha(float alpha) { CSimpleFrame::SetAlpha(alpha); }
void CSimpleModel::SetFogColor(const NTempest::CImVector& color) { m_fogColor = color; }
void CSimpleModel::SetFogRange(float nearPlane, float farPlane)
{
    m_fogNear = std::max(0.0f, nearPlane);
    m_fogFar = std::max(m_fogNear, farPlane);
    m_fogEnabled = m_fogFar > m_fogNear;
}

void CSimpleModel::OnLayerUpdate(float elapsedSec)
{
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    AdvanceTime(elapsedSec);
}

void CSimpleModel::OnFrameRender(CRenderBatch* batch, unsigned layer)
{
    CSimpleFrame::OnFrameRender(batch, layer);
    if (m_model && batch && layer == 2)
        batch->QueueCallback([this] { RenderModel(); });
}

void CSimpleModel::UpdateModel(HMODEL model)
{
    if (model) SetModel(model);
}

void CSimpleModel::RenderModel()
{
    auto* model = ModelFromFrame(*this);
    if (!model || !ModelIsLoaded(*model) || !m_camera) return;

    NTempest::CRect rect{};
    if (!GetRect(&rect) || rect.Width() <= 0.0F || rect.Height() <= 0.0F) return;

    NTempest::C44Matrix savedProjection, savedView;
    float savedMinX = 0.0F, savedMaxX = 1.0F, savedMinY = 0.0F, savedMaxY = 1.0F;
    float savedMinZ = 0.0F, savedMaxZ = 1.0F;
    GxXformProjection(&savedProjection);
    GxXformView(&savedView);
    GxXformViewport(&savedMinX, &savedMaxX, &savedMinY, &savedMaxY, &savedMinZ, &savedMaxZ);

    float viewportLeft = 0.0F, viewportBottom = 0.0F, viewportRight = 0.0F, viewportTop = 0.0F;
    DDCToNDC(&viewportLeft, &viewportBottom, rect.l, rect.t);
    DDCToNDC(&viewportRight, &viewportTop, rect.r, rect.b);
    viewportLeft = std::clamp(viewportLeft, 0.0F, 1.0F);
    viewportRight = std::clamp(viewportRight, 0.0F, 1.0F);
    viewportBottom = std::clamp(viewportBottom, 0.0F, 1.0F);
    viewportTop = std::clamp(viewportTop, 0.0F, 1.0F);
    if (viewportRight <= viewportLeft || viewportTop <= viewportBottom) return;

    const auto projection = CameraProjection(*m_camera, rect.Width(), rect.Height());
    const auto view = CameraView(*m_camera);
    NTempest::C44Matrix world = NTempest::C44Matrix::Rotation({0.0F, 0.0F, 1.0F}, m_facing);
    world.Scale(std::max(0.0001F, m_scale * m_layoutScale));
    world.d0 = m_position.x; world.d1 = m_position.y; world.d2 = m_position.z;

    GxXformSetViewport(viewportLeft, viewportRight, viewportBottom, viewportTop, 0.0F, 1.0F);
    GxSceneClear(2U);
    GxXformSetProjection(projection);
    GxXformSetView(view);
    GxXformPush(GxXform_World, world);
    const bool savedMasterFog = GxMasterEnable(GxMasterEnable_Fog);
    GxRsPush();
    // Temporary renderer isolation requested while the material path is
    // compared with the recovered client.  Disable both the master switch and
    // per-draw fog state so no geoset can re-enable it during this pass.
    GxMasterEnableSet(GxMasterEnable_Fog, false);
    GxRsSet(GxRs_Fog, 0);
    ModelSetVertexAlpha(*model, static_cast<float>(m_alpha) / 255.0F);
    auto cameraDirection = m_camera->target - m_camera->position;
    if (cameraDirection.SquaredMag() > 1.0e-12F) cameraDirection.Normalize();
    ModelScenePlaceCamera(m_camera->position, cameraDirection);
    ModelAddToScene(*model, 0U);
    ModelRenderScene();
    GxRsPop();
    GxMasterEnableSet(GxMasterEnable_Fog, savedMasterFog);
    GxXformPop(GxXform_World);
    GxXformSetProjection(savedProjection);
    GxXformSetView(savedView);
    GxXformSetViewport(savedMinX, savedMaxX, savedMinY, savedMaxY, savedMinZ, savedMaxZ);
}

const NTempest::CImVector* CSimpleModel::GetFogColor() const
{
    return &m_fogColor;
}

void CSimpleModel::CalculateBounds()
{
    if (!m_model) {
        m_cachedExtents = {};
        return;
    }
    if (auto* model = ModelFromFrame(*this); model && ModelGetExtents(*model, m_cachedExtents)) return;
    const float halfWidth = std::max(0.5f, GetWidth() * 0.5f) * m_scale;
    const float halfHeight = std::max(0.5f, GetHeight() * 0.5f) * m_scale;
    m_cachedExtents = {m_position - NTempest::C3Vector(halfWidth, halfWidth, halfHeight),
                       m_position + NTempest::C3Vector(halfWidth, halfWidth, halfHeight)};
}
