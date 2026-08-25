#pragma once

namespace NTempest {
class C2iVector {
public:
    int x;
    int y;
    constexpr C2iVector() : x(0), y(0) {}
    constexpr C2iVector(int xValue, int yValue) : x(xValue), y(yValue) {}
};
static_assert(sizeof(C2iVector) == 8);
}
