#pragma once

#include "ENGINE/Source/Tempest/c33matrix.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/cimvector.h"

#include <cstdint>
#include <vector>

struct C3Color { float r = 0.0F, g = 0.0F, b = 0.0F; };
struct UpdateInfo { void (__fastcall* updateFcn)(float, void*, void*) = nullptr; void* updateData = nullptr; float updatePriority = 0.0F; };

class CBaseManaged {
public:
    enum Flags : std::uint8_t { ALWAYSUPDATE = 1, READONLY = 2, REQUIRESUPDATE = 4, UPDATED = 8 };
    enum ManagedTypeIds : std::uint8_t { UNKNOWN, ALPHACOLOR, COLOR, COORD, C33MATRIX, INT, FLOAT, DATATYPEIDS };

    virtual ~CBaseManaged() = default;
    virtual void Update(float elapsedSec) = 0;
    virtual void UpdateR(float elapsedSec) = 0;
    virtual void SetData(const void* data) = 0;
    void SetUpdate(void (__fastcall* function)(float, void*, void*), void* data, float priority) { m_updateFcn = function; m_updateData = data; m_updatePriority = priority; }
    std::uint8_t m_dataTypeId = UNKNOWN;
    std::uint8_t m_flags = 0;
    void (__fastcall* m_updateFcn)(float, void*, void*) = nullptr;
    void* m_updateData = nullptr;
    float m_updatePriority = 0.0F;
};

template<class T> class TManaged : public CBaseManaged {
public:
    T m_data{};
    void Update(float elapsedSec) override { if (m_updateFcn) m_updateFcn(elapsedSec, m_updateData, &m_data); m_flags = static_cast<std::uint8_t>((m_flags | UPDATED) & ~REQUIRESUPDATE); }
    void UpdateR(float elapsedSec) override { Update(elapsedSec); }
    void SetData(const void* data) override { if (data) m_data = *static_cast<const T*>(data); m_flags = static_cast<std::uint8_t>((m_flags | UPDATED) & ~REQUIRESUPDATE); }
};

class CAngle : public TManaged<float> {
public:
    explicit CAngle(float angle = 0.0F) { Set_(angle); }
    float m_cos = 1.0F;
    float m_sin = 0.0F;
    static double ClampTo2Pi(float angle);
    void Set_(float angle);
};

class CDataMgr {
public:
    explicit CDataMgr(unsigned fieldCount = 0) : m_managedArray(fieldCount, nullptr) {}
    void Resize(unsigned fieldCount) { m_managedArray.resize(fieldCount, nullptr); }
    void AddManaged(CBaseManaged*, unsigned fieldId, std::uint8_t flags, unsigned dataTypeId);
    void AddManaged(TManaged<NTempest::CImVector>*, unsigned, std::uint8_t);
    void AddManaged(TManaged<C3Color>*, unsigned, std::uint8_t);
    void AddManaged(TManaged<NTempest::C3Vector>*, unsigned, std::uint8_t);
    void AddManaged(TManaged<NTempest::C33Matrix>*, unsigned, std::uint8_t);
    void AddManaged(TManaged<int>*, unsigned, std::uint8_t);
    void AddManaged(TManaged<float>*, unsigned, std::uint8_t);
    void LinkManaged(CBaseManaged* managed);
    void Update(float elapsedSec);

    std::vector<CBaseManaged*> m_managedArray;
    std::vector<CBaseManaged*> m_updateList;
};

int DataMgrGetBool(CDataMgr*, unsigned fieldId);
void DataMgrGetColor(CDataMgr*, unsigned, NTempest::CImVector*);
void DataMgrGetColor(CDataMgr*, unsigned, C3Color*);
void DataMgrGetCoord(CDataMgr*, unsigned, NTempest::C3Vector*);
void DataMgrGetC33Matrix(CDataMgr*, unsigned, NTempest::C33Matrix*);
int DataMgrGetInt(CDataMgr*, unsigned);
double DataMgrGetFloat(CDataMgr*, unsigned);
void DataMgrGetUpdateInfo(CDataMgr*, unsigned, UpdateInfo*);
void DataMgrSetBool(CDataMgr*, unsigned, int);
void DataMgrSetColor(CDataMgr*, unsigned, const NTempest::CImVector*);
void DataMgrSetColor(CDataMgr*, unsigned, const C3Color*);
void DataMgrSetCoord(CDataMgr*, unsigned, const NTempest::C3Vector*, char coordFlags);
void DataMgrSetC33Matrix(CDataMgr*, unsigned, const NTempest::C33Matrix*);
void DataMgrSetInt(CDataMgr*, unsigned, int);
void DataMgrSetFloat(CDataMgr*, unsigned, float);
void DataMgrSetFieldUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, void*), void*, float, unsigned typeId);
void DataMgrSetBoolUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, int*), void*, float);
void DataMgrSetColorUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, NTempest::CImVector*), void*, float);
void DataMgrSetColorUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, C3Color*), void*, float);
void DataMgrSetCoordUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, NTempest::C3Vector*), void*, float);
void DataMgrSetC33MatrixUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, NTempest::C33Matrix*), void*, float);
void DataMgrSetIntUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, int*), void*, float);
void DataMgrSetFloatUpdate(CDataMgr*, unsigned, void (__fastcall*)(float, void*, float*), void*, float);
