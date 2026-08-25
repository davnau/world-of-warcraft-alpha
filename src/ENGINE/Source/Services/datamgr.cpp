#include "ENGINE/Source/Services/datamgr.h"

#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace {
constexpr float TWO_PI = 6.2831853071795864769F;

CBaseManaged& Field(CDataMgr* mgr, unsigned id, unsigned type) {
    if (!mgr || id >= mgr->m_managedArray.size() || !mgr->m_managedArray[id]) throw std::out_of_range("managed field");
    CBaseManaged& result = *mgr->m_managedArray[id];
    if (result.m_dataTypeId != type) throw std::invalid_argument("managed field type");
    if (result.m_flags & CBaseManaged::REQUIRESUPDATE) result.Update(0.0F);
    return result;
}

template<class T> T& Value(CDataMgr* mgr, unsigned id, unsigned type) { return static_cast<TManaged<T>&>(Field(mgr, id, type)).m_data; }
template<class T> void Set(CDataMgr* mgr, unsigned id, unsigned type, const T& value) {
    CBaseManaged& managed = Field(mgr, id, type);
    if (managed.m_flags & CBaseManaged::READONLY) throw std::logic_error("managed field is read-only");
    managed.SetData(&value);
}
template<class F> void SetUpdate(CDataMgr* mgr, unsigned id, F function, void* data, float priority, unsigned type) {
    DataMgrSetFieldUpdate(mgr, id, reinterpret_cast<void (__fastcall*)(float, void*, void*)>(function), data, priority, type);
}
}

double CAngle::ClampTo2Pi(float angle) { float result = std::fmod(angle, TWO_PI); if (result < 0.0F) result += TWO_PI; return result; }

void CDataMgr::AddManaged(CBaseManaged* managed, unsigned fieldId, std::uint8_t flags, unsigned type) {
    if (!managed || fieldId >= m_managedArray.size() || type >= CBaseManaged::DATATYPEIDS) throw std::out_of_range("AddManaged");
    managed->m_flags = flags;
    managed->m_dataTypeId = static_cast<std::uint8_t>(type);
    m_managedArray[fieldId] = managed;
    if (flags & CBaseManaged::ALWAYSUPDATE) LinkManaged(managed);
}
#define ADD_MANAGED(TYPE, ID) void CDataMgr::AddManaged(TManaged<TYPE>* m,unsigned f,std::uint8_t flags){AddManaged(static_cast<CBaseManaged*>(m),f,flags,ID);}
ADD_MANAGED(NTempest::CImVector, CBaseManaged::ALPHACOLOR)
ADD_MANAGED(C3Color, CBaseManaged::COLOR)
ADD_MANAGED(NTempest::C3Vector, CBaseManaged::COORD)
ADD_MANAGED(NTempest::C33Matrix, CBaseManaged::C33MATRIX)
ADD_MANAGED(int, CBaseManaged::INT)
ADD_MANAGED(float, CBaseManaged::FLOAT)
#undef ADD_MANAGED

void CDataMgr::LinkManaged(CBaseManaged* managed) {
    m_updateList.erase(std::remove(m_updateList.begin(), m_updateList.end(), managed), m_updateList.end());
    const auto where = std::find_if(m_updateList.begin(), m_updateList.end(), [managed](const auto* item) { return item->m_updatePriority >= managed->m_updatePriority; });
    m_updateList.insert(where, managed);
}
void CDataMgr::Update(float elapsedSec) { for (auto* managed : m_updateList) (managed->m_flags & CBaseManaged::READONLY ? managed->UpdateR(elapsedSec) : managed->Update(elapsedSec)); }

int DataMgrGetBool(CDataMgr* m,unsigned f){return Value<int>(m,f,CBaseManaged::INT)!=0;}
void DataMgrGetColor(CDataMgr* m,unsigned f,NTempest::CImVector* v){if(!v)throw std::invalid_argument("color");*v=Value<NTempest::CImVector>(m,f,CBaseManaged::ALPHACOLOR);}
void DataMgrGetColor(CDataMgr* m,unsigned f,C3Color* v){if(!v)throw std::invalid_argument("color");*v=Value<C3Color>(m,f,CBaseManaged::COLOR);}
void DataMgrGetCoord(CDataMgr* m,unsigned f,NTempest::C3Vector* v){if(!v)throw std::invalid_argument("coord");*v=Value<NTempest::C3Vector>(m,f,CBaseManaged::COORD);}
void DataMgrGetC33Matrix(CDataMgr* m,unsigned f,NTempest::C33Matrix* v){if(!v)throw std::invalid_argument("matrix");*v=Value<NTempest::C33Matrix>(m,f,CBaseManaged::C33MATRIX);}
int DataMgrGetInt(CDataMgr* m,unsigned f){return Value<int>(m,f,CBaseManaged::INT);}
double DataMgrGetFloat(CDataMgr* m,unsigned f){return Value<float>(m,f,CBaseManaged::FLOAT);}
void DataMgrGetUpdateInfo(CDataMgr* m,unsigned f,UpdateInfo* i){if(!i)throw std::invalid_argument("update info");auto& x=Field(m,f,m->m_managedArray[f]->m_dataTypeId);*i={x.m_updateFcn,x.m_updateData,x.m_updatePriority};}
void DataMgrSetBool(CDataMgr* m,unsigned f,int v){Set(m,f,CBaseManaged::INT,v);}
void DataMgrSetColor(CDataMgr* m,unsigned f,const NTempest::CImVector* v){if(!v)throw std::invalid_argument("color");Set(m,f,CBaseManaged::ALPHACOLOR,*v);}
void DataMgrSetColor(CDataMgr* m,unsigned f,const C3Color* v){if(!v)throw std::invalid_argument("color");Set(m,f,CBaseManaged::COLOR,*v);}
void DataMgrSetCoord(CDataMgr* m,unsigned f,const NTempest::C3Vector* v,char){if(!v)throw std::invalid_argument("coord");Set(m,f,CBaseManaged::COORD,*v);}
void DataMgrSetC33Matrix(CDataMgr* m,unsigned f,const NTempest::C33Matrix* v){if(!v)throw std::invalid_argument("matrix");Set(m,f,CBaseManaged::C33MATRIX,*v);}
void DataMgrSetInt(CDataMgr* m,unsigned f,int v){Set(m,f,CBaseManaged::INT,v);}
void DataMgrSetFloat(CDataMgr* m,unsigned f,float v){Set(m,f,CBaseManaged::FLOAT,v);}

void DataMgrSetFieldUpdate(CDataMgr* m,unsigned f,void (__fastcall* fn)(float,void*,void*),void* data,float priority,unsigned type){auto& x=Field(m,f,type);x.m_updatePriority=priority;x.m_updateFcn=fn;x.m_updateData=data;x.m_flags|=CBaseManaged::REQUIRESUPDATE;if((x.m_flags&CBaseManaged::ALWAYSUPDATE)||fn)m->LinkManaged(&x);}
void DataMgrSetBoolUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,int*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::INT);}
void DataMgrSetColorUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,NTempest::CImVector*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::ALPHACOLOR);}
void DataMgrSetColorUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,C3Color*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::COLOR);}
void DataMgrSetCoordUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,NTempest::C3Vector*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::COORD);}
void DataMgrSetC33MatrixUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,NTempest::C33Matrix*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::C33MATRIX);}
void DataMgrSetIntUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,int*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::INT);}
void DataMgrSetFloatUpdate(CDataMgr*m,unsigned f,void(__fastcall*fn)(float,void*,float*),void*d,float p){SetUpdate(m,f,fn,d,p,CBaseManaged::FLOAT);}
