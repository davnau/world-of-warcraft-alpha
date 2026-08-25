#pragma once

#include "ENGINE/Source/Base/cdatastore.h"
#include "ENGINE/Source/Tempest/c3vector.h"

#include <cstdint>
#include <vector>

class CClientMoveUpdate {
public:
    void Skip(CDataStore* message);
    std::uint64_t transportGuid = 0;
    NTempest::C3Vector position{};
    float facing = 0.0F;
    NTempest::C3Vector movementPosition{};
    float movementFacing = 0.0F;
    float pitch = 0.0F;
    std::uint32_t moveFlags = 0;
    std::uint32_t updateFlags = 0;
    std::uint32_t movementFlags = 0;
    std::uint32_t movementTime = 0;
    std::vector<NTempest::C3Vector> splinePoints;
};

CDataStore& operator<<(CDataStore& message, const CClientMoveUpdate& update);
CDataStore& operator>>(CDataStore& message, CClientMoveUpdate& update);
bool IsAngleWithinRange(float a, float b, float fieldOfView);
double CalculateFacingTo(const NTempest::C3Vector* destination, const float* position);
