#include "ENGINE/Source/Services/camera.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"

#include <cmath>
#include <stdexcept>

namespace {
constexpr float DEFAULT_DIST = 100.0F;
constexpr float DEFAULT_FARZ = 1000.0F;
constexpr float DEFAULT_NEARZ = 1.0F;
constexpr float DEFAULT_FOV = 1.5707963267948966F;
CCamera* s_activeCamera = nullptr;

NTempest::C3Vector Forward(const CCamera& camera) {
    const float cp = camera.m_aoa.m_cos;
    return {-camera.m_rotation.m_cos * cp, -camera.m_rotation.m_sin * cp, camera.m_aoa.m_sin};
}

NTempest::C3Vector Cross(const NTempest::C3Vector& a, const NTempest::C3Vector& b) { return NTempest::C3Vector::Cross(a, b); }
}

void CAngle::Set_(float angle) { m_data = static_cast<float>(ClampTo2Pi(angle)); m_cos = std::cos(m_data); m_sin = std::sin(m_data); }

CCamera::CCamera() : CDataMgr(9), m_position(), m_target(), m_distance(), m_zFar(), m_zNear(), m_aoa(0.0F), m_fov(DEFAULT_FOV), m_roll(0.0F), m_rotation(0.0F) {
    m_position.m_data = {100.0F, 0.0F, 0.0F}; m_target.m_data = {0.0F, 0.0F, 0.0F};
    m_distance.m_data = DEFAULT_DIST; m_zFar.m_data = DEFAULT_FARZ; m_zNear.m_data = DEFAULT_NEARZ;
    AddManaged(&m_aoa,0,0); AddManaged(&m_distance,1,0); AddManaged(&m_zFar,2,0); AddManaged(&m_zNear,3,0); AddManaged(&m_fov,4,0); AddManaged(&m_roll,5,0); AddManaged(&m_rotation,6,0); AddManaged(&m_position,7,0); AddManaged(&m_target,8,0);
}

CCamera::CCamera(const CCamera& source) : CCamera() {
    m_position.m_data=source.m_position.m_data;m_target.m_data=source.m_target.m_data;m_distance.m_data=source.m_distance.m_data;m_zFar.m_data=source.m_zFar.m_data;m_zNear.m_data=source.m_zNear.m_data;
    m_aoa.Set_(source.m_aoa.m_data);m_fov.Set_(source.m_fov.m_data);m_roll.Set_(source.m_roll.m_data);m_rotation.Set_(source.m_rotation.m_data);
}

void CCamera::SetupWorldProjection(const NTempest::CRect* rect, unsigned) {
    if (!rect) throw std::invalid_argument("projection rectangle");
    const float width = rect->r - rect->l, height = rect->b - rect->t;
    if (width == 0.0F || height == 0.0F) throw std::invalid_argument("empty projection rectangle");
    // Recovered CCamera::SetupWorldProjection passes the reciprocal screen
    // aspect to GxuXformCreateProjection.
    const float aspect = height / width;
    const float tanHalfFov = std::tan((m_fov.m_data / std::sqrt(aspect * aspect + 1.0F)) * 0.5F);
    const float x = 1.0F / (aspect * tanHalfFov), y = 1.0F / tanHalfFov;
    const float range = m_zFar.m_data - m_zNear.m_data;
    m_projection = {x,0,0,0, 0,y,0,0, 0,0,(m_zNear.m_data+m_zFar.m_data)/range,1,
        0,0,-2.0F*m_zNear.m_data*m_zFar.m_data/range,0};
    auto forward = m_target.m_data - m_position.m_data; forward.Normalize();
    NTempest::C3Vector up{m_rotation.m_sin*m_roll.m_sin,-m_rotation.m_cos*m_roll.m_sin,m_roll.m_cos};
    auto right = Cross(up,forward); right.Normalize(); up = Cross(forward,right);
    m_view = {right.x,up.x,forward.x,0, right.y,up.y,forward.y,0, right.z,up.z,forward.z,0,
        -NTempest::C3Vector::Dot(right,m_position.m_data),-NTempest::C3Vector::Dot(up,m_position.m_data),-NTempest::C3Vector::Dot(forward,m_position.m_data),1};
    s_activeCamera = this;
}

void CameraCalcPosFromTarg(HCAMERA camera,NTempest::C3Vector* position){if(!camera||!position)throw std::invalid_argument("camera position");*position=camera->m_target.m_data-Forward(*camera)*camera->m_distance.m_data;}
void CameraCalcTargFromPos(HCAMERA camera,NTempest::C3Vector* target){if(!camera||!target)throw std::invalid_argument("camera target");*target=camera->m_position.m_data+Forward(*camera)*camera->m_distance.m_data;}
HCAMERA CameraCreate(){return new CCamera;}
HCAMERA CameraDuplicate(HCAMERA source){if(!source)throw std::invalid_argument("source camera");return new CCamera(*source);}
void CameraDestroy(HCAMERA camera){if(s_activeCamera==camera)s_activeCamera=nullptr;delete camera;}

void CameraGetLineSegment(NTempest::C3Vector* a,NTempest::C3Vector* b,float x,float y){
    if(!a||!b||x<0.0F||x>1.0F||y<0.0F||y>1.0F)throw std::invalid_argument("screen line segment");
    if(!s_activeCamera){*a={};*b={};return;}
    auto forward=s_activeCamera->m_target.m_data-s_activeCamera->m_position.m_data;forward.Normalize();
    NTempest::C3Vector worldUp{0,0,1};auto right=Cross(worldUp,forward);right.Normalize();auto up=Cross(forward,right);
    const float nx=2.0F*x-1.0F,ny=1.0F-2.0F*y,tanHalf=std::tan(s_activeCamera->m_fov.m_data*0.5F);
    auto Ray=[&](float depth){return s_activeCamera->m_position.m_data+forward*depth+right*(nx*depth*tanHalf)+up*(ny*depth*tanHalf);};
    *a=Ray(s_activeCamera->m_zNear.m_data);*b=Ray(s_activeCamera->m_zFar.m_data);
}

void CameraSetupScreenProjection(const NTempest::CRect* rect,const NTempest::C2Vector* point,float depth){
    if(!rect||!point)throw std::invalid_argument("screen projection");
    const float width=rect->r-rect->l,height=rect->b-rect->t;
    if(width==0.0F||height==0.0F)throw std::invalid_argument("empty screen projection");
    const float zExtent=depth>0.0F?depth:500.0F;
    const NTempest::C44Matrix projection{
        2.0F/width,0,0,0,
        0,2.0F/height,0,0,
        0,0,-1.0F/zExtent,0,
        -(rect->r+rect->l)/width,-(rect->b+rect->t)/height,0,1};
    NTempest::C44Matrix view;
    view.Translate(NTempest::C3Vector(point->x-rect->l,point->y-rect->t,0.0F));
    GxXformSetProjection(projection);
    GxXformSetView(view);
}
void CameraSetupWorldProjection(HCAMERA camera,const NTempest::CRect* rect,unsigned flags){if(!camera)throw std::invalid_argument("camera");camera->SetupWorldProjection(rect,flags);}
void CameraUpdate(HCAMERA camera,float elapsedSec){if(!camera)throw std::invalid_argument("camera");camera->Update(elapsedSec);}
