#include "WoW/common/lightsandfog.h"

#include "Storm/h/storm.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstring>
#include <limits>

LightMarker::LightMarker() : time(0), color(0) {}

LightDataItem::LightDataItem() = default;
LightDataItem::~LightDataItem() = default;
LightData::LightData() = default;
LightData::~LightData() = default;

CurrentLight::CurrentLight() = default;

namespace {

template <typename T>
void ReadArray(TSFixedArray<T>& destination, const T* source, unsigned int count) {
    destination.SetCount(count);
    if (count != 0 && source != nullptr) std::copy(source, source + count, destination.m_data);
}

std::uint32_t PackColor(const NTempest::CImVector& color) {
    std::uint32_t value = 0;
    std::memcpy(&value, &color, sizeof(value));
    return value;
}

NTempest::CImVector UnpackColor(std::uint32_t value) {
    NTempest::CImVector color;
    std::memcpy(&color, &value, sizeof(value));
    return color;
}

std::uint32_t BlendColor(std::uint32_t normal, std::uint32_t storm, int percent) {
    return static_cast<std::uint32_t>(
        (static_cast<std::uint64_t>(storm) * static_cast<unsigned int>(percent) +
         static_cast<std::uint64_t>(normal) * static_cast<unsigned int>(100 - percent)) / 100U);
}

template <typename T>
float InterpolateValue(const T& first, const T& second, int elapsed, int duration) {
    if (duration == 0) return static_cast<float>(second);
    return (static_cast<float>(second) - static_cast<float>(first)) *
               static_cast<float>(elapsed) / static_cast<float>(duration) +
           static_cast<float>(first);
}

float GetScalar(const LightDataItem* data, int band, unsigned int index) {
    if (data == nullptr) return 0.0F;
    if (band == 18 && index < data->m_fogData.m_count) return data->m_fogData.m_data[index].m_fogEnd;
    if (band == 19 && index < data->m_fogData.m_count) return data->m_fogData.m_data[index].m_fogStartScaler;
    if (band >= 20 && band <= 23 && index < data->m_skyData.m_count)
        return data->m_skyData.m_data[index].m_skyData[band - 20];
    return 0.0F;
}

} // namespace

bool __fastcall LoadLightsAndFog(const char* filename, LightGroup* lightgroup) {
    if (filename == nullptr || lightgroup == nullptr) return false;
    SFile* file = nullptr;
    if (SFile::Open(filename, &file) == 0 || file == nullptr) return false;

    std::int32_t version = 0;
    std::int32_t count = 0;
    const bool headerOk = SFile::Read(file, &version, sizeof(version), nullptr, nullptr, nullptr) != 0 &&
                          version == static_cast<std::int32_t>(0x80000004) &&
                          SFile::Read(file, &count, sizeof(count), nullptr, nullptr, nullptr) != 0 &&
                          count >= 0;
    if (!headerOk) {
        SFile::Close(file);
        return false;
    }

    if (!lightgroup->m_lightData.SetCount(static_cast<unsigned int>(count))) {
        SFile::Close(file);
        return false;
    }
    // LoadLightsAndFog (0x6C4110) performs two distinct passes over the file:
    // first every 0x40-byte LightListData record, then the four 0x1550-byte
    // LightDataItem blocks belonging to each record.  The on-disk data is not
    // interleaved one complete LightData at a time.
    for (int index = 0; index < count; ++index) {
        LightData& light = lightgroup->m_lightData.m_data[index];
        if (!SFile::Read(file, &light.m_lightlist, 0x40, nullptr, nullptr, nullptr)) {
            lightgroup->m_lightData.SetCount(0);
            SFile::Close(file);
            return false;
        }
    }
    for (int index = 0; index < count; ++index) {
        LightData& light = lightgroup->m_lightData.m_data[index];
        if (!ReadSingleLightGroup(file, &light.m_lightdata) ||
            !ReadSingleLightGroup(file, &light.m_stormdata) ||
            !ReadSingleLightGroup(file, &light.m_lightdataWater) ||
            !ReadSingleLightGroup(file, &light.m_stormdataWater)) {
            lightgroup->m_lightData.SetCount(0);
            SFile::Close(file);
            return false;
        }
    }
    SFile::Close(file);
    return true;
}

bool __fastcall ReadSingleLightGroup(SFile* dataitem, LightDataItem* lightdata) {
    if (dataitem == nullptr || lightdata == nullptr) return false;
    std::array<std::uint8_t, 0x1550> raw{};
    if (!SFile::Read(dataitem, raw.data(), static_cast<unsigned int>(raw.size()), nullptr, nullptr, nullptr)) return false;

    const auto* counts = reinterpret_cast<const std::int32_t*>(raw.data());
    const auto* markers = reinterpret_cast<const LightMarker*>(raw.data() + 0x48);
    const auto* values = reinterpret_cast<const float*>(raw.data() + 0x1248);
    // ReadSingleLightGroup (0x6C43C0) stores 65 scalar values at 0x1248,
    // four component-major arrays of 32 sky values at 0x134C, and the cloud
    // mask at 0x154C.
    const auto* sky = reinterpret_cast<const float*>(raw.data() + 0x134C);

    for (int band = 0; band < 18; ++band) {
        const unsigned int count = static_cast<unsigned int>(std::clamp(counts[band], 0, 32));
        ReadArray(lightdata->m_highlightMarker[band], markers + band * 32, count);
    }
    const unsigned int fogCount = static_cast<unsigned int>(std::clamp(counts[7], 0, 32));
    lightdata->m_fogData.SetCount(fogCount);
    for (unsigned int i = 0; i < fogCount; ++i) {
        lightdata->m_fogData.m_data[i].m_fogEnd = values[i];
        lightdata->m_fogData.m_data[i].m_fogStartScaler = values[i + 32U];
    }
    const unsigned int skyCount = static_cast<unsigned int>(std::clamp(counts[9], 0, 32));
    lightdata->m_skyData.SetCount(skyCount);
    for (unsigned int i = 0; i < skyCount; ++i) {
        for (unsigned int component = 0; component < 4; ++component)
            lightdata->m_skyData.m_data[i].m_skyData[component] = sky[component * 32U + i];
    }
    lightdata->m_highlightSky = static_cast<int>(values[64]);
    std::memcpy(&lightdata->m_cloudMask, raw.data() + 0x154C,
                sizeof(lightdata->m_cloudMask));
    return true;
}

void __fastcall CalcIndividualLightColor(int oband, int band, LightDataItem* lightdata,
                                          NTempest::CImVector* color, float* distance) {
    if (lightdata == nullptr) return;
    int markerBand = band;
    if (markerBand >= 18) markerBand = 7;
    if (markerBand >= 20) markerBand = 9;

    const int markerCount = markerBand >= 0 && markerBand < 18
        ? static_cast<int>(lightdata->m_highlightMarker[markerBand].m_count) : 0;
    if (markerCount <= 0) return;

    for (int index = 0; index < markerCount; ++index) {
        const int nextIndex = (index + 1) % markerCount;
        int firstTime = lightdata->m_highlightMarker[markerBand].m_data[index].time;
        int secondTime = lightdata->m_highlightMarker[markerBand].m_data[nextIndex].time;
        int sample = oband;
        if (secondTime <= firstTime) {
            secondTime += 2880;
            if (sample < firstTime) sample += 2880;
        }
        if (sample < firstTime || sample > secondTime) continue;

        const int duration = secondTime - firstTime;
        const int elapsed = sample - firstTime;
        const LightMarker& first = lightdata->m_highlightMarker[markerBand].m_data[index];
        const LightMarker& second = lightdata->m_highlightMarker[markerBand].m_data[nextIndex];
        if (band >= 18) {
            if (distance != nullptr) {
                *distance = InterpolateValue(GetScalar(lightdata, band, index),
                                             GetScalar(lightdata, band, nextIndex), elapsed, duration);
            }
        } else if (color != nullptr) {
            color->r = static_cast<std::uint8_t>(first.color.r + elapsed *
                (static_cast<int>(second.color.r) - first.color.r) / std::max(duration, 1));
            color->g = static_cast<std::uint8_t>(first.color.g + elapsed *
                (static_cast<int>(second.color.g) - first.color.g) / std::max(duration, 1));
            color->b = static_cast<std::uint8_t>(first.color.b + elapsed *
                (static_cast<int>(second.color.b) - first.color.b) / std::max(duration, 1));
            color->a = 0xFF;
        }
        return;
    }
}

void __fastcall CalcLightColors(int oband, CurrentLight* current, LightDataItem* lightdata,
                                LightDataItem* stormdata, int stormpercent) {
    if (current == nullptr || lightdata == nullptr) return;
    NTempest::CImVector* colors[] = {
        &current->DirectColor, &current->AmbientColor,
        &current->SkyArray[0], &current->SkyArray[1], &current->SkyArray[2],
        &current->SkyArray[3], &current->SkyArray[4], &current->SkyArray[5],
        &current->ShadowOpacity, &current->CloudArray[0], &current->CloudArray[1],
        &current->CloudArray[2], &current->CloudArray[3], &current->CloudArray[4],
        &current->WaterArray[0], &current->WaterArray[1], &current->WaterArray[2],
        &current->WaterArray[3],
    };
    for (int band = 0; band < 18; ++band) CalcIndividualLightColor(oband, band, lightdata, colors[band], nullptr);
    CalcIndividualLightColor(oband, 18, lightdata, nullptr, &current->FogEnd);
    CalcIndividualLightColor(oband, 19, lightdata, nullptr, &current->FogStartScalar);
    CalcIndividualLightColor(oband, 21, lightdata, nullptr, &current->CloudData[1]);
    current->Darkness = static_cast<float>(lightdata->m_highlightSky);

    if (stormdata == nullptr || stormpercent <= 0) return;
    stormpercent = std::clamp(stormpercent, 0, 100);
    CurrentLight storm;
    NTempest::CImVector* stormColors[] = {
        &storm.DirectColor, &storm.AmbientColor,
        &storm.SkyArray[0], &storm.SkyArray[1], &storm.SkyArray[2],
        &storm.SkyArray[3], &storm.SkyArray[4], &storm.SkyArray[5],
        &storm.ShadowOpacity, &storm.CloudArray[0], &storm.CloudArray[1],
        &storm.CloudArray[2], &storm.CloudArray[3], &storm.CloudArray[4],
        &storm.WaterArray[0], &storm.WaterArray[1], &storm.WaterArray[2],
        &storm.WaterArray[3],
    };
    for (int band = 0; band < 18; ++band)
        CalcIndividualLightColor(oband, band, stormdata, stormColors[band], nullptr);
    CalcIndividualLightColor(oband, 18, stormdata, nullptr, &storm.FogEnd);
    CalcIndividualLightColor(oband, 19, stormdata, nullptr, &storm.FogStartScalar);
    CalcIndividualLightColor(oband, 21, stormdata, nullptr, &storm.CloudData[1]);

    auto blend = [stormpercent](NTempest::CImVector& normal, const NTempest::CImVector& stormColor) {
        normal = UnpackColor(BlendColor(PackColor(normal), PackColor(stormColor), stormpercent));
    };
    blend(current->DirectColor, storm.DirectColor);
    blend(current->AmbientColor, storm.AmbientColor);
    for (int i = 0; i < 6; ++i) blend(current->SkyArray[i], storm.SkyArray[i]);
    for (int i = 0; i < 5; ++i) blend(current->CloudArray[i], storm.CloudArray[i]);
    for (int i = 0; i < 4; ++i) blend(current->WaterArray[i], storm.WaterArray[i]);
    blend(current->ShadowOpacity, storm.ShadowOpacity);
    current->FogEnd = storm.FogEnd * stormpercent * 0.01F + current->FogEnd * (100 - stormpercent) * 0.01F;
    current->FogStartScalar = storm.FogStartScalar * stormpercent * 0.01F +
                              current->FogStartScalar * (100 - stormpercent) * 0.01F;
    current->CloudData[1] = storm.CloudData[1] * stormpercent * 0.01F +
                            current->CloudData[1] * (100 - stormpercent) * 0.01F;
}
