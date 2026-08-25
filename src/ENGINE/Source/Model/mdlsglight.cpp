#include "ENGINE/Source/Model/mdlsglight.h"
#include "ENGINE/Source/Model/modelparse.h"

#include <memory>
#include <mutex>
#include <unordered_map>

namespace {
std::mutex g_lightMutex;
std::uintptr_t g_nextLight = 1;
std::unordered_map<std::uintptr_t, std::unique_ptr<CModelLight>> g_lights;
}

std::uintptr_t CreateGxLight(const CModelLight& light)
{
    auto copy = std::make_unique<CModelLight>(light);
    std::scoped_lock lock(g_lightMutex);
    const auto handle = g_nextLight++;
    g_lights.emplace(handle, std::move(copy));
    return handle;
}

const CModelLight* ModelGetLight(std::uintptr_t handle)
{
    std::scoped_lock lock(g_lightMutex);
    const auto found = g_lights.find(handle);
    return found == g_lights.end() ? nullptr : found->second.get();
}

void ModelDestroyLights(CModelComplex& model)
{
    std::scoped_lock lock(g_lightMutex);
    for (const auto handle : model.m_lights) g_lights.erase(handle);
    model.m_lights.clear();
}

bool MdlReadLoadLights(CModelComplex& model, std::span<const std::uint8_t> bytes)
{
    ModelByteReader reader(bytes);
    std::uint32_t count = 0;
    if (!reader.Read(count) || count > reader.Remaining() / sizeof(CModelLight)) return false;
    std::vector<CModelLight> lights;
    if (!reader.ReadArray(lights, count)) return false;
    ModelDestroyLights(model);
    model.m_lights.reserve(count);
    for (const auto& light : lights) model.m_lights.push_back(CreateGxLight(light));
    return true;
}

void MdxReadLights(const std::uint8_t* fileBytes, std::size_t byteCount, CModelComplex& model)
{
    if (fileBytes) (void)MdlReadLoadLights(model, {fileBytes, byteCount});
}

