#pragma once

#include "ENGINE/Source/Tempest/c2ivector.h"
#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/caabox.h"

#include <array>
#include <cstdint>
#include <vector>

class AreaPOIRec;
struct CMapEntity;
struct CGUnit_C;

struct QUADDATA {
    std::uintptr_t texture = 0;
    float u = 0.0F;
    float v = 0.0F;
    std::uint32_t flags = 0;
    int areaX = 0;
    int areaY = 0;
};

struct POIDIRECTIONDATA {
    char POIName[64]{};
    float rotation = 0.0F;
};

struct MINIMAPQUAD {
    NTempest::C2iVector area{};
    NTempest::CAaBox bounds{};
    unsigned groupNum = 0;
};

class CMinimap {
public:
    int Initialize();
    void Shutdown();
    void SetZoom(unsigned);
    unsigned GetZoom() const;
    unsigned GetZoomLevels() const;
    double GetViewRadius() const;
    double GetWorldRadius() const;
    const AreaPOIRec* GetPOI() const;
    int GetDistantPOI() const;
    void SetQuestPOI(const char*, float, float);
    void SetCurrentPosition(int, const NTempest::C3Vector&);
    bool IsInside() const { return m_inside; }

    unsigned m_zoom = 0;
    unsigned m_insideZoom = 0;
    unsigned m_zoomLevels = 6;
    float m_viewRadius = 533.33331F;
    float m_worldRadius = 533.33331F;
    bool m_initialized = false;
    bool m_inside = false;
    bool m_poiDirty = false;
    int m_continent = -1;
    NTempest::C3Vector m_currentPosition{};
    NTempest::C2iVector m_upperLeftArea{};
    NTempest::C2iVector m_lowerRightArea{};
    AreaPOIRec* m_questPOI = nullptr;
    std::vector<const AreaPOIRec*> m_visiblePOI;
    std::vector<POIDIRECTIONDATA> m_distantPOI;
    std::array<QUADDATA, 4> m_quads{};
};

extern CMinimap g_minimap;
int MinimapInitialize(int);
void MinimapShutdown();
int LoadMD5Names();
void SetupMapObj(CMapEntity*, int);
int MinimapUpdate(const NTempest::C44Matrix*, const NTempest::C3Vector*,
                  const NTempest::C3Vector*, NTempest::C2Vector*, float*, QUADDATA*, float);
void SetupQuad(QUADDATA*, int, const MINIMAPQUAD*, float, const char*);
int MinimapUpdatePosition(const NTempest::C3Vector*, float*, NTempest::C2Vector*, float*, QUADDATA*);
void UpdatePointsOfInterest();
NTempest::C2iVector* CoordinateToArea(unsigned*, float*);
NTempest::C3Vector* AreaToCoordinate(int, int*);
void SetupTextureHandles(unsigned*, unsigned, QUADDATA*);
void BuildPathName(char*, char*, unsigned);
void MinimapSetZoom(unsigned);
unsigned MinimapGetZoom();
unsigned MinimapGetZoomLevels();
double MinimapGetViewRadius();
const void* MinimapGetPOI(void*);
int MinimapGetDistantPOI(void*);
double MinimapGetWorldRadius();
void MinimapSetQuestPOI(const char*, char*, float, float);
void MinimapGetPartyMembers(float, CGUnit_C*);

extern float INFINITY_193;

int _E193_10(); void _E190_10(); int _E192_11(); void _E191_8();
int _E197_8(); void _E196_7(); int _E200_8(); void _E199_8();
int _E203_6(); void _E202_7(); int _E206_3(); void _E205_6();
int _E211_2(); void _E208_2(); int _E210_4(); void _E209_2();
int _E216_1(); int _E213_3(); int _E215_2(); void _E214_2();
int _E221_1(); int _E218_2(); int _E220_3(); void _E219_2();
int _E226_2(); void _E223_4(); int _E225_4(); void _E224_3();
int _E231_2(); void _E228_5(); int _E230_2(); void _E229_3();
int _E236_1(); void _E233_3(); int _E235_3(); void _E234_2();
int _E241_0(); const void* _E238_3(); int _E240_2(); void _E239_2();
int _E246_3(); int _E243_2(); int _E245_3(); void _E244_1();
int _E251_1(); int _E248_3(); int _E250_2(); void _E249_3();
int _E256_1(); int _E253_2(); int _E255_2(); void _E254_2();
int _E261_0(); int _E258_1(); int _E260_2(); void _E259_2();
int _E266_0(); void _E263_2(); int _E265_2(); void _E264_2();
