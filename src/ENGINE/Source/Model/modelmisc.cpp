#include "ENGINE/Source/Model/modelmisc.h"
#include "ENGINE/Source/Model/modelanim.h"
#include "ENGINE/Source/Model/mdlcamera.h"
#include "ENGINE/Source/Model/mdlsglight.h"
#include "ENGINE/Source/Anim/anim.h"
#include "ENGINE/Source/Services/particlesystem2.h"
#include "ENGINE/Source/Services/gfxsingletonmanager.h"
#include "ENGINE/Source/Services/ribbonemitter.h"

#include <algorithm>
#include <set>
#include <span>

namespace {
constexpr std::uint32_t kAnimBlending = 0x00010000U;
CModelComplex* Complex(CModel& model) { return dynamic_cast<CModelComplex*>(model.data.get()); }
const CModelComplex* Complex(const CModel& model) { return dynamic_cast<const CModelComplex*>(model.data.get()); }
}

CModelBase::CModelBase() = default;
CModelBase::~CModelBase() = default;
CModel::CModel() = default;
CModel::~CModel() = default;
CModel::CModel(CModel&&) noexcept = default;
CModel& CModel::operator=(CModel&&) noexcept = default;
CModelComplex::CModelComplex() = default;
CModelComplex::~CModelComplex()
{
    MdlDestroyCameras(m_cameras); ModelDestroyLights(*this);
    auto* particleManager = ParticleSystemManager::GetInstance();
    for (auto* emitter : m_emitters2) if (emitter) particleManager->DeleteEmitter2(emitter);
    for (auto* ribbon : m_ribbons) if (ribbon) ribbon->DecRef();
}
void CModelComplex::CopyAttachments(const CModelComplex&) {}
void CModelComplex::CopyCameras(const CModelComplex& source) { MdlDestroyCameras(m_cameras); for (const auto* camera : source.m_cameras) if (camera) m_cameras.push_back(new HCAMERA__(*camera)); }
void CModelComplex::CopyEmitters(const CModelComplex& source) { auto*m=ParticleSystemManager::GetInstance();for(auto*e:m_emitters2)if(e)m->DeleteEmitter2(e);m_emitters2.clear();for(auto*e:source.m_emitters2)if(e)m_emitters2.push_back(m->DuplicateEmitter(e)); }
void CModelComplex::CopyLights(const CModelComplex& source) { ModelDestroyLights(*this); for(auto handle:source.m_lights)if(const auto*light=::ModelGetLight(handle))m_lights.push_back(CreateGxLight(*light)); }
void CModelComplex::CopyRibbons(const CModelComplex& source) { for(auto*r:m_ribbons)if(r)r->DecRef();m_ribbons.clear();for(auto*r:source.m_ribbons)if(r)m_ribbons.push_back(new CRibbonEmitter(*r)); }
CModelSimple::CModelSimple() = default;
CModelSimple::~CModelSimple() = default;
void CModelSimple::CopyMaterials(const CModelSimple& source) { m_geosets=source.m_geosets;m_numGeosets=source.m_numGeosets; }
CModelTexture::CModelTexture() = default;
CModelTexture::CModelTexture(const CModelTexture&) = default;
CModelTexture& CModelTexture::operator=(const CModelTexture&) = default;
void CModel::FinishDuplication(const CModel& source) { shared=source.shared;create=source.create;load=source.load;sequences=source.sequences; }

void ModelEnableEmitters(CModel& model, bool enable)
{
    model.emittersEnabled = enable;
    if (auto* complex = Complex(model))
        for (auto* emitter : complex->m_emitters2) if (emitter) emitter->SetEnabled(enable);
}
void ModelEnableRibbons(CModel& model, bool enable)
{
    model.ribbonsEnabled = enable;
    if (auto* complex = Complex(model))
        for (auto* ribbon : complex->m_ribbons) if (ribbon) ribbon->SetEnabled(enable ? 1 : 0);
}
void ModelEnableLights(CModel& model, bool enable) { model.lightsEnabled = enable; }
void ModelEnableFullAlpha(CModel& model, bool enable) { model.fullAlpha = enable; }
void ModelEnableAnimBlending(CModel& model, bool enable)
{
    if (!model.data) return;
    if (enable) model.data->m_flags |= kAnimBlending; else model.data->m_flags &= ~kAnimBlending;
}
bool ModelUsesBlending(const CModel& model) { return model.data && (model.data->m_flags & kAnimBlending) != 0; }

std::uint32_t ModelGetNumTextures(const CModel& model)
{
    const auto* complex = Complex(model);
    return complex ? static_cast<std::uint32_t>(complex->m_textures.size()) : 0U;
}

std::uint32_t ModelGetTextureReplaceableId(const CModel& model, std::uint32_t texture)
{
    const auto* complex = Complex(model);
    return complex && texture < complex->m_textures.size() ? complex->m_textures[texture].replaceableId : 0U;
}

bool ModelReplaceTexture(CModel& model, std::uint32_t replaceableId,
                         HTEXTURE__* replacement, bool doLinkedModels)
{
    if (replaceableId == 0U) return false;
    return ComplexModelReplaceTexture(model, replaceableId, replacement,
                                      doLinkedModels) != 0U;
}

std::uint32_t ModelGetNumCameras(const CModel& model)
{
    const auto* complex = Complex(model);
    return complex ? static_cast<std::uint32_t>(complex->m_cameras.size()) : 0U;
}
HCAMERA__* ModelGetCamera(CModel& model, std::uint32_t camera)
{
    auto* complex = Complex(model);
    return complex && camera < complex->m_cameras.size() ? complex->m_cameras[camera] : nullptr;
}
std::uint32_t ModelGetNumLights(const CModel& model)
{
    const auto* complex = Complex(model);
    return complex ? static_cast<std::uint32_t>(complex->m_lights.size()) : 0U;
}
std::uintptr_t ModelGetLight(const CModel& model, std::uint32_t light)
{
    const auto* complex = Complex(model);
    return complex && light < complex->m_lights.size() ? complex->m_lights[light] : 0U;
}

void ModelHideGeosets(CModel& model, std::uint32_t selectionGroup, bool hide)
{
    auto* complex = Complex(model);
    if (!complex || !model.shared) return;
    const auto count = std::min(complex->m_geosets.size(), model.shared->geosets.size());
    for (std::size_t i = 0; i < count; ++i) {
        if (model.shared->geosets[i].selectionGroup != selectionGroup) continue;
        if (hide) complex->m_geosets[i].flags |= 1U;
        else complex->m_geosets[i].flags &= ~1U;
    }
}
void ModelHideGeosetsRange(CModel& model, std::uint32_t selectionStart,
                           std::uint32_t selectionEnd, bool hide)
{
    auto* complex = Complex(model);
    if (!complex || !model.shared || selectionStart > selectionEnd) return;
    const auto count = std::min(complex->m_geosets.size(), model.shared->geosets.size());
    for (std::size_t i = 0; i < count; ++i) {
        const auto selectionGroup = model.shared->geosets[i].selectionGroup;
        if (selectionGroup < selectionStart || selectionGroup > selectionEnd) continue;
        if (hide) complex->m_geosets[i].flags |= 1U;
        else complex->m_geosets[i].flags &= ~1U;
    }
}
void GeosetSetVertexAlpha(CGeosetColor& color, float alpha) { color.alpha = std::clamp(alpha, 0.0F, 1.0F); }
float GeosetGetVertexAlpha(const CGeosetColor& color) { return color.alpha; }
void GeosetSetVertexColor(CGeosetColor& target, const NTempest::C3Vector& color) { target.color = color; }
NTempest::C3Vector GeosetGetVertexColor(const CGeosetColor& color) { return color.color; }

std::uint32_t ModelCustGeosetAdd(CModel& model, const CCustomGeoset& geoset)
{
    auto* complex = Complex(model);
    if (!complex) return ~0U;
    complex->m_customGeosets.push_back(geoset);
    return static_cast<std::uint32_t>(complex->m_customGeosets.size() - 1U);
}
bool ModelCustGeosetRemove(CModel& model, std::uint32_t index)
{
    auto* complex = Complex(model);
    if (!complex || index >= complex->m_customGeosets.size()) return false;
    complex->m_customGeosets.erase(complex->m_customGeosets.begin() + index);
    return true;
}
bool ModelCustGeosetMove(CModel& model, std::uint32_t from, std::uint32_t to)
{
    auto* complex = Complex(model);
    if (!complex || from >= complex->m_customGeosets.size() || to >= complex->m_customGeosets.size()) return false;
    auto value = std::move(complex->m_customGeosets[from]);
    complex->m_customGeosets.erase(complex->m_customGeosets.begin() + from);
    complex->m_customGeosets.insert(complex->m_customGeosets.begin() + to, std::move(value));
    return true;
}

bool ModelAddLink(CModel& model, std::uint32_t point, CModel& linked, float scale)
{
    if (!ModelHasLinkPoint(model, point)) return false;
    model.links.push_back({point, &linked, scale});
    return true;
}
bool ModelRemoveLink(CModel& model, CModel& linked)
{
    const auto oldSize = model.links.size();
    std::erase_if(model.links, [&linked](const CModelLink& link) { return link.model == &linked; });
    return oldSize != model.links.size();
}
bool ModelClearLink(CModel& model, std::uint32_t point)
{
    if (!ModelHasLinkPoint(model, point)) return false;
    std::erase_if(model.links, [point](const CModelLink& link) { return link.point == point; });
    return true;
}
void ModelClearAllLinks(CModel& model) { model.links.clear(); }
std::uint32_t ModelGetNumLinkedAtPoint(const CModel& model, std::uint32_t point)
{
    return static_cast<std::uint32_t>(std::count_if(model.links.begin(), model.links.end(),
        [point](const CModelLink& link) { return link.point == point; }));
}
bool ModelHasLinkPoint(const CModel& model, std::uint32_t point)
{
    return model.shared && point < model.shared->attachIdToIndex.size() && model.shared->attachIdToIndex[point] != ~0U;
}
std::uint32_t ModelGetNumLinkPoints(const CModel& model)
{
    if (!model.shared) return 0;
    std::set<std::uint32_t> points;
    for (std::uint32_t i = 0; i < model.shared->attachIdToIndex.size(); ++i)
        if (model.shared->attachIdToIndex[i] != ~0U) points.insert(i);
    return static_cast<std::uint32_t>(points.size());
}

void ModelSetVertexAlpha(CModel& model,float alpha){if(auto*complex=Complex(model)){if(complex->m_geosetColor.size()<complex->m_geosets.size())complex->m_geosetColor.resize(complex->m_geosets.size());for(auto&color:complex->m_geosetColor)GeosetSetVertexAlpha(color,alpha);}}
float ModelGetVertexAlpha(const CModel& model){const auto*complex=Complex(model);return complex&&!complex->m_geosetColor.empty()?complex->m_geosetColor.front().alpha:1.0F;}
void ModelSetVertexColor(CModel& model,const NTempest::C3Vector& color){if(auto*complex=Complex(model)){if(complex->m_geosetColor.size()<complex->m_geosets.size())complex->m_geosetColor.resize(complex->m_geosets.size());for(auto&entry:complex->m_geosetColor)entry.color=color;}}
NTempest::C3Vector ModelGetVertexColor(const CModel& model){const auto*complex=Complex(model);return complex&&!complex->m_geosetColor.empty()?complex->m_geosetColor.front().color:NTempest::C3Vector{1.0F};}
void GeosetHideUnselectable(CGeoset& geoset){geoset.flags|=0x2U;} void GeosetShowUnselectable(CGeoset& geoset){geoset.flags&=~0x2U;} bool GeosetIsShowingUnselectable(const CGeoset& geoset){return(geoset.flags&0x2U)==0;}
void ModelHideUnselectable(CModel& model){if(auto*complex=Complex(model))for(auto&geoset:complex->m_geosets)GeosetHideUnselectable(geoset);}
void ModelShowUnselectable(CModel& model){if(auto*complex=Complex(model))for(auto&geoset:complex->m_geosets)GeosetShowUnselectable(geoset);}
bool ModelIsShowingUnselectable(const CModel& model){const auto*complex=Complex(model);return!complex||std::all_of(complex->m_geosets.begin(),complex->m_geosets.end(),GeosetIsShowingUnselectable);}
bool ModelIsShowingBoundingBox(const CModel& model){return model.data&&(model.data->m_flags&0x100U)!=0;} bool ModelIsShowingBoundingSphere(const CModel& model){return model.data&&(model.data->m_flags&0x200U)!=0;} bool ModelIsShowingHitTestGeometry(const CModel& model){return model.data&&(model.data->m_flags&0x400U)!=0;}
bool ModelIsCameraEnabled(const CModel& model,std::uint32_t camera){return camera<ModelGetNumCameras(model);}
void ModelSetBlendMode(CModel& model,std::uint32_t material,std::uint32_t layer,std::uint32_t blendMode){if(auto*complex=Complex(model);complex&&material<complex->m_materials.size()&&layer<complex->m_materials[material].layers.size())complex->m_materials[material].layers[layer].blendMode=blendMode;}
void ModelRestoreBlendMode(CModel& model,std::uint32_t material,std::uint32_t layer){ModelSetBlendMode(model,material,layer,0);}
void ModelSetEmissiveColor(CModel& model,std::uint32_t material,const NTempest::C3Vector& color){if(auto*complex=Complex(model);complex&&material<complex->m_materials.size())complex->m_materials[material].emissive=color;}
void ModelSetMaterialDisables(CModel& model,std::uint32_t material,std::uint32_t disables){if(auto*complex=Complex(model);complex&&material<complex->m_materials.size())complex->m_materials[material].flags=disables;}
void ModelMaterialShowLayer(CModel& model,std::uint32_t material,std::uint32_t layer,bool show){if(auto*complex=Complex(model);complex&&material<complex->m_materials.size()&&layer<complex->m_materials[material].layers.size()){if(show)complex->m_materials[material].layers[layer].flags&=~1U;else complex->m_materials[material].layers[layer].flags|=1U;}}
void ModelOptimizeVisibleGeosets(CModel&){/* The original rebuilds render batches; it does not reorder unique geosets. */}
void UpdateParticleEmitters(CModel& model,float elapsedSec){if(auto*complex=Complex(model))for(auto*emitter:complex->m_emitters2)if(emitter)emitter->Update(elapsedSec);}
void UpdateRibbonMaterial(CModel& model,std::uint32_t ribbon,const CRibbonMat& material){if(auto*complex=Complex(model);complex&&ribbon<complex->m_ribbons.size()&&complex->m_ribbons[ribbon])complex->m_ribbons[ribbon]->SetMats({material},{},{});}
bool ModelAnimHasObjectId(const CModel& model,std::uint32_t id){return ModelHasSequenceId(model,id);}
std::uint32_t ModelGetMatrixCount(const CModel& model){return model.shared?model.shared->numBones+model.shared->numTexBones:0;}
NTempest::C34Matrix ModelGetLinkPoint(const CModel& model,std::uint32_t point){NTempest::C34Matrix matrix=model.data?model.data->m_modelToWorld:NTempest::C34Matrix{};if(model.shared&&point<model.shared->attachIdToIndex.size()){const auto index=model.shared->attachIdToIndex[point];if(index<model.shared->positions.size())matrix.Translate(model.shared->positions[index]);}return matrix;}
void AddMatrixGroupRangeToSet(std::vector<std::uint32_t>& set,std::uint32_t first,std::uint32_t count){for(std::uint32_t i=0;i<count;++i)if(std::find(set.begin(),set.end(),first+i)==set.end())set.push_back(first+i);}
void AddGeosetMatrixGroups(const CGeosetShared& geoset,std::vector<std::uint32_t>& set){for(auto matrix:geoset.matrices)if(std::find(set.begin(),set.end(),matrix)==set.end())set.push_back(matrix);}
CGeosetShared BuildCompositeGeoset(std::span<const CGeosetShared> geosets){CGeosetShared result;for(const auto&geoset:geosets){const auto base=static_cast<std::uint16_t>(result.position.size());result.position.insert(result.position.end(),geoset.position.begin(),geoset.position.end());result.normal.insert(result.normal.end(),geoset.normal.begin(),geoset.normal.end());for(auto index:geoset.primitiveVertices)result.primitiveVertices.push_back(static_cast<std::uint16_t>(base+index));AddGeosetMatrixGroups(geoset,result.matrices);}result.primitive={{4U,0U,static_cast<std::uint32_t>(result.primitiveVertices.size())}};return result;}
std::uint32_t ModelGetPrimarySequence(const CModel& model){return model.primarySequence;} std::uint32_t ModelGetSequenceTime(const CModel& model){return model.sequenceTimeMs;}
std::uint32_t ComplexModelReplaceTexture(CModel& model, std::uint32_t replaceableId,
                                         HTEXTURE__* replacement, bool doLinkedModels)
{
    auto* complex = Complex(model);
    if (!complex) return 0U;

    std::uint32_t replaced = 0U;
    for (auto& texture : complex->m_textures) {
        if (texture.replaceableId != replaceableId) continue;
        texture.handle = replacement;
        ++replaced;
    }
    for (auto* ribbon : complex->m_ribbons)
        if (ribbon) replaced += ribbon->ReplaceTexture(replaceableId, replacement);
    for (auto* emitter : complex->m_emitters2) {
        if (emitter && emitter->ReplaceableId() == replaceableId) {
            emitter->SetTexture(replacement);
            ++replaced;
        }
    }
    if (doLinkedModels) {
        for (const auto& link : model.links)
            if (link.model)
                replaced += ComplexModelReplaceTexture(*link.model, replaceableId,
                                                       replacement, true);
    }
    return replaced;
}
void ComplexModelSetBlendMode(CModel& model,std::uint32_t material,std::uint32_t layer,std::uint32_t mode){ModelSetBlendMode(model,material,layer,mode);} void ComplexModelRestoreBlendMode(CModel& model,std::uint32_t material,std::uint32_t layer){ModelRestoreBlendMode(model,material,layer);} void ComplexModelSetEmissiveColor(CModel& model,std::uint32_t material,const NTempest::C3Vector& color){ModelSetEmissiveColor(model,material,color);} void ComplexModelSetMaterialDisables(CModel& model,std::uint32_t material,std::uint32_t disables){ModelSetMaterialDisables(model,material,disables);}
void IModelEnableFullAlpha(CModel& model,bool enable){ModelEnableFullAlpha(model,enable);} void IModelOptimizeVisibleGeosets(CModel& model){ModelOptimizeVisibleGeosets(model);} void IModelSetMaterialDisables(CModel& model,std::uint32_t material,std::uint32_t disables){ModelSetMaterialDisables(model,material,disables);} void IModelSetVertexAlpha(CModel& model,float alpha){ModelSetVertexAlpha(model,alpha);} void IModelSetVertexColor(CModel& model,const NTempest::C3Vector& color){ModelSetVertexColor(model,color);}
CMaterial MaterialDuplicate(const CMaterial& material){return material;} bool MaterialUsedOnce(const CModel& model,std::uint32_t material){const auto*complex=Complex(model);return complex&&material<complex->m_materials.size();}
std::vector<std::uint32_t> ModelEnumAnimObjects(const CModel& model){std::vector<std::uint32_t> ids;ids.reserve(model.sequences.size());for(const auto&sequence:model.sequences)ids.push_back(sequence.id);return ids;}
void ModelSetLightSelectCallback(CModel& model,CModelBase::PickLights callback,void* parameter){if(model.data){model.data->m_PickLights=callback;model.data->m_pickLightsParm=parameter;}}
void GxuLightSelectCallback(void*,NTempest::C3Vector,const NTempest::C3Vector*,unsigned){}
