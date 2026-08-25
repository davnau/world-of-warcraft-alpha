#pragma once
#include "ENGINE/Source/Model/model.h"
#include <span>

struct HCAMERA__ {
    std::uint32_t type = 0;
    float fieldOfView = 0.785398163F;
    float farClip = 1000.0F;
    float nearClip = 0.1F;
    NTempest::C3Vector position{};
    NTempest::C3Vector target{};
    float roll = 0.0F;
};

bool MdlReadCameras(std::span<const std::uint8_t> bytes, std::vector<HCAMERA__*>& cameras);
void MdxReadCameras(const std::uint8_t* fileBytes, std::size_t byteCount, std::vector<HCAMERA__*>& cameras);
void MdlDestroyCameras(std::vector<HCAMERA__*>& cameras);

