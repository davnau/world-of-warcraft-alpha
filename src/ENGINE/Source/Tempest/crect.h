#pragma once

namespace NTempest {

class CRect {
public:
    float l;
    float t;
    float r;
    float b;

    constexpr CRect() : l(0), t(0), r(0), b(0) {}
    constexpr CRect(float left, float top, float right, float bottom)
        : l(left), t(top), r(right), b(bottom) {}

    constexpr float Width() const { return r - l; }
    constexpr float Height() const { return b - t; }
    constexpr bool IsEmpty() const { return l == r || t == b; }
};

static_assert(sizeof(CRect) == sizeof(float) * 4);

}

