#include "WoW/Source/WowServices/misc.h"

#include <cmath>
#include <limits>

namespace {
float INFINITY_261 = 0.0F;
float TWO_PI_258 = 0.0F;
float OO_TWO_PI_258 = 0.0F;
}

int _E2_261()
{
    return _E1_261();
}

int _E1_261()
{
    INFINITY_261 = std::numeric_limits<float>::infinity();
    return 2139095040;
}

int _E5_258()
{
    _E4_258();
    return 0;
}

void _E4_258()
{
    TWO_PI_258 = 3.1415927F + 3.1415927F;
}

int _E8_258()
{
    _E7_258();
    return 0;
}

void _E7_258()
{
    OO_TWO_PI_258 = 1.0F / TWO_PI_258;
}

double __fastcall LinearSmooth(float from, float to, float progress)
{
    return static_cast<double>((to - from) * progress + from);
}

long double __fastcall OrganicSmooth(float from, float to, float progress)
{
    return (1.0L - std::cos(3.1415927L * progress)) * 0.5L * (to - from) + from;
}

