#include "ENGINE/Source/Model/modelcreate.h"
#include "ENGINE/Source/Model/collisiondata.h"
#include "ENGINE/Source/Model/mdlcamera.h"
#include "ENGINE/Source/Model/mdlparticleemitter2.h"
#include "ENGINE/Source/Model/mdlribbonemitter.h"
#include "ENGINE/Source/Model/mdlsglight.h"
#include "ENGINE/Source/Model/mdlsgmodel.h"
#include "ENGINE/Source/Model/modelparse.h"
#include "ENGINE/Source/Anim/mdlanim.h"
#include "ENGINE/Source/Anim/anim.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <mutex>
#include <unordered_map>

namespace {
std::mutex g_modelCacheMutex;
std::unordered_map<std::string, std::weak_ptr<CModel>> g_modelCache;
std::mutex g_modelCommandMutex;
std::vector<std::function<void()>> g_modelCommands;
struct MdxHeader { std::uint32_t magic; };
struct MdxChunk { std::uint32_t id; std::uint32_t size; };
constexpr std::uint32_t FourCC(char a, char b, char c, char d) { return std::uint32_t(std::uint8_t(a)) | (std::uint32_t(std::uint8_t(b)) << 8U) | (std::uint32_t(std::uint8_t(c)) << 16U) | (std::uint32_t(std::uint8_t(d)) << 24U); }
std::string NormalizeName(const char* name) { std::string key = name ? name : ""; std::replace(key.begin(), key.end(), '/', '\\'); std::transform(key.begin(), key.end(), key.begin(), [](unsigned char c) { return char(std::tolower(c)); }); return key; }
void CopyCreate(CModel& model, const CModelCreate* create) { if (create) model.create = *create; }

void ResolveEmitterOrder(CModel& model)
{
    if (!model.animData || !model.shared) return;
    std::vector<std::uint32_t> order;
    order.reserve(model.shared->emitter2Order.size());
    for (const auto* object : model.animData->obj)
        if (object && object->type == ANIM_OBJ_EMITTER2)
            order.push_back(object->animObjId);
    if (order.size() == model.shared->emitter2Order.size())
        model.shared->emitter2Order = std::move(order);
}

bool MdxReadModelProperties(std::span<const std::uint8_t> bytes, CModelShared& shared)
{
    // Early MDLX MODL is name[80], animationFile[260], radius, min/max
    // extents, blendTime, and one byte of model flags.
    if (bytes.size() < 368U) return false;
    ModelByteReader reader(bytes);
    std::array<char, 80> modelName{};
    std::array<char, 260> animationFile{};
    float radius = 0.0F;
    if (!reader.Read(modelName) || !reader.Read(animationFile) || !reader.Read(radius) ||
        !reader.Read(shared.bounds.extent.b) || !reader.Read(shared.bounds.extent.t))
        return false;
    shared.bounds.sphere.c = (shared.bounds.extent.b + shared.bounds.extent.t) * 0.5F;
    shared.bounds.sphere.r = radius;
    return true;
}

}

void ModelInitialize() { std::scoped_lock lock(g_modelCacheMutex); g_modelCache.clear(); }
void ModelCacheUpdate() { std::scoped_lock lock(g_modelCacheMutex); std::erase_if(g_modelCache, [](const auto& item) { return item.second.expired(); }); }
void ModelCacheFlush() { ModelInitialize(); }

bool IsBinaryFile(std::span<const std::uint8_t> bytes) { if (bytes.size() < 4U) return false; std::uint32_t magic = 0; std::memcpy(&magic, bytes.data(), 4U); return magic == FourCC('M','D','2','0') || magic == FourCC('M','D','L','X'); }
void ComputeBoundingBox(std::span<const NTempest::C3Vector> positions, NTempest::CAaBox& box) { if (positions.empty()) { box = {}; return; } box.Bounding(positions.data(), static_cast<unsigned>(positions.size())); }
float ComputeBoundingRadius(std::span<const NTempest::C3Vector> positions, const NTempest::C3Vector& center) { float squared = 0.0F; for (const auto& position : positions) squared = std::max(squared, (position - center).SquaredMag()); return std::sqrt(squared); }
void IModelComputeBounds(CModelShared& shared) { ComputeBoundingBox(shared.positions, shared.bounds.extent); shared.bounds.sphere.c = (shared.bounds.extent.b + shared.bounds.extent.t) * 0.5F; shared.bounds.sphere.r = ComputeBoundingRadius(shared.positions, shared.bounds.sphere.c); }

bool MdxReadPositions(std::span<const std::uint8_t> bytes, CModelShared& shared) { ModelByteReader reader(bytes); std::uint32_t count = 0; return reader.Read(count) && reader.ReadArray(shared.positions, count); }
bool MdxReadExtents(std::span<const std::uint8_t> bytes, CModelShared& shared) { ModelByteReader reader(bytes); return reader.Read(shared.bounds.extent) && reader.Read(shared.bounds.sphere); }
bool MdxReadHitTestData(std::span<const std::uint8_t> bytes, CModelShared& shared) { ModelByteReader reader(bytes); std::uint32_t count = 0; return reader.Read(count) && reader.ReadArray(shared.hitTest, count); }
bool MdxReadNumMatrices(std::span<const std::uint8_t> bytes, CModelShared& shared) { ModelByteReader reader(bytes); return reader.Read(shared.numBones) && reader.Read(shared.numTexBones); }

bool MdxReadCollision(std::span<const std::uint8_t> bytes, CModelShared& shared)
{
    // CollisionDataCreate in the recovered client seeks the CLID section and
    // reads its three nested arrays in this exact order:
    //   VRTX / uint32 count / C3Vector[count]
    //   TRI  / uint32 count / uint16[count]
    //   NRMS / uint32 count / C3Vector[count]
    ModelByteReader reader(bytes);
    std::uint32_t token = 0U;
    std::uint32_t count = 0U;
    std::vector<NTempest::C3Vector> vertices;
    std::vector<std::uint16_t> indices;
    std::vector<NTempest::C3Vector> normals;
    if (!reader.Read(token) || token != FourCC('V','R','T','X') ||
        !reader.Read(count) || !reader.ReadArray(vertices, count) ||
        !reader.Read(token) || token != FourCC('T','R','I',' ') ||
        !reader.Read(count) || !reader.ReadArray(indices, count) ||
        !reader.Read(token) || token != FourCC('N','R','M','S') ||
        !reader.Read(count) || !reader.ReadArray(normals, count)) {
        return false;
    }
    try {
        shared.collision = CollisionDataCreate(vertices, indices);
    } catch (...) {
        shared.collision.reset();
        return false;
    }
    if (shared.collision &&
        normals.size() == shared.collision->surfaceNormals.size()) {
        shared.collision->surfaceNormals = std::move(normals);
    }
    return shared.collision != nullptr;
}

std::shared_ptr<CModel> IModelCreateSimpleEmpty(const char* name)
{
    auto model = std::make_shared<CModel>(); model->data = std::make_unique<CModelSimple>(); model->shared = std::make_shared<CModelShared>();
    if (name) strncpy_s(model->shared->name.data(), model->shared->name.size(), name, _TRUNCATE);
    model->load = MODEL_LOAD_COMPLETE; return model;
}

std::shared_ptr<CModel> IModelCreateBlocking(std::span<const std::uint8_t> bytes, const char* name, const CModelCreate* create)
{
    if (!IsBinaryFile(bytes) || bytes.size() < sizeof(MdxHeader)) return {};
    auto model = std::make_shared<CModel>(); model->data = std::make_unique<CModelComplex>(); model->shared = std::make_shared<CModelShared>(); CopyCreate(*model, create);
    if (name) strncpy_s(model->shared->name.data(), model->shared->name.size(), name, _TRUNCATE);
    ModelByteReader reader(bytes); MdxHeader header{}; if (!reader.Read(header)) return {};
    while (reader.Remaining() >= sizeof(MdxChunk)) {
        MdxChunk chunk{}; if (!reader.Read(chunk) || chunk.size > reader.Remaining()) { model->load = MODEL_LOAD_FAILED; return model; }
        const auto body = bytes.subspan(reader.Position(), chunk.size); bool valid = true; auto& complex = static_cast<CModelComplex&>(*model->data);
        switch (chunk.id) {
        case FourCC('M','O','D','L'): valid = MdxReadModelProperties(body, *model->shared); break;
        case FourCC('P','O','S','I'): valid = MdxReadPositions(body, *model->shared); break;
        case FourCC('B','N','D','S'): valid = MdxReadExtents(body, *model->shared); break;
        case FourCC('H','I','T','S'): valid = MdxReadHitTestData(body, *model->shared); break;
        case FourCC('M','T','R','X'): valid = MdxReadNumMatrices(body, *model->shared); break;
        case FourCC('T','E','X','S'): valid = MdxReadTextures(body, complex); break;
        case FourCC('M','T','L','S'): valid = MdxReadMaterials(body, complex, model->create.flags); break;
        case FourCC('G','E','O','S'): valid = MdxReadGeosets(body, complex, *model->shared); break;
        case FourCC('C','L','I','D'): valid = MdxReadCollision(body, *model->shared); break;
        case FourCC('C','A','M','S'): valid = MdlReadCameras(body, complex.m_cameras); break;
        case FourCC('A','T','C','H'): valid = MdxReadAttachments(body, *model->shared); break;
        case FourCC('P','R','E','2'): valid = MdlReadLoadEmitters2(complex, *model->shared, body); break;
        case FourCC('R','I','B','B'): valid = MdlReadLoadRibbonEmitters(complex, *model->shared, body); break;
        default: break;
        }
        if (!valid) {
            std::fprintf(stderr, "ModelCreate: invalid %c%c%c%c chunk in %s\n",
                         char(chunk.id), char(chunk.id >> 8U), char(chunk.id >> 16U),
                         char(chunk.id >> 24U), name ? name : "<memory>");
            model->load = MODEL_LOAD_FAILED;
            return model;
        }
        if (!reader.Skip(chunk.size)) return {};
    }
    try {
        std::fprintf(stderr, "ModelCreate: building animation %s\n", name ? name : "<memory>");
        model->animData = AnimBuild(
            std::span<const std::byte>(reinterpret_cast<const std::byte*>(bytes.data()), bytes.size()),
            ConvertAnimCreateFlags(create ? create->flags : 0U));
        std::fprintf(stderr, "ModelCreate: animation data ready %s objects=%zu sequences=%zu\n",
                     name ? name : "<memory>", model->animData->obj.size(), model->animData->seq.size());
        model->anim = AnimCreate(model->animData);
        ResolveEmitterOrder(*model);
        model->data->m_anim = model->anim.get();
        model->shared->numBones = static_cast<std::uint32_t>(model->animData->obj.size());
        model->sequences.clear();
        model->sequences.reserve(model->animData->seq.size());
        for (std::uint32_t i = 0; i < model->animData->seq.size(); ++i) {
            const auto& source = model->animData->seq[i];
            CModelSequence sequence;
            sequence.id = i;
            sequence.name = source.name;
            sequence.durationMs = static_cast<std::uint32_t>(std::max(0, source.end - source.start));
            sequence.moveSpeed = source.moveSpeed;
            sequence.flags = source.flags;
            model->sequences.push_back(std::move(sequence));
        }
        if (create && create->sequenceNames && create->numSequences != 0U) {
            std::vector<std::string> sequenceNames;
            sequenceNames.reserve(create->numSequences);
            for (std::uint32_t i = 0; i < create->numSequences; ++i)
                sequenceNames.emplace_back(create->sequenceNames[i]
                    ? create->sequenceNames[i] : "");
            model->animData->sequenceOrder =
                SetSeqIndexOrdering(sequenceNames, model->animData->seq);
        }
        model->transforms.resize(model->animData->obj.size());
        if (auto* complex = dynamic_cast<CModelComplex*>(model->data.get())) {
            complex->m_geosetColor.resize(model->shared->geosets.size());
            model->layerAlpha.assign(model->animData->layers.size(), 1.0F);
            model->layerTextureIds.assign(model->animData->layers.size(), ~0U);
            for (const auto& material : complex->m_materials) {
                for (const auto& layer : material.layers) {
                    if (layer.animationId >= model->layerAlpha.size()) continue;
                    model->layerAlpha[layer.animationId] = layer.alpha;
                    model->layerTextureIds[layer.animationId] = layer.texture;
                }
            }
        }
        std::fprintf(stderr, "ModelCreate: animating initial pose %s\n", name ? name : "<memory>");
        AnimAnimateModel(*model->anim);
        std::fprintf(stderr, "ModelCreate: initial pose ready %s\n", name ? name : "<memory>");
    } catch (const std::exception& error) {
        std::fprintf(stderr, "ModelCreate: animation parse failed in %s: %s\n",
                     name ? name : "<memory>", error.what());
        model->anim.reset();
        model->animData.reset();
        model->data->m_anim = nullptr;
    }
    if (!model->shared->positions.empty() && model->shared->bounds.sphere.r == 0.0F) IModelComputeBounds(*model->shared);
    model->load = MODEL_LOAD_COMPLETE; return model;
}

std::shared_ptr<CModel> ModelCreate(std::span<const std::uint8_t> bytes, const char* name, const CModelCreate* create)
{
    const auto key = NormalizeName(name); { std::scoped_lock lock(g_modelCacheMutex); const auto found = g_modelCache.find(key); if (found != g_modelCache.end()) if (auto existing = found->second.lock()) return existing; }
    auto model = IModelCreateBlocking(bytes, name, create); if (model && model->load == MODEL_LOAD_COMPLETE) { std::scoped_lock lock(g_modelCacheMutex); g_modelCache[key] = model; } return model;
}

std::shared_ptr<CModel> ModelCreate(const char* fileName, const CModelCreate* create)
{
    if (!fileName) return {}; const auto key = NormalizeName(fileName); { std::scoped_lock lock(g_modelCacheMutex); const auto found = g_modelCache.find(key); if (found != g_modelCache.end()) if (auto existing = found->second.lock()) return existing; }
    void* archiveData = nullptr;
    unsigned archiveBytes = 0;
    if (SFile::LoadFile(fileName, &archiveData, &archiveBytes, 0, nullptr) && archiveData && archiveBytes) {
        const auto result = ModelCreate(
            std::span<const std::uint8_t>(static_cast<const std::uint8_t*>(archiveData), archiveBytes),
            fileName, create);
        SFile::Unload(archiveData);
        return result;
    }
    std::ifstream stream(fileName, std::ios::binary | std::ios::ate); if (!stream) return {}; const auto size = stream.tellg(); if (size <= 0) return {};
    std::vector<std::uint8_t> bytes(static_cast<std::size_t>(size)); stream.seekg(0); stream.read(reinterpret_cast<char*>(bytes.data()), size); return ModelCreate(bytes, fileName, create);
}

std::shared_ptr<CModel> ModelDuplicate(const CModel& source)
{
    auto copy = std::make_shared<CModel>();
    copy->FinishDuplication(source);
    copy->primarySequence = source.primarySequence;
    copy->sequenceTimeMs = source.sequenceTimeMs;
    copy->timeScale = source.timeScale;
    copy->paused = source.paused;
    copy->emittersEnabled = source.emittersEnabled;
    copy->ribbonsEnabled = source.ribbonsEnabled;
    copy->lightsEnabled = source.lightsEnabled;
    copy->fullAlpha = source.fullAlpha;
    copy->layerAlpha = source.layerAlpha;
    copy->layerTextureIds = source.layerTextureIds;
    copy->transforms = source.transforms;
    copy->fidgetSequence = source.fidgetSequence;
    copy->fidgetCount = source.fidgetCount;
    if (const auto* complex = dynamic_cast<const CModelComplex*>(source.data.get())) {
        auto destination = std::make_unique<CModelComplex>();
        destination->m_flags = complex->m_flags;
        destination->m_modelToWorld = complex->m_modelToWorld;
        destination->m_geosets = complex->m_geosets;
        destination->m_addlGeosets = complex->m_addlGeosets;
        destination->m_geosetColor = complex->m_geosetColor;
        destination->m_customGeosets = complex->m_customGeosets;
        destination->m_textures = complex->m_textures;
        destination->m_materials = complex->m_materials;
        destination->CopyCameras(*complex);
        destination->CopyEmitters(*complex);
        destination->CopyLights(*complex);
        destination->CopyRibbons(*complex);
        copy->data = std::move(destination);
    } else if (const auto* simple = dynamic_cast<const CModelSimple*>(source.data.get())) {
        auto destination = std::make_unique<CModelSimple>();
        destination->m_flags = simple->m_flags;
        destination->m_modelToWorld = simple->m_modelToWorld;
        destination->CopyMaterials(*simple);
        copy->data = std::move(destination);
    }
    if (source.animData) {
        copy->animData = source.animData;
        copy->anim = AnimCreate(copy->animData);
        if (copy->data) copy->data->m_anim = copy->anim.get();
        if (copy->anim && copy->primarySequence < copy->sequences.size()) {
            AnimSetSequence(*copy->anim, copy->primarySequence);
            AnimForceCurrentSequenceTime(*copy->anim, static_cast<int>(copy->sequenceTimeMs));
            AnimAnimateModel(*copy->anim);
        }
    }
    return copy;
}
void ModelDestroy(std::shared_ptr<CModel>& model) { model.reset(); }
bool ModelIsLoaded(const CModel& model) { return model.load == MODEL_LOAD_COMPLETE; }
bool ModelIsUsed(const CModel& model) { return model.load != MODEL_LOAD_FAILED; }
bool IsSimpleModel(const CModel& model) { return dynamic_cast<const CModelSimple*>(model.data.get()) != nullptr; }

void CModel::DeleteAsyncObj() { if (load == MODEL_LOAD_PENDING) load = MODEL_LOAD_FAILED; }
void CModel::RemoveModelCommandsFromQueue() { std::scoped_lock lock(g_modelCommandMutex); g_modelCommands.clear(); }

std::shared_ptr<CModelShared> CreateSharedModelData(const char* name)
{
    auto shared = std::make_shared<CModelShared>(); if (name) strncpy_s(shared->name.data(), shared->name.size(), name, _TRUNCATE); return shared;
}
std::shared_ptr<CModel> CreateDefaultModel(const char* name) { return IModelCreateSimpleEmpty(name); }
std::shared_ptr<CModel> BuildModelFromMdxData(std::span<const std::uint8_t> bytes, const char* name, const CModelCreate* create) { return IModelCreateBlocking(bytes,name,create); }
std::shared_ptr<CModel> BuildModelFromMdlData(std::span<const std::uint8_t> bytes, const char* name, const CModelCreate* create) { return IModelCreateBlocking(bytes,name,create); }
std::shared_ptr<CModel> BuildSimpleModelFromMdxData(std::span<const std::uint8_t> bytes, const char* name) { auto model=IModelCreateBlocking(bytes,name); if(model&&model->data){auto simple=std::make_unique<CModelSimple>();simple->m_numGeosets=model->shared?std::min<std::size_t>(16,model->shared->geosets.size()):0;model->data=std::move(simple);}return model; }
std::shared_ptr<CModel> BuildSimpleModelFromMdlData(std::span<const std::uint8_t> bytes, const char* name) { return BuildSimpleModelFromMdxData(bytes,name); }

std::shared_ptr<CModel> ModelCreateSimpleMesh(std::span<const NTempest::C3Vector> vertices,
                                              std::span<const std::uint16_t> indices, const char* name)
{
    auto model=IModelCreateSimpleEmpty(name);auto complex=std::make_unique<CModelComplex>();CGeosetShared geoset;geoset.position.assign(vertices.begin(),vertices.end());geoset.primitiveVertices.assign(indices.begin(),indices.end());geoset.primitive={{4U,0U,static_cast<std::uint32_t>(indices.size())}};model->shared->positions=geoset.position;model->shared->geosets.push_back(std::move(geoset));complex->m_geosets.push_back({~0U,0});model->data=std::move(complex);IModelComputeBounds(*model->shared);return model;
}
bool ModelGeosetAdd(CModel& model,const CGeosetShared& geoset){if(!model.shared)return false;model.shared->geosets.push_back(geoset);model.shared->numGeosets=static_cast<std::uint8_t>(std::min<std::size_t>(255,model.shared->geosets.size()));if(auto*complex=dynamic_cast<CModelComplex*>(model.data.get()))complex->m_geosets.push_back({~0U,geoset.flags});return true;}
void HashNewModel(const char* name,const std::shared_ptr<CModel>& model){if(!name||!model)return;std::scoped_lock lock(g_modelCacheMutex);g_modelCache[NormalizeName(name)]=model;}
std::shared_ptr<CModel> ModelGetModel(const char* name){if(!name)return{};std::scoped_lock lock(g_modelCacheMutex);const auto found=g_modelCache.find(NormalizeName(name));return found==g_modelCache.end()?std::shared_ptr<CModel>{}:found->second.lock();}
std::shared_ptr<CModel> GetModel(const char* name){return ModelGetModel(name);}
void ModelRemoveFromCache(const char* name){if(!name)return;std::scoped_lock lock(g_modelCacheMutex);g_modelCache.erase(NormalizeName(name));}
void IModelDerefHandle(std::shared_ptr<CModel>& model){ModelDestroy(model);}
std::uint32_t GetSectionCount(std::span<const std::uint8_t> bytes){if(bytes.size()<sizeof(MdxHeader))return 0;ModelByteReader reader(bytes);MdxHeader header{};reader.Read(header);std::uint32_t count=0;while(reader.Remaining()>=sizeof(MdxChunk)){MdxChunk chunk{};if(!reader.Read(chunk)||chunk.size>reader.Remaining())break;++count;if(!reader.Skip(chunk.size))break;}return count;}
std::uint32_t GetTextureCount(const CModel& model){if(const auto*complex=dynamic_cast<const CModelComplex*>(model.data.get()))return static_cast<std::uint32_t>(complex->m_textures.size());return 0;}
std::uint32_t ConvertAnimCreateFlags(std::uint32_t flags){return flags&0xFFFFU;}
bool LoadBoundsData(std::span<const std::uint8_t> bytes,CBoundsData& bounds){ModelByteReader reader(bytes);return reader.Read(bounds.extent)&&reader.Read(bounds.sphere);}
bool MdlReadLoadPositions(std::span<const std::uint8_t> bytes,CModelShared& shared){return MdxReadPositions(bytes,shared);}
bool MdlReadLoadExtents(std::span<const std::uint8_t> bytes,CModelShared& shared){return MdxReadExtents(bytes,shared);}
bool MdlReadLoadHitTestData(std::span<const std::uint8_t> bytes,CModelShared& shared){return MdxReadHitTestData(bytes,shared);}
bool MdlReadLoadNumMatrices(std::span<const std::uint8_t> bytes,CModelShared& shared){return MdxReadNumMatrices(bytes,shared);}
bool MdxReadAnimation(std::span<const std::uint8_t> bytes,CModel& model){ModelByteReader reader(bytes);std::uint32_t count=0;if(!reader.Read(count))return false;model.sequences.clear();model.sequences.reserve(count);for(std::uint32_t i=0;i<count;++i){CModelSequence sequence;if(!reader.Read(sequence.id)||!reader.Read(sequence.durationMs)||!reader.Read(sequence.moveSpeed)||!reader.Read(sequence.flags))return false;sequence.name="Sequence"+std::to_string(sequence.id);model.sequences.push_back(std::move(sequence));}return true;}
bool MdlReadLoadAnim(std::span<const std::uint8_t> bytes,CModel& model){return MdxReadAnimation(bytes,model);}
void ProcessAnimReorders(CModel& model,std::span<const std::uint32_t> reorder){if(reorder.size()!=model.sequences.size())return;auto old=model.sequences;for(std::size_t i=0;i<reorder.size();++i)if(reorder[i]<old.size())model.sequences[i]=old[reorder[i]];}
void EnqueueModelCommand(std::function<void()> command){if(!command)return;std::scoped_lock lock(g_modelCommandMutex);g_modelCommands.push_back(std::move(command));}
void ExecuteQueuedActions(){std::vector<std::function<void()>> commands;{std::scoped_lock lock(g_modelCommandMutex);commands.swap(g_modelCommands);}for(auto&command:commands)command();}
void AsyncModelHandler(){ExecuteQueuedActions();}
void AsnycModelPostLoadCallback(CModel& model,bool success){model.load=success?MODEL_LOAD_COMPLETE:MODEL_LOAD_FAILED;}
void BuildSimpleGeoset(CModel& model,const CGeosetShared& geoset){ModelGeosetAdd(model,geoset);}
void BuildSimpleMaterial(CModel& model,const CMaterial& material){if(auto*complex=dynamic_cast<CModelComplex*>(model.data.get()))complex->m_materials.push_back(material);}
