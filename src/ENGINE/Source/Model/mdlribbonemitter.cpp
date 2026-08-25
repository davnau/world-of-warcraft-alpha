#include "ENGINE/Source/Model/mdlribbonemitter.h"
#include "ENGINE/Source/Model/modelparse.h"

#include <algorithm>

CRibbonMat::CRibbonMat() = default;

void LoadRibbonMaterial(CRibbonMat& material, std::uint32_t flags, std::uint32_t blendMode)
{
    material.alpha = static_cast<int>(blendMode);
    material.enableLighting = (flags & 0x1U) == 0;
    material.enableFog = (flags & 0x2U) == 0;
    material.enableDepthTest = (flags & 0x4U) == 0;
    material.enableDepthWrite = (flags & 0x8U) == 0;
    material.enableCulling = (flags & 0x10U) == 0;
}

CRibbonEmitter* LoadEmitterData(const MDLRIBBONEMITTER& definition,
                                const std::vector<HTEXTURE__*>& textures)
{
    auto* ribbon = new CRibbonEmitter();
    CRibbonMat material;
    LoadRibbonMaterial(material, definition.materialFlags, definition.blendMode);
    std::vector<HTEXTURE__*> selected;
    if (definition.texture < textures.size()) selected.push_back(textures[definition.texture]);
    std::vector<unsigned> replaces(selected.size(), 0U);
    const NTempest::CImVector color(0xFFFFFFFFU);
    const NTempest::CRect box{0.0F, 0.0F, 1.0F, 1.0F};
    ribbon->Initialize(std::max(0.001F, definition.edgesPerSecond),
                       std::max(0.001F, definition.edgeLifeSpan), &color,
                       selected, {material}, replaces, &box,
                       static_cast<int>(std::max(1U, definition.rows)),
                       static_cast<int>(std::max(1U, definition.columns)));
    ribbon->SetAbove(definition.above);
    ribbon->SetBelow(definition.below);
    ribbon->SetGravity(definition.gravity);
    return ribbon;
}

bool MdlReadLoadRibbonEmitters(CModelComplex& model, CModelShared& shared,
                               std::span<const std::uint8_t> bytes,
                               const std::vector<HTEXTURE__*>& textures)
{
    ModelByteReader reader(bytes);
    std::uint32_t count = 0;
    if (!reader.Read(count) || count > reader.Remaining() / sizeof(MDLRIBBONEMITTER)) return false;
    std::vector<MDLRIBBONEMITTER> definitions;
    if (!reader.ReadArray(definitions, count)) return false;
    for (auto* ribbon : model.m_ribbons) if (ribbon) ribbon->DecRef();
    model.m_ribbons.clear();
    shared.ribbonOrder.clear();
    for (std::uint32_t i = 0; i < count; ++i) {
        model.m_ribbons.push_back(LoadEmitterData(definitions[i], textures));
        shared.ribbonOrder.push_back(i);
    }
    return true;
}

void MdxReadRibbonEmitters(const std::uint8_t* fileBytes, std::size_t byteCount,
                           CModelComplex& model, CModelShared& shared,
                           const std::vector<HTEXTURE__*>& textures)
{
    if (fileBytes) (void)MdlReadLoadRibbonEmitters(model, shared, {fileBytes, byteCount}, textures);
}
