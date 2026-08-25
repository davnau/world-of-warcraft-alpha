#pragma once

namespace NTempest {
class CiRect {
public:
    int minX;
    int minY;
    int maxX;
    int maxY;
    constexpr CiRect() : minX(0), minY(0), maxX(0), maxY(0) {}
    constexpr CiRect(int left, int top, int right, int bottom) : minX(left), minY(top), maxX(right), maxY(bottom) {}
    constexpr int Width() const { return maxX - minX; }
    constexpr int Height() const { return maxY - minY; }
};
static_assert(sizeof(CiRect) == 16);
}
