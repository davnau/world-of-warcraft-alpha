#include "ENGINE/Source/Services/lightning.h"

#include <algorithm>
#include <cmath>
#include <random>
#include <stdexcept>

CLightning::CLightning()=default;
void CLightning::BuildStroke(std::vector<NTempest::C3Vector>& points){
    const auto diff=mDstPos-mSrcPos;const float length=std::sqrt(diff.SquaredMag());const unsigned segments=std::max(2U,static_cast<unsigned>(length/std::max(0.001F,std::abs(mAvgSegLen))+2.0F));
    points.resize(segments+1);points.front()=mSrcPos;points.back()=mDstPos;std::minstd_rand random(static_cast<unsigned>(mAccTime*1000.0F)+segments);std::uniform_real_distribution<float> noise(-1.0F,1.0F);
    for(unsigned i=1;i<segments;++i){const float t=static_cast<float>(i)/segments;points[i]=mSrcPos+diff*t+NTempest::C3Vector{noise(random),noise(random),noise(random)}*(length*mNoiseScale);}
}
void CLightning::Update(float elapsed){
    mAccTime=mTexCoordScale==0.0F?0.0F:std::fmod(mAccTime+elapsed,std::abs(mTexCoordScale));if(mRebuildPoints){BuildStroke(mPoints);mRebuildPoints=0;}
    std::vector<NTempest::C3Vector> fresh;BuildStroke(fresh);for(unsigned i=1;i+1<mPoints.size()&&i<fresh.size();++i)mPoints[i]=mPoints[i]*0.75F+fresh[i]*0.25F;
    mPos.resize(mPoints.size()*2);mTexCoords.resize(mPos.size());mIndices.resize(mPos.size());for(unsigned i=0;i<mPos.size();++i){mIndices[i]=static_cast<unsigned short>(i);mTexCoords[i]={static_cast<float>(i/2)/std::max(1U,static_cast<unsigned>(mPoints.size()-1)),static_cast<float>(i&1)};}
}
void CLightning::Render(unsigned,const NTempest::C3Vector* camera){if(!camera||mPoints.size()<2)return;for(unsigned i=0;i<mPoints.size();++i){auto direction=(i+1<mPoints.size()?mPoints[i+1]-mPoints[i]:mPoints[i]-mPoints[i-1]);auto view=*camera-mPoints[i];auto side=NTempest::C3Vector::Cross(direction,view);side.Normalize();side*=mWidth*0.5F;mPos[i*2]=mPoints[i]-side;mPos[i*2+1]=mPoints[i]+side;}}
void CLightning::SetTexture(HTEXTURE__* texture){mTexture=texture;}
CLightning& CLightningManager::Bolt(unsigned id)const{if(id>=mLiveBolts.size()||!mLiveBolts[id])throw std::out_of_range("lightning bolt");return *mLiveBolts[id];}
unsigned CLightningManager::Add(const NTempest::C3Vector*s,const NTempest::C3Vector*d,float seg,float width,NTempest::CImVector color,float noise,float tex,float duration,HTEXTURE__*texture,LIGHTNING_COORD_UPDATE update,void*context){if(!s||!d)throw std::invalid_argument("lightning coordinates");unsigned id;if(mDeadBolts.empty()){id=static_cast<unsigned>(mLiveBolts.size());mLiveBolts.push_back(std::make_unique<CLightning>());}else{id=static_cast<unsigned>(mDeadBolts.back());mDeadBolts.pop_back();mLiveBolts[id]=std::make_unique<CLightning>();}auto&b=Bolt(id);b.mSrcPos=*s;b.mDstPos=*d;b.mAvgSegLen=seg;b.mWidth=width;b.mColor=color;b.mNoiseScale=noise;b.mTexCoordScale=tex;b.mDuration=duration;b.SetTexture(texture);b.mCoordUpdateData={update,context};return id;}
void CLightningManager::Move(unsigned id,NTempest::C3Vector*s,NTempest::C3Vector*d){if(!s||!d)throw std::invalid_argument("lightning move");auto&b=Bolt(id);b.mSrcPos=*s;b.mDstPos=*d;b.mRebuildPoints=1;}
void CLightningManager::SetCoordUpdate(unsigned id,LIGHTNING_COORD_UPDATE f,void*c){Bolt(id).mCoordUpdateData={f,c};}
void CLightningManager::SetColor(unsigned id,NTempest::CImVector c){Bolt(id).mColor=c;}void CLightningManager::GetColor(unsigned id,NTempest::CImVector*c)const{if(!c)throw std::invalid_argument("lightning color");*c=Bolt(id).mColor;}
double CLightningManager::GetDuration(unsigned id)const{return Bolt(id).mDuration;}
void CLightningManager::Update(float elapsed){for(unsigned id=0;id<mLiveBolts.size();++id)if(mLiveBolts[id]){auto&b=*mLiveBolts[id];if(b.mCoordUpdateData.callback)b.mCoordUpdateData.callback(b.mCoordUpdateData.context,id,&b.mSrcPos,&b.mDstPos);b.Update(elapsed);if(b.mDuration>0.0F&&(b.mDuration-=elapsed)<=0.0F)Remove(id);}}
void CLightningManager::Render(const NTempest::C3Vector*p){for(unsigned id=0;id<mLiveBolts.size();++id)if(mLiveBolts[id])mLiveBolts[id]->Render(id,p);}
void CLightningManager::Remove(unsigned id){Bolt(id);mLiveBolts[id].reset();mDeadBolts.push_back(static_cast<int>(id));}
