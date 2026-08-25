#include "WoW/Source/Game/GameClient/minimap.h"

#include "WoW/Source/DB/DBClient/AutoCode/areapoirec.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <limits>
#include <string>

float INFINITY_193 = 0.0F;
CMinimap g_minimap;

namespace {
constexpr float AREA_WORLD_SIZE = 533.33331F;
constexpr float HALF_WORLD_SIZE = 17066.666F;
constexpr std::array<unsigned, 6> CHUNKS_PER_ZOOM = {8, 6, 4, 3, 2, 1};

int s_initializerCount = 0;
NTempest::C3Vector s_currentPosition{};
NTempest::C2iVector s_currentUpperLeftArea{};
NTempest::C2iVector s_currentLowerRightArea{};
NTempest::C44Matrix s_mapObjInvMtx{};
NTempest::CAaBox s_queryCenterBox{};
NTempest::C3Vector s_queryCenter{};
AreaPOIRec s_questPOI{};
char s_questPOIName[64]{};
unsigned s_updatePOI = 0;
bool s_updateDistantPOI = false;
bool s_questHasPosition = false;

void CopyString(char* destination, std::size_t capacity, const char* source) {
    if (destination == nullptr || capacity == 0) {
        return;
    }
    const char* text = source != nullptr ? source : "";
    std::snprintf(destination, capacity, "%s", text);
}

float DistanceSquared(const NTempest::C3Vector& a, float x, float y) {
    const float dx = a.x - x;
    const float dy = a.y - y;
    return dx * dx + dy * dy;
}
}

int _E193_10() { _E190_10(); return _E192_11(); }

void _E190_10() { ++s_initializerCount; }

int _E192_11() { return std::atexit(_E191_8); }

void _E191_8() {
    if (s_initializerCount > 0) {
        --s_initializerCount;
    }
}

int _E197_8() { return _E196_7(), 0; }

void _E196_7() { g_minimap.m_viewRadius = AREA_WORLD_SIZE; }

int _E200_8() { return _E199_8(), 0; }

void _E199_8() { g_minimap.m_worldRadius = AREA_WORLD_SIZE; }

int _E203_6() { return _E202_7(), 0; }

void _E202_7() { INFINITY_193 = std::numeric_limits<float>::infinity(); }

int _E206_3() { return _E205_6(), 0; }

void _E205_6() { g_minimap.m_worldRadius = HALF_WORLD_SIZE; }

int _E211_2() { _E208_2(); return _E210_4(); }

void _E208_2() { s_currentPosition = NTempest::C3Vector(0.0F, 0.0F, 0.0F); }

int _E210_4() { return std::atexit(_E209_2); }

void _E209_2() {}

int _E216_1() { _E213_3(); return _E215_2(); }

int _E213_3() { s_currentUpperLeftArea = NTempest::C2iVector(0, 0); return 0; }

int _E215_2() { return std::atexit(_E214_2); }

void _E214_2() {}

int _E221_1() { _E218_2(); return _E220_3(); }

int _E218_2() { s_currentLowerRightArea = NTempest::C2iVector(0, 0); return 0; }

int _E220_3() { return std::atexit(_E219_2); }

void _E219_2() {}

int _E226_2() { _E223_4(); return _E225_4(); }

void _E223_4() { s_mapObjInvMtx = NTempest::C44Matrix(); }

int _E225_4() { return std::atexit(_E224_3); }

void _E224_3() {}

int _E231_2() { _E228_5(); return _E230_2(); }

void _E228_5() { s_queryCenterBox = NTempest::CAaBox(); }

int _E230_2() { return std::atexit(_E229_3); }

void _E229_3() {}

int _E236_1() { _E233_3(); return _E235_3(); }

void _E233_3() { s_queryCenter = NTempest::C3Vector(0.0F, 0.0F, 0.0F); }

int _E235_3() { return std::atexit(_E234_2); }

void _E234_2() {}

int _E241_0() { _E238_3(); return _E240_2(); }

const void* _E238_3() { return &s_questPOI; }

int _E240_2() { return std::atexit(_E239_2); }

void _E239_2() {}

int _E246_3() { _E243_2(); return _E245_3(); }

int _E243_2() { g_minimap.m_visiblePOI.clear(); return 0; }

int _E245_3() { return std::atexit(_E244_1); }

void _E244_1() { g_minimap.m_visiblePOI.clear(); }

int _E251_1() { _E248_3(); return _E250_2(); }

int _E248_3() { g_minimap.m_distantPOI.clear(); return 0; }

int _E250_2() { return std::atexit(_E249_3); }

void _E249_3() { g_minimap.m_distantPOI.clear(); }

int _E256_1() { _E253_2(); return _E255_2(); }

int _E253_2() { s_questPOI = AreaPOIRec(); return 0; }

int _E255_2() { return std::atexit(_E254_2); }

void _E254_2() {}

int _E261_0() { _E258_1(); return _E260_2(); }

int _E258_1() { g_minimap.m_quads = {}; return 0; }

int _E260_2() { return std::atexit(_E259_2); }

void _E259_2() { g_minimap.m_quads = {}; }

int _E266_0() { return _E265_2(); }

void _E263_2() { s_updatePOI = 1; }

int _E265_2() { _E263_2(); return 0; }

void _E264_2() { s_updateDistantPOI = false; }

int CMinimap::Initialize() {
    _E190_10();
    _E196_7();
    _E199_8();
    _E202_7();
    _E208_2();
    _E213_3();
    _E218_2();
    _E223_4();
    _E228_5();
    _E233_3();
    _E253_2();
    m_questPOI = &s_questPOI;
    m_initialized = true;
    m_zoom = 0;
    m_insideZoom = 0;
    UpdatePointsOfInterest();
    return 1;
}

void CMinimap::Shutdown() {
    m_visiblePOI.clear();
    m_distantPOI.clear();
    m_questPOI = nullptr;
    m_initialized = false;
    m_inside = false;
    m_continent = -1;
    s_updatePOI = 0;
    s_updateDistantPOI = false;
}

void CMinimap::SetZoom(unsigned zoom) {
    const unsigned bounded = zoom >= m_zoomLevels ? m_zoomLevels - 1 : zoom;
    unsigned& activeZoom = m_inside ? m_insideZoom : m_zoom;
    if (activeZoom != bounded) {
        activeZoom = bounded;
        m_poiDirty = true;
        s_updatePOI = 1;
    }
}

unsigned CMinimap::GetZoom() const { return m_inside ? m_insideZoom : m_zoom; }

unsigned CMinimap::GetZoomLevels() const { return 6; }

double CMinimap::GetViewRadius() const {
    if (m_inside) {
        return AREA_WORLD_SIZE / static_cast<double>(1U << m_insideZoom);
    }
    const unsigned zoom = GetZoom();
    return static_cast<double>(CHUNKS_PER_ZOOM[zoom < CHUNKS_PER_ZOOM.size() ? zoom : 0]) *
           0.5 * 33.333332;
}

double CMinimap::GetWorldRadius() const {
    if (m_inside) {
        return AREA_WORLD_SIZE / static_cast<double>(1U << m_insideZoom);
    }
    return GetViewRadius();
}

const AreaPOIRec* CMinimap::GetPOI() const {
    if (!m_visiblePOI.empty()) {
        return m_visiblePOI.front();
    }
    return m_questPOI;
}

int CMinimap::GetDistantPOI() const {
    return static_cast<int>(m_distantPOI.size());
}

void CMinimap::SetQuestPOI(const char* name, float x, float y) {
    m_questPOI = &s_questPOI;
    s_questPOI.m_x = x;
    s_questPOI.m_y = y;
    s_questPOI.m_z = 0.0F;
    s_questPOI.m_importance = 1;
    s_questPOI.m_icon = 5;
    s_questHasPosition = std::fabs(x) >= 0.000000238F || std::fabs(y) >= 0.000000238F;
    CopyString(s_questPOIName, sizeof(s_questPOIName), name);
    CopyString(s_questPOI.m_name_lang[0], sizeof(s_questPOI.m_name_lang[0]), s_questPOIName);
    m_poiDirty = true;
    s_updatePOI = 1;
    s_updateDistantPOI = true;
    UpdatePointsOfInterest();
}

void CMinimap::SetCurrentPosition(int continent, const NTempest::C3Vector& position) {
    m_continent = continent;
    m_currentPosition = position;
    s_currentPosition = position;
    UpdatePointsOfInterest();
}

int MinimapInitialize(int continent) {
    const int result = g_minimap.Initialize();
    g_minimap.m_continent = continent;
    return result;
}

void MinimapShutdown() { g_minimap.Shutdown(); }

int LoadMD5Names() { return 1; }

void SetupMapObj(CMapEntity* entity, int matrix) {
    g_minimap.m_inside = entity != nullptr && matrix != 0;
    s_updatePOI = 1;
}

int MinimapUpdate(const NTempest::C44Matrix*, const NTempest::C3Vector* continent,
                  const NTempest::C3Vector* position, NTempest::C2Vector* center,
                  float* radius, QUADDATA* quads, float) {
    if (position == nullptr || center == nullptr || radius == nullptr) {
        return 0;
    }
    g_minimap.SetCurrentPosition(g_minimap.m_continent, *position);
    if (quads != nullptr) {
        const float coordinates[2] = {position->x, position->y};
        unsigned area[2]{};
        CoordinateToArea(area, const_cast<float*>(coordinates));
        SetupTextureHandles(area, 0, quads);
    }
    center->x = 0.5F;
    center->y = 0.5F;
    *radius = static_cast<float>(g_minimap.GetViewRadius());
    return 1;
}

void SetupQuad(QUADDATA* quad, int textureHandle, const MINIMAPQUAD* source,
               float, const char*) {
    if (quad == nullptr || source == nullptr) {
        return;
    }
    quad->texture = static_cast<std::uintptr_t>(textureHandle);
    quad->areaX = source->area.x;
    quad->areaY = source->area.y;
    quad->flags |= 2U;
    quad->u = source->bounds.b.x;
    quad->v = source->bounds.b.y;
}

int MinimapUpdatePosition(const NTempest::C3Vector* continent, float* position,
                          NTempest::C2Vector* center, float* radius, QUADDATA* quads) {
    if (position == nullptr || center == nullptr || radius == nullptr) {
        return 0;
    }
    NTempest::C3Vector world(position[0], position[1], position[2]);
    g_minimap.SetCurrentPosition(g_minimap.m_continent, world);
    unsigned area[2]{};
    CoordinateToArea(area, position);
    if (quads != nullptr) {
        SetupTextureHandles(area, 0, quads);
    }
    center->x = 0.5F;
    center->y = 0.5F;
    *radius = static_cast<float>(g_minimap.GetViewRadius());
    return 1;
}

void UpdatePointsOfInterest() {
    g_minimap.m_visiblePOI.clear();
    g_minimap.m_distantPOI.clear();
    if (s_questHasPosition && g_minimap.m_questPOI != nullptr) {
        const float radius = static_cast<float>(g_minimap.GetWorldRadius());
        const float distance = DistanceSquared(g_minimap.m_currentPosition,
                                               g_minimap.m_questPOI->m_x,
                                               g_minimap.m_questPOI->m_y);
        if (distance <= radius * radius) {
            g_minimap.m_visiblePOI.push_back(g_minimap.m_questPOI);
        } else {
            POIDIRECTIONDATA distant{};
            CopyString(distant.POIName, sizeof(distant.POIName), s_questPOIName);
            distant.rotation = std::atan2(g_minimap.m_questPOI->m_y - g_minimap.m_currentPosition.y,
                                          g_minimap.m_questPOI->m_x - g_minimap.m_currentPosition.x);
            g_minimap.m_distantPOI.push_back(distant);
        }
    }
    g_minimap.m_poiDirty = false;
    s_updatePOI = 0;
    s_updateDistantPOI = false;
}

NTempest::C2iVector* CoordinateToArea(unsigned* result, float* worldPosition) {
    if (result == nullptr || worldPosition == nullptr) {
        return reinterpret_cast<NTempest::C2iVector*>(result);
    }
    const float yArea = (HALF_WORLD_SIZE - worldPosition[1]) / AREA_WORLD_SIZE;
    const float xArea = (HALF_WORLD_SIZE - worldPosition[0]) / AREA_WORLD_SIZE;
    result[0] = static_cast<unsigned>(std::floor(yArea));
    result[1] = static_cast<unsigned>(std::floor(xArea));
    return reinterpret_cast<NTempest::C2iVector*>(result);
}

NTempest::C3Vector* AreaToCoordinate(int, int* area) {
    static NTempest::C3Vector result{};
    if (area == nullptr) {
        result = NTempest::C3Vector();
        return &result;
    }
    result.x = HALF_WORLD_SIZE - static_cast<float>(area[1]) * AREA_WORLD_SIZE;
    result.y = HALF_WORLD_SIZE - static_cast<float>(area[0]) * AREA_WORLD_SIZE;
    result.z = 0.0F;
    return &result;
}

void SetupTextureHandles(unsigned* area, unsigned, QUADDATA* quads) {
    if (area == nullptr || quads == nullptr) {
        return;
    }
    static constexpr std::array<NTempest::C2iVector, 4> offsets = {
        NTempest::C2iVector(-1, -1), NTempest::C2iVector(0, -1),
        NTempest::C2iVector(-1, 0), NTempest::C2iVector(0, 0)};
    for (unsigned index = 0; index < 4; ++index) {
        quads[index].areaX = static_cast<int>(area[0]) + offsets[index].x;
        quads[index].areaY = static_cast<int>(area[1]) + offsets[index].y;
        quads[index].u = static_cast<float>(index & 1U);
        quads[index].v = static_cast<float>((index >> 1U) & 1U);
        quads[index].flags |= 2U;
    }
}

void BuildPathName(char* area, char* output, unsigned size) {
    if (output == nullptr || size == 0) {
        return;
    }
    output[0] = 0;
    if (area == nullptr) {
        return;
    }
    const auto* coordinates = reinterpret_cast<const int*>(area);
    std::snprintf(output, size, "World\\Minimaps\\%d_%d.blp", coordinates[0], coordinates[1]);
}

void MinimapSetZoom(unsigned zoom) { g_minimap.SetZoom(zoom); }
unsigned MinimapGetZoom() { return g_minimap.GetZoom(); }
unsigned MinimapGetZoomLevels() { return g_minimap.GetZoomLevels(); }
double MinimapGetViewRadius() { return g_minimap.GetViewRadius(); }

const void* MinimapGetPOI(void* result) {
    const AreaPOIRec* poi = g_minimap.GetPOI();
    if (result != nullptr) {
        *reinterpret_cast<const AreaPOIRec**>(result) = poi;
    }
    return poi;
}

int MinimapGetDistantPOI(void* result) {
    if (result != nullptr) {
        auto* output = reinterpret_cast<std::vector<POIDIRECTIONDATA>*>(result);
        *output = g_minimap.m_distantPOI;
    }
    return g_minimap.GetDistantPOI();
}

double MinimapGetWorldRadius() { return g_minimap.GetWorldRadius(); }

void MinimapSetQuestPOI(const char* name, char* title, float x, float y) {
    g_minimap.SetQuestPOI(title != nullptr ? title : name, x, y);
}

void MinimapGetPartyMembers(float, CGUnit_C*) {}
