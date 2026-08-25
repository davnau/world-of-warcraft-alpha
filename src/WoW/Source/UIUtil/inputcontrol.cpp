#include "WoW/Source/UIUtil/inputcontrol.h"

#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "WoW/Source/ObjectMgrClient/objectmgrclient.h"
#include "WoW/Source/UI/worldframe.h"
#include "WoW/Source/UIUtil/camera.h"
#include "WoW/Source/WorldClient/World.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <limits>

extern "C" {
#include "lauxlib.h"
}

float INFINITY_135 = 0.0F;
float TWO_PI_135 = 0.0F;
float OO_TWO_PI_135 = 0.0F;
float MAX_DUEL_DISTANCE_SQUARED_66 = 0.0F;
float MAX_TRADE_DISTANCE_SQUARED_66 = 0.0F;
float MAX_SHOP_DISTANCE_SQUARED_66 = 0.0F;
float MAX_INSPECT_DISTANCE_SQUARED_66 = 0.0F;
float MAX_LOOT_DISTANCE_SQUARED_66 = 0.0F;
float MAX_BIND_DISTANCE_SQUARED_66 = 0.0F;
float MAX_SITCHAIR_DISTANCE_SQUARED_66 = 0.0F;
float MAX_SITCHAIRUSE_DISTANCE_SQUARED_66 = 0.0F;
float MAX_SHARE_DISTANCE_SQUARED_66 = 0.0F;
float SECONDS_PER_HEARTBEAT_66 = 0.0F;
float SPELL_FOCUS_DISTANCE_SQUARED_65 = 0.0F;
float MAX_OBJ_INTEREST_RADIUS_SQUARED_66 = 0.0F;
float MD_OCEAN_DEPTH_SCALE_52 = 0.0F;
float MD_MAX_DEPTH_52 = 0.0F;
float WMOMM_INCHES_PER_PIXEL_52 = 0.0F;
float WMOMM_QUAD_SIZE_52 = 0.0F;
float OOSMOLTILE_SIZE_52 = 0.0F;

CGInputControl* CGInputControl::s_inputControl = nullptr;

namespace {
int s_joystickID = -1;
unsigned s_buttonState = 0;
float s_rate = 0.0F;
float s_speed = 0.0F;
unsigned s_lastMovementHeartbeat = 0U;

constexpr unsigned kMovementInputMask =
    INPUT_MOVE_PLAYER_MASK | INPUT_STRAFE_PLAYER_MASK |
    INPUT_TURN_PLAYER_MASK | INPUT_MOVE_PLAYER_AUTORUN;

unsigned Now() { return OsGetAsyncTimeMs(); }

unsigned ScriptTimestamp(lua_State* state) {
    return state && lua_isnumber(state, 1)
        ? static_cast<unsigned>(lua_tonumber(state, 1)) : Now();
}

int ScriptSticky(lua_State* state) {
    if (!state) return 0;
    if (lua_isnumber(state, 2))
        return static_cast<int>(lua_tonumber(state, 2));
    if (lua_isstring(state, 2)) {
        const char* value = lua_tostring(state, 2);
        return value && (_stricmp(value, "true") == 0 ||
                         _stricmp(value, "yes") == 0 ||
                         std::strtol(value, nullptr, 0) != 0);
    }
    return 0;
}

int SetScriptControl(lua_State* state, INPUT_CONTROL bit, bool set,
                     int sticky = 0) {
    CGInputControl* control = CGInputControl::GetActive();
    if (control == nullptr)
        return 0;
    control->SetControlBit(bit, set ? 1 : 0, ScriptTimestamp(state), sticky);
    return 0;
}
}

CGInputControl::CGInputControl() : m_initializeTime(Now()) {}

CGInputControl* CGInputControl::GetActive() { return s_inputControl; }

void InputControlInitialize(const char*) {
    if (CGInputControl::s_inputControl == nullptr)
        CGInputControl::s_inputControl = new CGInputControl();
}

void InputControlDestroy() {
    delete CGInputControl::s_inputControl;
    CGInputControl::s_inputControl = nullptr;
    s_joystickID = -1;
    s_lastMovementHeartbeat = 0U;
}

unsigned char JoystickCallback(const char* newValue, void*) {
    if (newValue != nullptr && *newValue == '1')
        s_joystickID = 0;
    else
        s_joystickID = -1;
    return 1;
}

namespace {
struct InputScriptFunction { const char* name; lua_CFunction function; };
constexpr InputScriptFunction kInputScripts[] = {
    {"ToggleAutoRun", Script_ToggleAutoRun},
    {"MoveForwardStart", Script_MoveForwardStart}, {"MoveForwardStop", Script_MoveForwardStop},
    {"MoveBackwardStart", Script_MoveBackwardStart}, {"MoveBackwardStop", Script_MoveBackwardStop},
    {"TurnLeftStart", Script_TurnLeftStart}, {"TurnLeftStop", Script_TurnLeftStop},
    {"TurnRightStart", Script_TurnRightStart}, {"TurnRightStop", Script_TurnRightStop},
    {"StrafeLeftStart", Script_StrafeLeftStart}, {"StrafeLeftStop", Script_StrafeLeftStop},
    {"StrafeRightStart", Script_StrafeRightStart}, {"StrafeRightStop", Script_StrafeRightStop},
    {"PitchUpStart", Script_PitchUpStart}, {"PitchUpStop", Script_PitchUpStop},
    {"PitchDownStart", Script_PitchDownStart}, {"PitchDownStop", Script_PitchDownStop},
    {"TurnOrActionStart", Script_TurnOrActionStart}, {"TurnOrActionStop", Script_TurnOrActionStop},
    {"CameraOrSelectOrMoveStart", Script_CameraOrSelectOrMoveStart},
    {"CameraOrSelectOrMoveStop", Script_CameraOrSelectOrMoveStop},
};
}

void InputControlRegisterScriptFunctions() {
    for (const auto& function : kInputScripts)
        FrameScript_RegisterFunction(function.name, function.function);
}
void InputControlUnregisterScriptFunctions() {
    for (const auto& function : kInputScripts)
        FrameScript_UnregisterFunction(function.name);
}

#define INPUT_SCRIPT_PAIR(Name, Bit) \
    int Script_##Name##Start(lua_State* state) { return SetScriptControl(state, Bit, true); } \
    int Script_##Name##Stop(lua_State* state) { return SetScriptControl(state, Bit, false); }

int Script_ToggleAutoRun(lua_State* state) {
    CGInputControl* control = CGInputControl::GetActive();
    if (control != nullptr)
        control->SetControlBit(INPUT_MOVE_PLAYER_AUTORUN,
                               (control->m_controlFlags & INPUT_MOVE_PLAYER_AUTORUN) == 0,
                               ScriptTimestamp(state), 0);
    return 0;
}
INPUT_SCRIPT_PAIR(MoveForward, INPUT_MOVE_PLAYER_FORWARD_KEY)
INPUT_SCRIPT_PAIR(MoveBackward, INPUT_MOVE_PLAYER_BACKWARD_KEY)
INPUT_SCRIPT_PAIR(TurnLeft, INPUT_TURN_PLAYER_LEFT_KEY)
INPUT_SCRIPT_PAIR(TurnRight, INPUT_TURN_PLAYER_RIGHT_KEY)
INPUT_SCRIPT_PAIR(StrafeLeft, INPUT_STRAFE_PLAYER_LEFT_KEY)
INPUT_SCRIPT_PAIR(StrafeRight, INPUT_STRAFE_PLAYER_RIGHT_KEY)
INPUT_SCRIPT_PAIR(PitchUp, INPUT_PITCH_PLAYER_UP_KEY)
INPUT_SCRIPT_PAIR(PitchDown, INPUT_PITCH_PLAYER_DOWN_KEY)
#undef INPUT_SCRIPT_PAIR

int Script_TurnOrActionStart(lua_State* state) {
    if (CGInputControl* control = CGInputControl::GetActive()) {
        control->SetReleaseAction(INPUT_RELEASE_ACTION);
        control->SetControlBit(INPUT_TURN_PLAYER, 1, ScriptTimestamp(state), 0);
    }
    return 0;
}

int Script_TurnOrActionStop(lua_State* state) {
    return SetScriptControl(state, INPUT_TURN_PLAYER, false);
}

int Script_CameraOrSelectOrMoveStart(lua_State* state) {
    if (CGInputControl* control = CGInputControl::GetActive()) {
        control->SetReleaseAction(INPUT_RELEASE_SELECT);
        control->SetControlBit(INPUT_MOVE_PLAYER_OR_TURN_CAMERA, 1,
                               ScriptTimestamp(state), 0);
    }
    return 0;
}

int Script_CameraOrSelectOrMoveStop(lua_State* state) {
    return SetScriptControl(state, INPUT_MOVE_PLAYER_OR_TURN_CAMERA, false,
                            ScriptSticky(state));
}

void CGInputControl::OnUpdate(float elapsedSec) {
    if (CWorld::IsPlayerDead()) {
        m_controlFlags &= ~(INPUT_MOVE_PLAYER_MASK |
            INPUT_STRAFE_PLAYER_MASK | INPUT_TURN_PLAYER_MASK |
            INPUT_PITCH_PLAYER_MASK | INPUT_MOVE_PLAYER_AUTORUN |
            INPUT_PLAYER_MOVED);
        return;
    }
    if (s_joystickID >= 0) {
        if ((s_buttonState & 1) != 0) m_controlFlags |= INPUT_TURN_PLAYER;
        else m_controlFlags &= ~INPUT_TURN_PLAYER;
    }
    const float elapsed = (std::clamp)(elapsedSec, 0.0F, 0.25F);
    float facing = ClntObjMgrGetActiveFacing();
    const int turn = ((m_controlFlags & INPUT_TURN_PLAYER_LEFT_KEY) != 0) -
                     ((m_controlFlags & INPUT_TURN_PLAYER_RIGHT_KEY) != 0);
    if (turn && (m_controlFlags & INPUT_TURN_PLAYER) == 0) {
        // The recovered client treats positive world yaw as a right turn.
        // INPUT_TURN_PLAYER_LEFT_KEY therefore decreases facing.
        facing -= static_cast<float>(turn) * 3.1415927F * elapsed;
        ClntObjMgrSetActiveFacing(facing);
    }
    const int move = ((m_controlFlags & INPUT_MOVE_PLAYER_AUTORUN) != 0) +
                     ((m_controlFlags & INPUT_MOVE_PLAYER_FORWARD_KEY) != 0) +
                     ((m_controlFlags & INPUT_MOVE_AND_TURN_PLAYER) == INPUT_MOVE_AND_TURN_PLAYER) -
                     ((m_controlFlags & INPUT_MOVE_PLAYER_BACKWARD_KEY) != 0);
    const int strafe = ((m_controlFlags & INPUT_STRAFE_PLAYER_LEFT_KEY) != 0) -
                       ((m_controlFlags & INPUT_STRAFE_PLAYER_RIGHT_KEY) != 0) +
                       (((m_controlFlags & INPUT_TURN_PLAYER) != 0) ? turn : 0);
    if (move || strafe) {
        const NTempest::C3Vector start = ClntObjMgrGetActivePosition();
        NTempest::C3Vector position = start;
        constexpr float speed = 7.0F;
        position.x += (std::cos(facing) * static_cast<float>(move) -
                       std::sin(facing) * static_cast<float>(strafe)) * speed * elapsed;
        position.y += (std::sin(facing) * static_cast<float>(move) +
                       std::cos(facing) * static_cast<float>(strafe)) * speed * elapsed;
        position = CWorld::ResolvePlayerMovement(start, position);
        ClntObjMgrSetActivePosition(position);
    }

    // The reconstructed client integrates movement locally.  Keep the
    // authoritative server position current while movement or keyboard
    // turning is active so range checks, aggro, and remote movement use the
    // same coordinates the player sees.
    if (move || strafe || turn) {
        const unsigned now = Now();
        if (now - s_lastMovementHeartbeat >= 100U) {
            CWorld::SendPlayerMovementUpdate();
            s_lastMovementHeartbeat = now;
        }
    }
}

void CGInputControl::OnMouseMoveRel(const CMouseEvent* event) {
    if (event == nullptr || m_controlFlags == 0)
        return;
    m_mouseChangeX += std::fabs(event->x);
    m_mouseChangeY += std::fabs(event->y);
    m_lastFrameMouseMoved = GxPerfCounter(GxPerf_FrameNum);
    if (CGCamera* camera = Camera())
        camera->UpdateFreeLookFacing(event->x, event->y);
}

void CGInputControl::UpdatePlayer(unsigned now) {
    if (ClntObjMgrGetActivePlayer() == 0) return;
    // The reconstructed object manager currently stores the active mover's
    // movement state directly. Keep the recovered transition ordering here;
    // the CGUnit_C argument becomes live when object allocation is restored.
    MovePlayer(now, nullptr);
    StrafePlayer(now, nullptr);
    TurnPlayer(now, nullptr);
    PitchPlayer(now, nullptr);
}

void CGInputControl::SetReleaseAction(CGInputReleaseAction action) {
    if ((m_controlFlags & INPUT_FREE_LOOK_MASK) == 0)
        m_releaseAction = action;
}

int CGInputControl::SetControlBit(INPUT_CONTROL bit) {
    if ((m_controlFlags & bit) != 0)
        return 0;
    if ((m_controlFlags & INPUT_FREE_LOOK_MASK) == 0 && (bit & INPUT_FREE_LOOK_MASK) != 0) {
        m_mouseChangeX = 0.0F;
        m_mouseChangeY = 0.0F;
        m_mouseDownTime = Now();
        if (CGCamera* camera = Camera()) camera->EnableFreeLook();
    }
    m_controlFlags |= bit;
    if ((bit & INPUT_MOVE_PLAYER_MASK) != 0)
        m_controlFlags &= ~INPUT_MOVE_PLAYER_AUTORUN;
    if ((m_controlFlags & INPUT_MOVE_AND_TURN_PLAYER) == INPUT_MOVE_AND_TURN_PLAYER)
        m_controlFlags &= ~INPUT_MOVE_PLAYER_AUTORUN;
    if ((bit & INPUT_MOVE_PLAYER_MASK) != 0)
        m_controlFlags |= INPUT_PLAYER_MOVED;
    if ((bit & INPUT_FREE_LOOK_MASK) != 0) {
        m_controlFlags |= INPUT_CAMERA_MOVED;
        if ((m_controlFlags & INPUT_FREE_LOOK_MASK) != bit)
            m_releaseAction = INPUT_RELEASE_NONE;
    }
    return 1;
}

bool CGInputControl::IsMouseDragging() const {
    const unsigned elapsed = Now() - m_mouseDownTime;
    return elapsed >= 800 ||
           ((m_mouseChangeX >= 8.0F || m_mouseChangeY >= 8.0F) && elapsed >= 200);
}

bool CGInputControl::IsMouseDragMoving() const {
    return IsMouseDragging() &&
        (GxPerfCounter(GxPerf_FrameNum) - m_lastFrameMouseMoved < 2);
}

int CGInputControl::UnsetControlBit(INPUT_CONTROL bit, int sticky) {
    if ((m_controlFlags & bit) == 0)
        return 0;
    const bool lastFreeLook = (m_controlFlags & INPUT_FREE_LOOK_MASK) != 0 &&
        (m_controlFlags & INPUT_FREE_LOOK_MASK & ~static_cast<unsigned>(bit)) == 0;
    if (lastFreeLook) {
        if (CGCamera* camera = Camera()) {
            camera->SyncFreeLookFacing();
            camera->DisableFreeLook(sticky);
        }
    }
    m_controlFlags &= ~static_cast<unsigned>(bit);
    if (lastFreeLook) {
        if (IsMouseDragging()) {
            m_releaseAction = INPUT_RELEASE_NONE;
            return 1;
        }
        if (CGWorldFrame::s_currentWorldFrame) {
            if (m_releaseAction == INPUT_RELEASE_SELECT)
                CGWorldFrame::s_currentWorldFrame->PerformDefaultAction(1, Now());
            else if (m_releaseAction == INPUT_RELEASE_ACTION)
                CGWorldFrame::s_currentWorldFrame->PerformDefaultAction(4, Now());
        }
    }
    return 1;
}

void CGInputControl::MovePlayer(unsigned, CGUnit_C*) {
    const int direction = ((m_controlFlags & INPUT_MOVE_PLAYER_AUTORUN) != 0) +
                          ((m_controlFlags & INPUT_MOVE_PLAYER_FORWARD_KEY) != 0) +
                          ((m_controlFlags & INPUT_MOVE_AND_TURN_PLAYER) == INPUT_MOVE_AND_TURN_PLAYER) -
                          ((m_controlFlags & INPUT_MOVE_PLAYER_BACKWARD_KEY) != 0);
    if (direction != 0) m_controlFlags |= INPUT_MOVE_PLAYER_SENT;
    else m_controlFlags &= ~INPUT_MOVE_PLAYER_SENT;
}

void CGInputControl::StrafePlayer(unsigned, CGUnit_C*) {
    const int direction = ((m_controlFlags & INPUT_STRAFE_PLAYER_LEFT_KEY) != 0) -
                          ((m_controlFlags & INPUT_STRAFE_PLAYER_RIGHT_KEY) != 0) +
                          (((m_controlFlags & INPUT_TURN_PLAYER) != 0 &&
                            (m_controlFlags & INPUT_TURN_PLAYER_LEFT_KEY) != 0) ? 1 : 0) -
                          (((m_controlFlags & INPUT_TURN_PLAYER) != 0 &&
                            (m_controlFlags & INPUT_TURN_PLAYER_RIGHT_KEY) != 0) ? 1 : 0);
    if (direction != 0) m_controlFlags |= INPUT_STRAFE_PLAYER_SENT;
    else m_controlFlags &= ~INPUT_STRAFE_PLAYER_SENT;
}

void CGInputControl::TurnPlayer(unsigned, CGUnit_C*) {
    const int direction = ((m_controlFlags & INPUT_TURN_PLAYER_LEFT_KEY) != 0) -
                          ((m_controlFlags & INPUT_TURN_PLAYER_RIGHT_KEY) != 0);
    if (direction != 0 && (m_controlFlags & INPUT_TURN_PLAYER) == 0)
        m_controlFlags |= INPUT_TURN_PLAYER_SENT;
    else
        m_controlFlags &= ~INPUT_TURN_PLAYER_SENT;
}

void CGInputControl::PitchPlayer(unsigned, CGUnit_C*) {
    const int direction = ((m_controlFlags & INPUT_PITCH_PLAYER_UP_KEY) != 0) -
                          ((m_controlFlags & INPUT_PITCH_PLAYER_DOWN_KEY) != 0);
    if (direction != 0 && (m_controlFlags & INPUT_TURN_PLAYER) == 0)
        m_controlFlags |= INPUT_PITCH_PLAYER_SENT;
    else
        m_controlFlags &= ~INPUT_PITCH_PLAYER_SENT;
}

void CGInputControl::SetControlBit(INPUT_CONTROL bit, int set, unsigned now, int sticky) {
    const int changed = set != 0 ? SetControlBit(bit)
                                 : UnsetControlBit(bit, sticky);
    if (changed) {
        UpdatePlayer(now);
        if ((static_cast<unsigned>(bit) & kMovementInputMask) != 0U) {
            CWorld::SendPlayerMovementUpdate();
            s_lastMovementHeartbeat = Now();
        }
    }
}

int CGInputControl::CameraCanTurnPlayer() const {
    CGCamera* camera = Camera();
    return camera && (camera->m_flags & 8) != 0 &&
        (m_controlFlags & INPUT_TURN_PLAYER) != 0;
}

void CGInputControl::CameraTurnPlayer(unsigned, float yaw, float,
                                      unsigned char) {
    if (CameraCanTurnPlayer()) {
        ClntObjMgrSetActiveFacing(yaw);
        m_controlFlags &= ~(INPUT_TURN_PLAYER_SENT | INPUT_PITCH_PLAYER_SENT);
        m_controlFlags |= INPUT_CAMERA_MOVED;
    }
}

CGCamera* Camera() {
    return CGWorldFrame::s_currentWorldFrame
        ? CGWorldFrame::s_currentWorldFrame->GetActiveCamera() : nullptr;
}

int _E2_135() { return _E1_135(); }
int _E1_135() { INFINITY_135 = std::numeric_limits<float>::infinity(); return 2139095040; }
int _E5_135() { _E4_135(); return 0; }
void _E4_135() { TWO_PI_135 = 3.1415927F + 3.1415927F; }
int _E8_135() { _E7_135(); return 0; }
void _E7_135() { OO_TWO_PI_135 = 1.0F / TWO_PI_135; }

#define INPUT_CONST_PAIR(thunk, body, variable, value) \
    int thunk() { body(); return 0; } \
    void body() { variable = value; }
INPUT_CONST_PAIR(_E28_59, _E27_56, MAX_DUEL_DISTANCE_SQUARED_66, 100.0F)
INPUT_CONST_PAIR(_E31_56, _E30_54, MAX_TRADE_DISTANCE_SQUARED_66, 123.45679F)
INPUT_CONST_PAIR(_E34_57, _E33_52, MAX_SHOP_DISTANCE_SQUARED_66, 30.864195F)
INPUT_CONST_PAIR(_E37_56, _E36_51, MAX_INSPECT_DISTANCE_SQUARED_66, 30.864195F)
INPUT_CONST_PAIR(_E40_57, _E39_53, MAX_LOOT_DISTANCE_SQUARED_66, 25.0F)
INPUT_CONST_PAIR(_E43_56, _E42_52, MAX_BIND_DISTANCE_SQUARED_66, 100.0F)
INPUT_CONST_PAIR(_E46_54, _E45_51, MAX_SITCHAIR_DISTANCE_SQUARED_66, 0.25F)
INPUT_CONST_PAIR(_E49_56, _E48_56, MAX_SITCHAIRUSE_DISTANCE_SQUARED_66, 9.0F)
INPUT_CONST_PAIR(_E52_55, _E51_50, MAX_SHARE_DISTANCE_SQUARED_66, 10000.0F)
INPUT_CONST_PAIR(_E55_51, _E54_55, SECONDS_PER_HEARTBEAT_66, 5.0F)
INPUT_CONST_PAIR(_E58_50, _E57_43, SPELL_FOCUS_DISTANCE_SQUARED_65, 2500.0F)
INPUT_CONST_PAIR(_E61_51, _E60_51, MAX_OBJ_INTEREST_RADIUS_SQUARED_66, 10000.0F)
INPUT_CONST_PAIR(_E68_17, _E67_47, MD_OCEAN_DEPTH_SCALE_52, 0.5833333F)
INPUT_CONST_PAIR(_E71_9, _E70_39, MD_MAX_DEPTH_52, -148.75F)
INPUT_CONST_PAIR(_E74_6, _E73_39, WMOMM_INCHES_PER_PIXEL_52, 0.5F)
INPUT_CONST_PAIR(_E77_5, _E76_38, WMOMM_QUAD_SIZE_52, 128.0F)
INPUT_CONST_PAIR(_E80_9, _E79_5, OOSMOLTILE_SIZE_52, 0.24000001F)
#undef INPUT_CONST_PAIR
