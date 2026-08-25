#include "WoW/Source/UIUtil/camera.h"

#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/Console/consolevar.h"
#include "WoW/Source/UI/worldframe.h"
#include "WoW/Source/UIUtil/inputcontrol.h"
#include "WoW/Source/WorldClient/World.h"
#include "WoW/Source/WowServices/misc.h"

extern "C" {
#include "lauxlib.h"
}

#include <algorithm>
#include <array>
#include <cmath>
#include <utility>

namespace {
constexpr float kPi = 3.1415927F;
constexpr float kTwoPi = kPi * 2.0F;
constexpr float kDegToRad = kPi / 180.0F;
constexpr float kEpsilon = 0.00000023841858F;
constexpr float kCameraSmoothingTime = 0.5F;
constexpr float kCameraLinearSpeed = 8.33F;
constexpr float kCameraAngularSpeed = 180.0F * kDegToRad;
constexpr float kMinPitch = -1.5533431F;
constexpr float kMaxPitch = 1.5533431F;
constexpr float kMaxDistance = 15.0F;

CVar* s_mouseInvertYaw = nullptr;
CVar* s_mouseInvertPitch = nullptr;
CVar* s_cameraSmooth = nullptr;
CVar* s_cameraSmoothingTime = nullptr;
CVar* s_cameraLinearSpeed = nullptr;
CVar* s_cameraAngularSpeed = nullptr;
std::array<CVar*, 5> s_cameraAngles{};
std::array<CVar*, 5> s_cameraDistances{};

float CVarFloat(CVar* variable, float fallback) {
    return variable ? variable->FloatValue() : fallback;
}

int CVarInt(CVar* variable, int fallback) {
    return variable ? variable->IntValue() : fallback;
}

CGCamera* ActiveCamera() {
    return CGWorldFrame::s_currentWorldFrame
        ? CGWorldFrame::s_currentWorldFrame->GetActiveCamera() : nullptr;
}

unsigned ScriptTimestamp(lua_State* state) {
    return lua_isnumber(state, 1)
        ? static_cast<unsigned>(lua_tonumber(state, 1)) : OsGetAsyncTimeMs();
}

int StartScriptMotion(lua_State* state, CGCameraMotion motion) {
    if (CGCamera* camera = ActiveCamera())
        camera->StartMotion(motion, ScriptTimestamp(state), 0);
    return 0;
}

int StopScriptMotion(lua_State* state, CGCameraMotion motion) {
    if (CGCamera* camera = ActiveCamera())
        camera->StopMotion(motion, ScriptTimestamp(state));
    return 0;
}

float Smooth(float from, float to, unsigned timestamp, unsigned start,
             float seconds) {
    if (seconds <= kEpsilon) return to;
    if (timestamp <= start) return from;
    const float progress = (std::min)(1.0F,
        static_cast<float>(timestamp - start) * 0.001F / seconds);
    return static_cast<float>(OrganicSmooth(from, to, progress));
}
}

CGCamera::CGCamera()
    : CSimpleCamera(0.11111111F, 277.77777F, 70.0F * kDegToRad) {
    m_modelMatrix = NTempest::C34Matrix{};
    m_flags = 17;
    m_distance = 5.55F;
    m_desiredDistance = m_distance;
    m_previousDistance = m_distance;
    ResetView(-1);
    SetTarget(0);
}

CGCamera::~CGCamera() {
    SetTarget(0);
    ClearModelCamera();
}

void CGCamera::AddShake() {}
void CGCamera::CCommand_CameraClip() {}
void CGCamera::CalcModelCamera() {}
void CGCamera::CheckUnderwater() {}
void CGCamera::ClearModelCamera() { m_model = nullptr; m_modelCamera = nullptr; }
void CGCamera::FinishLoadingModel() {}
void CGCamera::FinishLoadingTarget() {}
void CGCamera::MakeRelativeTo() {}
void CGCamera::ParentToWorld() { m_relativeTo = 0; }
void CGCamera::PerformTerrainTilt() {}
void CGCamera::ResetModelCamera() { ClearModelCamera(); }
void CGCamera::RunShakes() {}
void CGCamera::SetModelCamera() {}

void CGCamera::ClampAngles() {
    m_pitch = (std::clamp)(m_pitch, kMinPitch, kMaxPitch);
    while (m_yaw < 0.0F) m_yaw += kTwoPi;
    while (m_yaw > kTwoPi) m_yaw -= kTwoPi;
}

float CGCamera::CollideCameraWithWorld(
    const NTempest::C3Vector& targetPosition) {
    // CGCamera::CollideCameraWithWorld (0x53D130) traces from the target to
    // the requested third-person position with CWorld flags 0x151, shortens
    // to the nearest hit, then backs the camera away by its near clip plane.
    const float initialDistance = (std::max)(m_distance, m_desiredDistance);
    float cameraDistance = initialDistance;
    if (initialDistance - m_nearZ > 0.00000095367432F) {
        const float horizontal = std::cos(m_pitch) * initialDistance;
        const NTempest::C3Vector desiredPosition = {
            targetPosition.x - std::cos(m_yaw) * horizontal,
            targetPosition.y - std::sin(m_yaw) * horizontal,
            targetPosition.z + std::sin(m_pitch) * initialDistance,
        };
        float hitFraction = 1.0F;
        if (CWorld::Intersect(targetPosition, desiredPosition, nullptr,
                              nullptr, &hitFraction, true, true)) {
            cameraDistance = hitFraction * initialDistance;
        }
        if (std::fabs(initialDistance - cameraDistance) >= kEpsilon)
            cameraDistance = (std::max)(0.0F, cameraDistance - m_nearZ);
    }
    if (cameraDistance > m_distance)
        return m_distance;
    return cameraDistance;
}

void CGCamera::CalcThirdPerson(const NTempest::C3Vector& inputTarget,
                               float targetFacing, unsigned timestamp) {
    NTempest::C3Vector targetPosition = inputTarget;
    targetPosition.z += m_targetOffsetZ;
    const bool targetMoved = targetPosition.x != m_lastTarget.x ||
        targetPosition.y != m_lastTarget.y || targetPosition.z != m_lastTarget.z;
    if (targetMoved) m_lastTarget = targetPosition;

    if (m_motionMask) UpdateMotion(timestamp);
    if (std::fabs(m_desiredDistance - m_distance) < kEpsilon) {
        m_distance = m_desiredDistance;
        m_zoomSmoothingTimestamp = 0;
    } else {
        m_distance = Smooth(m_previousDistance, m_desiredDistance, timestamp,
                            m_zoomSmoothingTimestamp, m_zoomTime);
        if (std::fabs(m_desiredDistance - m_distance) < kEpsilon)
            m_zoomSmoothingTimestamp = 0;
    }
    if ((m_flags & 8) == 0) {
        if (std::fabs(m_desiredPitch - m_pitch) < kEpsilon) {
            m_pitch = m_desiredPitch;
            m_pitchSmoothingTimestamp = 0;
            m_flags |= 0x10;
        } else if (timestamp >= m_pitchSmoothingTimestamp) {
            m_pitch = Smooth(m_previousPitch, m_desiredPitch, timestamp,
                             m_pitchSmoothingTimestamp, m_pitchTime);
        }
        if (std::fabs(m_desiredYaw - m_yawOffset) < kEpsilon) {
            m_yawOffset = m_desiredYaw;
            m_yawSmoothingTimestamp = 0;
        } else if (timestamp >= m_yawSmoothingTimestamp) {
            m_yawOffset = Smooth(m_previousYaw, m_desiredYaw, timestamp,
                                 m_yawSmoothingTimestamp, m_yawTime);
        }
        m_yaw = GetSmoothedYawAngle(targetFacing, targetMoved) + m_yawOffset;
        ClampAngles();
    }

    const float cameraDistance = CollideCameraWithWorld(targetPosition);
    const float horizontal = std::cos(m_pitch) * cameraDistance;
    m_position = {
        targetPosition.x - std::cos(m_yaw) * horizontal,
        targetPosition.y - std::sin(m_yaw) * horizontal,
        targetPosition.z + std::sin(m_pitch) * cameraDistance,
    };
    NTempest::C3Vector forward = targetPosition - m_position;
    if (forward.SquaredMag() > kEpsilon) forward.Normalize();
    else forward = {std::cos(m_yaw), std::sin(m_yaw), 0.0F};
    CSimpleCamera::SetFacing(forward);
    m_targetFadeValue = cameraDistance - m_nearZ < 0.0027777778F ? 0 : 255;
}

void CGCamera::CalcFirstPerson(const NTempest::C3Vector& inputTarget,
                               float targetFacing, unsigned) {
    m_position = inputTarget;
    m_position.z += m_targetOffsetZ;
    if ((m_flags & 8) == 0) m_yaw = targetFacing;
    ClampAngles();
    const NTempest::C3Vector forward{
        std::cos(m_pitch) * std::cos(m_yaw),
        std::cos(m_pitch) * std::sin(m_yaw),
        -std::sin(m_pitch)};
    CSimpleCamera::SetFacing(forward);
    m_zoomSmoothingTimestamp = 0;
    m_pitchSmoothingTimestamp = 0;
    m_yawSmoothingTimestamp = 0;
}

void CGCamera::Update(const NTempest::C3Vector& targetPosition,
                      float targetFacing, unsigned timestamp) {
    if ((m_flags & 7) != 0 || m_distance > 0.027777778F)
        CalcThirdPerson(targetPosition, targetFacing, timestamp);
    else
        CalcFirstPerson(targetPosition, targetFacing, timestamp);
    RunShakes();
    CheckUnderwater();
}

void CGCamera::SetPositionAndFacing(const NTempest::C3Vector& position,
                                    const NTempest::C3Vector& facing) {
    m_position = position;
    CSimpleCamera::SetFacing(facing);
}

void CGCamera::SetPositionAndTarget(const NTempest::C3Vector& position,
                                    const NTempest::C3Vector& target) {
    m_position = position;
    NTempest::C3Vector forward = target - position;
    if (forward.SquaredMag() > kEpsilon) forward.Normalize();
    CSimpleCamera::SetFacing(forward);
}

void CGCamera::SetPositionAndTargetWithRoll(const NTempest::C3Vector& position,
                                            const NTempest::C3Vector& target,
                                            float roll) {
    m_position = position;
    NTempest::C3Vector forward = target - position;
    if (forward.SquaredMag() > kEpsilon) forward.Normalize();
    const NTempest::C3Vector up{0.0F, std::sin(roll), std::cos(roll)};
    CSimpleCamera::SetFacing(forward, up);
}

void CGCamera::CreateViewFromParams(int view, float dist, float pitch,
                                    float yaw) {
    if (view <= 0 || view >= static_cast<int>(m_views.size())) return;
    m_views[view] = {dist, pitch, yaw};
}

void CGCamera::CreateViewFromCamera(int view) {
    CreateViewFromParams(view, m_desiredDistance,
                         m_desiredPitch - m_smoothingAngle, m_desiredYaw);
}

void CGCamera::CycleView() {
    int view = (m_flags & 7) + m_cycleDirection;
    if (view >= 5) { m_cycleDirection = -m_cycleDirection; view = 3; }
    else if (view < 0) { m_cycleDirection = -m_cycleDirection; view = 1; }
    SetView(view);
}

void CGCamera::NextView() {
    if ((m_flags & 7) + 1 < 5) SetView((m_flags & 7) + 1);
}

void CGCamera::PreviousView() {
    if ((m_flags & 7) != 0) SetView((m_flags & 7) - 1);
}

void CGCamera::ResetView(int view) {
    if (view < 0) {
        for (int i = 0; i < 5; ++i) ResetView(i);
        return;
    }
    if (view >= 5) return;
    static constexpr CGCameraView defaults[5] = {
        {0.0F, 0.0F, 0.0F},
        {5.55F, 0.0F, 0.0F},
        {5.55F, 20.0F * kDegToRad, 0.0F},
        {13.88F, 30.0F * kDegToRad, 0.0F},
        {13.88F, 0.0F, 0.0F},
    };
    m_views[view] = defaults[view];
    if (view > 0) {
        m_views[view].dist = CVarFloat(s_cameraDistances[view], defaults[view].dist);
        m_views[view].pitch = CVarFloat(s_cameraAngles[view],
                                        defaults[view].pitch / kDegToRad) * kDegToRad;
    }
    if (view == (m_flags & 7)) SetView(view);
}

void CGCamera::SetView(int newView) {
    if (newView < 0 || newView >= 5) return;
    if ((m_flags & 7) == newView) {
        m_zoomSmoothingTimestamp = 0;
        if ((m_flags & 0x10) == 0) m_pitchSmoothingTimestamp = 0;
        m_yawSmoothingTimestamp = 0;
        m_distance = m_desiredDistance = m_views[newView].dist;
        m_pitch = m_desiredPitch = m_views[newView].pitch;
        m_yawOffset = m_desiredYaw = m_views[newView].yaw;
        return;
    }
    const unsigned now = OsGetAsyncTimeMs();
    m_flags = newView | (m_flags & ~7);
    SetDesiredDistance(m_views[newView].dist, now);
    SetDesiredPitchAngle(m_views[newView].pitch, 0.0F, now);
    SetDesiredYawAngle(m_views[newView].yaw, 0.0F, now);
}

void CGCamera::ZoomIn(float distance, unsigned timestamp) {
    const float linearSpeed = CVarFloat(s_cameraLinearSpeed, kCameraLinearSpeed);
    const unsigned timeout = static_cast<unsigned>(
        (std::max)(0.0F, distance) / (std::max)(linearSpeed, kEpsilon) * 1000.0F);
    if (m_motionMask & (1U << (2 * CAMERA_MOVE_OUT)))
        StopMotion(CAMERA_MOVE_OUT, timestamp);
    if (m_motionTimeout[CAMERA_MOVE_IN] &&
        (m_motionMask & (1U << (2 * CAMERA_MOVE_IN))))
        m_motionTimeout[CAMERA_MOVE_IN] += timeout;
    else StartMotion(CAMERA_MOVE_IN, timestamp, timeout);
}

void CGCamera::ZoomOut(float distance, unsigned timestamp) {
    const float linearSpeed = CVarFloat(s_cameraLinearSpeed, kCameraLinearSpeed);
    const unsigned timeout = static_cast<unsigned>(
        (std::max)(0.0F, distance) / (std::max)(linearSpeed, kEpsilon) * 1000.0F);
    if (m_motionMask & (1U << (2 * CAMERA_MOVE_IN)))
        StopMotion(CAMERA_MOVE_IN, timestamp);
    if (m_motionTimeout[CAMERA_MOVE_OUT] &&
        (m_motionMask & (1U << (2 * CAMERA_MOVE_OUT))))
        m_motionTimeout[CAMERA_MOVE_OUT] += timeout;
    else StartMotion(CAMERA_MOVE_OUT, timestamp, timeout);
}

void CGCamera::StartMotion(CGCameraMotion move, unsigned timestamp,
                           unsigned timeout) {
    if (move >= NUM_CAMERA_MOTIONS || (m_flags & 7) == 0) return;
    m_motionMask |= 1U << (2 * move);
    m_motionStart[move] = timestamp;
    m_motionTimeout[move] = timeout ? timestamp + timeout : 0;
}

void CGCamera::StopMotion(CGCameraMotion move, unsigned timestamp) {
    if (move >= NUM_CAMERA_MOTIONS) return;
    if (m_motionMask & (1U << (2 * move))) {
        m_motionStop[move] = timestamp;
        m_motionMask |= 1U << (2 * move + 1);
    }
}

void CGCamera::UpdateMotion(unsigned timestamp) {
    for (unsigned i = 0; i < NUM_CAMERA_MOTIONS; ++i) {
        const unsigned startBit = 1U << (2 * i);
        if ((m_motionMask & startBit) == 0) continue;
        if (m_motionTimeout[i] && static_cast<int>(timestamp - m_motionTimeout[i]) >= 0)
            StopMotion(static_cast<CGCameraMotion>(i), timestamp);
        unsigned delta;
        if (m_motionMask & (startBit << 1)) {
            delta = static_cast<int>(m_motionStop[i] - m_motionStart[i]) < 0
                ? 0U : m_motionStop[i] - m_motionStart[i];
            m_motionMask &= ~(3U << (2 * i));
        } else {
            const int signedDelta = static_cast<int>(timestamp - m_motionStart[i]);
            if (signedDelta < 0) continue;
            delta = static_cast<unsigned>(signedDelta);
            m_motionStart[i] = timestamp;
        }
        const float seconds = static_cast<float>(delta) * 0.001F;
        const float linearSpeed = CVarFloat(s_cameraLinearSpeed, kCameraLinearSpeed);
        const float angularSpeed = CVarFloat(s_cameraAngularSpeed, 180.0F) * kDegToRad;
        switch (static_cast<CGCameraMotion>(i)) {
        case CAMERA_MOVE_IN:
            if (!m_zoomSmoothingTimestamp) {
                m_desiredDistance = (std::max)(m_nearZ,
                    m_desiredDistance - linearSpeed * seconds);
                m_distance = m_desiredDistance;
            }
            break;
        case CAMERA_MOVE_OUT:
            if (!m_zoomSmoothingTimestamp) {
                m_desiredDistance = (std::min)(kMaxDistance,
                    m_desiredDistance + linearSpeed * seconds);
                m_distance = m_desiredDistance;
            }
            break;
        case CAMERA_MOVE_RIGHT:
            if (!m_yawSmoothingTimestamp) {
                m_desiredYaw += angularSpeed * seconds;
                if (m_desiredYaw > kTwoPi) m_desiredYaw -= kTwoPi;
                m_yawOffset = m_desiredYaw;
            }
            break;
        case CAMERA_MOVE_LEFT:
            if (!m_yawSmoothingTimestamp) {
                m_desiredYaw -= angularSpeed * seconds;
                if (m_desiredYaw < 0.0F) m_desiredYaw += kTwoPi;
                m_yawOffset = m_desiredYaw;
            }
            break;
        case CAMERA_MOVE_UP:
            if (!m_pitchSmoothingTimestamp) {
                m_desiredPitch = (std::min)(m_smoothingAngle + kMaxPitch,
                    m_desiredPitch + angularSpeed * seconds * 0.25F);
                m_pitch = m_desiredPitch;
            }
            break;
        case CAMERA_MOVE_DOWN:
            if (!m_pitchSmoothingTimestamp) {
                const float minimum = (m_flags & 7) ? -20.0F * kDegToRad : kMinPitch;
                m_desiredPitch = (std::max)(m_smoothingAngle + minimum,
                    m_desiredPitch - angularSpeed * seconds * 0.25F);
                m_pitch = m_desiredPitch;
            }
            break;
        default: break;
        }
    }
}

void CGCamera::SetDesiredDistance(float desiredDistance, unsigned timestamp) {
    desiredDistance = (std::clamp)(desiredDistance, m_nearZ, kMaxDistance);
    if (std::fabs(m_desiredDistance - desiredDistance) < kEpsilon) return;
    const float denominator = desiredDistance - m_desiredDistance;
    m_zoomTime = std::fabs(denominator) > kEpsilon
        ? std::fabs((desiredDistance - m_distance) / denominator) *
            CVarFloat(s_cameraSmoothingTime, kCameraSmoothingTime)
        : 0.0F;
    m_zoomSmoothingTimestamp = timestamp;
    m_previousDistance = m_distance;
    m_desiredDistance = desiredDistance;
}

void CGCamera::SetDesiredDistanceOverTime(float desiredDistance,
                                          float motionTime,
                                          unsigned timestamp) {
    m_zoomTime = motionTime;
    m_previousDistance = m_distance;
    m_desiredDistance = (std::clamp)(desiredDistance, m_nearZ, kMaxDistance);
    m_zoomSmoothingTimestamp = timestamp;
}

void CGCamera::SetDesiredPitchAngle(float desiredAngle, float delay,
                                    unsigned timestamp) {
    if (std::fabs(m_desiredPitch - desiredAngle) < kEpsilon) return;
    m_flags &= ~0x10;
    m_smoothingAngle = 0.0F;
    const float denominator = desiredAngle - m_desiredPitch;
    const float time = std::fabs(denominator) > kEpsilon
        ? std::fabs((desiredAngle - m_pitch) / denominator) *
            CVarFloat(s_cameraSmoothingTime, kCameraSmoothingTime)
        : 0.0F;
    SetDesiredPitchAngleOverTime(desiredAngle, time,
        timestamp + static_cast<unsigned>((std::max)(0.0F, delay) * 1000.0F));
}

void CGCamera::SetDesiredPitchAngleOverTime(float desiredAngle,
                                            float motionTime,
                                            unsigned timestamp) {
    m_previousPitch = m_pitch;
    m_desiredPitch = (std::clamp)(desiredAngle, kMinPitch, kMaxPitch);
    m_pitchTime = motionTime;
    m_pitchSmoothingTimestamp = timestamp;
}

void CGCamera::SetDesiredYawAngle(float desiredAngle, float delay,
                                  unsigned timestamp) {
    if (std::fabs(desiredAngle - m_yawOffset) > kPi)
        desiredAngle = kTwoPi - desiredAngle;
    if (m_desiredYaw == desiredAngle) return;
    const float denominator = desiredAngle - m_desiredYaw;
    const float time = std::fabs(denominator) > kEpsilon
        ? (desiredAngle - m_yawOffset) / denominator *
            CVarFloat(s_cameraSmoothingTime, kCameraSmoothingTime)
        : 0.0F;
    SetDesiredYawAngleOverTime(desiredAngle, std::fabs(time),
        timestamp + static_cast<unsigned>((std::max)(0.0F, delay) * 1000.0F));
}

void CGCamera::SetDesiredYawAngleOverTime(float desiredAngle, float motionTime,
                                          unsigned timestamp) {
    m_previousYaw = m_yawOffset;
    m_desiredYaw = desiredAngle;
    m_yawTime = motionTime;
    m_yawSmoothingTimestamp = timestamp;
}

void CGCamera::SetSmoothingAngle(float smoothingAngle, unsigned timestamp,
                                 int quickly) {
    smoothingAngle = (std::clamp)(smoothingAngle,
        -30.0F * kDegToRad, 35.0F * kDegToRad);
    const float newAngle = (std::clamp)(
        m_desiredPitch - m_smoothingAngle + smoothingAngle, -kPi * 0.5F,
        kPi * 0.5F);
    if (std::fabs(newAngle - m_desiredPitch) >= kEpsilon)
        SetDesiredPitchAngleOverTime(newAngle, quickly ? 0.25F : 1.0F,
                                     timestamp);
    m_smoothingAngle = smoothingAngle;
}

void CGCamera::EnableFreeLook() { SetModeFreeLook(); }
void CGCamera::DisableFreeLook(int sticky) {
    // CGCamera::DisableFreeLook (0x53D9C0) transfers the free-look result
    // into the normal-mode yaw state before the next CalcThirdPerson call.
    // Omitting this makes that call reconstruct m_yaw from the old offset and
    // visibly snap the camera back.
    SetModeNormal();
    m_distance = m_desiredDistance;
    if ((m_flags & 7) == 0) {
        SetTargetFadeValue(0);
        return;
    }

    if (sticky != 0) {
        const float delta = m_yaw - m_yawFreelookStart;
        m_desiredPitch = m_pitch;
        m_desiredYaw += delta;
        m_yawOffset = m_desiredYaw;
        return;
    }

    float desiredAngle = m_views[m_flags & 7].yaw;
    m_yawOffset = m_yaw - ClntObjMgrGetActiveFacing();
    while (m_yawOffset < 0.0F) m_yawOffset += kTwoPi;
    while (m_yawOffset > kTwoPi) m_yawOffset -= kTwoPi;
    if (std::fabs(desiredAngle - m_yawOffset) > kPi)
        desiredAngle = kTwoPi - desiredAngle;
    const float motionTime = std::fabs(m_yawOffset - desiredAngle) / kPi *
        CVarFloat(s_cameraSmoothingTime, kCameraSmoothingTime);
    SetDesiredYawAngleOverTime(desiredAngle, motionTime, OsGetAsyncTimeMs());
    m_desiredPitch = m_pitch;
}
void CGCamera::ToggleFreeLook() {
    if (m_flags & 8) DisableFreeLook(0); else EnableFreeLook();
}

void CGCamera::SetModeNormal() {
    if ((m_flags & 8) == 0) return;
    m_flags &= ~8;
    EventSetMouseMode(MOUSE_MODE_NORMAL, 0);
    if (CGWorldFrame::s_currentWorldFrame)
        CGWorldFrame::s_currentWorldFrame->OnMouseModeNormal();
}

void CGCamera::SetModeFreeLook() {
    if (m_flags & 8) return;
    m_flags |= 8;
    EventSetMouseMode(MOUSE_MODE_RELATIVE, 0);
    if (CGWorldFrame::s_currentWorldFrame)
        CGWorldFrame::s_currentWorldFrame->OnMouseModeRelative();
    m_previousPitch = m_pitch;
    if ((m_flags & 7) == 0)
        m_yaw = ClntObjMgrGetActiveFacing();
    m_yawFreelookStart = m_yaw;
}

void CGCamera::UpdateFreeLookFacing(float dx, float dy) {
    DDCToNDC(&dx, &dy, dx, dy);
    dx *= 0.00125F * (kPi * 0.5F);
    dy *= 0.0016666667F * (kPi * 0.5F);
    m_pitch += (CVarInt(s_mouseInvertPitch, 0) ? -1.0F : 1.0F) * dy;
    if ((m_flags & 7) != 0 && CVarInt(s_mouseInvertYaw, 0)) dx = -dx;
    m_yaw -= dx;
    ClampAngles();
    if (CGInputControl* input = CGInputControl::GetActive()) {
        float playerYaw = m_yaw - m_yawOffset;
        if (playerYaw < 0.0F) playerYaw += kTwoPi;
        input->CameraTurnPlayer(OsGetAsyncTimeMs(), playerYaw, m_pitch, 0);
    }
}

void CGCamera::SyncFreeLookFacing() {
    if (CGInputControl* input = CGInputControl::GetActive()) {
        float playerYaw = m_yaw - m_yawOffset;
        if (playerYaw < 0.0F) playerYaw += kTwoPi;
        input->CameraTurnPlayer(OsGetAsyncTimeMs(), playerYaw, m_pitch, 1);
    }
}

void CGCamera::SetTarget(std::uint64_t target) { m_target = target; }
void CGCamera::SetTargetFadeValue(std::uint8_t value) { m_targetFadeValue = value; }
float CGCamera::GetCameraDistance() const { return m_distance; }
float CGCamera::GetSmoothedHeight() const { return m_targetOffsetZ; }
float CGCamera::GetSmoothedYawAngle(float targetFacing, bool targetMoved) {
    float result = targetFacing;
    if (CVarInt(s_cameraSmooth, 1) != 0 && targetMoved) {
        float delta = targetFacing - m_lastFacing;
        if (std::fabs(delta) > 0.09F) {
            if (delta > kPi) delta -= kTwoPi;
            else if (delta < -kPi) delta += kTwoPi;
            if (std::fabs(delta) <= 15.0F * kDegToRad)
                result = m_lastFacing + (delta < 0.0F ? -0.09F : 0.09F);
            else
                result = m_lastFacing + delta +
                    (delta < 0.0F ? 15.0F * kDegToRad : -15.0F * kDegToRad);
            if (result < 0.0F) result += kTwoPi;
            else if (result > kTwoPi) result -= kTwoPi;
        }
    }
    m_lastFacing = result;
    return result;
}
void CGCamera::SetupWorldProjection(const NTempest::CRect* projectionRect) {
    SetGxProjectionAndView(projectionRect);
}
NTempest::C3Vector* CGCamera::Forward(NTempest::C3Vector* result) {
    return CSimpleCamera::Forward(result);
}
NTempest::C3Vector* CGCamera::Right(NTempest::C3Vector* result) {
    return CSimpleCamera::Right(result);
}
NTempest::C3Vector* CGCamera::Up(NTempest::C3Vector* result) {
    return CSimpleCamera::Up(result);
}
int __fastcall CGCamera::UpdateCallback(void*, int) { return 1; }

void RangeList::RemoveRange() {}
void CameraDestroy() {}
void CameraInitialize() {
    if (s_mouseInvertYaw) return;
    s_mouseInvertYaw = CVar::Register("mouseInvertYaw", "mouseInvertYaw", 0,
                                      "1", nullptr, 5, 0, nullptr);
    s_mouseInvertPitch = CVar::Register("mouseInvertPitch", "mouseInvertPitch", 0,
                                        "0", nullptr, 5, 0, nullptr);
    s_cameraSmooth = CVar::Register("cameraSmooth", "cameraSmooth", 0,
                                    "1", nullptr, 5, 0, nullptr);
    s_cameraSmoothingTime = CVar::Register("cameraSmoothingRate", "cameraSmoothingRate", 0,
                                           "0.5", nullptr, 5, 0, nullptr);
    s_cameraLinearSpeed = CVar::Register("cameraLinearSpeed", "cameraLinearSpeed", 0,
                                         "8.33", nullptr, 5, 0, nullptr);
    s_cameraAngularSpeed = CVar::Register("cameraAngularSpeed", "cameraAngularSpeed", 0,
                                          "180.0", nullptr, 5, 0, nullptr);
    static constexpr const char* angleNames[5] = {
        nullptr, "cameraAngleA", "cameraAngleB", "cameraAngleC", "cameraAngleD"};
    static constexpr const char* distanceNames[5] = {
        nullptr, "cameraDistanceA", "cameraDistanceB", "cameraDistanceC", "cameraDistanceD"};
    static constexpr const char* angleDefaults[5] = {nullptr, "0", "20", "30", "0"};
    static constexpr const char* distanceDefaults[5] = {nullptr, "5.55", "5.55", "13.88", "13.88"};
    for (int view = 1; view < 5; ++view) {
        s_cameraAngles[view] = CVar::Register(angleNames[view], angleNames[view], 0,
                                              angleDefaults[view], nullptr, 5, 0, nullptr);
        s_cameraDistances[view] = CVar::Register(distanceNames[view], distanceNames[view], 0,
                                                  distanceDefaults[view], nullptr, 5, 0, nullptr);
    }
}

int Script_CameraZoomIn(lua_State* state) {
    if (CGCamera* camera = ActiveCamera())
        camera->ZoomIn(lua_isnumber(state, 2)
            ? static_cast<float>(lua_tonumber(state, 2)) : 1.0F,
            ScriptTimestamp(state));
    return 0;
}
int Script_CameraZoomOut(lua_State* state) {
    if (CGCamera* camera = ActiveCamera())
        camera->ZoomOut(lua_isnumber(state, 2)
            ? static_cast<float>(lua_tonumber(state, 2)) : 1.0F,
            ScriptTimestamp(state));
    return 0;
}
int Script_MoveViewInStart(lua_State* s) { return StartScriptMotion(s, CAMERA_MOVE_IN); }
int Script_MoveViewInStop(lua_State* s) { return StopScriptMotion(s, CAMERA_MOVE_IN); }
int Script_MoveViewOutStart(lua_State* s) { return StartScriptMotion(s, CAMERA_MOVE_OUT); }
int Script_MoveViewOutStop(lua_State* s) { return StopScriptMotion(s, CAMERA_MOVE_OUT); }
int Script_MoveViewRightStart(lua_State* s) { return StartScriptMotion(s, CAMERA_MOVE_RIGHT); }
int Script_MoveViewRightStop(lua_State* s) { return StopScriptMotion(s, CAMERA_MOVE_RIGHT); }
int Script_MoveViewLeftStart(lua_State* s) { return StartScriptMotion(s, CAMERA_MOVE_LEFT); }
int Script_MoveViewLeftStop(lua_State* s) { return StopScriptMotion(s, CAMERA_MOVE_LEFT); }
int Script_MoveViewUpStart(lua_State* s) { return StartScriptMotion(s, CAMERA_MOVE_UP); }
int Script_MoveViewUpStop(lua_State* s) { return StopScriptMotion(s, CAMERA_MOVE_UP); }
int Script_MoveViewDownStart(lua_State* s) { return StartScriptMotion(s, CAMERA_MOVE_DOWN); }
int Script_MoveViewDownStop(lua_State* s) { return StopScriptMotion(s, CAMERA_MOVE_DOWN); }
int Script_ToggleMouseMove(lua_State*) { if (auto* c = ActiveCamera()) c->ToggleFreeLook(); return 0; }
int Script_SetView(lua_State* s) { if (auto* c = ActiveCamera()) c->SetView((int)luaL_checknumber(s, 1) - 1); return 0; }
int Script_SaveView(lua_State* s) { if (auto* c = ActiveCamera()) c->CreateViewFromCamera((int)luaL_checknumber(s, 1) - 1); return 0; }
int Script_ResetView(lua_State* s) { if (auto* c = ActiveCamera()) c->ResetView((int)luaL_checknumber(s, 1) - 1); return 0; }
int Script_NextView(lua_State*) { if (auto* c = ActiveCamera()) c->NextView(); return 0; }
int Script_PrevView(lua_State*) { if (auto* c = ActiveCamera()) c->PreviousView(); return 0; }

namespace {
constexpr std::array<std::pair<const char*, lua_CFunction>, 20> kCameraScripts{{
    {"CameraZoomIn", Script_CameraZoomIn}, {"CameraZoomOut", Script_CameraZoomOut},
    {"MoveViewInStart", Script_MoveViewInStart}, {"MoveViewInStop", Script_MoveViewInStop},
    {"MoveViewOutStart", Script_MoveViewOutStart}, {"MoveViewOutStop", Script_MoveViewOutStop},
    {"MoveViewLeftStart", Script_MoveViewLeftStart}, {"MoveViewLeftStop", Script_MoveViewLeftStop},
    {"MoveViewRightStart", Script_MoveViewRightStart}, {"MoveViewRightStop", Script_MoveViewRightStop},
    {"MoveViewUpStart", Script_MoveViewUpStart}, {"MoveViewUpStop", Script_MoveViewUpStop},
    {"MoveViewDownStart", Script_MoveViewDownStart}, {"MoveViewDownStop", Script_MoveViewDownStop},
    {"ToggleMouseMove", Script_ToggleMouseMove}, {"SetView", Script_SetView},
    {"SaveView", Script_SaveView}, {"ResetView", Script_ResetView},
    {"NextView", Script_NextView}, {"PrevView", Script_PrevView},
}};
}

void CameraRegisterScriptFunctions() {
    for (const auto& [name, function] : kCameraScripts)
        FrameScript_RegisterFunction(name, function);
}
void CameraUnregisterScriptFunctions() {
    for (const auto& [name, function] : kCameraScripts) {
        (void)function;
        FrameScript_UnregisterFunction(name);
    }
}
void ValidateCameraAngle() {}
void ValidateCameraDistance() {}
void ValidateIsInRange() {}
