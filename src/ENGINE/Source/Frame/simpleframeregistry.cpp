#include "ENGINE/Source/Frame/frame.h"

#include <string>
#include <unordered_map>
#include <cstdint>

struct SIMPLEFRAMEREGHASH { std::uintptr_t context = 0; unsigned nameHash = 0; };
struct SIMPLETEXTUREREGHASH { std::uintptr_t context = 0; unsigned nameHash = 0; };
struct SIMPLEFONTSTRINGREGHASH { std::uintptr_t context = 0; unsigned nameHash = 0; };

namespace {
struct RegistryKey {
    std::string context;
    std::string name;
    bool operator==(const RegistryKey& other) const
    {
        return context == other.context && name == other.name;
    }
};
struct RegistryKeyHash {
    std::size_t operator()(const RegistryKey& key) const
    {
        return std::hash<std::string>{}(key.context) ^
               (std::hash<std::string>{}(key.name) << 1);
    }
};

std::unordered_map<RegistryKey, CSimpleFrame*, RegistryKeyHash> g_frames;
std::unordered_map<RegistryKey, CSimpleTexture*, RegistryKeyHash> g_textures;
std::unordered_map<RegistryKey, CSimpleFontString*, RegistryKeyHash> g_fontStrings;
std::unordered_map<std::uintptr_t, SIMPLEFRAMEREGHASH> g_frameHashes;
std::unordered_map<std::uintptr_t, SIMPLETEXTUREREGHASH> g_textureHashes;
std::unordered_map<std::uintptr_t, SIMPLEFONTSTRINGREGHASH> g_fontStringHashes;

std::string Safe(const char* value) { return value ? value : ""; }

template<class T>
void RemoveObject(std::unordered_map<RegistryKey, T*, RegistryKeyHash>& registry, T* object)
{
    for (auto it = registry.begin(); it != registry.end();) {
        if (it->second == object) it = registry.erase(it);
        else ++it;
    }
}

template<class T>
T* GetRegistryHash(std::unordered_map<std::uintptr_t, T>& hashes, char* context,
                   unsigned nameHash, int create, int* alreadyExisted)
{
    const auto key = reinterpret_cast<std::uintptr_t>(context) ^
                     (static_cast<std::uintptr_t>(nameHash) << 1);
    const bool existed = hashes.find(key) != hashes.end();
    if (alreadyExisted) *alreadyExisted = existed;
    if (!create && !existed) return nullptr;
    auto [it, inserted] = hashes.emplace(key, T{});
    if (inserted) {
        it->second.context = reinterpret_cast<std::uintptr_t>(context);
        it->second.nameHash = nameHash;
    }
    return &it->second;
}
}

SIMPLEFRAMEREGHASH* GetSimpleFrameHash(char* context, unsigned nameHash,
                                       int create, int* alreadyExisted)
{
    return GetRegistryHash(g_frameHashes, context, nameHash, create, alreadyExisted);
}

SIMPLETEXTUREREGHASH* GetSimpleTextureHash(char* context, unsigned nameHash,
                                           int create, int* alreadyExisted)
{
    return GetRegistryHash(g_textureHashes, context, nameHash, create, alreadyExisted);
}

SIMPLEFONTSTRINGREGHASH* GetSimpleFontStringHash(char* context, unsigned nameHash,
                                                 int create, int* alreadyExisted)
{
    return GetRegistryHash(g_fontStringHashes, context, nameHash, create, alreadyExisted);
}

void SimpleFrameRegistryAddEntry(CSimpleFrame* object, const char* context, const char* name)
{
    if (!object || !name || !*name) return;
    g_frames[{Safe(context), name}] = object;
}

void SimpleTextureRegistryAddEntry(CSimpleTexture* object, const char* context, const char* name)
{
    if (!object || !name || !*name) return;
    g_textures[{Safe(context), name}] = object;
}

void SimpleFontStringRegistryAddEntry(CSimpleFontString* object, const char* context, const char* name)
{
    if (!object || !name || !*name) return;
    g_fontStrings[{Safe(context), name}] = object;
}

void SimpleFrameRegistryRemoveEntry(CSimpleFrame* object) { RemoveObject(g_frames, object); }
void SimpleTextureRegistryRemoveEntry(CSimpleTexture* object) { RemoveObject(g_textures, object); }
void SimpleFontStringRegistryRemoveEntry(CSimpleFontString* object) { RemoveObject(g_fontStrings, object); }

CSimpleFrame* SimpleFrameRegistryGetEntry(const char* context, const char* name)
{
    if (!name) return nullptr;
    const RegistryKey key{Safe(context), name};
    if (auto it = g_frames.find(key); it != g_frames.end()) return it->second;
    if (auto it = g_frames.find({"", name}); it != g_frames.end()) return it->second;
    return nullptr;
}

CSimpleTexture* SimpleTextureRegistryGetEntry(const char* context, const char* name)
{
    if (!name) return nullptr;
    const RegistryKey key{Safe(context), name};
    if (auto it = g_textures.find(key); it != g_textures.end()) return it->second;
    if (auto it = g_textures.find({"", name}); it != g_textures.end()) return it->second;
    return nullptr;
}

CSimpleFontString* SimpleFontStringRegistryGetEntry(const char* context, const char* name)
{
    if (!name) return nullptr;
    const RegistryKey key{Safe(context), name};
    if (auto it = g_fontStrings.find(key); it != g_fontStrings.end()) return it->second;
    if (auto it = g_fontStrings.find({"", name}); it != g_fontStrings.end()) return it->second;
    return nullptr;
}

void SimpleFrameRegistryClear()
{
    g_frames.clear();
    g_textures.clear();
    g_fontStrings.clear();
    g_frameHashes.clear();
    g_textureHashes.clear();
    g_fontStringHashes.clear();
}
