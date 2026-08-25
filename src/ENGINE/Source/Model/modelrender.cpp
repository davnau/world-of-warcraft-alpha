#include "ENGINE/Source/Model/modelrender.h"
#include "ENGINE/Source/Model/collisiondata.h"
#include "ENGINE/Source/Model/collisiondisplay.h"
#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/mdlsgmodel.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Services/texture.h"
#include "ENGINE/Source/Services/particlesystem2.h"
#include "ENGINE/Source/Services/ribbonemitter.h"
#include "ENGINE/Source/Anim/anim.h"
#include "ENGINE/Source/Tempest/cfacet.h"
#include "ENGINE/Source/Tempest/c44matrix.h"
#include "ENGINE/Source/Tempest/tempest_intersect.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <cstdint>
#include <limits>
#include <mutex>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

namespace {
constexpr std::uint32_t kAllLayers = std::numeric_limits<std::uint32_t>::max();

struct CQueuedOpaqueGeoset {
    CModel* model = nullptr;
    std::uint32_t geoset = 0;
    std::uint32_t layer = kAllLayers;
    std::uint32_t renderFlags = 0;
};

enum class ETransparentObject : std::uint32_t {
    Geoset = 0,
    ParticleEmitter = 1,
    RibbonEmitter = 2,
};

struct CQueuedTransparentObject {
    ETransparentObject type = ETransparentObject::Geoset;
    CModel* model = nullptr;
    void* object = nullptr;
    std::uint32_t geoset = 0;
    std::uint32_t renderFlags = 0;
    std::int32_t priorityPlane = 0;
    float squaredDistance = 0.0F;
    std::uint64_t insertionOrder = 0;
};

std::mutex g_sceneMutex;
std::vector<CQueuedOpaqueGeoset> g_opaque;
std::vector<CQueuedTransparentObject> g_transparent;
bool g_renderReady = false;
bool g_renderLogging = false;
float g_sceneSharpness = 1.0F;
std::vector<NTempest::C34Matrix> g_matrixPool;
std::uint32_t g_nextMatrix = 0;
std::uint32_t g_lastMatrixFrame = 0;
std::uint16_t g_currAnimFrame = 0;
std::array<NTempest::C4Plane, 6> g_frustumPlanes{};
NTempest::C3Vector g_cameraPosition{};
NTempest::C3Vector g_cameraDirection{1.0F, 0.0F, 0.0F};
std::uint64_t g_sceneInsertionOrder = 0;
std::function<NTempest::C2Vector(const NTempest::C3Vector&)> g_project2d;
std::vector<std::vector<NTempest::C2Vector>> g_transformedUv;
std::uint32_t g_fogState = 1;
thread_local std::uint32_t g_activeRenderFlags = 0;
constexpr float kPi = 3.14159265358979323846F;

const CGeoset* GetUniqueGeoset(const CModel& model, std::uint32_t geoset)
{
    if (const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get()))
        return geoset < complex->m_geosets.size() ? &complex->m_geosets[geoset] : nullptr;
    if (const auto* simple = dynamic_cast<const CModelSimple*>(model.data.get()))
        return geoset < simple->m_numGeosets ? &simple->m_geosets[geoset] : nullptr;
    return nullptr;
}

float EffectiveLayerAlpha(const CModel& model, const CTexLayer& layer)
{
    if (layer.animationId < model.layerAlpha.size())
        return std::clamp(model.layerAlpha[layer.animationId], 0.0F, 1.0F);
    return std::clamp(layer.alpha, 0.0F, 1.0F);
}

std::uint32_t EffectiveLayerTexture(const CModel& model, const CTexLayer& layer)
{
    if (layer.animationId < model.layerTextureIds.size() &&
        model.layerTextureIds[layer.animationId] != ~0U)
        return model.layerTextureIds[layer.animationId];
    return layer.texture;
}

bool IsOpaque(const CModel& model, const CMaterial& material)
{
    for (const auto& layer : material.layers) {
        if (EffectiveLayerAlpha(model, layer) <= 0.0F) continue;
        return layer.blendMode < GxBlend_Alpha && (layer.flags & 0x04U) == 0U;
    }
    return true;
}

NTempest::C44Matrix ToC44Matrix(const NTempest::C34Matrix& matrix)
{
    return {
        matrix.a0, matrix.a1, matrix.a2, 0.0F,
        matrix.b0, matrix.b1, matrix.b2, 0.0F,
        matrix.c0, matrix.c1, matrix.c2, 0.0F,
        matrix.d0, matrix.d1, matrix.d2, 1.0F};
}

void RenderGeosetFinish(const CGeosetShared& geoset)
{
    if (geoset.vertexShader == GxVS_PassThru) GxXformPop(GxXform_World);
}

float SquaredDistanceFromCamera(const NTempest::C3Vector& position)
{
    return (position - g_cameraPosition).SquaredMag();
}

void SetModelLights(CModel& model)
{
    static bool reportLights = true;
    unsigned slot = 0;
    if (model.lightsEnabled && model.anim && model.animData) {
        for (std::size_t i = 0; i < model.animData->obj.size() && slot < 8U; ++i) {
            if (!model.animData->obj[i] || model.animData->obj[i]->type != ANIM_OBJ_LIGHT ||
                i >= model.anim->lightStatus.size()) continue;
            const auto& value = model.anim->lightStatus[i];
            if (value.visibility <= 0.0F) continue;
            CGxLight light;
            light.m_flags = 1U | (value.type == 0U ? 2U : 0U);
            light.m_dir = value.type == 0U ? value.position : value.direction;
            light.m_dirColor = NTempest::C3Vector{value.color.r, value.color.g, value.color.b};
            light.m_ambColor = NTempest::C3Vector{value.ambientColor.r, value.ambientColor.g, value.ambientColor.b};
            light.m_dirIntensity = std::max(0.0F, value.intensity);
            light.m_ambIntensity = std::max(0.0F, value.ambientIntensity);
            light.m_attenStart = value.attenuationStart;
            light.m_attenEnd = value.attenuationEnd;
            // The recovered model light constructor leaves the CGxLight
            // attenuation values at their GX defaults.  The MDLX attenuation
            // tracks are retained by Anim but are not copied into CGxLight.
            // CGxDevice::LightSet converts positional lights to the same
            // camera-relative space used by model and particle rendering.
            // The recovered call path supplies the active scene camera here.
            GxLightSet(slot, &light, g_cameraPosition);
            GxLightEnable(slot, true);
            if (reportLights) {
                std::fprintf(stderr,
                             "model light %u type=%u pos/dir=%g,%g,%g diffuse=%u,%u,%u I=%g ambient=%u,%u,%u I=%g atten=%g,%g,%g range=%g,%g\n",
                             slot, value.type, light.m_dir.x, light.m_dir.y, light.m_dir.z,
                             light.m_dirColor.r, light.m_dirColor.g, light.m_dirColor.b,
                             light.m_dirIntensity, light.m_ambColor.r, light.m_ambColor.g,
                             light.m_ambColor.b, light.m_ambIntensity,
                             light.m_constantAttenuation, light.m_linearAttenuation,
                             light.m_quadraticAttenuation, light.m_attenStart, light.m_attenEnd);
            }
            ++slot;
        }
    }
    if (reportLights) {
        std::fprintf(stderr, "model lights uploaded=%u camera=%g,%g,%g\n", slot,
                     g_cameraPosition.x, g_cameraPosition.y, g_cameraPosition.z);
        reportLights = false;
    }
    // ModelRenderScene in the original client renders queued geosets against
    // the lights already selected for the scene.  An attached model with no
    // embedded lights therefore inherits that selection; it does not clear
    // the GX light slots.  Only replace the current selection when this model
    // actually contributed one or more lights.
    if (slot != 0U)
        while (slot < 8U) GxLightEnable(slot++, false);
}

void DrawGeosetLayer(CModel& model, const CGeosetShared& geoset, const CTexLayer* layer)
{
    if (geoset.position.empty() || geoset.primitiveVertices.empty()) return;
    auto* complex = dynamic_cast<CModelComplex*>(model.data.get());

    CGxTex* texture = nullptr;
    std::uint32_t layerFlags = 0;
    EGxBlend blend = GxBlend_Opaque;
    float alpha = 1.0F;
    int coordId = 0;
    const std::uint32_t modelFlags = model.data ? model.data->m_flags : 0U;
    if (layer) {
        layerFlags = layer->flags;
        blend = static_cast<EGxBlend>(layer->blendMode);
        alpha = EffectiveLayerAlpha(model, *layer);
        coordId = layer->coordId;
        const auto textureId = EffectiveLayerTexture(model, *layer);
        if (complex && textureId < complex->m_textures.size())
            texture = TextureGetGxTex(complex->m_textures[textureId].handle, 1, nullptr);

        static std::array<bool, 64> reportedMaterials{};
        if (complex && model.shared &&
            _stricmp(model.shared->name.data(), "Interface\\Glues\\Models\\UI_MainMenu\\UI_MainMenu.mdx") == 0 &&
            geoset.materialId < reportedMaterials.size() && !reportedMaterials[geoset.materialId]) {
            reportedMaterials[geoset.materialId] = true;
            const auto* handle = textureId < complex->m_textures.size()
                ? complex->m_textures[textureId].handle : nullptr;
            std::array<unsigned, 4> pixelMin{255U,255U,255U,255U};
            std::array<unsigned, 4> pixelMax{};
            if (handle && !handle->mipBits.empty()) {
                const auto& bits = handle->mipBits[0].bits;
                for (std::size_t pixel = 0; pixel + 3U < bits.size(); pixel += 4U) {
                    for (std::size_t channel = 0; channel < 4U; ++channel) {
                        pixelMin[channel] = std::min<unsigned>(pixelMin[channel], bits[pixel + channel]);
                        pixelMax[channel] = std::max<unsigned>(pixelMax[channel], bits[pixel + channel]);
                    }
                }
            }
            const auto mipBytes = handle && !handle->mipBits.empty()
                ? handle->mipBits[0].bits.size() : 0U;
            std::fprintf(stderr,
                         "MATERIALDUMP m=%u layer=%u tex=%u file=%s gx=%p %ux%u mipBytes=%zu bgra=(%u,%u,%u,%u)-(%u,%u,%u,%u) alpha=%g base=%g blend=%u disables=%02X vf=%u coord=%d xform=%d shader=%u tmu=%u modelFlags=%08X priority=%d\n",
                         geoset.materialId, layer->animationId, textureId,
                         handle ? handle->filename.c_str() : "<null>", static_cast<void*>(texture),
                         texture ? texture->width : 0U, texture ? texture->height : 0U,
                         mipBytes,
                         pixelMin[0], pixelMin[1], pixelMin[2], pixelMin[3],
                         pixelMax[0], pixelMax[1], pixelMax[2], pixelMax[3],
                         alpha, layer->alpha, layer->blendMode, layer->flags,
                         layer->vertexFormat, layer->coordId, layer->transformId,
                         layer->textureShader, layer->tmuFlags, modelFlags,
                         complex->m_materials[geoset.materialId].priorityPlane);
        }

        static std::array<bool, 64> reportedCharacterMaterials{};
        static std::array<bool, 64> reportedWorldCharacterMaterials{};
        const bool worldCharacter = model.data && model.data->m_PickLights;
        auto& characterMaterials = worldCharacter
            ? reportedWorldCharacterMaterials : reportedCharacterMaterials;
        if (complex && model.shared &&
            _stricmp(model.shared->name.data(), "Character\\Human\\Male\\HumanMale.MDX") == 0 &&
            geoset.materialId < characterMaterials.size() &&
            !characterMaterials[geoset.materialId]) {
            characterMaterials[geoset.materialId] = true;
            const auto* handle = textureId < complex->m_textures.size()
                ? complex->m_textures[textureId].handle : nullptr;
            std::array<unsigned, 4> pixelMin{255U, 255U, 255U, 255U};
            std::array<unsigned, 4> pixelMax{};
            if (handle && !handle->mipBits.empty()) {
                const auto& bits = handle->mipBits[0].bits;
                for (std::size_t pixel = 0; pixel + 3U < bits.size(); pixel += 4U) {
                    for (std::size_t channel = 0; channel < 4U; ++channel) {
                        pixelMin[channel] = std::min<unsigned>(pixelMin[channel],
                                                               bits[pixel + channel]);
                        pixelMax[channel] = std::max<unsigned>(pixelMax[channel],
                                                               bits[pixel + channel]);
                    }
                }
            }
            std::fprintf(stderr,
                         "%s geoset=%u selection=%u material=%u tex=%u replaceable=%u "
                         "file=%s handle=%p gx=%p %ux%u mips=%zu "
                         "bgra=(%u,%u,%u,%u)-(%u,%u,%u,%u) "
                         "alpha=%g blend=%u flags=%02X combiner=%u coord=%d\n",
                         worldCharacter ? "WORLD_CHARMATERIAL" : "CHARMATERIAL",
                         geoset.geosetId, geoset.selectionGroup, geoset.materialId,
                         textureId,
                         textureId < complex->m_textures.size()
                             ? complex->m_textures[textureId].replaceableId : 0U,
                         handle ? handle->filename.c_str() : "<null>",
                         static_cast<const void*>(handle), static_cast<void*>(texture),
                         texture ? texture->width : 0U, texture ? texture->height : 0U,
                         handle ? handle->mipBits.size() : 0U,
                         pixelMin[0], pixelMin[1], pixelMin[2], pixelMin[3],
                         pixelMax[0], pixelMax[1], pixelMax[2], pixelMax[3],
                         alpha, layer->blendMode, layer->flags, layer->combiner,
                         layer->coordId);
        }
    }

    if (alpha <= 0.0F) return;

    NTempest::C3Vector materialEmissive{};
    if (complex && geoset.materialId < complex->m_materials.size()) {
        materialEmissive = complex->m_materials[geoset.materialId].emissive;
    }

    NTempest::CImVector color(0xFFFFFFFFU);
    if (complex && geoset.geosetId < complex->m_geosetColor.size()) {
        const auto& geosetColor = complex->m_geosetColor[geoset.geosetId];
        NTempest::CImVector rgb;
        rgb = geosetColor.color;
        color.r = rgb.r; color.g = rgb.g; color.b = rgb.b;
        alpha *= std::clamp(geosetColor.alpha, 0.0F, 1.0F);
    }
    color.a = static_cast<std::uint8_t>(alpha * 255.0F + 0.5F);

    static std::array<bool, 64> reportedGeosets{};
    if (model.shared &&
        _stricmp(model.shared->name.data(), "Interface\\Glues\\Models\\UI_MainMenu\\UI_MainMenu.mdx") == 0 &&
        geoset.geosetId < reportedGeosets.size() && !reportedGeosets[geoset.geosetId]) {
        reportedGeosets[geoset.geosetId] = true;
        NTempest::C44Matrix world;
        NTempest::C44Matrix view;
        GxXform(GxXform_World, &world);
        GxXform(GxXform_View, &view);
        const NTempest::C4Vector center{geoset.centroid.x, geoset.centroid.y,
                                        geoset.centroid.z, 1.0F};
        const auto eyeNested = view * (world * center);
        const auto eyeConcat = (world * view) * center;
        int fogEnabled = 0;
        float fogStart = 0.0F;
        float fogEnd = 0.0F;
        GxRsGet(GxRs_Fog, &fogEnabled);
        GxRsGet(GxRs_FogStart, &fogStart);
        GxRsGet(GxRs_FogEnd, &fogEnd);
        std::fprintf(stderr,
                     "GEOSTATE id=%u material=%u color=%u,%u,%u,%u worldD=%g,%g,%g eyeNested=%g,%g,%g,%g eyeConcat=%g,%g,%g,%g fog=%d masterFog=%d range=%g,%g\n",
                     geoset.geosetId, geoset.materialId, color.r, color.g, color.b, color.a,
                     world.d0, world.d1, world.d2,
                     eyeNested.x, eyeNested.y, eyeNested.z, eyeNested.w,
                     eyeConcat.x, eyeConcat.y, eyeConcat.z, eyeConcat.w,
                     fogEnabled, GxMasterEnable(GxMasterEnable_Fog) ? 1 : 0,
                     fogStart, fogEnd);
    }

    const NTempest::C3Vector* positions = geoset.position.data();
    const NTempest::C3Vector* normals = geoset.normal.size() == geoset.position.size()
        ? geoset.normal.data() : nullptr;
    const std::vector<NTempest::C2Vector>* coordinates = nullptr;
    if (coordId >= 0 && !geoset.texCoord.empty()) {
        const auto selected = static_cast<std::size_t>(coordId) < geoset.texCoord.size()
            ? static_cast<std::size_t>(coordId) : 0U;
        if (geoset.texCoord[selected].size() == geoset.position.size()) coordinates = &geoset.texCoord[selected];
    }

    GxRsSet(GxRs_Texture0, texture);
    GxRsSet(GxRs_TexBlend0, static_cast<int>(layer ? layer->combiner : 1U));
    GxRsSet(GxRs_Blend, blend);
    GxRsSet(GxRs_MatDiffuse, color);
    NTempest::CImVector emissiveColor;
    emissiveColor = materialEmissive;
    GxRsSet(GxRs_MatEmissive, emissiveColor);
    GxRsSet(GxRs_Lighting,
            (modelFlags & 0x4U) == 0U && (layerFlags & 0x1U) == 0U);
    GxRsSet(GxRs_Fog,
            (modelFlags & 0x8U) == 0U && (layerFlags & 0x2U) == 0U);
    GxRsSet(GxRs_DepthTest, (layerFlags & 0x4U) == 0U);
    GxRsSet(GxRs_DepthWrite, (layerFlags & 0x8U) == 0U);
    const bool cullingEnabled = (layerFlags & 0x10U) == 0U;
    GxRsSet(GxRs_Culling,
            cullingEnabled
                ? ((g_activeRenderFlags & MODEL_RENDER_REVERSE_CULL) != 0U ? 2 : 1)
                : 0);
    GxPrimLockVertexPtrs(static_cast<unsigned>(geoset.position.size()), positions,
                         sizeof(NTempest::C3Vector), normals,
                         normals ? sizeof(NTempest::C3Vector) : 0U,
                         &color, 0,
                         geoset.boneWeights.empty() ? nullptr : geoset.boneWeights.data(),
                         geoset.boneWeights.empty() ? 0U : 1U,
                         coordinates ? coordinates->data() : nullptr, sizeof(NTempest::C2Vector),
                         nullptr, 0);
    for (const auto& primitive : geoset.primitive) {
        if (primitive.type >= GxPrims_Last || primitive.start > geoset.primitiveVertices.size() ||
            primitive.count > geoset.primitiveVertices.size() - primitive.start)
            continue;
        GxPrimDrawElements(static_cast<EGxPrim>(primitive.type), primitive.count,
                           geoset.primitiveVertices.data() + primitive.start);
    }
    GxPrimUnlockVertexPtrs();
}

void RenderQueuedGeoset(CModel& model, std::uint32_t geoset, std::uint32_t layer,
                        std::uint32_t renderFlags)
{
    if (!model.shared || geoset >= model.shared->geosets.size()) return;
    GxRsPush();
    const std::uint32_t previousRenderFlags = g_activeRenderFlags;
    g_activeRenderFlags = renderFlags;
    GxRsSet(GxRs_NormalizeNormals, 1);
    SetModelLights(model);
    if (layer == kAllLayers) RenderGeoset(model, geoset);
    else RenderGeosetSingleLayer(model, geoset, layer);
    GxRsSet(GxRs_Texture0, static_cast<CGxTex*>(nullptr));
    g_activeRenderFlags = previousRenderFlags;
    GxRsPop();
}

void EnqueueOpaqueGeoset(CModel& model, std::uint32_t geoset, std::uint32_t layer,
                         std::uint32_t renderFlags)
{
    std::scoped_lock lock(g_sceneMutex);
    g_opaque.push_back({&model, geoset, layer, renderFlags});
}

void EnqueueTransparentObject(ETransparentObject type, CModel& model, void* object,
                              std::uint32_t geoset, std::uint32_t renderFlags,
                              std::int32_t priorityPlane,
                              const NTempest::C3Vector& position)
{
    std::scoped_lock lock(g_sceneMutex);
    g_transparent.push_back({type, &model, object, geoset, renderFlags, priorityPlane,
                             SquaredDistanceFromCamera(position), g_sceneInsertionOrder++});
}
}

void ModelRenderInitialize() { std::scoped_lock lock(g_sceneMutex); g_nextMatrix = 0; g_lastMatrixFrame = 0; g_renderReady = true; }
void ModelRenderDestroy() { std::scoped_lock lock(g_sceneMutex); g_opaque.clear(); g_transparent.clear(); g_renderReady = false; }

void FillInRenderData(CModel& model, std::uint32_t renderFlags, CModelRenderData& data)
{
    data.model = &model;
    data.shared = model.shared.get();
    data.renderFlags = renderFlags;
    data.visibleGeosets.clear();
    if (!model.shared) return;
    const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get());
    for (std::uint32_t i = 0; i < model.shared->geosets.size(); ++i)
        if (!complex || i >= complex->m_geosets.size() ||
            (complex->m_geosets[i].flags & 1U) == 0)
            data.visibleGeosets.push_back(i);
}

void ModelAddToScene(CModel& model, std::uint32_t renderFlags)
{
    const char* modelName = model.shared ? model.shared->name.data() : "";
    const bool characterDiagnostic = std::strstr(modelName, "HumanMale") != nullptr;
    static unsigned characterDiagnosticCount = 0;
    if (characterDiagnostic && characterDiagnosticCount < 4U) {
        unsigned visible = 0;
        if (const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get()))
            for (const auto& geoset : complex->m_geosets)
                visible += (geoset.flags & 1U) == 0U;
        std::fprintf(stderr, "Character scene: model=%s geosets=%zu visible=%u world=(%g,%g,%g)\n",
                     modelName, model.shared ? model.shared->geosets.size() : 0U, visible,
                     model.data ? model.data->m_modelToWorld.d0 : 0.0F,
                     model.data ? model.data->m_modelToWorld.d1 : 0.0F,
                     model.data ? model.data->m_modelToWorld.d2 : 0.0F);
        ++characterDiagnosticCount;
    }
    static bool reportedHumanGeosets = false;
    static bool reportedWorldHumanGeosets = false;
    const bool worldCharacter = model.data && model.data->m_PickLights;
    bool& geosetsReported = worldCharacter
        ? reportedWorldHumanGeosets : reportedHumanGeosets;
    if (characterDiagnostic && !geosetsReported) {
        geosetsReported = true;
        if (const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get())) {
            for (std::size_t i = 0; i < model.shared->geosets.size() &&
                                    i < complex->m_geosets.size(); ++i) {
                const auto& geoset = model.shared->geosets[i];
                NTempest::C2Vector uvMin{std::numeric_limits<float>::max()};
                NTempest::C2Vector uvMax{-std::numeric_limits<float>::max()};
                if (!geoset.texCoord.empty()) {
                    for (const auto& uv : geoset.texCoord.front()) {
                        uvMin.x = std::min(uvMin.x, uv.x);
                        uvMin.y = std::min(uvMin.y, uv.y);
                        uvMax.x = std::max(uvMax.x, uv.x);
                        uvMax.y = std::max(uvMax.y, uv.y);
                    }
                }
                std::fprintf(stderr,
                             "%s i=%zu selection=%u material=%u vertices=%zu "
                             "uvLayers=%zu uv0=(%g,%g)-(%g,%g) groups=%zu "
                             "gndx=%zu visible=%u\n",
                             worldCharacter ? "WORLD_CHARGEOSET" : "CHARGEOSET",
                             i, geoset.selectionGroup, geoset.materialId,
                             geoset.position.size(), geoset.texCoord.size(),
                             uvMin.x, uvMin.y, uvMax.x, uvMax.y,
                             geoset.groupMatrixCounts.size(),
                             geoset.boneWeights.size(),
                             (complex->m_geosets[i].flags & 1U) == 0U ? 1U : 0U);
            }
        }
    }
    if (!g_renderReady) ModelRenderInitialize();
    if (dynamic_cast<CModelComplex*>(model.data.get()))
        IModelComplexAddToScene(model, renderFlags);
    else
        IModelSimpleAddToScene(model, renderFlags);

    for (const auto& link : model.links) {
        if (!link.model || link.model == &model ||
            !model.shared || link.point >= model.shared->attachIdToIndex.size())
            continue;
        const auto attachment = model.shared->attachIdToIndex[link.point];
        const bool enabled = attachment != ~0U &&
            (!model.anim || AnimIsAttachmentEnabled(*model.anim, attachment));
        static unsigned linkDiagnosticCount = 0;
        if (linkDiagnosticCount < 4U && std::strstr(modelName, "UI_HUMAN") != nullptr) {
            std::fprintf(stderr, "Character scene link: point=%u attachment=%u enabled=%u child=%s\n",
                         link.point, attachment, enabled ? 1U : 0U,
                         link.model->shared ? link.model->shared->name.data() : "");
            ++linkDiagnosticCount;
        }
        if (!enabled)
            continue;
        ModelAddToScene(*link.model, renderFlags);
    }
}

void ModelBaseRender(CModel& model, std::uint32_t)
{
    if (!model.shared) return;
    GxRsPush();
    GxRsSet(GxRs_NormalizeNormals, 1);
    SetModelLights(model);
    for (std::uint32_t geoset = 0; geoset < model.shared->geosets.size(); ++geoset)
        RenderGeoset(model, geoset);
    GxRsSet(GxRs_Texture0, static_cast<CGxTex*>(nullptr));
    GxRsPop();
}
void ModelComplexRender(CModel& model, std::uint32_t renderFlags) { ModelBaseRender(model, renderFlags); AddEmitters2ToScene(model); AddRibbonsToScene(model); }
void ModelSimpleRender(CModel& model, std::uint32_t renderFlags) { ModelBaseRender(model, renderFlags); }
void ModelRender(CModel& model, std::uint32_t renderFlags)
{
    if (dynamic_cast<CModelComplex*>(model.data.get())) ModelComplexRender(model, renderFlags);
    else ModelSimpleRender(model, renderFlags);
}

void ModelRenderSceneOpaque()
{
    std::vector<CQueuedOpaqueGeoset> queue;
    { std::scoped_lock lock(g_sceneMutex); queue.swap(g_opaque); }
    for (const auto& item : queue)
        if (item.model)
            RenderQueuedGeoset(*item.model, item.geoset, item.layer, item.renderFlags);
}
void ModelRenderSceneTransparent()
{
    std::vector<CQueuedTransparentObject> queue;
    { std::scoped_lock lock(g_sceneMutex); queue.swap(g_transparent); }
    std::stable_sort(queue.begin(), queue.end(), [](const auto& left, const auto& right) {
        if (left.priorityPlane != right.priorityPlane)
            return left.priorityPlane < right.priorityPlane;
        if (left.squaredDistance != right.squaredDistance)
            return left.squaredDistance > right.squaredDistance;
        return left.insertionOrder < right.insertionOrder;
    });
    for (const auto& item : queue) {
        if (!item.model) continue;
        switch (item.type) {
        case ETransparentObject::Geoset:
            RenderQueuedGeoset(*item.model, item.geoset, kAllLayers, item.renderFlags);
            break;
        case ETransparentObject::ParticleEmitter:
            if (auto* emitter = static_cast<CParticleEmitter2*>(item.object)) {
                if (item.model->data && item.model->data->m_PickLights) {
                    const auto lightPosition = g_cameraPosition +
                        NTempest::C3Vector{item.model->data->m_modelToWorld.d0,
                                           item.model->data->m_modelToWorld.d1,
                                           item.model->data->m_modelToWorld.d2};
                    item.model->data->m_PickLights(item.model->data->m_pickLightsParm,
                                                   lightPosition, &g_cameraPosition, 8U);
                }
                emitter->Render(&g_cameraPosition);
            }
            break;
        case ETransparentObject::RibbonEmitter:
            if (auto* ribbon = static_cast<CRibbonEmitter*>(item.object)) ribbon->Render();
            break;
        }
    }
}
void ModelRenderScene() { ModelRenderSceneOpaque(); ModelRenderSceneTransparent(); }

void TransformBounds(const NTempest::CAaBox& source, const NTempest::C34Matrix& transform,
                     NTempest::CAaBox& destination)
{
    std::array<NTempest::C3Vector, 8> corners{{
        {source.b.x, source.b.y, source.b.z}, {source.t.x, source.b.y, source.b.z},
        {source.b.x, source.t.y, source.b.z}, {source.t.x, source.t.y, source.b.z},
        {source.b.x, source.b.y, source.t.z}, {source.t.x, source.b.y, source.t.z},
        {source.b.x, source.t.y, source.t.z}, {source.t.x, source.t.y, source.t.z},
    }};
    for (auto& corner : corners) corner = transform * corner;
    destination.Bounding(corners.data(), static_cast<unsigned>(corners.size()));
}

bool ModelGetBounds(const CModel& model, CBoundsData& bounds)
{
    if (!model.shared) return false;
    bounds = model.shared->bounds;
    if (model.data) {
        TransformBounds(model.shared->bounds.extent, model.data->m_modelToWorld, bounds.extent);
        bounds.sphere.c = model.data->m_modelToWorld * model.shared->bounds.sphere.c;
    }
    return true;
}
bool ModelGetExtents(const CModel& model, NTempest::CAaBox& extents)
{
    CBoundsData bounds;
    if (!ModelGetBounds(model, bounds)) return false;
    extents = bounds.extent;
    return true;
}
bool ModelGetSeqExtents(const CModel& model, std::uint32_t sequence, NTempest::CAaBox& extents)
{
    if (!model.shared || sequence >= model.shared->seqBounds.size()) return ModelGetExtents(model, extents);
    extents = model.shared->seqBounds[sequence].extent;
    if (model.data) { NTempest::CAaBox transformed; TransformBounds(extents, model.data->m_modelToWorld, transformed); extents = transformed; }
    return true;
}

bool LineSegmentIntersectBox(const NTempest::C3Vector& start, const NTempest::C3Vector& end,
                             const NTempest::CAaBox& box, float& distance)
{
    const auto delta = end - start;
    float low = 0.0F, high = 1.0F;
    for (unsigned axis = 0; axis < 3; ++axis) {
        if (std::fabs(delta[axis]) < 1.0e-7F) {
            if (start[axis] < box.b[axis] || start[axis] > box.t[axis]) return false;
        } else {
            float a = (box.b[axis] - start[axis]) / delta[axis];
            float b = (box.t[axis] - start[axis]) / delta[axis];
            if (a > b) std::swap(a, b);
            low = std::max(low, a); high = std::min(high, b);
            if (low > high) return false;
        }
    }
    distance = low;
    return true;
}

bool LineSegmentIntersectSphere(const NTempest::C3Vector& start, const NTempest::C3Vector& end,
                                const NTempest::CAaSphere& sphere, float& distance)
{
    const auto delta = end - start;
    const auto offset = start - sphere.c;
    const float a = NTempest::C3Vector::Dot(delta, delta);
    const float b = 2.0F * NTempest::C3Vector::Dot(offset, delta);
    const float c = NTempest::C3Vector::Dot(offset, offset) - sphere.r * sphere.r;
    const float disc = b * b - 4.0F * a * c;
    if (disc < 0.0F || a <= 0.0F) return false;
    const float root = std::sqrt(disc);
    const float t0 = (-b - root) / (2.0F * a);
    const float t1 = (-b + root) / (2.0F * a);
    const float t = t0 >= 0.0F ? t0 : t1;
    if (t < 0.0F || t > 1.0F) return false;
    distance = t;
    return true;
}

bool LineSegmentIntersectCylinder(const NTempest::C3Vector& start, const NTempest::C3Vector& end,
                                  const NTempest::C3Vector& base, float radius, float height,
                                  float& distance)
{
    const auto d = end - start;
    const float ox = start.x - base.x, oy = start.y - base.y;
    const float a = d.x * d.x + d.y * d.y;
    const float b = 2.0F * (ox * d.x + oy * d.y);
    const float c = ox * ox + oy * oy - radius * radius;
    const float disc = b * b - 4.0F * a * c;
    if (disc < 0.0F || a <= 0.0F) return false;
    const float root = std::sqrt(disc);
    for (const float t : {(-b - root) / (2.0F * a), (-b + root) / (2.0F * a)}) {
        const float z = start.z + d.z * t;
        if (t >= 0.0F && t <= 1.0F && z >= base.z && z <= base.z + height) { distance = t; return true; }
    }
    return false;
}

bool ModelTestSphere(const CModel& model, const NTempest::CAaSphere& sphere)
{
    CBoundsData bounds;
    return ModelGetBounds(model, bounds) && NTempest::Intersect(bounds.sphere, sphere);
}
bool ModelIntersectLineSegment(const CModel& model, const NTempest::C3Vector& start,
                               const NTempest::C3Vector& end, float& distance)
{
    const auto result = ModelIntersectLineSegmentEx(model, start, end);
    if (result.hit) distance = result.distance;
    return result.hit;
}
ModelIntersectResult ModelIntersectLineSegmentEx(const CModel& model,
                                                 const NTempest::C3Vector& start,
                                                 const NTempest::C3Vector& end)
{
    ModelIntersectResult result;
    if (model.shared && model.shared->collision && ModelCollisionVectorIntersect(model, start, end - start, result)) return result;
    CBoundsData bounds;
    if (ModelGetBounds(model, bounds) && LineSegmentIntersectBox(start, end, bounds.extent, result.distance)) {
        result.hit = true; result.point = start + (end - start) * result.distance;
    }
    return result;
}
bool ModelHitTestSphere(const CModel& model, const NTempest::CAaSphere& sphere) { return ModelTestSphere(model, sphere); }
bool ModelHasHitTestVolumes(const CModel& model) { return model.shared && !model.shared->hitTest.empty(); }

bool ModelHitTestVolumes(const CModel& model, const NTempest::C3Vector& start,
                         const NTempest::C3Vector& end, ModelIntersectResult& result)
{
    if (!model.shared) return false;
    float nearest = std::numeric_limits<float>::max();
    for (std::uint32_t i = 0; i < model.shared->hitTest.size(); ++i) {
        const auto& hit = model.shared->hitTest[i];
        float t = 0.0F;
        bool found = false;
        if (hit.type == COLLIDE_SPHERE) found = LineSegmentIntersectSphere(start, end, {hit.extent[0], hit.radius}, t);
        else if (hit.type == COLLIDE_BOX) found = LineSegmentIntersectBox(start, end, {hit.extent[0], hit.extent[1]}, t);
        else if (hit.type == COLLIDE_CYLINDER) found = LineSegmentIntersectCylinder(start, end, hit.extent[0], hit.radius, hit.extent[1].z, t);
        if (found && t < nearest) { nearest = t; result.index = i; result.hit = true; }
    }
    if (result.hit) { result.distance = nearest; result.point = start + (end - start) * nearest; }
    return result.hit;
}

void GenerateSphereVerts(float radius, std::uint32_t rings, std::uint32_t segments,
                         std::vector<NTempest::C3Vector>& vertices, std::vector<std::uint16_t>& indices)
{
    rings = std::max(2U, rings); segments = std::max(3U, segments);
    vertices.clear(); indices.clear();
    if ((rings + 1ULL) * (segments + 1ULL) > 65535ULL) return;
    for (std::uint32_t ring = 0; ring <= rings; ++ring) {
        const float phi = kPi * float(ring) / float(rings);
        for (std::uint32_t segment = 0; segment <= segments; ++segment) {
            const float theta = 2.0F * kPi * float(segment) / float(segments);
            vertices.push_back({radius * std::sin(phi) * std::cos(theta), radius * std::sin(phi) * std::sin(theta), radius * std::cos(phi)});
        }
    }
    for (std::uint32_t ring = 0; ring < rings; ++ring) for (std::uint32_t segment = 0; segment < segments; ++segment) {
        const auto a = std::uint16_t(ring * (segments + 1U) + segment), b = std::uint16_t(a + segments + 1U);
        indices.insert(indices.end(), {a, b, std::uint16_t(a + 1U), std::uint16_t(a + 1U), b, std::uint16_t(b + 1U)});
    }
}

void GenerateCylinderVerts(float radius, float height, std::uint32_t segments,
                           std::vector<NTempest::C3Vector>& vertices, std::vector<std::uint16_t>& indices)
{
    segments = std::clamp(segments, 3U, 32766U); vertices.clear(); indices.clear();
    for (std::uint32_t i = 0; i < segments; ++i) {
        const float angle = 2.0F * kPi * float(i) / float(segments);
        const float x = radius * std::cos(angle), y = radius * std::sin(angle);
        vertices.push_back({x, y, 0.0F}); vertices.push_back({x, y, height});
    }
    for (std::uint32_t i = 0; i < segments; ++i) {
        const auto a = std::uint16_t(i * 2U), b = std::uint16_t(((i + 1U) % segments) * 2U);
        indices.insert(indices.end(), {a, b, std::uint16_t(a + 1U), std::uint16_t(a + 1U), b, std::uint16_t(b + 1U)});
    }
}

void CreateBoxGeometry(const NTempest::CAaBox& box, std::vector<NTempest::C3Vector>& vertices,
                       std::vector<std::uint16_t>& indices)
{
    vertices = {{box.b.x,box.b.y,box.b.z},{box.t.x,box.b.y,box.b.z},{box.t.x,box.t.y,box.b.z},{box.b.x,box.t.y,box.b.z},
                {box.b.x,box.b.y,box.t.z},{box.t.x,box.b.y,box.t.z},{box.t.x,box.t.y,box.t.z},{box.b.x,box.t.y,box.t.z}};
    indices = {0,2,1,0,3,2,4,5,6,4,6,7,0,1,5,0,5,4,1,2,6,1,6,5,2,3,7,2,7,6,3,0,4,3,4,7};
}

void AddAllGeosetsToScene(CModel& model, std::uint32_t renderFlags)
{
    if (!model.shared) return;
    for (std::uint32_t geoset = 0; geoset < model.shared->geosets.size(); ++geoset)
        AddGeosetToScene(model, geoset, renderFlags);
}
void AddGeosetToScene(CModel& model, std::uint32_t geoset, std::uint32_t renderFlags)
{
    if (!model.shared || geoset >= model.shared->geosets.size()) return;
    const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get());
    if (complex && geoset < complex->m_geosets.size() &&
        (complex->m_geosets[geoset].flags & 1U) != 0U)
        return;

    const auto& sharedGeoset = model.shared->geosets[geoset];
    if (complex && sharedGeoset.geosetId < complex->m_geosetColor.size() &&
        complex->m_geosetColor[sharedGeoset.geosetId].alpha <= 0.0F)
        return;

    const CMaterial* material = nullptr;
    if (complex && sharedGeoset.materialId < complex->m_materials.size())
        material = &complex->m_materials[sharedGeoset.materialId];

    const bool opaque = !material || IsOpaque(model, *material);
    if (!opaque) {
        if ((renderFlags & 3U) == 0U) {
            const auto sortPosition = GetGeosetSortPos(model, geoset);
            EnqueueTransparentObject(ETransparentObject::Geoset, model, nullptr, geoset,
                                     renderFlags, material->priorityPlane, sortPosition);
        }
        if ((renderFlags & 1U) != 0U || (renderFlags & 2U) == 0U) return;
    }

    if (!material || material->layers.empty()) {
        EnqueueOpaqueGeoset(model, geoset, kAllLayers, renderFlags);
        return;
    }
    for (std::uint32_t layer = 0; layer < material->layers.size(); ++layer)
        if (EffectiveLayerAlpha(model, material->layers[layer]) > 0.0F)
            EnqueueOpaqueGeoset(model, geoset, layer, renderFlags);
}
void AddEmitters2ToScene(CModel& model)
{
    auto* complex = dynamic_cast<CModelComplex*>(model.data.get());
    if (!complex) return;
    for (auto* emitter : complex->m_emitters2) {
        if (!emitter) continue;
        const auto position = NTempest::C3Vector{emitter->m_modelToWorld.d0,
                                                  emitter->m_modelToWorld.d1,
                                                  emitter->m_modelToWorld.d2} -
            emitter->m_cameraWorldPos;
        EnqueueTransparentObject(ETransparentObject::ParticleEmitter, model, emitter, 0, 0,
                                 emitter->m_priorityPlane, position);
    }
}
void AddRibbonsToScene(CModel& model)
{
    auto* complex = dynamic_cast<CModelComplex*>(model.data.get());
    if (!complex) return;
    for (auto* ribbon : complex->m_ribbons)
        if (ribbon)
            EnqueueTransparentObject(ETransparentObject::RibbonEmitter, model, ribbon, 0, 0,
                                     0, ribbon->m_currPos);
}
void IModelBaseAddToScene(CModel&, std::uint32_t) {}
void IModelComplexAddToScene(CModel& model, std::uint32_t renderFlags)
{
    IModelBaseAddToScene(model, renderFlags);
    if (!model.data || (model.data->m_flags & 0x10U) == 0U)
        AddAllGeosetsToScene(model, renderFlags);
    if ((renderFlags & 2U) == 0U) {
        AddEmitters2ToScene(model);
        AddRibbonsToScene(model);
    }
}
void IModelSimpleAddToScene(CModel& model, std::uint32_t renderFlags)
{
    IModelBaseAddToScene(model, renderFlags);
    if (!model.data || (model.data->m_flags & 0x10U) == 0U)
        AddAllGeosetsToScene(model, renderFlags);
}
bool IModelTestRay(const CModel& model, const NTempest::C3Vector& start, const NTempest::C3Vector& end, ModelIntersectResult& result) { result = ModelIntersectLineSegmentEx(model, start, end); return result.hit; }
bool IModelTestCollisionVolumes(const CModel& model, const NTempest::C3Vector& start, const NTempest::C3Vector& end, ModelIntersectResult& result) { return ModelHitTestVolumes(model, start, end, result); }

bool GeosetTestRay(const CGeosetShared& geoset, const NTempest::C3Vector& start,
                   const NTempest::C3Vector& end, ModelIntersectResult& result)
{
    const auto delta = end - start; float nearest = std::numeric_limits<float>::max();
    for (std::size_t i = 0; i + 2U < geoset.primitiveVertices.size(); i += 3U) {
        const auto ia = geoset.primitiveVertices[i], ib = geoset.primitiveVertices[i+1U], ic = geoset.primitiveVertices[i+2U]; if (ia >= geoset.position.size() || ib >= geoset.position.size() || ic >= geoset.position.size()) continue;
        NTempest::CFacet facet; facet.Set(geoset.position[ia], geoset.position[ib], geoset.position[ic]); float t = 0;
        if (NTempest::Intersect(facet, start, delta, t) && t >= 0 && t <= 1 && t < nearest) { nearest = t; result.hit = true; result.index = static_cast<std::uint32_t>(i / 3U); result.normal = facet.plane.n; }
    }
    if (result.hit) { result.distance = nearest; result.point = start + delta * nearest; } return result.hit;
}

void CreateSphereGeometry(const NTempest::CAaSphere& sphere, CGeosetShared& geoset, std::uint32_t rings, std::uint32_t segments)
{
    GenerateSphereVerts(sphere.r, rings, segments, geoset.position, geoset.primitiveVertices); for (auto& position : geoset.position) position += sphere.c; geoset.normal.resize(geoset.position.size()); for (std::size_t i=0;i<geoset.position.size();++i){geoset.normal[i]=geoset.position[i]-sphere.c;if(geoset.normal[i].SquaredMag()>0)geoset.normal[i].Normalize();} geoset.primitive={{4U,0U,static_cast<std::uint32_t>(geoset.primitiveVertices.size())}};
}
void CreateCylinderGeometry(float radius, float height, CGeosetShared& geoset, std::uint32_t segments) { GenerateCylinderVerts(radius,height,segments,geoset.position,geoset.primitiveVertices); geoset.primitive={{4U,0U,static_cast<std::uint32_t>(geoset.primitiveVertices.size())}}; }
void CreatePlanarQuadGeometry(float width, float height, CGeosetShared& geoset) { const float x=width*0.5F,y=height*0.5F;geoset.position={{-x,-y,0},{x,-y,0},{x,y,0},{-x,y,0}};geoset.normal.assign(4,{0,0,1});geoset.texCoord={{{0,0},{1,0},{1,1},{0,1}}};geoset.primitiveVertices={0,1,2,0,2,3};geoset.primitive={{4,0,6}}; }
std::shared_ptr<CModel> ModelCreateBox(const NTempest::CAaBox& box) { return CollisionDataRenderAABox(box); }
std::shared_ptr<CModel> ModelCreateSolidSphere(const NTempest::CAaSphere& sphere, std::uint32_t rings, std::uint32_t segments) { CGeosetShared geoset;CreateSphereGeometry(sphere,geoset,rings,segments);return CreateSimpleModel(geoset,"SolidSphere"); }

void ModelShowBoundingBox(CModel& model) { if(model.data)model.data->m_flags|=0x100U; }
void ModelShowBoundingSphere(CModel& model) { if(model.data)model.data->m_flags|=0x200U; }
void ModelShowHitTestGeometry(CModel& model) { if(model.data)model.data->m_flags|=0x400U; }
void ModelHideBounds(CModel& model) { if(model.data)model.data->m_flags&=~0x300U; }
void ModelHideHitTestGeometry(CModel& model) { if(model.data)model.data->m_flags&=~0x400U; }
bool ModelHitTestGeometry(const CModel& model,const NTempest::C3Vector& start,const NTempest::C3Vector& end,ModelIntersectResult& result){return IModelTestRay(model,start,end,result);}
void ModelRenderSceneLogStart(){g_renderLogging=true;} void ModelRenderSceneLogStop(){g_renderLogging=false;} void ModelRenderSceneLogToggle(){g_renderLogging=!g_renderLogging;}
void ModelSceneSetSharpness(float sharpness){g_sceneSharpness=std::max(0.0F,sharpness);}
NTempest::C2Vector Project2d(const NTempest::C3Vector& world){const float z=std::fabs(world.z)>1.0e-6F?world.z:1.0F;return {world.x/z*g_sceneSharpness,world.y/z*g_sceneSharpness};}
bool IsOpaque(const CMaterial& material)
{
    for (const auto& layer : material.layers) {
        if (layer.alpha <= 0.0F) continue;
        return layer.blendMode < GxBlend_Alpha && (layer.flags & 0x4U) == 0U;
    }
    return true;
}
std::uint32_t GetNumTexCoordLayers(const CGeosetShared& geoset){return static_cast<std::uint32_t>(geoset.texCoord.size());}
std::span<CModelTexture> GetModelTextures(CModel& model){if(auto* complex=dynamic_cast<CModelComplex*>(model.data.get()))return complex->m_textures;return {};}
std::span<CModelTexture> GetTextureList(CModel& model){return GetModelTextures(model);}
std::uint32_t GetInvalidMatrixId()
{
    return static_cast<std::uint32_t>(static_cast<std::uint16_t>(g_currAnimFrame - 1U)) << 16U;
}

std::uint32_t MatrixAlloc(std::uint32_t count)
{
    const auto frame = GxPerfCounter(GxPerf_FrameNum);
    if (frame != g_lastMatrixFrame) {
        ++g_currAnimFrame;
        g_lastMatrixFrame = frame;
        g_nextMatrix = 0;
    }
    if (count == 0) return GetInvalidMatrixId();
    if (g_nextMatrix > 0xFFFFU || count > 0x10000U - g_nextMatrix)
        return GetInvalidMatrixId();

    const auto first = g_nextMatrix;
    const auto required = first + count;
    if (g_matrixPool.size() < required) g_matrixPool.resize(required);
    g_nextMatrix = required;
    return first | (static_cast<std::uint32_t>(g_currAnimFrame) << 16U);
}

NTempest::C34Matrix* MatrixDeref(std::uint32_t matrix)
{
    if (static_cast<std::uint16_t>(matrix >> 16U) != g_currAnimFrame) return nullptr;
    const auto index = static_cast<std::uint16_t>(matrix);
    return index < g_matrixPool.size() ? &g_matrixPool[index] : nullptr;
}
void SetVertexMatrixIndices(CGeosetShared& geoset,std::span<const std::uint32_t> indices){geoset.hwBoneIndices.assign(indices.begin(),indices.end());}

bool COpaqueLayer::HasHigherPriority(const COpaqueLayer& other) const{return priority>other.priority;}
bool CTransparentObject::HasHigherPriority(const CTransparentObject& other) const{return depth>other.depth;}
int CompareTexLayers(const CTexLayer& left,const CTexLayer& right){if(left.blendMode!=right.blendMode)return left.blendMode<right.blendMode?-1:1;if(left.texture!=right.texture)return left.texture<right.texture?-1:1;return 0;}
void AddHitTestGeometryGeoset(CModel& model,const CGeosetShared& geoset){IModelGeosetAdd(model,geoset);}
CGeosetShared BuildComplexGeoset(std::span<const NTempest::C3Vector> vertices,std::span<const std::uint16_t> indices){CGeosetShared geoset;geoset.position.assign(vertices.begin(),vertices.end());geoset.primitiveVertices.assign(indices.begin(),indices.end());geoset.primitive={{4U,0U,static_cast<std::uint32_t>(indices.size())}};return geoset;}
std::shared_ptr<CModel> CreateModelBoundingBox(const CModel& model){NTempest::CAaBox box;if(!ModelGetExtents(model,box))return{};return ModelCreateBox(box);}
std::shared_ptr<CModel> CreateModelBoundingSphere(const CModel& model){CBoundsData bounds;if(!ModelGetBounds(model,bounds))return{};return ModelCreateSolidSphere(bounds.sphere);}
void EnqueueSimpleObject(CModel& model,std::uint32_t renderFlags){ModelAddToScene(model,renderFlags);}
void EnqueueTransparentGeoset(CModel& model,std::uint32_t geoset)
{
    if (!model.shared || geoset >= model.shared->geosets.size()) return;
    const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get());
    const auto materialId = model.shared->geosets[geoset].materialId;
    const auto priority = complex && materialId < complex->m_materials.size()
        ? complex->m_materials[materialId].priorityPlane : 0;
    EnqueueTransparentObject(ETransparentObject::Geoset, model, nullptr, geoset, 0,
                             priority, GetGeosetSortPos(model, geoset));
}
void GeosetComplexRender(CModel& model,std::uint32_t geoset,std::uint32_t){if(RenderGeosetCheckVis(model,geoset))RenderGeoset(model,geoset);}
NTempest::C3Vector GetGeosetSortPos(const CModel& model,std::uint32_t geoset)
{
    if (!model.shared || geoset >= model.shared->geosets.size()) return {};
    const auto& shared = model.shared->geosets[geoset];
    auto position = model.data ? model.data->m_modelToWorld * shared.centroid : shared.centroid;
    if ((shared.flags & 0x4U) != 0U) position -= g_cameraDirection * shared.radius;
    else if ((shared.flags & 0x8U) != 0U) position += g_cameraDirection * shared.radius;
    return position;
}
bool IModelGeosetAdd(CModel& model,const CGeosetShared& geoset){if(!model.shared)return false;model.shared->geosets.push_back(geoset);if(auto*complex=dynamic_cast<CModelComplex*>(model.data.get()))complex->m_geosets.push_back({GetInvalidMatrixId(),geoset.flags});return true;}
bool IModelHandleGeosetAdd(CModel& model,const CGeosetShared& geoset){return IModelGeosetAdd(model,geoset);}
bool IModelGetBoundingSphere(const CModel& model,NTempest::CAaSphere& sphere){CBoundsData bounds;if(!ModelGetBounds(model,bounds))return false;sphere=bounds.sphere;return true;}
bool IModelGetExtents(const CModel& model,NTempest::CAaBox& extents){return ModelGetExtents(model,extents);}
void IModelRenderSceneOpaque(){ModelRenderSceneOpaque();} void IModelRenderSceneTransparent(){ModelRenderSceneTransparent();}
std::span<NTempest::C3Vector> LockVertices(CGeosetShared& geoset,std::size_t count){geoset.position.resize(count);return geoset.position;}
bool LockVertsAndIndices(CGeosetShared& geoset,std::size_t vertexCount,std::size_t indexCount){if(vertexCount>65535U)return false;geoset.position.resize(vertexCount);geoset.primitiveVertices.resize(indexCount);return true;}

void ModelSceneCalcFrustumPlanes(const NTempest::C44Matrix& matrix)
{
    const float* m=matrix.Data(); const std::array<std::array<float,4>,6> p{{{{m[3]+m[0],m[7]+m[4],m[11]+m[8],m[15]+m[12]}},{{m[3]-m[0],m[7]-m[4],m[11]-m[8],m[15]-m[12]}},{{m[3]+m[1],m[7]+m[5],m[11]+m[9],m[15]+m[13]}},{{m[3]-m[1],m[7]-m[5],m[11]-m[9],m[15]-m[13]}},{{m[3]+m[2],m[7]+m[6],m[11]+m[10],m[15]+m[14]}},{{m[3]-m[2],m[7]-m[6],m[11]-m[10],m[15]-m[14]}}}};
    for(std::size_t i=0;i<p.size();++i){g_frustumPlanes[i]={NTempest::C3Vector{p[i][0],p[i][1],p[i][2]},p[i][3]};const float mag=std::sqrt(g_frustumPlanes[i].n.SquaredMag());if(mag>0){g_frustumPlanes[i].n/=mag;g_frustumPlanes[i].d/=mag;}}
}
std::span<const NTempest::C4Plane> ModelSceneGetFrustumPlanes(){return g_frustumPlanes;}
void ModelSceneSetFrustumPlanes(std::span<const NTempest::C4Plane> planes){for(std::size_t i=0;i<std::min(planes.size(),g_frustumPlanes.size());++i)g_frustumPlanes[i]=planes[i];}
void ModelScenePlaceCamera(const NTempest::C3Vector& position)
{
    ModelScenePlaceCamera(position, g_cameraDirection);
}
void ModelScenePlaceCamera(const NTempest::C3Vector& position,
                           const NTempest::C3Vector& direction)
{
    std::scoped_lock lock(g_sceneMutex);
    g_cameraPosition = position;
    g_cameraDirection = direction;
}
void ModelSetProject2dCallback(std::function<NTempest::C2Vector(const NTempest::C3Vector&)> callback){g_project2d=std::move(callback);}
void ClearTransformedUVLayer(std::uint32_t layer){if(layer<g_transformedUv.size())g_transformedUv[layer].clear();}
void ClearUvTransforms(){g_transformedUv.clear();}
std::span<const NTempest::C2Vector> GetTransformedUVLayer(std::uint32_t layer){return layer<g_transformedUv.size()?std::span<const NTempest::C2Vector>(g_transformedUv[layer]):std::span<const NTempest::C2Vector>{};}
void SetUvTransforms(std::uint32_t layer,std::span<const NTempest::C2Vector> coordinates){if(g_transformedUv.size()<=layer)g_transformedUv.resize(layer+1U);g_transformedUv[layer].assign(coordinates.begin(),coordinates.end());}
void RenderCustomGeoset(CModel& model,std::uint32_t geoset){RenderGeoset(model,geoset);}
void RenderGeoset(CModel& model,std::uint32_t geoset){RenderGeosetLayers(model,geoset);}
bool RenderGeosetCheckVis(const CModel& model,std::uint32_t geoset)
{
    const auto* unique = GetUniqueGeoset(model, geoset);
    return unique != nullptr && (unique->flags & 1U) == 0U;
}
void RenderGeosetLayers(CModel& model,std::uint32_t geoset)
{
    if (!RenderGeosetPrep(model, geoset)) return;
    const auto& shared = model.shared->geosets[geoset];
    const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get());
    if (!complex || shared.materialId >= complex->m_materials.size() ||
        complex->m_materials[shared.materialId].layers.empty()) {
        DrawGeosetLayer(model, shared, nullptr);
        RenderGeosetFinish(shared);
        return;
    }
    for (const auto& layer : complex->m_materials[shared.materialId].layers)
        DrawGeosetLayer(model, shared, &layer);
    RenderGeosetFinish(shared);
}
void RenderGeosetMultiUvMapping(CModel& model,std::uint32_t geoset){RenderGeosetLayers(model,geoset);}
void RenderGeosetOneUvMapping(CModel& model,std::uint32_t geoset){SingleUvMapping(model,geoset);}
bool RenderGeosetPrep(const CModel& model,std::uint32_t geoset)
{
    if (!model.shared || geoset >= model.shared->geosets.size() ||
        !RenderGeosetCheckVis(model, geoset)) return false;

    const auto& shared = model.shared->geosets[geoset];
    const auto* unique = GetUniqueGeoset(model, geoset);
    auto* weightedBones = unique ? MatrixDeref(unique->weightedBones) : nullptr;
    const auto shader = static_cast<EGxVertexShader>(shared.vertexShader);
    if (shader == GxVS_PassThru) {
        GxXformPush(GxXform_World);
        if (weightedBones) GxXformMult(GxXform_World, ToC44Matrix(*weightedBones));
    } else {
        if (!weightedBones || shared.groupMatrixCounts.empty()) return false;
        GxXformSetBones(static_cast<unsigned>(shared.groupMatrixCounts.size()), weightedBones);
    }
    GxVertexShaderSelect(shader);
    return true;
}
void RenderGeosetSingleLayer(CModel& model,std::uint32_t geoset,std::uint32_t layer)
{
    if (!RenderGeosetPrep(model, geoset)) return;
    const auto* complex = dynamic_cast<const CModelComplex*>(model.data.get());
    const auto& shared = model.shared->geosets[geoset];
    if (!complex || shared.materialId >= complex->m_materials.size() ||
        layer >= complex->m_materials[shared.materialId].layers.size()) {
        RenderGeosetFinish(shared);
        return;
    }
    DrawGeosetLayer(model, shared, &complex->m_materials[shared.materialId].layers[layer]);
    RenderGeosetFinish(shared);
}
void RenderSingleUVMapPrep(CModel& model,std::uint32_t geoset){if(model.shared&&geoset<model.shared->geosets.size()&&RenderGeosetCheckVis(model,geoset)&&!model.shared->geosets[geoset].texCoord.empty())SetUvTransforms(0,model.shared->geosets[geoset].texCoord[0]);}
void RenderSortedGeoset(CModel& model,std::uint32_t geoset){RenderGeoset(model,geoset);}
void RenderUniformUVMapLayers(CModel& model,std::uint32_t geoset){RenderGeosetLayers(model,geoset);}
void SingleUvMapping(CModel& model,std::uint32_t geoset){RenderGeosetLayers(model,geoset);}
std::uint32_t SaveFog(){return g_fogState;} void RestoreFog(std::uint32_t state){g_fogState=state;}
CModelTexture::~CModelTexture() = default;
int CTexLayer::Compare(const CModelTexture*,const CTexLayer& left,const CTexLayer& right){return CompareTexLayers(left,right);}
