#pragma once
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
namespace NTempest {
struct CRndSeed { unsigned int rndacc=0; unsigned int rndvls=0; void SetSeed(unsigned int); };
class CRandom { public:
 static unsigned Seed(const char*); static unsigned lattice_(CRndSeed&); static unsigned lattice2_(CRndSeed&); static unsigned lattice3_(CRndSeed&); static unsigned lattice4_(CRndSeed&); static unsigned lattice8_(CRndSeed&); static unsigned lattice9_(CRndSeed&); static unsigned lattice27_(CRndSeed&);
 static float reale_(CRndSeed&); static double lreale_(CRndSeed&); static float reale_(CRndSeed&,float); static double lreale_(CRndSeed&,double); static float realg_(CRndSeed&); static double lrealg_(CRndSeed&); static float realg_(CRndSeed&,float,float); static double lrealg_(CRndSeed&,double,double);
 static void array_(float*,unsigned,CRndSeed&);static void array_(double*,unsigned,CRndSeed&);static void arrayp_(float*,unsigned,CRndSeed&);static void arrays_(float*,unsigned,CRndSeed&);static void arraye_(float*,unsigned,CRndSeed&);static void arraye_(double*,unsigned,CRndSeed&);static void arrayg_(float*,unsigned,CRndSeed&);static void shuffle_(void*,unsigned,unsigned,CRndSeed&);static void crypt_(void*,unsigned,CRndSeed&);static C2Vector C2Vector_(CRndSeed&);static C3Vector C3Vector_(CRndSeed&);static float noise_(float,CRndSeed&);static float turbulence_(float,unsigned,CRndSeed&);
};}
