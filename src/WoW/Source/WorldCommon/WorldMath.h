#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c33matrix.h"
#include "ENGINE/Source/Tempest/c34matrix.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/c4plane.h"
#include "ENGINE/Source/Tempest/caabox.h"

namespace NTempest { struct C3Segment; }

class CWorldMath {
public:
    static int __fastcall EdgeIntersectEdge(const NTempest::C2Vector& a,
                                            const NTempest::C2Vector& b,
                                            const NTempest::C2Vector& c,
                                            const NTempest::C2Vector& d,
                                            NTempest::C2Vector& point);
    static int __fastcall RayIntersectTri(
        const NTempest::C3Vector& rayOrigin,
        const NTempest::C3Vector& rayDirection,
        const NTempest::C3Vector& v0,
        const NTempest::C3Vector& v1,
        const NTempest::C3Vector& v2,
        float& dist);
    static void __fastcall TransformAABox(const NTempest::C33Matrix& matrix,
                                          const NTempest::CAaBox& box,
                                          NTempest::CAaBox& result);
    static void __fastcall TransformAABox(const NTempest::C34Matrix& matrix,
                                          const NTempest::CAaBox& box,
                                          NTempest::CAaBox& result);
    static void __fastcall TransformAABox(const NTempest::C44Matrix& matrix,
                                          const NTempest::CAaBox& box,
                                          NTempest::CAaBox& result);
    static int __fastcall VectorIntersectAABox2(const NTempest::CAaBox& box,
                                                const NTempest::C3Vector& start,
                                                const NTempest::C3Vector& end);
    static int __fastcall VectorIntersectAABox2(const NTempest::CAaBox& box,
                                                const NTempest::C3Segment& segment);
    static int __fastcall SphereIntersectAABox(const NTempest::C3Vector& center,
                                               const NTempest::CAaBox& box,
                                               float radius);
    static unsigned __fastcall AABoxIntersectPlane(const NTempest::C4Plane& plane,
                                                   const NTempest::CAaBox& box);
    static long double __fastcall TriSqrDistance(const NTempest::C3Vector& origin,
                                                 const NTempest::C3Vector& a,
                                                 const NTempest::C3Vector& edge0,
                                                 const NTempest::C3Vector& edge1);
};

extern float INFINITY_371;
extern float TWO_PI_330;
extern float OO_TWO_PI_331;

int _E2_371(); int _E1_371(); int _E5_342(); void _E4_342();
int _E8_332(); void _E7_342(); int _E14_23(); void _E13_14();
int _E17_26(); void _E16_29(); int _E20_74(); void _E19_56();
int _E23_76(); void _E22_64(); int _E26_73(); void _E25_64();
int _E29_72(); void _E28_100(); int _E32_64(); void _E31_98();
int _E35_77(); void _E34_75(); int _E38_75(); void _E37_70();
int _E41_72(); void _E40_71(); int _E44_74(); void _E43_68();
int _E47_75(); void _E46_67(); int _E53_66(); void _E52_66();
int _E56_41(); void _E55_58(); int _E59_56(); void _E58_59();
int _E62_53(); void _E61_57(); int _E65_49(); void _E64_47();
