// Reconstructed from D:\build\buildWoW\Storm\Source\SRgn.cpp.
// Handle tags remain opaque at the public boundary; RGN retains its 108-byte
// PDB layout in Storm/h/sclass.h.

#include "Storm/h/sclass.h"

#include <algorithm>
#include <cstring>
#include <limits>

namespace {
RGN* Region(void* handle) { return reinterpret_cast<RGN*>(handle); }
const RGN* Region(const void* handle) { return reinterpret_cast<const RGN*>(handle); }
bool Valid(const RECTF& rect) { return rect.left < rect.right && rect.bottom < rect.top; }
bool Intersects(const RECTF& a, const RECTF& b) { return a.left < b.right && a.bottom < b.top && a.right > b.left && a.top > b.bottom; }
RECTF Intersection(const RECTF& a, const RECTF& b) { return {std::max(a.left,b.left),std::max(a.bottom,b.bottom),std::min(a.right,b.right),std::min(a.top,b.top)}; }
RECTF ToFloat(const RECT& rect) { return {static_cast<float>(rect.left),static_cast<float>(std::min(rect.top,rect.bottom)),static_cast<float>(rect.right),static_cast<float>(std::max(rect.top,rect.bottom))}; }
void Rebuild(RGN* region) { region->combined.SetCount(0); for(unsigned int i=0;i<region->source.Count();++i) if(!(region->source[i].flags&0x10000U)&&Valid(region->source[i].rect)) region->combined.Add(region->source[i].rect); region->dirty=0; }
void Add(RGN* region,const RECTF& rect,void* param,unsigned int flags=0){if(!Valid(rect))return;region->source.Add({rect,param,++region->sequence,flags});region->dirty=1;}
}

void __stdcall SRgnCreate(HSRGN__** handle, unsigned int reserved)
{
    if (!handle || reserved) { SErrSetLastError(ERROR_INVALID_PARAMETER); return; }
    auto* region = new RGN; std::memset(region->m_hashObject, 0, sizeof(region->m_hashObject)); region->sequence=0; region->dirty=0; *handle=reinterpret_cast<HSRGN__*>(region);
}

void __stdcall SRgnDelete(HSRGN__* handle) { delete Region(handle); }
void __stdcall SRgnDestroy() {}
void __stdcall SRgnClear(HLOCKEDRGN__* handle) { if(auto* r=Region(handle)){r->source.SetCount(0);r->combined.SetCount(0);r->foundparams.SetCount(0);r->sequence=0;r->dirty=0;} }

void __stdcall SRgnDuplicate(HLOCKEDRGN__* originalHandle, HSRGN__** handle, unsigned int reserved)
{
    if(!handle||reserved){SErrSetLastError(ERROR_INVALID_PARAMETER);return;} const RGN* source=Region(originalHandle); if(!source){*handle=nullptr;return;} auto* copy=new RGN; std::memset(copy->m_hashObject,0,sizeof(copy->m_hashObject)); copy->source=source->source; copy->combined=source->combined; copy->foundparams=source->foundparams; copy->foundparamsrect=source->foundparamsrect; copy->sequence=source->sequence; copy->dirty=source->dirty; *handle=reinterpret_cast<HSRGN__*>(copy);
}

void __stdcall SRgnCombineRectf(HLOCKEDRGN__* handle, const RECTF* rect, void* param, int combineMode)
{
    RGN* region=Region(handle); if(!region||!rect||combineMode<1||combineMode>6){SErrSetLastError(ERROR_INVALID_PARAMETER);return;}
    if(combineMode==2||combineMode==6){Add(region,*rect,param,combineMode==6?0x10000U:0);return;}
    if(combineMode==5){SRgnClear(handle);Add(region,*rect,param);return;}
    bool any=false; TSGrowableArray<RGN_SOURCE> replacement;
    for(unsigned int i=0;i<region->source.Count();++i){RGN_SOURCE item=region->source[i];const bool hit=Intersects(item.rect,*rect);any|=hit;if(combineMode==1){if(hit){item.rect=Intersection(item.rect,*rect);replacement.Add(item);}}else if(combineMode==3){if(!hit)replacement.Add(item);}else if(combineMode==4){if(!hit)replacement.Add(item);}}
    region->source=replacement; if(combineMode==4&&!any)Add(region,*rect,param); region->dirty=1;
}

void __stdcall SRgnCombineRecti(HSRGN__* handle,const RECT* rect,void* param,int mode){if(!rect)return;const RECTF converted=ToFloat(*rect);SRgnCombineRectf(reinterpret_cast<HLOCKEDRGN__*>(handle),&converted,param,mode);}

void __stdcall SRgnGetBoundingRectf(HLOCKEDRGN__* handle, RECTF* rect)
{
    if(!rect)return;RGN* region=Region(handle);if(!region||!region->source.Count()){*rect={0,0,0,0};return;}rect->left=rect->bottom=std::numeric_limits<float>::max();rect->right=rect->top=-std::numeric_limits<float>::max();for(unsigned int i=0;i<region->source.Count();++i){const RECTF& r=region->source[i].rect;rect->left=std::min(rect->left,r.left);rect->bottom=std::min(rect->bottom,r.bottom);rect->right=std::max(rect->right,r.right);rect->top=std::max(rect->top,r.top);}
}

void __stdcall SRgnGetBoundingRecti(HSRGN__* handle,RECT* rect){if(!rect)return;RECTF value{};SRgnGetBoundingRectf(reinterpret_cast<HLOCKEDRGN__*>(handle),&value);rect->left=static_cast<LONG>(value.left);rect->top=static_cast<LONG>(value.bottom);rect->right=static_cast<LONG>(value.right);rect->bottom=static_cast<LONG>(value.top);}

void __stdcall SRgnGetRectsf(HLOCKEDRGN__* handle,unsigned int* numRects,RECTF* buffer){if(!numRects)return;RGN* region=Region(handle);if(!region){*numRects=0;return;}if(region->dirty)Rebuild(region);if(!buffer){*numRects=region->combined.Count();return;}const unsigned int count=std::min(*numRects,region->combined.Count());for(unsigned int i=0;i<count;++i)buffer[i]=region->combined[i];*numRects=count;}
void __stdcall SRgnGetRectsi(HSRGN__* handle,unsigned int* numRects,RECT* buffer){if(!numRects)return;unsigned int available=0;SRgnGetRectsf(reinterpret_cast<HLOCKEDRGN__*>(handle),&available,nullptr);if(!buffer){*numRects=available;return;}const unsigned int count=std::min(*numRects,available);TSGrowableArray<RECTF> values;values.SetCount(count);unsigned int requested=count;SRgnGetRectsf(reinterpret_cast<HLOCKEDRGN__*>(handle),&requested,values.Ptr());for(unsigned int i=0;i<requested;++i){buffer[i]={static_cast<LONG>(values[i].left),static_cast<LONG>(values[i].bottom),static_cast<LONG>(values[i].right),static_cast<LONG>(values[i].top)};}*numRects=requested;}

void __stdcall SRgnGetRectParamsf(HSRGN__* handle,const RECTF* rect,unsigned int* numParams,void** buffer){if(!numParams)return;RGN* region=Region(handle);if(!region||!rect){*numParams=0;if(buffer)*buffer=nullptr;return;}region->foundparams.SetCount(0);for(unsigned int i=0;i<region->source.Count();++i)if(Intersects(region->source[i].rect,*rect)&&region->source[i].param)region->foundparams.Add({region->source[i].param,region->source[i].sequence});std::sort(region->foundparams.Ptr(),region->foundparams.Ptr()+region->foundparams.Count(),[](const auto&a,const auto&b){return a.sequence<b.sequence;});*numParams=region->foundparams.Count();if(buffer)*buffer=region->foundparams.Count()?region->foundparams[0].param:nullptr;}
void __stdcall SRgnGetRectParamsi(HSRGN__* handle,const RECT* rect,unsigned int* numParams,void** buffer){if(!rect){if(numParams)*numParams=0;return;}RECTF converted=ToFloat(*rect);SRgnGetRectParamsf(handle,&converted,numParams,buffer);}

int __stdcall SRgnIsPointInRegionf(HLOCKEDRGN__* handle,float x,float y){const RGN* region=Region(handle);if(!region)return 0;for(unsigned int i=0;i<region->source.Count();++i){const auto&s=region->source[i];if(!(s.flags&0x10000U)&&x>=s.rect.left&&x<s.rect.right&&y>=s.rect.bottom&&y<s.rect.top)return 1;}return 0;}
int __stdcall SRgnIsPointInRegioni(HSRGN__* handle,int x,int y){return SRgnIsPointInRegionf(reinterpret_cast<HLOCKEDRGN__*>(handle),static_cast<float>(x),static_cast<float>(y));}
int __stdcall SRgnIsRectInRegionf(HSRGN__* handle,const RECTF* rect){if(!rect)return 0;const RGN* region=Region(handle);if(!region)return 0;for(unsigned int i=0;i<region->source.Count();++i)if(!(region->source[i].flags&0x10000U)&&Intersects(region->source[i].rect,*rect))return 1;return 0;}
int __stdcall SRgnIsRectInRegioni(HSRGN__* handle,const RECT* rect){if(!rect)return 0;RECTF converted=ToFloat(*rect);return SRgnIsRectInRegionf(handle,&converted);}

void __stdcall SRgnOffsetf(HLOCKEDRGN__* handle,float x,float y){RGN* region=Region(handle);if(!region)return;for(unsigned int i=0;i<region->source.Count();++i){auto&r=region->source[i].rect;r.left+=x;r.right+=x;r.bottom+=y;r.top+=y;}region->dirty=1;}
void __stdcall SRgnOffseti(HSRGN__* handle,int x,int y){SRgnOffsetf(reinterpret_cast<HLOCKEDRGN__*>(handle),static_cast<float>(x),static_cast<float>(y));}
