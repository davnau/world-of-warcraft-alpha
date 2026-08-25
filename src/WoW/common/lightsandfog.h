#pragma once

#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/c2ivector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "Storm/h/stpl.h"

#include <cstdint>

struct LightMarker {
    int time = 0;
    NTempest::CImVector color;
    LightMarker();
};

struct LightDataSky {
    float m_skyData[4]{};
};

struct LightDataFog {
    float m_fogEnd = 0.0F;
    float m_fogStartScaler = 0.0F;
};

struct LightDataItem {
    TSFixedArray<LightMarker> m_highlightMarker[18];
    TSFixedArray<LightDataSky> m_skyData;
    TSFixedArray<LightDataFog> m_fogData;
    int m_highlightSky = 0;
    int m_cloudMask = 0;

    LightDataItem();
    ~LightDataItem();
};

struct LightListData {
    NTempest::C2iVector m_chunk;
    int m_chunkRadius = 0;
    NTempest::C3Vector m_lightLocation;
    float m_lightRadius = 0.0F;
    float m_lightDropoff = 0.0F;
    char m_lightName[32]{};
};

struct LightData {
    LightListData m_lightlist;
    LightDataItem m_lightdata;
    LightDataItem m_stormdata;
    LightDataItem m_lightdataWater;
    LightDataItem m_stormdataWater;

    LightData();
    ~LightData();
};

struct LightGroup {
    TSFixedArray<LightData> m_lightData;
};

struct CurrentLight {
    NTempest::CImVector DirectColor;
    NTempest::CImVector AmbientColor;
    NTempest::CImVector SkyArray[6];
    NTempest::CImVector CloudArray[5];
    NTempest::CImVector WaterArray[4];
    float FogEnd = 0.0F;
    float FogStartScalar = 0.0F;
    NTempest::CImVector ShadowOpacity;
    float Darkness = 0.0F;
    float CloudData[4]{};

    CurrentLight();
};

bool __fastcall LoadLightsAndFog(const char* filename, LightGroup* lightgroup);
bool __fastcall ReadSingleLightGroup(class SFile* dataitem, LightDataItem* lightdata);
void __fastcall CalcIndividualLightColor(int oband, int band, LightDataItem* lightdata,
                                          NTempest::CImVector* color, float* distance);
void __fastcall CalcLightColors(int oband, CurrentLight* current, LightDataItem* lightdata,
                                LightDataItem* stormdata, int stormpercent);
