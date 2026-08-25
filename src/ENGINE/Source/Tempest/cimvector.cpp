#include "ENGINE/Source/Tempest/cimvector.h"

#include <algorithm>
#include <cmath>

namespace NTempest {
namespace {
std::uint8_t ToChannel(float value) {
    return static_cast<std::uint8_t>(std::clamp(value, 0.0F, 1.0F) * 255.0F);
}
}

void __fastcall RGBtoHSV(const C3Vector& rgb, C3Vector& hsv) {
    const float maximum = std::max({rgb.x, rgb.y, rgb.z});
    const float minimum = std::min({rgb.x, rgb.y, rgb.z});
    const float delta = maximum - minimum;
    hsv.z = maximum;
    hsv.y = maximum == 0.0F ? 0.0F : delta / maximum;
    if (delta == 0.0F) {
        hsv.x = -1.0F;
    } else if (maximum == rgb.x) {
        hsv.x = 60.0F * ((rgb.y - rgb.z) / delta);
    } else if (maximum == rgb.y) {
        hsv.x = 60.0F * ((rgb.z - rgb.x) / delta + 2.0F);
    } else {
        hsv.x = 60.0F * ((rgb.x - rgb.y) / delta + 4.0F);
    }
    if (hsv.x < 0.0F && hsv.y != 0.0F) hsv.x += 360.0F;
}

void __fastcall HSVtoRGB(const C3Vector& hsv, C3Vector& rgb) {
    if (hsv.y == 0.0F) { rgb = C3Vector(hsv.z); return; }
    float hue = hsv.x == 360.0F ? 0.0F : hsv.x;
    hue /= 60.0F;
    const int sector = static_cast<int>(std::floor(hue));
    const float fraction = hue - static_cast<float>(sector);
    const float p = hsv.z * (1.0F - hsv.y);
    const float q = hsv.z * (1.0F - hsv.y * fraction);
    const float t = hsv.z * (1.0F - hsv.y * (1.0F - fraction));
    switch (sector) {
    case 0: rgb = C3Vector(hsv.z, t, p); break;
    case 1: rgb = C3Vector(q, hsv.z, p); break;
    case 2: rgb = C3Vector(p, hsv.z, t); break;
    case 3: rgb = C3Vector(p, q, hsv.z); break;
    case 4: rgb = C3Vector(t, p, hsv.z); break;
    default: rgb = C3Vector(hsv.z, p, q); break;
    }
}

CImVector& CImVector::operator=(const C3Vector& color) {
    r = ToChannel(color.x);
    g = ToChannel(color.y);
    b = ToChannel(color.z);
    a = 255;
    return *this;
}

CImVector::operator C3Vector() const {
    constexpr float reciprocal = 1.0F / 255.0F;
    return C3Vector(r * reciprocal, g * reciprocal, b * reciprocal);
}

void CImVector::Scale255_(std::int16_t scale) {
    const auto blend = [scale](std::uint8_t channel) {
        return static_cast<std::uint8_t>((scale * channel + 255) >> 8);
    };
    b = blend(b);
    g = blend(g);
    r = blend(r);
}

void CImVector::Blend255_(std::uint32_t amount, const CImVector* source) {
    if (source == nullptr) return;
    if (amount >= 255) { *this = *source; return; }
    const auto blend = [amount](std::uint8_t left, std::uint8_t right) {
        return static_cast<std::uint8_t>(left + ((amount * (right - left)) >> 8));
    };
    b = blend(b, source->b);
    g = blend(g, source->g);
    r = blend(r, source->r);
}

void CImVector::BlendRGB255_(std::uint32_t amount, const CImVector* const* source) {
    if (source == nullptr || *source == nullptr) return;
    const std::uint8_t alpha = a;
    Blend255_(amount, *source);
    a = alpha;
}

void CImVector::SetRGB(const CImVector* source) {
    if (source == nullptr) return;
    b = source->b;
    g = source->g;
    r = source->r;
}

} // namespace NTempest
