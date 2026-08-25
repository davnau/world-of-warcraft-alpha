#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

class CMovement {
public:
    void AddPlayerMoveEvent();
    void AddSpline();
    void AddToMoversList();
    void ApplyAdjustedMove();
    void ApplyMovement();
    void BothLogWrite();
    void BuildFullZoneUpdate();
    void CheckInvalidPositionOrMove();
    void CollisionStateChanged();
    void CollisionStateChangedLocal();
    void EnableCollision();
    void FallLogWrite();
    void FallTime();
    void ForceJump();
    void ForceStopMove();
    void ForceStopStrafe();
    void GetCurrentPitchRate();
    void GetCurrentSpeed();
    void GetCurrentTurnRate();
    void GetDiagonalDirection();
    void GetDiagonalDirection2d();
    void GetDirection();
    void GetHandoffData();
    void GetMoveEventMsgId();
    void GetMoveStatus();
    void GetMovingDirection();
    void GetMovingDirection2d();
    void GetStrafingDirection();
    void GetStrafingDirection2d();
    void GetUpdateInfo();
    void Halt();
    void IsFallLoggingOn();
    void IsLoggingOn();
    void Jump();
    void LogUpdateInfo();
    void LogWrite();
    void MoveLocalPlayer();
    void MoveUnit();
    void MoveUnits();
    void MoversOnList();
    void OnCollideRedirServer();
    void OnDisableGravity();
    void OnEnableGravity();
    void OnFall();
    void OnFallLocal();
    void OnJump();
    void OnJumpLocal();
    void OnMoveStart();
    void OnMoveStartLocal();
    void OnMoveStop();
    void OnMoveStopLocal();
    void OnPitchStart();
    void OnPitchStartLocal();
    void OnPitchStop();
    void OnPitchStopLocal();
    void OnRunSpeedChange();
    void OnSetFacing();
    void OnSetFacingLocal();
    void OnSetPitch();
    void OnSetPitchLocal();
    void OnSetRawFacingLocal();
    void OnSetRunMode();
    void OnSetRunModeLocal();
    void OnSpline();
    void OnSplineDoneFace();
    void OnStrafeStart();
    void OnStrafeStartLocal();
    void OnStrafeStop();
    void OnStrafeStopLocal();
    void OnStuckServer();
    void OnSwimSpeedChange();
    void OnSwimStart();
    void OnSwimStartLocal();
    void OnSwimStop();
    void OnSwimStopLocal();
    void OnTeleport();
    void OnTeleportLocal();
    void OnTurnRateChange();
    void OnTurnStart();
    void OnTurnStartLocal();
    void OnTurnStop();
    void OnTurnStopLocal();
    void OnWalkSpeedChange();
    void PlotDiagonalCircularPosition();
    void PlotDiagonalLinearPosition();
    void PlotDiagonalPitchingCircularPosition();
    void PlotDiagonalSpiralPosition();
    void PlotHorzCircularPosition();
    void PlotLinearPosition();
    void PlotNormalCircularPosition();
    void PlotNormalLinearPosition();
    void PlotNormalPitchingCircularPosition();
    void PlotNormalSpiralPosition();
    void PlotSpiralPosition();
    void PlotStrafeCircularPosition();
    void PlotStrafeLinearPosition();
    void PlotUnitMovement();
    void PlotUnitPitch();
    void PlotUnitRotation();
    void PlotUnitSplineMovement();
    void PlotVertCircularPosition();
    void PutHandoffData();
    void RestoreMoveState();
    void SaveMoveState();
    void SetCollisionBox();
    void SetFacing();
    void SetIdleUpdates();
    void SetPitch();
    void SetRunMode();
    void SetServerInitData();
    void SetUpdateInfo();
    void SimpleRequestMove();
    void SkipFullZoneUpdate();
    void SkipHandoffData();
    void StartFallLogging();
    void StartLogging();
    void StartMove();
    void StartPitch();
    void StartStrafe();
    void StartSwim();
    void StartSwimLocal();
    void StartTurn();
    void StopAllLogging();
    void StopFallLogging();
    void StopLogging();
    void StopMove();
    void StopPitch();
    void StopStrafe();
    void StopSwim();
    void StopSwimLocal();
    void StopTurn();
    void Teleport();
    void ToggleCollision();
    void ToggleFallLogging();
    void ToggleLogging();
    void UnpackFullZoneUpdate();
    void UpdateAnchors();
    void UpdateCurrentSpeed();
    void UpdatePlayerMovement();
    void UpdateStatus();
    void UpdateStatusInternal();
    void UpdateStatusLocal();
    void UpdateTransportStatus();
};

class CMovementData {
public:
    CMovementData();
    void CalcDirection();
    void ForceSetTransport();
    void GetFacing();
    void GetPosition();
    void IsLocalPlayer();
    void RemoveFromMoversList();
    void RemoveSpline();
    void SetTransport();
    ~CMovementData();

    // State consumed by the unit movement-update serializer.  These fields
    // are intentionally kept on the movement object so the serializer does
    // not need to reach into unrelated unit implementation details.
    std::uint64_t m_transportGUID = 0;
    NTempest::C3Vector m_position{};
    float m_facing = 0.0f;
    float m_pitch = 0.0f;
    std::uint32_t m_moveFlags = 0;
};

// Recovered symbol declarations for movement.cpp
