#pragma once
#include "ENGINE/Source/Tempest/caabox.h"
#include "ENGINE/Source/Tempest/caasphere.h"
#include "ENGINE/Source/Tempest/cfacet.h"
#include "ENGINE/Source/Tempest/c33matrix.h"
namespace NTempest {
enum SolidIntersect { SI_HollowHollow=0,SI_HollowSolid=1,SI_SolidHollow=2,SI_SolidSolid=3 };
struct CCone { C3Vector position;float angle;C3Vector axis;float height;float cosAngle; };
struct CObBox { C3Vector c;C3Vector e;C33Matrix b; };
bool __fastcall Intersect(const CAaBox&,const C3Vector&);bool __fastcall Intersect(const CAaBox&,const CAaBox&);bool __fastcall Intersect(const CAaSphere&,const C3Vector&,SolidIntersect=SI_SolidSolid);bool __fastcall Intersect2d(const CAaSphere&,const C3Vector&,SolidIntersect=SI_SolidSolid);bool __fastcall Intersect(const CAaSphere&,const CAaSphere&,SolidIntersect=SI_SolidSolid);bool __fastcall Intersect(const C4Plane&,const C3Vector&,float&);bool __fastcall Intersect(const CFacet&,const C3Vector&,const C3Vector&,float&);bool __fastcall IntersectCull(const CFacet&,const C3Vector&,const C3Vector&,float&);bool __fastcall Intersect(const CCone&,const C3Vector&);bool __fastcall Intersect(const CObBox&,const C3Vector&);bool __fastcall Intersect(const CFacet&,const C3Vector&);
}
