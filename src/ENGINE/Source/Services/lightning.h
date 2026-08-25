#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/cimvector.h"

#include <memory>
#include <vector>

class CTexture;using HTEXTURE__=CTexture;
using LIGHTNING_COORD_UPDATE = void (__fastcall*)(void*, unsigned, NTempest::C3Vector*, NTempest::C3Vector*);
struct LightningCoordUpdateData { LIGHTNING_COORD_UPDATE callback=nullptr; void* context=nullptr; };

class CLightning {
public:
    CLightning();
    void BuildStroke(std::vector<NTempest::C3Vector>& points);
    void Update(float elapsed);
    void Render(unsigned boltId,const NTempest::C3Vector* cameraPos);
    void SetTexture(HTEXTURE__* texture);

    NTempest::C3Vector mSrcPos{},mDstPos{};float mAvgSegLen=-2.0F,mWidth=1.0F;NTempest::CImVector mColor{};
    float mNoiseScale=0.0F,mTexCoordScale=0.0F,mDuration=0.0F;int mRebuildPoints=1;
    std::vector<NTempest::C3Vector> mPoints,mPos;std::vector<NTempest::C2Vector> mTexCoords;std::vector<unsigned short> mIndices;
    float mAccTime=0.0F;HTEXTURE__* mTexture=nullptr;LightningCoordUpdateData mCoordUpdateData{};
};

class CLightningManager {
public:
    unsigned Add(const NTempest::C3Vector* source,const NTempest::C3Vector* dest,float avgSegLen,float width,NTempest::CImVector color,float noiseScale,float texCoordScale,float duration,HTEXTURE__* texture,LIGHTNING_COORD_UPDATE updateproc,void* context);
    void Move(unsigned boltId,NTempest::C3Vector* src,NTempest::C3Vector* dst);
    void SetCoordUpdate(unsigned boltId,LIGHTNING_COORD_UPDATE,void*);
    void SetColor(unsigned boltId,NTempest::CImVector);void GetColor(unsigned boltId,NTempest::CImVector*) const;
    double GetDuration(unsigned boltId) const;void Update(float elapsed);void Render(const NTempest::C3Vector* cameraPos);void Remove(unsigned boltId);
    std::vector<std::unique_ptr<CLightning>> mLiveBolts;std::vector<int> mDeadBolts;
private: CLightning& Bolt(unsigned id) const;
};
