#pragma once

#include <cstdint>
class CDataStore;

union unreal {
    float value;
    std::uint32_t bits;
    constexpr unreal():value(0){} constexpr unreal(float v):value(v){} constexpr unreal(double v):value(static_cast<float>(v)){} constexpr unreal(int v):value(static_cast<float>(v)){}
    unreal& fromInt(int); int asInt() const; void asString(char* out,int iWidth=0,int fPrecision=6) const; unreal& fromString(const char*);
    constexpr operator float()const{return value;}
};
unreal operator*(unreal,unreal);unreal operator/(unreal,unreal);unreal operator-(unreal,unreal);unreal operator+(unreal,unreal);unreal operator-(unreal);
unreal reciprocal(unreal);unreal floor(unreal);unreal ceil(unreal);unreal trunc(unreal);unreal fract(unreal);unreal round(unreal);unreal mod(unreal,unreal);
int ipow(unsigned base,unsigned exponent);unreal ln(unreal);unreal e(unreal);unreal pow(unreal,unreal);unreal pow(unreal,int);unreal sqrt(unreal);unreal sqrtinv(unreal);
unreal sin(unreal);unreal cos(unreal);void sincos(unreal angle,unreal* s,unreal* c);unreal tan(unreal);unreal acos(unreal);unreal asin(unreal);unreal atan(unreal);unreal atan2(unreal y,unreal x);
CDataStore& operator<<(CDataStore&,unreal);CDataStore& operator>>(CDataStore&,unreal&);
void UnrealInitialize();void UnrealDestroy();
extern const unreal UNREAL_ZERO,UNREAL_ONE,UNREAL_HALF,UNREAL_PI,UNREAL_TWO_PI,UNREAL_HALF_PI;
