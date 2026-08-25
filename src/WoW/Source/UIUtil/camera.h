#pragma once

#include "WoW/Source/UIUtil/csimplecamera.h"

#include "ENGINE/Source/Tempest/c34matrix.h"

#include <array>
#include <cstdint>

struct lua_State;
struct HMODEL__;
struct HCAMERA__;

enum CGCameraMotion : std::uint32_t {
    CAMERA_MOVE_IN = 0,
    CAMERA_MOVE_OUT = 1,
    CAMERA_MOVE_RIGHT = 2,
    CAMERA_MOVE_LEFT = 3,
    CAMERA_MOVE_UP = 4,
    CAMERA_MOVE_DOWN = 5,
    NUM_CAMERA_MOTIONS = 6,
};

struct CGCameraView {
    float dist = 0.0F;
    float pitch = 0.0F;
    float yaw = 0.0F;
};

class CGCamera : public CSimpleCamera {
public:
    CGCamera();
    ~CGCamera() override;

    void AddShake();
    void CCommand_CameraClip();
    void CalcFirstPerson(const NTempest::C3Vector& targetPosition,
                         float targetFacing, unsigned timestamp);
    void CalcModelCamera();
    void CalcThirdPerson(const NTempest::C3Vector& targetPosition,
                         float targetFacing, unsigned timestamp);
    void CheckUnderwater();
    void ClampAngles();
    void ClearModelCamera();
    float CollideCameraWithWorld(const NTempest::C3Vector& targetPosition);
    void CreateViewFromCamera(int view);
    void CreateViewFromParams(int view, float dist, float pitch, float yaw);
    void CycleView();
    void DisableFreeLook(int sticky = 0);
    void EnableFreeLook();
    void FinishLoadingModel();
    void FinishLoadingTarget();
    NTempest::C3Vector* Forward(NTempest::C3Vector* result) override;
    float GetCameraDistance() const;
    float GetSmoothedHeight() const;
    float GetSmoothedYawAngle(float targetFacing, bool targetMoved);
    void MakeRelativeTo();
    void NextView();
    void ParentToWorld();
    void PerformTerrainTilt();
    void PreviousView();
    void ResetModelCamera();
    void ResetView(int view = -1);
    NTempest::C3Vector* Right(NTempest::C3Vector* result) override;
    void RunShakes();
    void SetDesiredDistance(float desiredDistance, unsigned timestamp);
    void SetDesiredDistanceOverTime(float desiredDistance, float motionTime,
                                    unsigned timestamp);
    void SetDesiredPitchAngle(float desiredAngle, float delay,
                              unsigned timestamp);
    void SetDesiredPitchAngleOverTime(float desiredAngle, float motionTime,
                                      unsigned timestamp);
    void SetDesiredYawAngle(float desiredAngle, float delay,
                            unsigned timestamp);
    void SetDesiredYawAngleOverTime(float desiredAngle, float motionTime,
                                    unsigned timestamp);
    void SetModeFreeLook();
    void SetModeNormal();
    void SetModelCamera();
    void SetPositionAndFacing(const NTempest::C3Vector& position,
                              const NTempest::C3Vector& facing);
    void SetPositionAndTarget(const NTempest::C3Vector& position,
                              const NTempest::C3Vector& target);
    void SetPositionAndTargetWithRoll(const NTempest::C3Vector& position,
                                      const NTempest::C3Vector& target,
                                      float roll);
    void SetSmoothingAngle(float smoothingAngle, unsigned timestamp,
                           int quickly);
    void SetTarget(std::uint64_t target = 0);
    void SetTargetFadeValue(std::uint8_t value);
    void SetView(int newView);
    void SetupWorldProjection(const NTempest::CRect* projectionRect);
    void StartMotion(CGCameraMotion move, unsigned timestamp,
                     unsigned timeout = 0);
    void StopMotion(CGCameraMotion move, unsigned timestamp);
    void SyncFreeLookFacing();
    void ToggleFreeLook();
    NTempest::C3Vector* Up(NTempest::C3Vector* result) override;
    void Update(const NTempest::C3Vector& targetPosition, float targetFacing,
                unsigned timestamp);
    static int __fastcall UpdateCallback(void* param, int camera);
    void UpdateFreeLookFacing(float dx, float dy);
    void UpdateMotion(unsigned timestamp);
    void ZoomIn(float distance, unsigned timestamp);
    void ZoomOut(float distance, unsigned timestamp);

    HMODEL__* m_model = nullptr;
    HCAMERA__* m_modelCamera = nullptr;
    NTempest::C34Matrix m_modelMatrix{};
    std::uint64_t m_target = 0;
    float m_targetOffsetZ = 2.0F;
    int m_flags = 17;
    std::uint64_t m_relativeTo = 0;
    std::array<CGCameraView, 5> m_views{};
    float m_distance = 5.55F;
    float m_yaw = 0.0F;
    float m_pitch = 0.0F;
    float m_roll = 0.0F;
    float m_yawOffset = 0.0F;
    float m_yawFreelookStart = 0.0F;
    unsigned m_motionMask = 0;
    std::array<unsigned, NUM_CAMERA_MOTIONS> m_motionStart{};
    std::array<unsigned, NUM_CAMERA_MOTIONS> m_motionStop{};
    std::array<unsigned, NUM_CAMERA_MOTIONS> m_motionTimeout{};
    NTempest::C3Vector m_lastTarget{};
    float m_savedTargetZ = 0.0F;
    float m_lastFacing = 0.0F;
    unsigned m_lastDeltaZ = 0;
    float m_smoothingAngle = 0.0F;
    unsigned m_zoomSmoothingTimestamp = 0;
    float m_zoomTime = 0.0F;
    float m_desiredDistance = 5.55F;
    float m_previousDistance = 5.55F;
    unsigned m_pitchSmoothingTimestamp = 0;
    float m_pitchTime = 0.0F;
    float m_desiredPitch = 0.0F;
    float m_previousPitch = 0.0F;
    unsigned m_yawSmoothingTimestamp = 0;
    float m_yawTime = 0.0F;
    float m_desiredYaw = 0.0F;
    float m_previousYaw = 0.0F;
    int m_cycleDirection = 1;
    unsigned m_savedLiquid = 0;
    std::uint8_t m_targetFadeValue = 255;
};

class RangeList {
public:
    void RemoveRange();
};

void CameraDestroy();
void CameraInitialize();
void CameraRegisterScriptFunctions();
void CameraUnregisterScriptFunctions();

int Script_CameraZoomIn(lua_State* state);
int Script_CameraZoomOut(lua_State* state);
int Script_MoveViewDownStart(lua_State* state);
int Script_MoveViewDownStop(lua_State* state);
int Script_MoveViewInStart(lua_State* state);
int Script_MoveViewInStop(lua_State* state);
int Script_MoveViewLeftStart(lua_State* state);
int Script_MoveViewLeftStop(lua_State* state);
int Script_MoveViewOutStart(lua_State* state);
int Script_MoveViewOutStop(lua_State* state);
int Script_MoveViewRightStart(lua_State* state);
int Script_MoveViewRightStop(lua_State* state);
int Script_MoveViewUpStart(lua_State* state);
int Script_MoveViewUpStop(lua_State* state);
int Script_NextView(lua_State* state);
int Script_PrevView(lua_State* state);
int Script_ResetView(lua_State* state);
int Script_SaveView(lua_State* state);
int Script_SetView(lua_State* state);
int Script_ToggleMouseMove(lua_State* state);

void ValidateCameraAngle();
void ValidateCameraDistance();
void ValidateIsInRange();
