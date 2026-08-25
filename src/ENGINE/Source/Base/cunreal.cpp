#include "ENGINE/Source/Base/cunreal.h"
#include "ENGINE/Source/Base/cdatastore.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>

unreal&unreal::fromInt(int v){value=static_cast<float>(v);return*this;}int unreal::asInt()const{return static_cast<int>(value);}void unreal::asString(char*out,int w,int p)const{if(out)sprintf_s(out,64,"%*.*f",w,p,value);}unreal&unreal::fromString(const char*s){value=s?std::strtof(s,nullptr):0;return*this;}
unreal operator*(unreal a,unreal b){return a.value*b.value;}unreal operator/(unreal a,unreal b){return a.value/b.value;}unreal operator-(unreal a,unreal b){return a.value-b.value;}unreal operator+(unreal a,unreal b){return a.value+b.value;}unreal operator-(unreal a){return -a.value;}
unreal reciprocal(unreal a){return 1.0f/a.value;}unreal floor(unreal a){return std::floor(a.value);}unreal ceil(unreal a){return std::ceil(a.value);}unreal trunc(unreal a){return std::trunc(a.value);}unreal fract(unreal a){return a.value-std::floor(a.value);}unreal round(unreal a){return std::round(a.value);}unreal mod(unreal a,unreal b){return std::fmod(a.value,b.value);}
int ipow(unsigned b,unsigned e){unsigned r=1;while(e){if(e&1)r*=b;b*=b;e>>=1;}return static_cast<int>(r);}unreal ln(unreal a){return std::log(a.value);}unreal e(unreal a){return std::exp(a.value);}unreal pow(unreal a,unreal b){return std::pow(a.value,b.value);}unreal pow(unreal a,int b){return std::pow(a.value,static_cast<float>(b));}unreal sqrt(unreal a){return std::sqrt(a.value);}unreal sqrtinv(unreal a){return 1.0f/std::sqrt(a.value);}
unreal sin(unreal a){return std::sin(a.value);}unreal cos(unreal a){return std::cos(a.value);}void sincos(unreal a,unreal*s,unreal*c){if(s)*s=sin(a);if(c)*c=cos(a);}unreal tan(unreal a){return std::tan(a.value);}unreal acos(unreal a){return std::acos(a.value);}unreal asin(unreal a){return std::asin(a.value);}unreal atan(unreal a){return std::atan(a.value);}unreal atan2(unreal y,unreal x){return std::atan2(y.value,x.value);}
CDataStore&operator<<(CDataStore&s,unreal v){s.Put(v.bits);return s;}CDataStore&operator>>(CDataStore&s,unreal&v){s.Get(&v.bits);return s;}void UnrealInitialize(){}void UnrealDestroy(){}
