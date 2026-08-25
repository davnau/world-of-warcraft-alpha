#pragma once

#include "ENGINE/Source/Services/datamgr.h"
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/crect.h"

class CCamera : public CDataMgr {
public:
    CCamera();
    CCamera(const CCamera&);
    void SetupWorldProjection(const NTempest::CRect* projectionRect, unsigned flags);

    TManaged<NTempest::C3Vector> m_position;
    TManaged<NTempest::C3Vector> m_target;
    TManaged<float> m_distance;
    TManaged<float> m_zFar;
    TManaged<float> m_zNear;
    CAngle m_aoa;
    CAngle m_fov;
    CAngle m_roll;
    CAngle m_rotation;
    NTempest::C44Matrix m_projection;
    NTempest::C44Matrix m_view;
};

using HCAMERA = CCamera*;

void CameraCalcPosFromTarg(HCAMERA camera, NTempest::C3Vector* position);
void CameraCalcTargFromPos(HCAMERA camera, NTempest::C3Vector* target);
HCAMERA CameraCreate();
HCAMERA CameraDuplicate(HCAMERA source);
void CameraDestroy(HCAMERA camera);
void CameraGetLineSegment(NTempest::C3Vector* a, NTempest::C3Vector* b, float x, float y);
void CameraSetupScreenProjection(const NTempest::CRect* screenRect, const NTempest::C2Vector* screenPoint, float depth);
void CameraSetupWorldProjection(HCAMERA camera, const NTempest::CRect* projectionRect, unsigned flags);
void CameraUpdate(HCAMERA camera, float elapsedSec);
