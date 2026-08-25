#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/c4vector.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "Storm/h/stpl.h"
#include "WoW/common/lightsandfog.h"

#include <cstdint>

struct CGxTex;
struct HMODEL__;

struct DNFogInfo {
    NTempest::CImVector color;
    float start = 0.0F;
    float end = 0.0F;
};

struct DNLightInfo {
    NTempest::C3Vector dir;
    NTempest::CImVector dirColor;
    NTempest::CImVector ambColor;
    NTempest::CImVector windowDirColor;
    NTempest::CImVector windowAmbColor;
    NTempest::C4Vector shaderShadowColor;
};

struct DNInfo {
    unsigned int time = 0;
    float dayProgression = 0.0F;
    float day = 0.0F;
    NTempest::C3Vector playerPos;
    NTempest::C3Vector cameraPos;
    NTempest::C3Vector cameraDir;
    float faceAngle = 0.0F;
    float nearClipScaled = 0.0F;
    float farClip = 0.0F;
    float elapsedSec = 0.0F;
    float stormPercentage = 0.0F;
    std::uint8_t eclipseAmount = 0;
    NTempest::CImVector eclipseColor;
    CurrentLight light;
    DNFogInfo fogInfo;
    std::uint8_t intFog = 0;
    DNFogInfo intFogInfo;
    DNLightInfo lightInfo;
    NTempest::CImVector shadowClr;
    float billbRescale = 1.0F;
    int showSky = 1;
    CGxTex* cloudTex = nullptr;
    NTempest::C2Vector sunPosTexPt;
    float sunMoonPath = 0.0F;
    float sidn = 0.0F;
    float unitSelect = 0.0F;
};

struct GlareBase {
    int m_enabled = 0;
    NTempest::C3Vector m_pos;
    NTempest::CImVector m_color;
    void* m_texid = nullptr;
    float m_baseScale = 1.0F;
    float m_curScale = 0.0F;
    float m_fadeRate = 1.0F;
    float m_opacity = 0.0F;
    float m_targetOpacity = 0.0F;
    static int m_masterEnable;
};

struct DNGlare : GlareBase {
    NTempest::C2Vector m_fadeTable[4];
    float m_scaleMin = 0.0F;
    float m_scaleMax = 0.0F;
    float m_dotMin = 0.0F;
    float m_alphaMin = 0.0F;
    float m_alphaMax = 1.0F;

    virtual double GetCloudDensityFade() const;
    void Update(float elapsedSec);
};

struct DNSunGlare : DNGlare {
    double GetCloudDensityFade() const override;
};

struct DNMoonGlare : DNGlare {
    double GetCloudDensityFade() const override;
};

struct DNClouds {
    struct Vector3us { std::uint16_t x = 0, y = 0, z = 0; };
    struct Octave {
        Vector3us value, delta, min, max;
        float amplitude = 0.0F;
        unsigned int permy00 = 0, permy10 = 0, permy01 = 0, permy11 = 0;
        float x000 = 0.0F, x100 = 0.0F, x010 = 0.0F, x110 = 0.0F;
        float x001 = 0.0F, x101 = 0.0F, x011 = 0.0F, x111 = 0.0F;
        unsigned int iv = 0, lastiv = 0;
    };

    unsigned int m_lastTime = 0;
    float m_sharpness = 1.0F;
    std::uint8_t m_density = 0;
    float m_densityOverride = 0.0F;
    unsigned int m_lod = 0;
    unsigned int m_updateSize = 0;
    unsigned int m_updateRow = 0;
    unsigned int m_tmSize = 0;
    unsigned int m_tmShift = 0;
    unsigned int m_wrapMask = 0;
    unsigned int m_nOctaves = 0;
    unsigned int m_nLayers = 0;
    TSFixedArray<NTempest::CImVector> m_texels;
    TSFixedArray<std::uint8_t> m_height;
    TSFixedArray<float> m_noise;
    TSFixedArray<float> m_lastBumpNoiseY;
    TSFixedArray<NTempest::C2Vector> m_bump;
    TSFixedArray<NTempest::C3Vector> m_geoVerts;
    TSFixedArray<NTempest::C2Vector> m_texVerts;
    TSFixedArray<std::uint16_t> m_indices;
    std::uint16_t m_nIndices = 0;
    std::uint16_t m_nVerts = 0;
    std::uint16_t m_timeX = 0;
    DNFogInfo m_fogInfo;
    float m_waitTime = 0.0F;
    CGxTex* m_texid = nullptr;

    DNClouds();
    static void __fastcall Callback_GxTex(unsigned int width, int stride, unsigned int height,
                                          unsigned int depth, unsigned int mipLevel, void* userArg,
                                          unsigned int* texelStride, const void** texels);
    void WorldToTexture(const NTempest::C3Vector& worldPt, NTempest::C2Vector& tex) const;
    void Collide(const NTempest::C3Vector& origin, const NTempest::C3Vector& dir,
                 NTempest::C3Vector& hitPoint) const;
    double GetDensity(const NTempest::C3Vector& worldPoint, float area) const;
    void BumpMap();
    void FullUpdate();
    void Update();
    void GenSphere(float size);
    void Destroy();
    void OverrideDensitySharpness(float newDensity, float newSharpness);
    void SetSharpness(float newSharpness);
    void SetDensity(float newDensity);
    void SetLOD(unsigned int newLod, unsigned int newUpdateSize);
};

struct DNSky {
    TSFixedArray<NTempest::C3Vector> m_geoVerts;
    TSFixedArray<NTempest::CImVector> m_clrVerts;
    TSFixedArray<std::uint16_t> m_indices;
    int m_sphThetaTess = 0;
    std::uint16_t m_nVerts = 0;
    std::uint16_t m_nIndices = 0;
    float m_sphRadius = 0.0F;

    void GenSphere(float radius);
    void GenTexture(unsigned int width, unsigned int height, float texels);
    void SetColors();
    void Render();
};

struct DNPlanet {
    enum Type { SUN = 0, MOON = 1, MOON2 = 2 };
    NTempest::C3Vector m_pos;
    NTempest::CImVector m_color;
    void* m_texid = nullptr;
    float m_scale = 0.0F;
    float m_baseScale = 1.0F;
    float m_period = 1.0F;

    void GenGeometry(NTempest::C3Vector* geo, NTempest::C2Vector* tex,
                     NTempest::CImVector* color, std::uint16_t* indices,
                     unsigned int* vertexCount, unsigned int* indexCount) const;
};

struct DNStars {
    HMODEL__* m_hModel = nullptr;
    NTempest::CImVector m_color;
    NTempest::C3Vector m_pos;
    NTempest::C2Vector m_fadeTable[4];

    void Update();
};

struct LightQE {
    float dist = 0.0F;
    int subscript = 0;
};

extern DNInfo s_dnInfo;
extern DNClouds s_clouds;
extern DNSky s_sky;
extern DNSunGlare s_sunGlare;
extern DNMoonGlare s_moonGlare;
extern DNStars s_stars;
extern DNPlanet s_planets[3];
extern LightGroup g_areaLights;
extern CurrentLight s_magmaLight;
extern CurrentLight s_slimeLight;
extern float s_cloudLODCvar;
extern bool s_paused_1;
extern bool s_initialized_2;

double __fastcall InterpTable(const NTempest::C2Vector* table, unsigned int count, float key);
double __fastcall Interp(float first, float second, float percent);
NTempest::CImVector* __fastcall DarkenColor(NTempest::CImVector* color, float clr, float amount);

void __fastcall DNCloudsCallbackGxTex(unsigned int width, int stride, unsigned int height,
                                       unsigned int depth, unsigned int mipLevel, void* userArg,
                                       unsigned int* texelStride, const void** texels);
void __fastcall DayNightSkyTexCallback(unsigned int width, unsigned int stride, unsigned int height,
                                       unsigned int depth, unsigned int mipLevel, void* userArg,
                                       unsigned int* texelStride, const void** texels);

void DayNightInitialize(LightGroup* lightGroup);
int ResetLightPos();
int __fastcall ConsoleCommand_SkyCloudDensity(const char* args, const char* unused);
std::uint8_t __fastcall CloudLODCallback(const char* oldValue, int unused, const char* newValue, void* arg);
int __fastcall ConsoleCommand_SkyCloudLayers(const char* args, const char* unused);
int __fastcall ConsoleCommand_SkySunGlare(const char* args, const char* unused);
int __fastcall ConsoleCommand_SkyShow(const char* args, const char* unused);
void DayNightDestroy();
void DayNightForceFullUpdate();
void SetColors();
void DoAreaLights();
void __fastcall ScaleOutputs(CurrentLight* areaLight, std::uint8_t* color, float scale);
int SetLightColors();
std::uint8_t SetFogColors();
void SetDirection();
void SetPlanets();
void DayNightUpdateLighting();
void __fastcall DayNightSetEclipse(NTempest::CImVector color, float amount);
double __fastcall DayNightSI(float offset);
double __fastcall DayNightUnitSelectColor();
DNInfo* __fastcall DayNightGetInfo();
void DayNightRenderGlares();
void DayNightRenderSky();

int ValueTableInit();
int _E151_4(); int _E148_4(); int _E150_5(); void _E149_4();
int _E156_3(); int _E153_5(); int _E155_1(); void _E154_2();
int _E161_3(); NTempest::C4Vector* _E158_4(); int _E160_2(); void _E159_4();
int _E167_1(); void _E163_4(); int _E166_1(); void _E164_3();
int _E172_1(); void _E169_3(); int _E171_2(); void _E170_2();
int _E175_2(); void _E174_3(); int _E178_1(); void _E177_3();
int _E183_7(); void _E180_3(); int _E182_8(); void _E181_0();
int _E188_9(); void _E185_16(); int _E187_11(); void _E186_9();
int _E193_9(); void _E190_9(); int _E192_10(); void _E191_7();
int _E198_9(); void _E195_8(); int _E197_7(); void _E196_6();
int _E203_5(); void _E200_7(); int _E202_6(); void _E201_6();
int _E208_1(); int _E205_5(); int _E207_4(); void _E206_2();
int _E213_2(); DNClouds* _E210_3(); int _E212_2(); void _E211_1();
int _E218_1(); int _E215_1(); int _E217_2(); void _E216_0();
int _E223_3(); int* _E220_2(); int _E222_3(); void _E221_0();
int _E228_4(); NTempest::C2Vector* _E225_3(); int _E227_2(); void _E226_1();
int _E233_2(); NTempest::C2Vector* _E230_1(); int _E232_3(); void _E231_1();
int _E238_2(); void _E235_2(); int _E237_1(); void _E236_0();
int _E260_1(); void _E256_0(); int _E259_1(); void _E257_0();
int _E265_1(); void _E262_0(); int _E264_1(); void _E263_1();
void _E269_1(); void _E268_1();
