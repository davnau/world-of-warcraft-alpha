#include "ENGINE/Source/Model/mdlparticleemitter2.h"

#include "ENGINE/Source/Model/modelparse.h"
#include "ENGINE/Source/Services/gfxsingletonmanager.h"
#include "ENGINE/Source/Services/particleemitters.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>

namespace {

constexpr std::size_t kGenericObjectBaseSize = 96;
constexpr std::size_t kPre2FixedStaticSize = 791;

std::string FixedString(const std::array<char, 260>& value)
{
    const auto end = std::find(value.begin(), value.end(), '\0');
    return std::string(value.begin(), end);
}

std::uint8_t ColorByte(float value)
{
    // MDL text colors are normalized. A few early tools emitted byte-domain
    // defaults, so accept either representation.
    if (value <= 1.0F) value *= 255.0F;
    return static_cast<std::uint8_t>(std::clamp(std::lround(value), 0L, 255L));
}

NTempest::CImVector ParticleColor(const NTempest::C3Vector& color, std::uint8_t alpha)
{
    NTempest::CImVector result;
    result.r = ColorByte(color.x);
    result.g = ColorByte(color.y);
    result.b = ColorByte(color.z);
    result.a = alpha;
    return result;
}

bool ReadFixedString(ModelByteReader& reader, std::string& value)
{
    std::array<char, 260> bytes{};
    if (!reader.Read(bytes)) return false;
    value = FixedString(bytes);
    return true;
}

bool ReadDefinition(ModelByteReader& reader, MDLPARTICLEEMITTER2& out)
{
    const std::size_t outerStart = reader.Position();
    std::uint32_t outerSize = 0;
    if (!reader.Read(outerSize) || outerSize < 8 || outerSize > reader.Remaining() + 4)
        return false;
    const std::size_t outerEnd = outerStart + outerSize;

    const std::size_t genericStart = reader.Position();
    std::uint32_t genericSize = 0;
    if (!reader.Read(genericSize) || genericSize < kGenericObjectBaseSize ||
        genericSize > outerEnd - genericStart)
        return false;
    std::array<char, 80> name{};
    if (!reader.Read(name) || !reader.Read(out.objectId) || !reader.Read(out.parentId) ||
        !reader.Read(out.flags) || !reader.Skip(genericSize - kGenericObjectBaseSize))
        return false;

    const std::size_t staticStart = reader.Position();
    std::uint32_t staticSize = 0;
    if (!reader.Read(staticSize) || staticSize < kPre2FixedStaticSize ||
        staticSize > outerEnd - staticStart)
        return false;

    if (!reader.Read(out.emitterType) ||
        !reader.Read(out.staticSpeed) || !reader.Read(out.staticVariation) ||
        !reader.Read(out.staticLatitude) || !reader.Read(out.staticLongitude) ||
        !reader.Read(out.staticGravity) || !reader.Read(out.staticZsource) ||
        !reader.Read(out.staticLife) || !reader.Read(out.staticEmissionRate) ||
        !reader.Read(out.staticLength) || !reader.Read(out.staticWidth) ||
        !reader.Read(out.rows) || !reader.Read(out.columns) ||
        !reader.Read(out.particleType) || !reader.Read(out.tailLength) ||
        !reader.Read(out.middleTime))
        return false;

    for (auto& color : out.colors) if (!reader.Read(color)) return false;
    for (auto& alpha : out.alpha) if (!reader.Read(alpha)) return false;
    for (auto& scale : out.scale) if (!reader.Read(scale)) return false;
    for (auto& cell : out.lifespanUV) if (!reader.Read(cell)) return false;
    for (auto& cell : out.decayUV) if (!reader.Read(cell)) return false;
    for (auto& cell : out.tailUV) if (!reader.Read(cell)) return false;
    for (auto& cell : out.tailDecayUV) if (!reader.Read(cell)) return false;

    if (!reader.Read(out.blendMode) || !reader.Read(out.textureId) ||
        !reader.Read(out.priorityPlane) || !reader.Read(out.replaceableId) ||
        !ReadFixedString(reader, out.geometryModel) ||
        !ReadFixedString(reader, out.recursionModel) ||
        !reader.Read(out.twinkleFPS) || !reader.Read(out.twinkleOnOff) ||
        !reader.Read(out.twinkleScaleMin) || !reader.Read(out.twinkleScaleMax) ||
        !reader.Read(out.inverseVelocityScale) ||
        !reader.Read(out.tumbleX[0]) || !reader.Read(out.tumbleX[1]) ||
        !reader.Read(out.tumbleY[0]) || !reader.Read(out.tumbleY[1]) ||
        !reader.Read(out.tumbleZ[0]) || !reader.Read(out.tumbleZ[1]) ||
        !reader.Read(out.drag) || !reader.Read(out.spin) ||
        !reader.Read(out.windVector) || !reader.Read(out.windTime) ||
        !reader.Read(out.followSpeed1) || !reader.Read(out.followScale1) ||
        !reader.Read(out.followSpeed2) || !reader.Read(out.followScale2))
        return false;

    std::uint32_t splineCount = 0;
    if (!reader.Read(splineCount) ||
        splineCount > reader.Remaining() / sizeof(NTempest::C3Vector) ||
        splineCount > (std::numeric_limits<std::uint32_t>::max)() /
                          sizeof(NTempest::C3Vector))
        return false;
    if (!reader.ReadArray(out.spline, splineCount)) return false;
    if (reader.Position() != staticStart + staticSize || !reader.Read(out.squirts))
        return false;

    // Anim decodes the tagged tracks; Model only needs the complete static
    // definition and advances to the next object.
    if (reader.Position() > outerEnd || !reader.Skip(outerEnd - reader.Position()))
        return false;
    return true;
}

void SetEmitterDimensions(CParticleEmitter2& emitter,
                          const MDLPARTICLEEMITTER2& definition)
{
    if (auto* plane = dynamic_cast<CPlaneParticleEmitter*>(&emitter)) {
        plane->SetLatitude(definition.staticLatitude);
        plane->SetLongitude(definition.staticLongitude);
        plane->SetHeight(definition.staticLength);
        plane->SetWidth(definition.staticWidth);
    } else if (auto* sphere = dynamic_cast<CSphereParticleEmitter*>(&emitter)) {
        sphere->SetLatitude(definition.staticLatitude);
        sphere->SetLongitude(definition.staticLongitude);
        sphere->SetHeight(definition.staticLength);
        sphere->SetWidth(definition.staticWidth);
    } else if (auto* spline = dynamic_cast<CSplineParticleEmitter*>(&emitter)) {
        spline->SetLatitude(definition.staticLatitude);
        spline->SetLongitude(definition.staticLongitude);
        spline->SetHeight(definition.staticLength);
        spline->SetWidth(definition.staticWidth);
        if (!definition.spline.empty())
            spline->SetSpline(definition.spline.data(),
                              static_cast<unsigned>(definition.spline.size()));
    }
}

} // namespace

CParticleEmitter2* CreateEmitterObject(std::uint32_t emitterType)
{
    auto* manager = ParticleSystemManager::GetInstance();
    if (emitterType == 2U) return manager->CreateSphereEmitter();
    if (emitterType == 3U) return manager->CreateSplineEmitter();
    return manager->CreateQuadEmitter();
}

CParticleEmitter2* CreateEmitter(const MDLPARTICLEEMITTER2& definition,
                                 const CModelComplex* model)
{
    auto* emitter = CreateEmitterObject(definition.emitterType);
    emitter->SetEnabled(0);
    emitter->SetEnabled2(1);
    emitter->SetVelocity(definition.staticSpeed);
    emitter->SetVelocityVariation(std::max(0.0F, definition.staticVariation));
    emitter->SetAcceleration(definition.staticGravity);
    emitter->SetZsource(definition.staticZsource);
    emitter->SetLifeSpan(std::max(0.0001F, definition.staticLife));
    emitter->SetEmissionRate(std::max(0.0F, definition.staticEmissionRate));
    emitter->SetTextureDimensions(std::max(1U, definition.rows),
                                  std::max(1U, definition.columns));
    SetEmitterDimensions(*emitter, definition);
    emitter->m_particleTailLength = definition.tailLength;
    emitter->m_sourceFlags = definition.flags;
    SetParticleStyle(*emitter, definition.particleType);
    SetParticleKeys(*emitter, definition);

    CParticleMat material;
    CreateParticleMaterial(material, definition.blendMode, definition.flags);
    emitter->SetMaterial(material);
    if (model) emitter->SetTexture(LoadModelTexture(*model, definition.textureId));
    emitter->SetReplaceableId(definition.replaceableId);
    emitter->m_priorityPlane = definition.priorityPlane;
    emitter->m_twinkleFPS = definition.twinkleFPS;
    emitter->m_twinkleOnOff = definition.twinkleOnOff;
    emitter->m_twinkleScaleMin = definition.twinkleScaleMin;
    emitter->m_twinkleScaleMax = definition.twinkleScaleMax;
    emitter->m_twinkleScaleRange = definition.twinkleScaleMax - definition.twinkleScaleMin;
    emitter->m_ivelScale = definition.inverseVelocityScale;
    emitter->m_tumblex = {definition.tumbleX[0],
                          definition.tumbleX[1] - definition.tumbleX[0]};
    emitter->m_tumbley = {definition.tumbleY[0],
                          definition.tumbleY[1] - definition.tumbleY[0]};
    emitter->m_tumblez = {definition.tumbleZ[0],
                          definition.tumbleZ[1] - definition.tumbleZ[0]};
    emitter->m_drag = definition.drag;
    emitter->m_particleAngularVelocity = definition.spin;
    emitter->m_windVector = definition.windVector;
    emitter->m_windTime = definition.windTime;
    emitter->SetFollowParams(definition.followSpeed1, definition.followScale1,
                             definition.followSpeed2, definition.followScale2);
    emitter->SetSourceFlags(definition.flags);
    return emitter;
}

void SetParticleStyle(CParticleEmitter2& emitter, std::uint32_t style)
{
    const bool hasHead = style == 0U || style == 2U;
    const bool hasTail = style == 1U || style == 2U;
    emitter.SetParticleStyle(CParticleEmitter2::PT_QUAD, hasHead, hasTail,
                             emitter.m_particleTailLength,
                             (emitter.m_sourceFlags & 0x02000000U) != 0);
}

void SetKeyColors(CParticleKey& key1, const MDLPARTICLEEMITTER2& definition,
                  CParticleKey& key2)
{
    const auto start = ParticleColor(definition.colors[0], definition.alpha[0]);
    const auto middle = ParticleColor(definition.colors[1], definition.alpha[1]);
    const auto end = ParticleColor(definition.colors[2], definition.alpha[2]);
    key1.SetColors(start, middle);
    key2.SetColors(middle, end);
}

void SetParticleKeys(CParticleEmitter2& emitter,
                     const MDLPARTICLEEMITTER2& definition)
{
    CParticleKey key1;
    CParticleKey key2;
    const float middle = std::clamp(definition.middleTime, 0.0001F, 0.9999F);
    key1.SetSegment(0.0F, middle);
    key2.SetSegment(middle, 1.0F);
    SetKeyColors(key1, definition, key2);
    key1.SetScales(definition.scale[0], definition.scale[1]);
    key2.SetScales(definition.scale[1], definition.scale[2]);
    key1.SetHeadCells(static_cast<int>(definition.lifespanUV[0]),
                      static_cast<int>(definition.lifespanUV[1]));
    key1.SetRepeat(static_cast<float>(std::max(1U, definition.lifespanUV[2])));
    key2.SetHeadCells(static_cast<int>(definition.decayUV[0]),
                      static_cast<int>(definition.decayUV[1]));
    key2.SetRepeat(static_cast<float>(std::max(1U, definition.decayUV[2])));
    key1.SetTailCells(static_cast<int>(definition.tailUV[0]),
                      static_cast<int>(definition.tailUV[1]));
    key2.SetTailCells(static_cast<int>(definition.tailDecayUV[0]),
                      static_cast<int>(definition.tailDecayUV[1]));
    key1.SetLifeSpan(std::max(0.0001F, definition.staticLife));
    key2.SetLifeSpan(std::max(0.0001F, definition.staticLife));
    emitter.SetKey(0, key1);
    emitter.SetKey(1, key2);
}

void SetMaterialBlendMode(CParticleMat& material, std::uint32_t blendMode)
{
    // Recovered PBM -> GX blend mapping: Blend, Add, Modulate,
    // Modulate2x, AlphaKey.
    static constexpr std::array<int, 5> gxBlend{{2, 3, 4, 5, 1}};
    if (blendMode < gxBlend.size()) material.alpha = gxBlend[blendMode];
}

void CreateParticleMaterial(CParticleMat& material, std::uint32_t blendMode,
                            std::uint32_t flags)
{
    SetMaterialBlendMode(material, blendMode);
    material.enableLighting = (flags & 0x00008000U) == 0;
    material.enableFog = (flags & 0x00040000U) == 0;
    material.enableDepthWrites = blendMode == 4U;
}

void CreateChildEmitter(std::uint32_t flags, CParticleEmitter2& parent)
{
    if (parent.m_childCount < parent.m_childEmitter.size())
        parent.AddChildEmitter(CreateEmitterObject(flags));
}

std::uint32_t GetEmitterFlags(const CParticleEmitter2& emitter)
{
    return emitter.m_sourceFlags;
}

void SetParticleTumble(CParticleEmitter2& emitter,
                       const NTempest::C3Vector& tumble)
{
    emitter.m_particleAngularVelocity = (tumble.x + tumble.y + tumble.z) / 3.0F;
}

void LoadC3Vector(NTempest::C3Vector& vector, const float* values)
{
    vector = values ? NTempest::C3Vector(values[0], values[1], values[2])
                    : NTempest::C3Vector{};
}

HTEXTURE__* LoadModelTexture(const CModelComplex& model,
                             std::uint32_t textureIndex)
{
    return textureIndex < model.m_textures.size()
               ? model.m_textures[textureIndex].handle
               : nullptr;
}

bool MdlReadLoadEmitters2(CModelComplex& model, CModelShared& shared,
                          std::span<const std::uint8_t> bytes)
{
    ModelByteReader reader(bytes);
    std::uint32_t count = 0;
    if (!reader.Read(count) || count > 65535U) return false;

    std::vector<MDLPARTICLEEMITTER2> definitions(count);
    for (auto& definition : definitions)
        if (!ReadDefinition(reader, definition)) return false;
    if (reader.Remaining() != 0) return false;

    auto* manager = ParticleSystemManager::GetInstance();
    for (auto* emitter : model.m_emitters2)
        if (emitter) manager->DeleteEmitter2(emitter);
    model.m_emitters2.clear();
    shared.emitter2Order.clear();
    model.m_emitters2.reserve(count);
    shared.emitter2Order.reserve(count);
    for (const auto& definition : definitions) {
        model.m_emitters2.push_back(CreateEmitter(definition, &model));
        // Resolved to the dense animation-object index after AnimBuild.
        shared.emitter2Order.push_back(definition.objectId);
    }
    return true;
}

void MdxReadEmitters2(const std::uint8_t* fileBytes, std::size_t byteCount,
                      CModelComplex& model, CModelShared& shared)
{
    if (fileBytes)
        (void)MdlReadLoadEmitters2(model, shared, {fileBytes, byteCount});
}
