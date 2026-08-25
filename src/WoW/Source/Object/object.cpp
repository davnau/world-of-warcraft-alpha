#include "WoW/Source/Object/object.h"

#include <cmath>

float INFINITY_95 = 0.0F;
float TWO_PI_95 = 6.2831853071795864769F;
float OO_TWO_PI_95 = 0.15915494309189535F;

void CClientMoveUpdate::Skip(CDataStore* message) {
    if (!message) return;
    void* unused = nullptr;
    std::uint32_t flags = 0;
    message->GetDataInSitu(&unused, 0x2Cu);
    message->Get(&flags);
    message->GetDataInSitu(&unused, 0x14u);
    if ((flags & 0x04000000u) == 0) return;
    message->Get(&flags);
    unsigned bytes = ((flags & 0x00010000u) ? 12u : 0u) +
                     ((flags & 0x00020000u) ? 8u : 0u) +
                     ((flags & 0x00040000u) ? 4u : 0u);
    message->GetDataInSitu(&unused, bytes + 8u);
    std::uint32_t pointCount = 0;
    message->Get(&pointCount);
    message->GetDataInSitu(&unused, 12u * pointCount);
}

CDataStore& operator<<(CDataStore& message, const CClientMoveUpdate& update) {
    message.Put(update.transportGuid);
    message.Put(update.position.x); message.Put(update.position.y); message.Put(update.position.z);
    message.Put(update.facing);
    message.Put(update.movementPosition.x); message.Put(update.movementPosition.y); message.Put(update.movementPosition.z);
    message.Put(update.movementFacing); message.Put(update.pitch); message.Put(update.moveFlags);
    message.Put(update.updateFlags);
    if (update.updateFlags & 0x04000000u) {
        message.Put(update.movementFlags);
        if (update.movementFlags & 0x00010000u) {
            message.Put(update.position.x); message.Put(update.position.y); message.Put(update.position.z);
        }
        if (update.movementFlags & 0x00020000u) message.Put(update.transportGuid);
        if (update.movementFlags & 0x00040000u) message.Put(update.pitch);
        message.Put(update.movementTime);
        const auto count = static_cast<std::uint32_t>(update.splinePoints.size());
        message.Put(count);
        for (const auto& point : update.splinePoints) {
            message.Put(point.x); message.Put(point.y); message.Put(point.z);
        }
    }
    return message;
}

CDataStore& operator>>(CDataStore& message, CClientMoveUpdate& update) {
    message.Get(&update.transportGuid);
    message.Get(&update.position.x); message.Get(&update.position.y); message.Get(&update.position.z);
    message.Get(&update.facing);
    message.Get(&update.movementPosition.x); message.Get(&update.movementPosition.y); message.Get(&update.movementPosition.z);
    message.Get(&update.movementFacing); message.Get(&update.pitch); message.Get(&update.moveFlags);
    message.Get(&update.updateFlags);
    update.splinePoints.clear();
    if (update.updateFlags & 0x04000000u) {
        message.Get(&update.movementFlags);
        if (update.movementFlags & 0x00010000u) {
            message.Get(&update.position.x); message.Get(&update.position.y); message.Get(&update.position.z);
        }
        if (update.movementFlags & 0x00020000u) message.Get(&update.transportGuid);
        if (update.movementFlags & 0x00040000u) message.Get(&update.pitch);
        message.Get(&update.movementTime);
        std::uint32_t count = 0; message.Get(&count);
        void* points = nullptr;
        if (count && message.GetDataInSitu(&points, 12u * count)) {
            const auto* values = static_cast<const float*>(points);
            update.splinePoints.reserve(count);
            for (std::uint32_t i = 0; i < count; ++i)
                update.splinePoints.emplace_back(values[i * 3], values[i * 3 + 1], values[i * 3 + 2]);
        }
    }
    return message;
}

bool IsAngleWithinRange(float a, float b, float fieldOfView) {
    const float view = std::fabs(fieldOfView);
    a = std::fmod(std::fmod(a, TWO_PI_95) + TWO_PI_95, TWO_PI_95);
    b = std::fmod(std::fmod(b, TWO_PI_95) + TWO_PI_95, TWO_PI_95);
    const float direct = std::fabs(a - b);
    return direct < view || std::fabs(TWO_PI_95 - direct) < view;
}

double CalculateFacingTo(const NTempest::C3Vector* destination, const float* position) {
    if (!destination || !position) return 0.0;
    const float dx = position[0] - destination->x;
    const float dy = position[1] - destination->y;
    if (std::fabs(dx) >= 0.00000023841858F && std::fabs(dy) >= 0.00000023841858F)
        return std::atan2(dy, dx);
    if (std::fabs(dx) < 0.00000023841858F)
        return dy >= 0.0F ? 0.5 * 3.141592653589793 : 1.5 * 3.141592653589793;
    return dx >= 0.0F ? 0.0 : 3.141592653589793;
}
