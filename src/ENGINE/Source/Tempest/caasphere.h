#pragma once
#include "ENGINE/Source/Tempest/c3vector.h"
namespace NTempest { class CAaSphere { public: C3Vector c; float r; constexpr CAaSphere():c(),r(0){} constexpr CAaSphere(const C3Vector&c_,float r_):c(c_),r(r_){} }; static_assert(sizeof(CAaSphere)==16,"CAaSphere layout mismatch"); }
