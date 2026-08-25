#pragma once

#include "ENGINE/Source/Event/event.h"

#include <cstdint>

struct lua_State;
class CGUnit_C;
class CGCamera;

enum INPUT_CONTROL : std::uint32_t {
    INPUT_TURN_PLAYER = 0x00000001,
    INPUT_TURN_CAMERA = 0x00000002,
    INPUT_MOVE_PLAYER_OR_TURN_CAMERA = 0x00000004,
    INPUT_MOVE_PLAYER_FORWARD_KEY = 0x00000008,
    INPUT_MOVE_PLAYER_BACKWARD_KEY = 0x00000010,
    INPUT_STRAFE_PLAYER_LEFT_KEY = 0x00000020,
    INPUT_STRAFE_PLAYER_RIGHT_KEY = 0x00000040,
    INPUT_TURN_PLAYER_LEFT_KEY = 0x00000080,
    INPUT_TURN_PLAYER_RIGHT_KEY = 0x00000100,
    INPUT_PITCH_PLAYER_UP_KEY = 0x00000200,
    INPUT_PITCH_PLAYER_DOWN_KEY = 0x00000400,
    INPUT_MOVE_PLAYER_AUTORUN = 0x00000800,
    INPUT_MOVE_PLAYER_SENT = 0x00001000,
    INPUT_STRAFE_PLAYER_SENT = 0x00002000,
    INPUT_TURN_PLAYER_SENT = 0x00004000,
    INPUT_PITCH_PLAYER_SENT = 0x00008000,
    INPUT_PLAYER_MOVED = 0x10000000,
    INPUT_CAMERA_MOVED = 0x20000000,
    INPUT_FREE_LOOK_MASK = 0x00000007,
    INPUT_MOVE_PLAYER_MASK = 0x00000018,
    INPUT_STRAFE_PLAYER_MASK = 0x00000060,
    INPUT_TURN_PLAYER_MASK = 0x00000180,
    INPUT_PITCH_PLAYER_MASK = 0x00000600,
    INPUT_MOVE_AND_TURN_PLAYER = 0x00000005,
};

enum CGInputReleaseAction : std::uint32_t {
    INPUT_RELEASE_NONE = 0,
    INPUT_RELEASE_SELECT = 1,
    INPUT_RELEASE_ACTION = 2,
};

class CGInputControl {
public:
    CGInputControl();

    static CGInputControl* GetActive();
    void OnUpdate(float elapsedSec);
    void OnMouseMoveRel(const CMouseEvent* event);
    void UpdatePlayer(unsigned now);
    void SetReleaseAction(CGInputReleaseAction action);
    int SetControlBit(INPUT_CONTROL bit);
    bool IsMouseDragging() const;
    bool IsMouseDragMoving() const;
    int UnsetControlBit(INPUT_CONTROL bit, int sticky);
    void MovePlayer(unsigned now, CGUnit_C* player);
    void StrafePlayer(unsigned now, CGUnit_C* player);
    void TurnPlayer(unsigned now, CGUnit_C* player);
    void PitchPlayer(unsigned now, CGUnit_C* player);
    void SetControlBit(INPUT_CONTROL bit, int set, unsigned now, int sticky);
    int CameraCanTurnPlayer() const;
    void CameraTurnPlayer(unsigned timestamp, float yaw, float pitch,
                          unsigned char setSmoothFacing);

    static CGInputControl* s_inputControl;
    unsigned m_initializeTime = 0;
    unsigned m_controlFlags = 0;
    float m_mouseChangeX = 0.0F;
    float m_mouseChangeY = 0.0F;
    unsigned m_lastFrameMouseMoved = 0;
    unsigned m_mouseDownTime = 0;
    CGInputReleaseAction m_releaseAction = INPUT_RELEASE_NONE;
};

void InputControlInitialize(const char* cvarName = nullptr);
void InputControlDestroy();
unsigned char JoystickCallback(const char* newValue, void* userArg);
void InputControlRegisterScriptFunctions();
void InputControlUnregisterScriptFunctions();

int Script_ToggleAutoRun(lua_State* state);
int Script_MoveForwardStart(lua_State* state); int Script_MoveForwardStop(lua_State* state);
int Script_MoveBackwardStart(lua_State* state); int Script_MoveBackwardStop(lua_State* state);
int Script_TurnLeftStart(lua_State* state); int Script_TurnLeftStop(lua_State* state);
int Script_TurnRightStart(lua_State* state); int Script_TurnRightStop(lua_State* state);
int Script_StrafeLeftStart(lua_State* state); int Script_StrafeLeftStop(lua_State* state);
int Script_StrafeRightStart(lua_State* state); int Script_StrafeRightStop(lua_State* state);
int Script_PitchUpStart(lua_State* state); int Script_PitchUpStop(lua_State* state);
int Script_PitchDownStart(lua_State* state); int Script_PitchDownStop(lua_State* state);
int Script_TurnOrActionStart(lua_State* state); int Script_TurnOrActionStop(lua_State* state);
int Script_CameraOrSelectOrMoveStart(lua_State* state);
int Script_CameraOrSelectOrMoveStop(lua_State* state);

extern float INFINITY_135;
extern float TWO_PI_135;
extern float OO_TWO_PI_135;
extern float MAX_DUEL_DISTANCE_SQUARED_66;
extern float MAX_TRADE_DISTANCE_SQUARED_66;
extern float MAX_SHOP_DISTANCE_SQUARED_66;
extern float MAX_INSPECT_DISTANCE_SQUARED_66;
extern float MAX_LOOT_DISTANCE_SQUARED_66;
extern float MAX_BIND_DISTANCE_SQUARED_66;
extern float MAX_SITCHAIR_DISTANCE_SQUARED_66;
extern float MAX_SITCHAIRUSE_DISTANCE_SQUARED_66;
extern float MAX_SHARE_DISTANCE_SQUARED_66;
extern float SECONDS_PER_HEARTBEAT_66;
extern float SPELL_FOCUS_DISTANCE_SQUARED_65;
extern float MAX_OBJ_INTEREST_RADIUS_SQUARED_66;
extern float MD_OCEAN_DEPTH_SCALE_52;
extern float MD_MAX_DEPTH_52;
extern float WMOMM_INCHES_PER_PIXEL_52;
extern float WMOMM_QUAD_SIZE_52;
extern float OOSMOLTILE_SIZE_52;

CGCamera* Camera();
int _E2_135(); int _E1_135(); int _E5_135(); void _E4_135();
int _E8_135(); void _E7_135(); int _E28_59(); void _E27_56();
int _E31_56(); void _E30_54(); int _E34_57(); void _E33_52();
int _E37_56(); void _E36_51(); int _E40_57(); void _E39_53();
int _E43_56(); void _E42_52(); int _E46_54(); void _E45_51();
int _E49_56(); void _E48_56(); int _E52_55(); void _E51_50();
int _E55_51(); void _E54_55(); int _E58_50(); void _E57_43();
int _E61_51(); void _E60_51(); int _E68_17(); void _E67_47();
int _E71_9(); void _E70_39(); int _E74_6(); void _E73_39();
int _E77_5(); void _E76_38(); int _E80_9(); void _E79_5();
