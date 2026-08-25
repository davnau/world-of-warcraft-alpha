#include "WoW/common/daynight.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

int GlareBase::m_masterEnable = 1;

DNInfo s_dnInfo;
DNClouds s_clouds;
DNSky s_sky;
DNSunGlare s_sunGlare;
DNMoonGlare s_moonGlare;
DNStars s_stars;
DNPlanet s_planets[3];
LightGroup g_areaLights;
CurrentLight s_magmaLight;
CurrentLight s_slimeLight;
float s_cloudLODCvar = 0.0F;
bool s_paused_1 = false;
bool s_initialized_2 = false;

namespace {

constexpr float kPi = 3.14159265358979323846F;
constexpr float kTwoPi = 6.2831853071795864769F;

NTempest::C2Vector s_sidnTable[4] = {
    {0.0F, 0.0F}, {0.22916667F, 1.0F}, {0.72916667F, 0.0F}, {1.0F, 0.0F},
};
NTempest::C2Vector s_unitColorTable[2] = {{0.0F, 0.0F}, {1.0F, 1.0F}};
float s_cloudTable[256]{};

float Clamp01(float value) { return std::clamp(value, 0.0F, 1.0F); }

NTempest::CImVector LerpColor(const NTempest::CImVector& left,
                              const NTempest::CImVector& right, float amount) {
    const float t = Clamp01(amount);
    NTempest::CImVector out;
    out.b = static_cast<std::uint8_t>(std::clamp(left.b + (right.b - left.b) * t, 0.0F, 255.0F));
    out.g = static_cast<std::uint8_t>(std::clamp(left.g + (right.g - left.g) * t, 0.0F, 255.0F));
    out.r = static_cast<std::uint8_t>(std::clamp(left.r + (right.r - left.r) * t, 0.0F, 255.0F));
    out.a = static_cast<std::uint8_t>(std::clamp(left.a + (right.a - left.a) * t, 0.0F, 255.0F));
    return out;
}

template <typename T>
void ResizeArray(TSFixedArray<T>& array, unsigned int count) {
    array.ReallocAndClearData(count);
}

template <typename T>
void ClearArray(TSFixedArray<T>& array) {
    array.ReallocAndClearData(0);
}

void ClearLight(CurrentLight& light) { light = CurrentLight{}; }

float HashNoise(int x, int y, int z) {
    std::uint32_t value = static_cast<std::uint32_t>(x) * 73856093U ^
                           static_cast<std::uint32_t>(y) * 19349663U ^
                           static_cast<std::uint32_t>(z) * 83492791U;
    value ^= value >> 13;
    value *= 1274126177U;
    return static_cast<float>(value & 0xFFFFU) / 65535.0F;
}

float SampleCloudHeight(const DNClouds& clouds, int x, int y) {
    if (clouds.m_tmSize == 0 || clouds.m_height.m_count == 0) return 0.0F;
    const unsigned int ix = static_cast<unsigned int>(x) & clouds.m_wrapMask;
    const unsigned int iy = static_cast<unsigned int>(y) & clouds.m_wrapMask;
    const unsigned int index = (iy << clouds.m_tmShift) + ix;
    if (index >= clouds.m_height.m_count) return 0.0F;
    return static_cast<float>(clouds.m_height.m_data[index]) / 255.0F;
}

void ResetTableColors() {
    for (unsigned int i = 0; i < std::size(s_cloudTable); ++i)
        s_cloudTable[i] = static_cast<float>(i);
}

} // namespace

double __fastcall InterpTable(const NTempest::C2Vector* table, unsigned int count, float key) {
    if (table == nullptr || count == 0) return 0.0;
    if (count == 1) return table[0].y;
    unsigned int next = 0;
    while (next < count && key > table[next].x) ++next;
    unsigned int previous;
    if (next == count) {
        next = 0;
        previous = count - 1;
    } else if (next != 0) {
        previous = next - 1;
    } else {
        previous = count - 1;
    }
    float range = table[next].x - table[previous].x;
    if (range < 0.0F) range += 1.0F;
    float delta = key - table[previous].x;
    if (delta < 0.0F) delta += 1.0F;
    const float amount = range == 0.0F ? 0.0F : delta / range;
    if (table[next].y < table[previous].y)
        return table[previous].y - amount * (table[previous].y - table[next].y);
    return table[previous].y + amount * (table[next].y - table[previous].y);
}

double __fastcall Interp(float first, float second, float percent) {
    return static_cast<double>(first) +
           static_cast<double>(second - first) * static_cast<double>(percent);
}

NTempest::CImVector* __fastcall DarkenColor(NTempest::CImVector* color, float clr, float amount) {
    if (color == nullptr) return nullptr;
    const float factor = Clamp01(clr + amount);
    color->r = static_cast<std::uint8_t>(std::clamp(color->r * factor, 0.0F, 255.0F));
    color->g = static_cast<std::uint8_t>(std::clamp(color->g * factor, 0.0F, 255.0F));
    color->b = static_cast<std::uint8_t>(std::clamp(color->b * factor, 0.0F, 255.0F));
    return color;
}

void DNGlare::Update(float elapsedSec) {
    m_targetOpacity = 1.0F;
    const NTempest::C3Vector delta = m_pos - s_dnInfo.cameraPos;
    const float squared = delta.SquaredMag();
    if (squared <= 0.0F) {
        m_targetOpacity = 0.0F;
        return;
    }
    const float inverseDistance = 1.0F / std::sqrt(squared);
    const NTempest::C3Vector direction = delta * inverseDistance;
    const float cloudFade = static_cast<float>(GetCloudDensityFade());
    m_targetOpacity *= inverseDistance * cloudFade;
    m_targetOpacity *= static_cast<float>(InterpTable(m_fadeTable, 4, s_dnInfo.dayProgression));
    if (!m_enabled || !GlareBase::m_masterEnable) m_targetOpacity = 0.0F;
    if (m_targetOpacity > m_opacity)
        m_opacity = std::min(m_targetOpacity, m_opacity + elapsedSec * m_fadeRate);
    else
        m_opacity = std::max(m_targetOpacity, m_opacity - elapsedSec * m_fadeRate);
    float dot = NTempest::C3Vector::Dot(s_dnInfo.cameraDir, direction);
    dot = std::max(dot, m_dotMin);
    const float angleFactor = (dot - m_dotMin) / (1.0F - m_dotMin);
    m_curScale = ((m_scaleMax - m_scaleMin) * angleFactor + m_scaleMin) *
                 m_baseScale * s_dnInfo.billbRescale;
    m_color.a = static_cast<std::uint8_t>(std::clamp(
        ((m_alphaMax - m_alphaMin) * angleFactor + m_alphaMin) * m_opacity * 255.0F,
        0.0F, 255.0F));
}

double DNGlare::GetCloudDensityFade() const { return 1.0; }

double DNSunGlare::GetCloudDensityFade() const {
    return 1.0 - s_clouds.GetDensity(m_pos, 1.0F);
}

double DNMoonGlare::GetCloudDensityFade() const {
    const double density = s_clouds.GetDensity(m_pos, 1.0F);
    return 1.0 - std::fabs(density - 0.5 + density - 0.5);
}

DNClouds::DNClouds() = default;

void DNClouds::WorldToTexture(const NTempest::C3Vector& worldPt, NTempest::C2Vector& tex) const {
    const NTempest::C3Vector local(worldPt.x - s_dnInfo.cameraPos.x,
                                   worldPt.y - s_dnInfo.cameraPos.y,
                                   worldPt.z - s_dnInfo.cameraPos.z);
    const NTempest::C3Vector spherePoint(local.x, local.y, local.z + 736.0F);
    const NTempest::C3Vector up(0.0F, 0.0F, 800.0F);
    const float upLength = std::sqrt(up.SquaredMag());
    const float pointLength = std::sqrt(spherePoint.SquaredMag());
    float angle = pointLength == 0.0F ? 0.0F :
        std::acos(std::clamp(NTempest::C3Vector::Dot(up, spherePoint) /
                             (upLength * pointLength), -1.0F, 1.0F));
    const float maxAngle = kPi * 0.22222222F;
    angle = std::min(angle, maxAngle);
    const float radius = angle * 0.5F / (maxAngle * 0.75F);
    const float planarLength = std::sqrt(local.x * local.x + local.y * local.y);
    const float nx = planarLength <= 0.00001F ? 0.0F : local.x / planarLength;
    const float ny = planarLength <= 0.00001F ? 0.0F : local.y / planarLength;
    tex.x = (nx * radius + 0.5F) * static_cast<float>(m_tmSize);
    tex.y = (ny * radius + 0.5F) * static_cast<float>(m_tmSize);
}

void DNClouds::Collide(const NTempest::C3Vector& origin, const NTempest::C3Vector& dir,
                       NTempest::C3Vector& hitPoint) const {
    const float radius = 800.0F;
    const NTempest::C3Vector center(s_dnInfo.cameraPos.x, s_dnInfo.cameraPos.y,
                                    s_dnInfo.cameraPos.z - 736.0F);
    const NTempest::C3Vector relative = origin - center;
    const float b = NTempest::C3Vector::Dot(relative, dir);
    const float c = relative.SquaredMag() - radius * radius;
    const float discriminant = b * b - c;
    if (discriminant < 0.0F) {
        hitPoint = origin;
        return;
    }
    float distance = -b - std::sqrt(discriminant);
    if (distance < 0.0F) distance = -b + std::sqrt(discriminant);
    hitPoint = origin + dir * distance;
}

double DNClouds::GetDensity(const NTempest::C3Vector& worldPoint, float area) const {
    if (m_nLayers == 0 || m_tmSize == 0 || m_height.m_count == 0) return 0.0;
    NTempest::C2Vector tex;
    WorldToTexture(worldPoint, tex);
    const float density = SampleCloudHeight(*this, static_cast<int>(std::floor(tex.x)),
                                            static_cast<int>(std::floor(tex.y)));
    const float sharpness = m_sharpness == 0.0F ? 1.0F : m_sharpness;
    return std::clamp(std::pow(std::max(density, 0.0F), sharpness) *
                      std::max(area, 0.0F), 0.0F, 1.0F);
}

void DNClouds::BumpMap() {
    if (m_tmSize == 0 || m_height.m_count == 0) return;
    ResizeArray(m_lastBumpNoiseY, m_height.m_count);
    ResizeArray(m_bump, m_height.m_count);
    for (unsigned int i = 0; i < m_height.m_count; ++i) {
        const unsigned int x = i & m_wrapMask;
        const unsigned int y = (i >> m_tmShift) & m_wrapMask;
        const float center = SampleCloudHeight(*this, static_cast<int>(x), static_cast<int>(y));
        const float right = SampleCloudHeight(*this, static_cast<int>(x + 1), static_cast<int>(y));
        const float down = SampleCloudHeight(*this, static_cast<int>(x), static_cast<int>(y + 1));
        m_lastBumpNoiseY.m_data[i] = down;
        m_bump.m_data[i] = NTempest::C2Vector(right - center, down - center);
    }
}

void DNClouds::FullUpdate() { m_updateRow = 0; m_lastTime = 0; Update(); }

void DNClouds::Update() {
    if (m_nLayers == 0 || m_tmSize == 0) return;
    if (m_height.m_count != m_tmSize * m_tmSize)
        ResizeArray(m_height, m_tmSize * m_tmSize);
    for (unsigned int y = 0; y < m_tmSize; ++y) {
        for (unsigned int x = 0; x < m_tmSize; ++x) {
            const float noise = HashNoise(static_cast<int>(x + m_timeX), static_cast<int>(y), 0);
            m_height.m_data[y * m_tmSize + x] = static_cast<std::uint8_t>(
                std::clamp(noise * 255.0F * (m_density / 255.0F), 0.0F, 255.0F));
        }
    }
    BumpMap();
    ++m_timeX;
    m_lastTime = s_dnInfo.time;
}

void DNClouds::GenSphere(float size) {
    constexpr unsigned int rows = 8;
    constexpr unsigned int columns = 8;
    ResizeArray(m_geoVerts, rows * columns);
    ResizeArray(m_texVerts, rows * columns);
    ResizeArray(m_indices, 102);
    m_nVerts = static_cast<std::uint16_t>(rows * columns);
    m_nIndices = 102;
    unsigned int vertex = 0;
    for (unsigned int row = 0; row < rows; ++row) {
        const float phi = kPi * static_cast<float>(row) / static_cast<float>(rows - 1);
        for (unsigned int column = 0; column < columns; ++column) {
            const float theta = kTwoPi * static_cast<float>(column) / static_cast<float>(columns);
            const float sinPhi = std::sin(phi);
            m_geoVerts.m_data[vertex] = NTempest::C3Vector(
                size * sinPhi * std::cos(theta), size * sinPhi * std::sin(theta), size * std::cos(phi));
            m_texVerts.m_data[vertex] = NTempest::C2Vector(
                static_cast<float>(column) / columns, static_cast<float>(row) / (rows - 1));
            ++vertex;
        }
    }
    unsigned int index = 0;
    for (unsigned int row = 0; row + 1 < rows && index + 1 < m_indices.m_count; ++row) {
        for (unsigned int column = 0; column < columns && index + 5 < m_indices.m_count; ++column) {
            const unsigned int next = (column + 1) % columns;
            const unsigned int a = row * columns + column;
            const unsigned int b = row * columns + next;
            const unsigned int c = (row + 1) * columns + column;
            const unsigned int d = (row + 1) * columns + next;
            m_indices.m_data[index++] = static_cast<std::uint16_t>(a);
            m_indices.m_data[index++] = static_cast<std::uint16_t>(c);
            m_indices.m_data[index++] = static_cast<std::uint16_t>(b);
            if (index + 2 < m_indices.m_count) {
                m_indices.m_data[index++] = static_cast<std::uint16_t>(b);
                m_indices.m_data[index++] = static_cast<std::uint16_t>(c);
                m_indices.m_data[index++] = static_cast<std::uint16_t>(d);
            }
        }
    }
}

void DNClouds::Destroy() {
    m_texid = nullptr;
    ClearArray(m_indices); ClearArray(m_texVerts); ClearArray(m_geoVerts);
    ClearArray(m_bump); ClearArray(m_lastBumpNoiseY); ClearArray(m_noise);
    ClearArray(m_height); ClearArray(m_texels);
}

void DNClouds::OverrideDensitySharpness(float newDensity, float newSharpness) {
    m_densityOverride = newDensity;
    SetDensity(newDensity);
    SetSharpness(newSharpness);
}

void DNClouds::SetSharpness(float newSharpness) {
    m_sharpness = newSharpness;
    const float delta = (255.0F - m_density) * (1.0F / 256.0F);
    for (unsigned int i = 0; i < 256; ++i)
        s_cloudTable[i] = 255.0F - std::pow(std::max(m_sharpness, 0.0F), delta * i) * 255.0F;
}

void DNClouds::SetDensity(float newDensity) {
    if (m_densityOverride != 0.0F) newDensity = m_densityOverride;
    m_density = static_cast<std::uint8_t>(std::clamp(newDensity * 255.0F, 0.0F, 255.0F));
}

void DNClouds::SetLOD(unsigned int newLod, unsigned int newUpdateSize) {
    m_lod = newLod;
    m_updateSize = newUpdateSize;
    m_tmSize = newLod == 0 ? 0 : 1U << std::min(newLod + 5, 10U);
    m_tmShift = m_tmSize == 0 ? 0 : static_cast<unsigned int>(std::log2(m_tmSize));
    m_wrapMask = m_tmSize == 0 ? 0 : m_tmSize - 1;
    ResizeArray(m_texels, m_tmSize * m_tmSize);
    ResizeArray(m_height, m_tmSize * m_tmSize);
    ResizeArray(m_noise, m_tmSize * m_tmSize);
}

void DNSky::GenSphere(float radius) {
    // DNSky::m_stripSizes at 0x81195C in the preserved executable.  The
    // sphere is seven latitude rows: two poles and five sixteen-point rings.
    static constexpr float stripSizes[] = {
        0.0F, 0.35F, 0.41F, 0.46F, 0.49F, 0.5F, 1.0F
    };
    constexpr unsigned int thetaTess = 16;
    m_sphRadius = radius;
    m_sphThetaTess = thetaTess;
    ResizeArray(m_geoVerts, 112);
    ResizeArray(m_clrVerts, 112);
    ResizeArray(m_indices, 204);

    unsigned int vertex = 0;
    unsigned int index = 0;
    unsigned int previousRow = 0;
    float previousPhi = 0.0F;
    for (unsigned int row = 0; row < std::size(stripSizes); ++row) {
        const float phi = kPi * stripSizes[row];
        const float sinPhi = std::sin(phi);
        const float cosPhi = std::cos(phi) * radius;
        const unsigned int thisRow = vertex;
        const bool pole = std::fabs(phi) < 0.00000095367432F ||
                          std::fabs(phi - kPi) < 0.00000095367432F;
        const unsigned int rowVertices = pole ? 1U : thetaTess;
        for (unsigned int thetaStep = 0; thetaStep < rowVertices; ++thetaStep) {
            const float theta = kTwoPi * static_cast<float>(thetaStep) /
                                static_cast<float>(thetaTess);
            m_geoVerts.m_data[vertex++] = NTempest::C3Vector(
                std::sin(theta) * sinPhi * radius,
                std::cos(theta) * sinPhi * radius,
                cosPhi);
        }
        if (row != 0U) {
            const bool previousPole = std::fabs(previousPhi) < 0.00000095367432F;
            for (unsigned int thetaStep = 0; thetaStep <= thetaTess; ++thetaStep) {
                m_indices.m_data[index++] = static_cast<std::uint16_t>(
                    previousRow + (previousPole ? 0U : thetaStep % thetaTess));
                m_indices.m_data[index++] = static_cast<std::uint16_t>(
                    thisRow + (pole ? 0U : thetaStep % thetaTess));
            }
        }
        previousPhi = phi;
        previousRow = thisRow;
    }
    m_nVerts = static_cast<std::uint16_t>(vertex);
    m_nIndices = static_cast<std::uint16_t>(index);
}

void DNSky::GenTexture(unsigned int width, unsigned int, float texels) {
    if (width == 0) return;
    if (m_clrVerts.m_count == 0) ResizeArray(m_clrVerts, 112);
    for (unsigned int i = 0; i < m_clrVerts.m_count; ++i) {
        const float t = static_cast<float>(i % width) / static_cast<float>(std::max(width - 1, 1U));
        const std::uint8_t value = static_cast<std::uint8_t>(
            std::clamp(texels * (0.5F + t), 0.0F, 255.0F));
        m_clrVerts.m_data[i] = NTempest::CImVector(
            static_cast<std::uint32_t>(value) | static_cast<std::uint32_t>(value) << 8 |
            static_cast<std::uint32_t>(value) << 16 | 0xFF000000U);
    }
}

void DNSky::SetColors() {
    if (!m_clrVerts.m_data || m_clrVerts.m_count == 0 || m_sphThetaTess <= 0)
        return;

    // DNSky::SetColors (0x6B9F80) uses five azimuth-sensitive latitude
    // bands between the two poles.  The previous reconstruction collapsed
    // all six recovered sky colors into one vertical lerp, losing both the
    // horizon bands and the directional day/night fade.
    static constexpr NTempest::C2Vector darkTable[6] = {
        {0.12500001F, 0.0F}, {0.27083334F, 1.0F},
        {0.29166667F, 0.0F}, {0.85416663F, 0.0F},
        {0.89583331F, 1.0F}, {0.99930555F, 0.0F},
    };
    static constexpr NTempest::C2Vector fadeTable[6] = {
        {0.125F, 1.0F}, {0.375F, 0.0F}, {0.500F, -0.5F},
        {0.625F, -0.7F}, {0.750F, -0.5F}, {0.875F, 0.0F},
    };

    const float darkness = static_cast<float>(InterpTable(
        darkTable, static_cast<unsigned>(std::size(darkTable)),
        s_dnInfo.dayProgression)) * s_dnInfo.light.Darkness;

    NTempest::CImVector midColors[6]{};
    for (unsigned band = 1; band <= 5; ++band)
        midColors[band] = LerpColor(s_dnInfo.light.SkyArray[band],
                                    s_dnInfo.light.SkyArray[1], darkness);

    unsigned vertex = 0;
    m_clrVerts.m_data[vertex++] = s_dnInfo.light.SkyArray[0];
    const float angleDelta = -1.0F / static_cast<float>(m_sphThetaTess);
    for (unsigned band = 1; band <= 4; ++band) {
        float angle = s_dnInfo.faceAngle / kTwoPi + 0.25F;
        if (angle > 1.0F) angle -= 1.0F;
        for (int column = 0; column < m_sphThetaTess; ++column) {
            if (angle < 0.0F) angle += 1.0F;
            const float fade = static_cast<float>(InterpTable(
                fadeTable, static_cast<unsigned>(std::size(fadeTable)), angle));
            NTempest::CImVector color;
            if (fade >= 0.0F) {
                color = LerpColor(s_dnInfo.light.SkyArray[band], midColors[band],
                                  (1.0F - fade) * darkness);
            } else {
                const NTempest::CImVector darkTarget = LerpColor(
                    midColors[band], s_dnInfo.light.SkyArray[0], darkness * 0.7F);
                color = LerpColor(midColors[band], darkTarget, -fade * darkness);
            }
            if (s_dnInfo.eclipseAmount != 0) {
                const NTempest::CImVector* eclipse = &s_dnInfo.eclipseColor;
                color.BlendRGB255_(s_dnInfo.eclipseAmount, &eclipse);
            }
            if (vertex < m_clrVerts.m_count)
                m_clrVerts.m_data[vertex++] = color;
            angle += angleDelta;
        }
    }

    NTempest::CImVector bottom = s_dnInfo.light.SkyArray[5];
    if (s_dnInfo.eclipseAmount != 0) {
        const NTempest::CImVector* eclipse = &s_dnInfo.eclipseColor;
        bottom.BlendRGB255_(s_dnInfo.eclipseAmount, &eclipse);
    }
    for (int column = 0; column < m_sphThetaTess && vertex < m_clrVerts.m_count; ++column)
        m_clrVerts.m_data[vertex++] = bottom;
    if (vertex < m_clrVerts.m_count)
        m_clrVerts.m_data[vertex] = bottom;
}

void DNSky::Render() {
    if (m_nVerts == 0U || m_nIndices == 0U || !m_geoVerts.m_data ||
        !m_clrVerts.m_data || !m_indices.m_data)
        return;

    static bool logged = false;
    if (!logged) {
        logged = true;
        std::fprintf(stderr,
            "SKY_RENDER verts=%u indices=%u day=%.4f dark=%.4f far=%.3f "
            "bands=%02X%02X%02X/%02X%02X%02X/%02X%02X%02X "
            "colors=%02X%02X%02X/%02X%02X%02X\n",
            static_cast<unsigned>(m_nVerts), static_cast<unsigned>(m_nIndices),
            s_dnInfo.dayProgression, s_dnInfo.light.Darkness, s_dnInfo.farClip,
            s_dnInfo.light.SkyArray[0].r, s_dnInfo.light.SkyArray[0].g,
            s_dnInfo.light.SkyArray[0].b, s_dnInfo.light.SkyArray[1].r,
            s_dnInfo.light.SkyArray[1].g, s_dnInfo.light.SkyArray[1].b,
            s_dnInfo.light.SkyArray[5].r, s_dnInfo.light.SkyArray[5].g,
            s_dnInfo.light.SkyArray[5].b, m_clrVerts.m_data[0].r,
            m_clrVerts.m_data[0].g, m_clrVerts.m_data[0].b,
            m_clrVerts.m_data[m_nVerts - 1U].r,
            m_clrVerts.m_data[m_nVerts - 1U].g,
            m_clrVerts.m_data[m_nVerts - 1U].b);
    }

    float minX = 0.0F, maxX = 1.0F;
    float minY = 0.0F, maxY = 1.0F;
    float minZ = 0.0F, maxZ = 1.0F;
    GxXformViewport(&minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    GxXformSetViewport(minX, maxX, minY, maxY, 1.0F, 1.0F);

    NTempest::C44Matrix worldScale;
    worldScale.Scale((std::max)(1.0F, s_dnInfo.farClip * 0.5F));
    GxXformPush(GxXform_World, worldScale);

    NTempest::C44Matrix savedView;
    GxXformView(&savedView);
    NTempest::C3Vector forward(1.0F, 0.0F, savedView.c2);
    forward.Normalize();
    const NTempest::C3Vector worldUp(0.0F, 0.0F, 1.0F);
    NTempest::C3Vector right = NTempest::C3Vector::Cross(worldUp, forward);
    right.Normalize();
    const NTempest::C3Vector up = NTempest::C3Vector::Cross(forward, right);
    const NTempest::C44Matrix skyView(
        right.x, up.x, forward.x, 0.0F,
        right.y, up.y, forward.y, 0.0F,
        right.z, up.z, forward.z, 0.0F,
        0.0F, 0.0F, 0.0F, 1.0F);
    GxXformSetView(skyView);

    GxRsPush();
    GxVertexShaderSelect(GxVS_PassThru);
    GxRsSet(GxRs_Texture0, static_cast<CGxTex*>(nullptr));
    GxRsSet(GxRs_Blend, GxBlend_Opaque);
    GxRsSet(GxRs_Lighting, 0);
    GxRsSet(GxRs_Fog, 0);
    GxRsSet(GxRs_DepthTest, 0);
    GxRsSet(GxRs_Culling, 0);
    GxPrimLockVertexPtrs(m_nVerts, m_geoVerts.m_data, sizeof(NTempest::C3Vector),
                         nullptr, 0, m_clrVerts.m_data, sizeof(NTempest::CImVector),
                         nullptr, 0, nullptr, 0, nullptr, 0);
    GxPrimDrawElements(GxPrim_TriangleStrip, m_nIndices, m_indices.m_data);
    GxPrimUnlockVertexPtrs();
    GxRsPop();

    GxXformSetView(savedView);
    GxXformPop(GxXform_World);
    GxXformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void DNPlanet::GenGeometry(NTempest::C3Vector* geo, NTempest::C2Vector* tex,
                           NTempest::CImVector* color, std::uint16_t* indices,
                           unsigned int* vertexCount, unsigned int* indexCount) const {
    if (geo == nullptr || tex == nullptr || color == nullptr || indices == nullptr ||
        vertexCount == nullptr || indexCount == nullptr) return;
    const float scale = m_scale == 0.0F ? m_baseScale : m_scale;
    const NTempest::C3Vector corners[4] = {
        {m_pos.x, m_pos.y - scale, m_pos.z + scale},
        {m_pos.x, m_pos.y + scale, m_pos.z + scale},
        {m_pos.x, m_pos.y - scale, m_pos.z - scale},
        {m_pos.x, m_pos.y + scale, m_pos.z - scale},
    };
    const NTempest::C2Vector uv[4] = {{0, 0}, {1, 0}, {0, 1}, {1, 1}};
    for (int i = 0; i < 4; ++i) { geo[i] = corners[i]; tex[i] = uv[i]; color[i] = m_color; }
    indices[0] = 0; indices[1] = 2; indices[2] = 1; indices[3] = 1; indices[4] = 2; indices[5] = 3;
    *vertexCount = 4;
    *indexCount = 6;
}

void DNStars::Update() {
    m_pos = s_dnInfo.cameraPos;
    m_color.a = static_cast<std::uint8_t>(std::clamp(
        InterpTable(m_fadeTable, 4, s_dnInfo.dayProgression) * 254.0 + 1.0, 0.0, 255.0));
}

void __fastcall DNClouds::Callback_GxTex(unsigned int width, int stride, unsigned int, unsigned int,
                                          unsigned int mipLevel, void* userArg,
                                          unsigned int* texelStride, const void** texels) {
    if (width == 1 && mipLevel == 0 && texelStride != nullptr && texels != nullptr) {
        *texelStride = 4U * static_cast<unsigned int>(stride);
        *texels = userArg;
    }
}

void __fastcall DayNightSkyTexCallback(unsigned int width, unsigned int stride, unsigned int,
                                       unsigned int, unsigned int mipLevel, void* userArg,
                                       unsigned int* texelStride, const void** texels) {
    if (width == 1 && mipLevel == 0 && texelStride != nullptr && texels != nullptr) {
        *texelStride = 4U * stride;
        *texels = userArg;
    }
}

int ResetLightPos() {
    s_dnInfo.playerPos = NTempest::C3Vector{};
    s_dnInfo.cameraPos = NTempest::C3Vector{};
    s_dnInfo.cameraDir = NTempest::C3Vector(0.0F, 1.0F, 0.0F);
    s_dnInfo.faceAngle = 0.0F;
    return 0;
}

int __fastcall ConsoleCommand_SkyCloudDensity(const char* args, const char*) {
    if (args != nullptr) s_clouds.SetDensity(static_cast<float>(std::atof(args)));
    return 0;
}

std::uint8_t __fastcall CloudLODCallback(const char*, int, const char* newValue, void*) {
    if (newValue != nullptr)
        s_clouds.SetLOD(static_cast<unsigned int>(std::max(std::atoi(newValue), 0)), s_clouds.m_updateSize);
    return 1;
}

int __fastcall ConsoleCommand_SkyCloudLayers(const char* args, const char*) {
    if (args != nullptr) s_clouds.m_nLayers = static_cast<unsigned int>(std::max(std::atoi(args), 0));
    return 0;
}

int __fastcall ConsoleCommand_SkySunGlare(const char* args, const char*) {
    if (args != nullptr) s_sunGlare.m_enabled = std::atoi(args) != 0;
    return 0;
}

int __fastcall ConsoleCommand_SkyShow(const char* args, const char*) {
    if (args != nullptr) s_dnInfo.showSky = std::atoi(args) != 0;
    return 0;
}

void DayNightInitialize(LightGroup* lightGroup) {
    if (lightGroup != nullptr) g_areaLights = *lightGroup;
    ResetLightPos();
    s_sky.GenSphere(1.0F);
    s_clouds.GenSphere(22.222221F);
    s_clouds.SetLOD(6, 32);
    s_clouds.SetDensity(0.60000002F);
    s_clouds.SetSharpness(0.95999998F);
    s_clouds.m_nLayers = 1;
    s_dnInfo.showSky = 1;
    s_sunGlare.m_enabled = 1;
    s_sunGlare.m_baseScale = 1.0F;
    s_sunGlare.m_fadeRate = 1.0F;
    s_sunGlare.m_alphaMin = 0.5F;
    s_sunGlare.m_alphaMax = 1.0F;
    s_sunGlare.m_scaleMin = 0.5F;
    s_sunGlare.m_scaleMax = 6.0F;
    s_sunGlare.m_dotMin = 0.7F;
    s_sunGlare.m_fadeTable[0] = {0.27083334F, 0.0F};
    s_sunGlare.m_fadeTable[1] = {0.3125F, 1.0F};
    s_sunGlare.m_fadeTable[2] = {0.8125F, 1.0F};
    s_sunGlare.m_fadeTable[3] = {0.875F, 0.0F};
    s_moonGlare.m_enabled = 1;
    s_moonGlare.m_baseScale = 2.0F;
    s_moonGlare.m_fadeRate = 1.0F;
    s_moonGlare.m_scaleMin = 1.0F;
    s_moonGlare.m_scaleMax = 1.0F;
    s_moonGlare.m_dotMin = 0.7F;
    s_stars.m_fadeTable[0] = {0.0F, 0.0F};
    s_stars.m_fadeTable[1] = {0.25F, 1.0F};
    s_stars.m_fadeTable[2] = {0.75F, 1.0F};
    s_stars.m_fadeTable[3] = {1.0F, 0.0F};
    s_initialized_2 = true;
}

void DayNightDestroy() {
    s_clouds.Destroy();
    ClearArray(s_sky.m_indices); ClearArray(s_sky.m_clrVerts); ClearArray(s_sky.m_geoVerts);
    g_areaLights.m_lightData.SetCount(0);
    s_initialized_2 = false;
}

void DayNightForceFullUpdate() {
    SetColors();
    SetDirection();
    SetPlanets();
    s_clouds.FullUpdate();
}

void SetColors() {
    // The outdoor branch of recovered SetColors (0x6BB5D0) evaluates the
    // first light group at dayProgression * 2880.  Additional spatial area
    // lights are blended by DoAreaLights; group zero itself must not be
    // replaced by its first, usually night-time, marker.
    if (g_areaLights.m_lightData.m_count != 0) {
        LightData& base = g_areaLights.m_lightData.m_data[0];
        const int timeBand = static_cast<int>(
            s_dnInfo.dayProgression * 2880.0F - 0.5F);
        CalcLightColors(timeBand, &s_dnInfo.light, &base.m_lightdata,
                        &base.m_stormdata,
                        static_cast<int>(s_dnInfo.stormPercentage * 100.0F));
        DoAreaLights();
    } else {
        // Exact recovered no-Lights.lit fallback: all color channels are
        // white and fog is effectively at infinity.
        std::memset(&s_dnInfo.light, 0xFF, sizeof(s_dnInfo.light));
        s_dnInfo.light.FogEnd = 1.0e10F;
        s_dnInfo.light.FogStartScalar = 0.5F;
    }

    SetLightColors();
    SetFogColors();
    s_sky.SetColors();
    s_dnInfo.sidn = static_cast<float>(DayNightSI(0.0F));
    s_dnInfo.unitSelect = static_cast<float>(DayNightUnitSelectColor());
}

void DoAreaLights() {
    // Recovered DoAreaLights starts at light-group index one.  Group zero is
    // the already-interpolated map-wide light selected above.  Spatial area
    // blending is restored separately as those map-volume queries come back.
}

void __fastcall ScaleOutputs(CurrentLight* areaLight, std::uint8_t* color, float scale) {
    if (areaLight == nullptr) return;
    auto scaleColor = [scale](NTempest::CImVector& value) {
        value.r = static_cast<std::uint8_t>(std::clamp(value.r * scale, 0.0F, 255.0F));
        value.g = static_cast<std::uint8_t>(std::clamp(value.g * scale, 0.0F, 255.0F));
        value.b = static_cast<std::uint8_t>(std::clamp(value.b * scale, 0.0F, 255.0F));
    };
    scaleColor(areaLight->DirectColor); scaleColor(areaLight->AmbientColor);
    for (auto& value : areaLight->SkyArray) scaleColor(value);
    for (auto& value : areaLight->CloudArray) scaleColor(value);
    for (auto& value : areaLight->WaterArray) scaleColor(value);
    areaLight->FogEnd *= scale; areaLight->FogStartScalar *= scale; areaLight->Darkness *= scale;
    if (color != nullptr) for (int i = 0; i < 3; ++i)
        color[i] = static_cast<std::uint8_t>(std::clamp(color[i] * scale, 0.0F, 255.0F));
}

int SetLightColors() {
    s_dnInfo.lightInfo.dirColor = s_dnInfo.light.DirectColor;
    s_dnInfo.lightInfo.ambColor = s_dnInfo.light.AmbientColor;
    s_dnInfo.lightInfo.windowDirColor = s_dnInfo.light.DirectColor;
    s_dnInfo.lightInfo.windowAmbColor = s_dnInfo.light.AmbientColor;
    return 1;
}

std::uint8_t SetFogColors() {
    // Recovered SetFogColors (0x6BC780): FogEnd is clipped to the active
    // camera range, FogStartScalar is a fraction of that clipped distance,
    // and the horizon color is SkyArray[5] (not the ambient-light color).
    const float farClip = s_dnInfo.farClip > 0.0F
        ? s_dnInfo.farClip : s_dnInfo.light.FogEnd;
    s_dnInfo.fogInfo.end = (std::min)(s_dnInfo.light.FogEnd, farClip);
    s_dnInfo.fogInfo.start = s_dnInfo.light.FogStartScalar
        * s_dnInfo.fogInfo.end;
    s_dnInfo.fogInfo.color = s_dnInfo.light.SkyArray[5];
    return 1;
}

void SetDirection() {
    // Recovered SetDirection (0x6BCA40).  The sun azimuth is fixed at 225
    // degrees while its elevation follows this four-key cyclic table.
    static const NTempest::C2Vector phiTable[4] = {
        {0.00F, kPi * 0.70555556F},
        {0.25F, kPi * 0.61111110F},
        {0.50F, kPi * 0.70555556F},
        {0.75F, kPi * 0.61111110F},
    };
    static const NTempest::C2Vector thetaTable[4] = {
        {0.00F, kPi * 1.25F}, {0.25F, kPi * 1.25F},
        {0.50F, kPi * 1.25F}, {0.75F, kPi * 1.25F},
    };
    const float phi = static_cast<float>(
        InterpTable(phiTable, std::size(phiTable), s_dnInfo.dayProgression));
    const float theta = static_cast<float>(
        InterpTable(thetaTable, std::size(thetaTable), s_dnInfo.dayProgression));
    const float sinPhi = std::sin(phi);
    s_dnInfo.lightInfo.dir = {
        std::cos(theta) * sinPhi,
        std::sin(theta) * sinPhi,
        std::cos(phi),
    };
}

void SetPlanets() {
    const float angle = s_dnInfo.dayProgression * kTwoPi;
    for (int i = 0; i < 3; ++i) {
        const float phase = angle * s_planets[i].m_period + i * kPi;
        s_planets[i].m_pos = s_dnInfo.cameraPos +
            NTempest::C3Vector(std::cos(phase) * 100.0F, std::sin(phase) * 100.0F, 50.0F);
        s_planets[i].m_scale = s_planets[i].m_baseScale;
    }
}

void DayNightUpdateLighting() {
    const float x = s_dnInfo.cameraDir.x;
    s_dnInfo.faceAngle = (s_dnInfo.cameraDir.y * s_dnInfo.cameraDir.y + x * x <= 0.0001F)
        ? std::atan2(s_dnInfo.cameraDir.z, x) : std::atan2(s_dnInfo.cameraDir.y, x);
    if (s_dnInfo.faceAngle < 0.0F) s_dnInfo.faceAngle += kTwoPi;
    if (!s_paused_1) {
        SetColors(); SetDirection(); SetPlanets();
        s_clouds.Update(); s_stars.Update();
    }
}

void __fastcall DayNightSetEclipse(NTempest::CImVector color, float amount) {
    s_dnInfo.eclipseColor = color;
    s_dnInfo.eclipseAmount = static_cast<std::uint8_t>(std::clamp(amount * 255.0F, 0.0F, 255.0F));
}

double __fastcall DayNightSI(float offset) {
    return InterpTable(s_sidnTable, 4, s_dnInfo.dayProgression + offset);
}

double __fastcall DayNightUnitSelectColor() {
    return InterpTable(s_unitColorTable, 2, s_dnInfo.dayProgression);
}

DNInfo* __fastcall DayNightGetInfo() { return &s_dnInfo; }

void DayNightRenderGlares() {
    if (!s_dnInfo.showSky) return;
    s_sunGlare.Update(s_dnInfo.elapsedSec);
    s_moonGlare.Update(s_dnInfo.elapsedSec);
}

void DayNightRenderSky() {
    if (s_dnInfo.showSky) {
        GxSceneSetClearColor(NTempest::CImVector(0xff000000U));
        if (!GxMasterEnable(GxMasterEnable_PolygonFill))
            GxSceneClear(3U);
        s_sky.Render();
    } else {
        GxSceneSetClearColor(NTempest::CImVector(0xff000000U));
        GxSceneClear(3U);
    }
}

int ValueTableInit() {
    ResetTableColors();
    s_dnInfo.billbRescale = 1.0F;
    return 0;
}

int _E151_4() { return _E148_4() + _E150_5(); }
int _E148_4() { ClearLight(s_magmaLight); return 0; }
int _E150_5() { return 0; }
void _E149_4() {}
int _E156_3() { return _E153_5() + _E155_1(); }
int _E153_5() { ClearLight(s_slimeLight); return 0; }
int _E155_1() { return 0; }
void _E154_2() {}
int _E161_3() { _E158_4(); return _E160_2(); }
NTempest::C4Vector* _E158_4() { static NTempest::C4Vector value; value = NTempest::C4Vector{}; return &value; }
int _E160_2() { return 0; }
void _E159_4() {}
int _E167_1() { _E163_4(); return 0; }
void _E163_4() { s_sky = DNSky{}; }
int _E166_1() { _E164_3(); return 0; }
void _E164_3() {}
int _E172_1() { _E169_3(); return _E171_2(); }
void _E169_3() { s_clouds = DNClouds{}; }
int _E171_2() { return 0; }
void _E170_2() {}
int _E175_2() { _E174_3(); return 0; }
void _E174_3() {}
int _E178_1() { _E177_3(); return 0; }
void _E177_3() {}
int _E183_7() { _E180_3(); return _E182_8(); }
void _E180_3() { g_areaLights = LightGroup{}; }
int _E182_8() { return 0; }
void _E181_0() {}
int _E188_9() { _E185_16(); return _E187_11(); }
void _E185_16() { std::memset(s_dnInfo.light.CloudData, 0, sizeof(s_dnInfo.light.CloudData)); }
int _E187_11() { return 0; }
void _E186_9() {}
int _E193_9() { _E190_9(); return _E192_10(); }
void _E190_9() { std::memset(s_dnInfo.light.SkyArray, 0, sizeof(s_dnInfo.light.SkyArray)); }
int _E192_10() { return 0; }
void _E191_7() {}
int _E198_9() { _E195_8(); return _E197_7(); }
void _E195_8() { std::memset(s_dnInfo.light.WaterArray, 0, sizeof(s_dnInfo.light.WaterArray)); }
int _E197_7() { return 0; }
void _E196_6() {}
int _E203_5() { _E200_7(); return _E202_6(); }
void _E200_7() { s_dnInfo.light.DirectColor = NTempest::CImVector(0); }
int _E202_6() { return 0; }
void _E201_6() {}
int _E208_1() { return _E205_5(); }
int _E205_5() { _E207_4(); return 0; }
int _E207_4() { return 0; }
void _E206_2() {}
int _E213_2() { _E210_3(); return _E212_2(); }
DNClouds* _E210_3() { return &s_clouds; }
int _E212_2() { return 0; }
void _E211_1() {}
int _E218_1() { _E215_1(); return _E217_2(); }
int _E215_1() { g_areaLights.m_lightData.SetCount(0); return 0; }
int _E217_2() { return 0; }
void _E216_0() { g_areaLights.m_lightData.SetCount(0); }
int _E223_3() { _E220_2(); return _E222_3(); }
int* _E220_2() { static int values[32]{}; std::fill(std::begin(values), std::end(values), 0); return values; }
int _E222_3() { return 0; }
void _E221_0() {}
int _E228_4() { _E225_3(); return _E227_2(); }
NTempest::C2Vector* _E225_3() { static NTempest::C2Vector values[4]{}; return values; }
int _E227_2() { return 0; }
void _E226_1() {}
int _E233_2() { _E230_1(); return _E232_3(); }
NTempest::C2Vector* _E230_1() { static NTempest::C2Vector values[4]{}; return values; }
int _E232_3() { return 0; }
void _E231_1() {}
int _E238_2() { _E235_2(); return _E237_1(); }
void _E235_2() { s_dnInfo = DNInfo{}; }
int _E237_1() { return 0; }
void _E236_0() {}
int _E260_1() { _E256_0(); return _E259_1(); }
void _E256_0() { s_sunGlare = DNSunGlare{}; }
int _E259_1() { return 0; }
void _E257_0() {}
int _E265_1() { _E262_0(); return _E264_1(); }
void _E262_0() { s_moonGlare = DNMoonGlare{}; }
int _E264_1() { return 0; }
void _E263_1() {}
void _E269_1() {}
void _E268_1() {}
