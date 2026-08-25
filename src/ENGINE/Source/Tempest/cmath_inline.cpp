#include "ENGINE/Source/Tempest/cmath.h"
#include <algorithm>
#include <cmath>
#include <cstdint>
namespace NTempest {
int CMath::mulhw_(int a,int b){return int((std::int64_t(a)*b)>>32);}unsigned CMath::mulhwu_(unsigned a,unsigned b){return unsigned((std::uint64_t(a)*b)>>32);}int CMath::div3_(int a){return a/3;}unsigned CMath::div3_(unsigned a){return a/3;}int CMath::div5_(int a){return a/5;}unsigned CMath::div5_(unsigned a){return a/5;}int CMath::div9_(int a){return a/9;}unsigned CMath::div9_(unsigned a){return a/9;}
int CMath::min_(int a,int b,int c){return std::min({a,b,c});}int CMath::med_(int a,int b,int c){return a+b+c-min_(a,b,c)-max_(a,b,c);}int CMath::max_(int a,int b,int c){return std::max({a,b,c});}int CMath::span_(int a,int b,int c){return max_(a,b,c)-min_(a,b,c);}int CMath::mean_(int a,int b,int c){return div3_(a+b+c);}
void CMath::normalize_(float&x,float&y){float m=std::sqrt(x*x+y*y);if(m){x/=m;y/=m;}}void CMath::normalize_(float&x,float&y,float&z){float m=std::sqrt(x*x+y*y+z*z);if(m){x/=m;y/=m;z/=m;}}
float CMath::step_(float x,float a){return x>=a?1.0F:0.0F;}float CMath::pulse_(float x,float a,float b){return step_(x,a)-step_(x,b);}float CMath::bstep_(float x,float a,float b){return std::clamp((x-a)/(b-a),0.0F,1.0F);}float CMath::smoothstep_(float x,float a,float b){x=bstep_(x,a,b);return x*x*(3-2*x);}double CMath::gammai_(float x,float g){return std::pow(x,g);}double CMath::gamma_(float x,float g){return std::pow(x,1/g);}double CMath::bias_(float x,float g){return std::pow(x,-std::log2(g));}double CMath::gain_(float x,float g){return x<.5?bias_(2*x,g)/2:1-bias_(2-2*x,g)/2;}double CMath::sinc_(double x,double a){double v=x*a*3.141592653589793;return v==0?1:std::sin(v)/v;}float CMath::sinoid_(float x,float r){return .5F+.5F*std::sin(x/r*3.141592653589793F-1.57079632679F);}float CMath::cosoid_(float x,float r){return .5F-.5F*std::cos(x/r*3.141592653589793F);}
}
