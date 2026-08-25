#pragma once

#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>

namespace NTempest {

class CImVector {
public:
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;

    constexpr CImVector() : b(0), g(0), r(0), a(0) {}
    constexpr explicit CImVector(std::uint32_t argb)
        : b(static_cast<std::uint8_t>(argb)), g(static_cast<std::uint8_t>(argb >> 8)),
          r(static_cast<std::uint8_t>(argb >> 16)), a(static_cast<std::uint8_t>(argb >> 24)) {}

    CImVector& operator=(const C3Vector& color);
    operator C3Vector() const;

    void Scale255_(std::int16_t scale);
    void Blend255_(std::uint32_t amount, const CImVector* source);
    void BlendRGB255_(std::uint32_t amount, const CImVector* const* source);
    void SetRGB(const CImVector* source);
};

static_assert(sizeof(CImVector) == sizeof(std::uint32_t), "CImVector layout must remain binary-compatible.");

void __fastcall RGBtoHSV(const C3Vector& rgb, C3Vector& hsv);
void __fastcall HSVtoRGB(const C3Vector& hsv, C3Vector& rgb);

} // namespace NTempest
