#include "ENGINE/Source/Model/mdlcamera.h"
#include "ENGINE/Source/Model/modelparse.h"

#include <memory>

namespace {
constexpr std::size_t kCameraNameBytes = 80U;
}

bool MdlReadCameras(std::span<const std::uint8_t> bytes, std::vector<HCAMERA__*>& cameras)
{
    ModelByteReader reader(bytes);
    std::uint32_t count = 0;
    if (!reader.Read(count)) return false;
    std::vector<HCAMERA__*> loaded;
    loaded.reserve(count);
    for (std::uint32_t i = 0; i < count; ++i) {
        std::uint32_t recordBytes = 0;
        std::array<char, kCameraNameBytes> name{};
        if (!reader.Read(recordBytes) || recordBytes < sizeof(recordBytes) + name.size() + 36U ||
            recordBytes - sizeof(recordBytes) > reader.Remaining() || !reader.Read(name)) {
            MdlDestroyCameras(loaded); return false;
        }
        auto camera = std::make_unique<HCAMERA__>();
        if (!reader.Read(camera->position) || !reader.Read(camera->fieldOfView) ||
            !reader.Read(camera->farClip) || !reader.Read(camera->nearClip) ||
            !reader.Read(camera->target)) {
            MdlDestroyCameras(loaded); return false;
        }
        const std::size_t consumed = sizeof(recordBytes) + name.size() + 36U;
        if (!reader.Skip(recordBytes - consumed)) { MdlDestroyCameras(loaded); return false; }
        loaded.push_back(camera.release());
    }
    MdlDestroyCameras(cameras);
    cameras = std::move(loaded);
    return reader.Remaining() == 0U;
}

void MdxReadCameras(const std::uint8_t* fileBytes, std::size_t byteCount, std::vector<HCAMERA__*>& cameras)
{
    if (!fileBytes) return;
    (void)MdlReadCameras({fileBytes, byteCount}, cameras);
}

void MdlDestroyCameras(std::vector<HCAMERA__*>& cameras)
{
    for (auto* camera : cameras) delete camera;
    cameras.clear();
}
