#include "ENGINE/Source/Model/modelanim.h"
#include "ENGINE/Source/Model/modelrender.h"
#include "ENGINE/Source/Anim/anim.h"
#include "ENGINE/Source/Services/particlesystem2.h"
#include "ENGINE/Source/Services/particleemitters.h"
#include "ENGINE/Source/Services/ribbonemitter.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstring>

namespace {
bool g_animationReady = false;
bool g_animationLogging = false;
constexpr std::uint32_t kLookAt = 0x01000000U;
constexpr std::uint32_t kFaceDir = 0x02000000U;

CGeoset* GetUniqueGeoset(CModel& model, std::uint32_t geoset)
{
    if (auto* complex = dynamic_cast<CModelComplex*>(model.data.get()))
        return geoset < complex->m_geosets.size() ? &complex->m_geosets[geoset] : nullptr;
    if (auto* simple = dynamic_cast<CModelSimple*>(model.data.get()))
        return geoset < simple->m_numGeosets ? &simple->m_geosets[geoset] : nullptr;
    return nullptr;
}
}

void ModelAnimateInitialize() { g_animationReady = true; }
void ModelAnimateDestroy() { g_animationReady = false; }
void ModelPauseTime(CModel& model, bool pause) { model.paused = pause; if (model.anim) AnimPauseTime(*model.anim, pause); }
void ModelSetTimeScale(CModel& model, float scale) { model.timeScale = std::max(0.0F, scale); if (model.anim) AnimSetTimeScale(*model.anim, model.timeScale); }
float ModelGetTimeScale(const CModel& model) { return model.timeScale; }
void ModelSetObjectTimeScale(CModel& model, float scale) { ModelSetTimeScale(model, scale); }
float ModelGetObjectTimeScale(const CModel& model) { return ModelGetTimeScale(model); }

bool ModelSetSequence(CModel& model, std::uint32_t sequence)
{
    if (sequence >= model.sequences.size()) return false;
    if (model.anim && !AnimSetSequence(*model.anim, sequence)) return false;
    model.primarySequence = sequence;
    model.sequenceTimeMs = 0;
    return true;
}

bool ModelForceSequenceTime(CModel& model, std::uint32_t sequence, std::uint32_t timeMs)
{
    if (!ModelSetSequence(model, sequence)) return false;
    ModelForceCurrentSequenceTime(model, timeMs);
    return true;
}

void ModelForceCurrentSequenceTime(CModel& model, std::uint32_t timeMs)
{
    const auto duration = ModelGetSequenceDuration(model, model.primarySequence);
    model.sequenceTimeMs = duration ? std::min(timeMs, duration) : 0U;
    if (model.anim) {
        AnimForceCurrentSequenceTime(*model.anim, static_cast<int>(model.sequenceTimeMs));
        AnimAnimateModel(*model.anim);
    }
}

std::uint32_t ModelGetNumSequences(const CModel& model) { return static_cast<std::uint32_t>(model.sequences.size()); }

std::uint32_t ModelGetSequenceDuration(const CModel& model, std::uint32_t sequence)
{
    return sequence < model.sequences.size() ? model.sequences[sequence].durationMs : 0U;
}

float ModelGetSequenceMoveSpeed(const CModel& model, std::uint32_t sequence)
{
    return sequence < model.sequences.size() ? model.sequences[sequence].moveSpeed : 0.0F;
}

const char* ModelGetSequenceName(const CModel& model, std::uint32_t sequence)
{
    return sequence < model.sequences.size() ? model.sequences[sequence].name.c_str() : nullptr;
}

bool ModelHasSequenceId(const CModel& model, std::uint32_t id)
{
    return std::any_of(model.sequences.begin(), model.sequences.end(),
                       [id](const CModelSequence& sequence) { return sequence.id == id; });
}

int ModelMatchSequence(const CModel& model, const char* name)
{
    if (!name) return -1;
    for (std::size_t i = 0; i < model.sequences.size(); ++i)
        if (_stricmp(model.sequences[i].name.c_str(), name) == 0) return static_cast<int>(i);
    return -1;
}

float ModelGetPrimarySequenceCompletion(const CModel& model)
{
    if (model.anim) return AnimGetPrimarySequenceCompletion(*model.anim);
    const auto duration = ModelGetSequenceDuration(model, model.primarySequence);
    return duration ? std::clamp(float(model.sequenceTimeMs) / float(duration), 0.0F, 1.0F) : 0.0F;
}

void ModelResetGlobalSequenceTimes(CModel& model) { model.sequenceTimeMs = 0; if (model.anim) AnimResetGlobalSequenceTimes(*model.anim); }
void ModelSetSeqFinishedHandler(CModel& model, std::function<void(CModel&)> callback) { model.sequenceFinished = std::move(callback); if (model.anim) AnimSetSeqFinishedHandler(*model.anim, [&model]() { if (model.sequenceFinished) model.sequenceFinished(model); return 0; }); }
void ModelSetEventCallback(CModel& model, std::function<void(const char*, const NTempest::C3Vector&)> callback) { model.eventCallback = std::move(callback); if (model.anim) AnimSetEventCallback(*model.anim, model.eventCallback); }

void ModelProcessEvents(CModel& model, std::uint32_t oldTime, std::uint32_t newTime)
{
    if (model.eventCallback && newTime < oldTime)
        model.eventCallback("SequenceLoop", model.data ? model.data->m_modelToWorld * NTempest::C3Vector{} : NTempest::C3Vector{});
}

void UpdateEmitters(CModel& model, float elapsedSec)
{
    auto* complex = dynamic_cast<CModelComplex*>(model.data.get());
    if (!complex) return;
    for (std::size_t index = 0; index < complex->m_emitters2.size(); ++index) {
        auto* emitter = complex->m_emitters2[index];
        if (!emitter) continue;
        const auto objectId = model.shared && index < model.shared->emitter2Order.size()
                                  ? model.shared->emitter2Order[index]
                                  : ~0U;
        const auto* object = model.animData && objectId < model.animData->obj.size() &&
                                     model.animData->obj[objectId] &&
                                     model.animData->obj[objectId]->type == ANIM_OBJ_EMITTER2
                                 ? static_cast<const CAnimEmitter2Obj*>(model.animData->obj[objectId])
                                 : nullptr;
        const auto* values = model.anim && objectId < model.anim->emitter2Status.size()
                                 ? &model.anim->emitter2Status[objectId]
                                 : nullptr;
        NTempest::C34Matrix emitterToModel = model.data->m_modelToWorld;
        if (object && values) {
            if (!object->particleSpeed.keys.empty()) emitter->SetVelocity(values->speed);
            if (!object->variation.keys.empty()) emitter->SetVelocityVariation(values->variation);
            if (!object->gravity.keys.empty()) emitter->SetAcceleration(values->gravity);
            if (!object->emissionRate.keys.empty()) emitter->SetEmissionRate(values->emissionRate);
            if (!object->zsource.keys.empty()) emitter->SetZsource(values->zsource);
            if (!object->lifeSpan.keys.empty()) emitter->SetLifeSpan(values->lifeSpan);
            if (auto* plane = dynamic_cast<CPlaneParticleEmitter*>(emitter)) {
                if (!object->latitude.keys.empty()) plane->SetLatitude(values->latitude);
                if (!object->longitude.keys.empty()) plane->SetLongitude(values->longitude);
                if (!object->length.keys.empty()) plane->SetHeight(values->length);
                if (!object->width.keys.empty()) plane->SetWidth(values->width);
            } else if (auto* sphere = dynamic_cast<CSphereParticleEmitter*>(emitter)) {
                if (!object->latitude.keys.empty()) sphere->SetLatitude(values->latitude);
                if (!object->longitude.keys.empty()) sphere->SetLongitude(values->longitude);
                if (!object->length.keys.empty()) sphere->SetHeight(values->length);
                if (!object->width.keys.empty()) sphere->SetWidth(values->width);
            } else if (auto* spline = dynamic_cast<CSplineParticleEmitter*>(emitter)) {
                if (!object->latitude.keys.empty()) spline->SetLatitude(values->latitude);
                if (!object->longitude.keys.empty()) spline->SetLongitude(values->longitude);
                if (!object->length.keys.empty()) spline->SetHeight(values->length);
                if (!object->width.keys.empty()) spline->SetWidth(values->width);
            }
            const bool visible = object->visibility.keys.empty() || values->visibility > 0.0F;
            emitter->SetEnabled(model.emittersEnabled && visible);
            if (objectId < model.anim->status.size())
                emitterToModel *= model.anim->status[objectId].transform;
        } else {
            emitter->SetEnabled(model.emittersEnabled);
        }
        // The animation world-matrix stack is positioned at the object's
        // pivot when particle emitters are updated.  Bone matrices include a
        // trailing -pivot translation for skinning, so applying the pivot
        // here recovers the emitter's actual origin and orientation.
        if (model.animData && objectId < model.animData->pivots.size())
            emitterToModel.Translate(model.animData->pivots[objectId]);
        emitterToModel *= NTempest::C34Matrix::Rotation(
            {0.0F, 0.0F, 1.0F}, 1.5707963267948966F);
        emitter->UpdateXform(emitterToModel);
        if (emitter->Enabled()) emitter->Update(elapsedSec);
    }
    if (model.ribbonsEnabled)
        for (auto* ribbon : complex->m_ribbons) if (ribbon) ribbon->Update(elapsedSec, 0);
}

void ModelAdvanceTime(CModel& model, std::uint32_t elapsedMs)
{
    if (!g_animationReady) ModelAnimateInitialize();
    if (model.anim) {
        AnimAdvanceTime(*model.anim, elapsedMs);
        model.sequenceTimeMs = AnimGetElapsedTime(*model.anim);
        ApplyWorldTransforms(model);
        return;
    }
    if (model.paused || model.sequences.empty()) return;
    const auto oldTime = model.sequenceTimeMs;
    const auto duration = ModelGetSequenceDuration(model, model.primarySequence);
    if (!duration) return;
    const auto advance = static_cast<std::uint32_t>(std::lround(double(elapsedMs) * model.timeScale));
    const std::uint64_t total = std::uint64_t(oldTime) + advance;
    model.sequenceTimeMs = static_cast<std::uint32_t>(total % duration);
    if (total >= duration && model.sequenceFinished) model.sequenceFinished(model);
    ModelProcessEvents(model, oldTime, model.sequenceTimeMs);
}

void ModelAnimate(CModel& model, std::uint32_t elapsedMs)
{
    ModelAdvanceTime(model, elapsedMs);
    ApplyWorldTransforms(model);
    UpdateEmitters(model, float(elapsedMs) / 1000.0F);
}

void ModelAnimateAttached(CModel& model, std::uint32_t elapsedMs)
{
    ModelAnimate(model, elapsedMs);
    for (const auto& link : model.links) {
        if (!link.model || link.model == &model || !link.model->data ||
            !model.shared || !model.anim || !model.animData ||
            link.point >= model.shared->attachIdToIndex.size())
            continue;

        const auto attachment = model.shared->attachIdToIndex[link.point];
        if (attachment == ~0U || !AnimIsAttachmentEnabled(*model.anim, attachment))
            continue;
        const auto objectId = AnimGetAttachmentObjId(*model.animData, attachment);
        if (objectId >= model.anim->status.size()) continue;

        NTempest::C34Matrix transform = model.data
            ? model.data->m_modelToWorld : NTempest::C34Matrix{};
        transform *= model.anim->status[objectId].transform;
        const NTempest::C3Vector attachmentPivot = objectId < model.animData->pivots.size()
            ? model.animData->pivots[objectId] : NTempest::C3Vector{};
        if (objectId < model.animData->pivots.size())
            transform.Translate(attachmentPivot);
        transform.Scale(link.scale);
        static unsigned attachmentDiagnosticCount = 0;
        if (attachmentDiagnosticCount < 4U && model.shared &&
            std::strstr(model.shared->name.data(), "UI_HUMAN") != nullptr) {
            const auto& status = model.anim->status[objectId].transform;
            std::fprintf(stderr,
                         "Character attachment transform: object=%u pivot=(%g,%g,%g) status=(%g,%g,%g) final=(%g,%g,%g)\n",
                         objectId, attachmentPivot.x, attachmentPivot.y, attachmentPivot.z,
                         status.d0, status.d1, status.d2,
                         transform.d0, transform.d1, transform.d2);
            ++attachmentDiagnosticCount;
        }
        link.model->data->m_modelToWorld = transform;
        ModelAnimateAttached(*link.model, elapsedMs);
    }
}

void ModelAnimateCameras(CModel& model, std::uint32_t) { if (model.anim) AnimAnimateCameras(*model.anim); }
void ApplyWorldTransforms(CModel& model)
{
    if (!model.shared) return;
    if (!model.anim || !model.animData) {
        // The recovered IModelAnimate path still calls
        // SetUnanimatedGeosetMatrices after loading m_modelToWorld into the
        // world-matrix stack.  Without this, models whose animation block
        // cannot be built retain an expired palette and render at the camera.
        SetUnanimatedGeosetMatrices(model);
        return;
    }
    model.transforms.resize(model.anim->status.size());
    for (std::size_t i = 0; i < model.anim->status.size(); ++i)
        model.transforms[i] = model.anim->status[i].transform;
    static unsigned worldHumanBoneReports = 0U;
    if (worldHumanBoneReports < 8U && model.data && model.data->m_PickLights &&
        _stricmp(model.shared->name.data(),
                 "Character\\Human\\Male\\HumanMale.MDX") == 0) {
        const unsigned sequence = model.anim->primarySeq;
        const int elapsed = sequence < model.anim->seq.size()
            ? model.anim->seq[sequence].elapsed : 0;
        const auto& root = model.transforms.empty()
            ? model.data->m_modelToWorld : model.transforms.front();
        std::fprintf(stderr,
                     "WORLD_CHARBONES report=%u seq=%u elapsed=%d paused=%u "
                     "rootBasis=(%.4f,%.4f,%.4f;%.4f,%.4f,%.4f;%.4f,%.4f,%.4f) "
                     "rootPos=(%.4f,%.4f,%.4f) standing=(%.4f,%.4f,%.4f)\n",
                     worldHumanBoneReports, sequence, elapsed,
                     model.anim->paused ? 1U : 0U,
                     root.a0, root.a1, root.a2,
                     root.b0, root.b1, root.b2,
                     root.c0, root.c1, root.c2,
                     root.d0, root.d1, root.d2,
                     model.data->m_modelToWorld.d0,
                     model.data->m_modelToWorld.d1,
                     model.data->m_modelToWorld.d2);
        ++worldHumanBoneReports;
    }
    static bool reportedHumanBones = false;
    if (!reportedHumanBones &&
        _stricmp(model.shared->name.data(), "Character\\Human\\Male\\HumanMale.MDX") == 0) {
        reportedHumanBones = true;
        const unsigned sequence = model.anim->primarySeq;
        const auto& sequenceData = model.animData->seq[sequence];
        const int sequenceTime = sequence < model.anim->seq.size()
            ? model.anim->seq[sequence].elapsed : 0;
        std::fprintf(stderr,
                     "CHARBONES sequence=%u name=%s range=%d,%d time=%d objects=%zu\n",
                     sequence, sequenceData.name, sequenceData.start, sequenceData.end,
                     sequenceTime, model.animData->obj.size());
        for (std::size_t i = 0; i < model.animData->obj.size() && i < model.transforms.size(); ++i) {
            const auto* object = model.animData->obj[i];
            if (!object || object->type != ANIM_OBJ_BONE) continue;
            const auto& matrix = model.transforms[i];
            const auto countForSequence = [sequence](const CKeyFrameTrackBase& track) {
                const unsigned index = track.m_globalSeqId == ~0U ? sequence : 0U;
                return index < track.m_indices.size() ? track.m_indices[index].count : 0U;
            };
            const auto parent = object->parent ? object->parent->animObjId : ~0U;
            const auto pivot = i < model.animData->pivots.size()
                ? model.animData->pivots[i] : NTempest::C3Vector{};
            const float sx = std::sqrt(matrix.a0 * matrix.a0 + matrix.a1 * matrix.a1 + matrix.a2 * matrix.a2);
            const float sy = std::sqrt(matrix.b0 * matrix.b0 + matrix.b1 * matrix.b1 + matrix.b2 * matrix.b2);
            const float sz = std::sqrt(matrix.c0 * matrix.c0 + matrix.c1 * matrix.c1 + matrix.c2 * matrix.c2);
            std::fprintf(stderr,
                         "CHARBONE i=%zu id=%u parent=%u name=%s pivot=(%g,%g,%g) "
                         "tracks=%u,%u,%u basis=(%g,%g,%g) pos=(%g,%g,%g)\n",
                         i, object->animObjId, parent, object->name,
                         pivot.x, pivot.y, pivot.z,
                         countForSequence(object->translation),
                         countForSequence(object->rotation),
                         countForSequence(object->scale), sx, sy, sz,
                         matrix.d0, matrix.d1, matrix.d2);
            if (_stricmp(object->name, "Head") == 0 ||
                _stricmp(object->name, "SpineUp") == 0) {
                const auto& track = object->rotation;
                const auto keyRange = sequence < track.m_indices.size()
                    ? track.m_indices[sequence] : CKeySeq{};
                for (unsigned key = keyRange.start;
                     key < keyRange.start + keyRange.count && key < track.keys.size(); ++key) {
                    const auto q = static_cast<NTempest::C4Quaternion>(track.keys[key].value);
                    std::fprintf(stderr,
                                 "CHARROT bone=%s key=%u time=%d q=(%g,%g,%g,%g)\n",
                                 object->name, key, track.keys[key].time,
                                 q.w, q.x, q.y, q.z);
                }
            }
        }
    }
    if (model.layerAlpha.size() < model.anim->layerStatus.size())
        model.layerAlpha.resize(model.anim->layerStatus.size(), 1.0F);
    if (model.layerTextureIds.size() < model.anim->layerStatus.size())
        model.layerTextureIds.resize(model.anim->layerStatus.size(), ~0U);
    for (std::size_t i = 0; i < model.anim->layerStatus.size(); ++i) {
        if (i < model.animData->layers.size() &&
            !model.animData->layers[i].visibility.m_keyFrames.empty())
            model.layerAlpha[i] = std::clamp(model.anim->layerStatus[i].alpha, 0.0F, 1.0F);
        if (i < model.animData->layers.size() &&
            !model.animData->layers[i].flip.m_keyFrames.empty())
            model.layerTextureIds[i] = model.anim->layerStatus[i].flip;
    }
    if (auto* complex = dynamic_cast<CModelComplex*>(model.data.get())) {
        if (complex->m_geosetColor.size() < model.shared->geosets.size())
            complex->m_geosetColor.resize(model.shared->geosets.size());
        for (std::size_t i = 0; i < model.animData->geo.size() && i < model.anim->geosetStatus.size(); ++i) {
            const auto geoset = model.animData->geo[i].sgGeosetId;
            if (geoset < complex->m_geosetColor.size()) complex->m_geosetColor[geoset] = model.anim->geosetStatus[i];
        }
    }
    if (model.data) {
        auto worldTransforms = model.transforms;
        for (auto& transform : worldTransforms)
            transform = model.data->m_modelToWorld * transform;
        SetGeosetMatrices(model, worldTransforms);
    } else {
        SetGeosetMatrices(model, model.transforms);
    }
}

void ModelApplyObjectLookAt(CModel& model, const NTempest::C3Vector& target)
{
    if (!model.data) return;
    model.data->m_flags |= kLookAt;
    model.data->m_modelToWorld.d0 = target.x;
    model.data->m_modelToWorld.d1 = target.y;
    model.data->m_modelToWorld.d2 = target.z;
}
void ModelRemoveObjectLookAt(CModel& model) { if (model.data) model.data->m_flags &= ~kLookAt; }

void ModelApplyObjectFaceDir(CModel& model, const NTempest::C3Vector& direction)
{
    if (!model.data || direction.SquaredMag() == 0.0F) return;
    model.data->m_flags |= kFaceDir;
    const float yaw = std::atan2(direction.y, direction.x);
    model.data->m_modelToWorld = NTempest::C34Matrix::Rotation({0.0F, 0.0F, 1.0F}, yaw);
}
void ModelApplyObjectFaceDir(CModel& model, std::uint32_t objectId,
                             const NTempest::C3Vector& direction)
{
    if (!model.anim || direction.SquaredMag() == 0.0F) return;
    AnimApplyObjectFaceDir(*model.anim, objectId, direction);
}
void ModelRemoveObjectFaceDir(CModel& model) { if (model.data) model.data->m_flags &= ~kFaceDir; }
NTempest::C34Matrix ModelGetStandingMatrix(const CModel& model) { return model.data ? model.data->m_modelToWorld : NTempest::C34Matrix{}; }
void ModelForceStandingMatrix(CModel& model, const NTempest::C34Matrix& matrix) { if (model.data) model.data->m_modelToWorld = matrix; }

void IModelAnimate(CModel& model, std::uint32_t elapsedMs) { ModelAnimate(model, elapsedMs); }
void IModelAnimateBounds(CModel&) {}
void IModelProcessEvents(CModel& model, std::uint32_t oldTime, std::uint32_t newTime) { ModelProcessEvents(model, oldTime, newTime); }
NTempest::C34Matrix IModelGetStandingMatrix(const CModel& model) { return ModelGetStandingMatrix(model); }
void IModelGetStandingBasis(const CModel& model, NTempest::C3Vector& right, NTempest::C3Vector& forward, NTempest::C3Vector& up)
{
    const auto matrix = ModelGetStandingMatrix(model); right = {matrix.a0, matrix.a1, matrix.a2}; forward = {matrix.b0, matrix.b1, matrix.b2}; up = {matrix.c0, matrix.c1, matrix.c2};
}
std::uint32_t GetTransformListIndex(CModel& model, std::uint32_t requested) { if (model.transforms.size() <= requested) model.transforms.resize(requested + 1U); return requested; }
NTempest::C34Matrix* GetTransformPtr(CModel& model, std::uint32_t index) { return index < model.transforms.size() ? &model.transforms[index] : nullptr; }
void ReleaseTransformListIndex(CModel& model, std::uint32_t index) { if (index < model.transforms.size()) model.transforms[index] = {}; }
std::uint32_t GetLayerIndex(CModel& model, std::uint32_t requested) { if (model.layerAlpha.size() <= requested) model.layerAlpha.resize(requested + 1U, 1.0F); return requested; }
float* GetLayerPtr(CModel& model, std::uint32_t index) { return index < model.layerAlpha.size() ? &model.layerAlpha[index] : nullptr; }
void ReleaseLayerIndex(CModel& model, std::uint32_t index) { if (index < model.layerAlpha.size()) model.layerAlpha[index] = 1.0F; }
float GetLayerAlpha(const CModel& model, std::uint32_t layer) { return layer < model.layerAlpha.size() ? model.layerAlpha[layer] : 1.0F; }
void SetLayerAlpha(CModel& model, std::uint32_t layer, float alpha) { GetLayerIndex(model, layer); model.layerAlpha[layer] = std::clamp(alpha, 0.0F, 1.0F); }
void SetGeosetMatrix(CModel& model, std::uint32_t geoset,
                     std::span<const NTempest::C34Matrix> boneMatrices)
{
    if (!model.shared || geoset >= model.shared->geosets.size()) return;
    auto* unique = GetUniqueGeoset(model, geoset);
    if (!unique || (unique->flags & 1U) != 0U) return;

    const auto& shared = model.shared->geosets[geoset];
    const auto count = static_cast<std::uint32_t>(shared.groupMatrixCounts.size());
    unique->weightedBones = MatrixAlloc(count);
    auto* weighted = MatrixDeref(unique->weightedBones);
    if (!weighted || count == 0) return;
    BuildPrimBones(boneMatrices, shared, {weighted, count});
}

void SetGeosetMatrices(CModel& model, std::span<const NTempest::C34Matrix> matrices)
{
    if (!model.shared) return;
    for (std::uint32_t geoset = 0; geoset < model.shared->geosets.size(); ++geoset)
        SetGeosetMatrix(model, geoset, matrices);

    static bool dumpedMainMenuMatrices = false;
    if (!dumpedMainMenuMatrices && std::strstr(model.shared->name.data(), "UI_MainMenu") != nullptr) {
        dumpedMainMenuMatrices = true;
        float maxBasis = 0.0F;
        float maxTranslation = 0.0F;
        for (const auto& matrix : matrices) {
            for (std::size_t i = 0; i < 9U; ++i) maxBasis = std::max(maxBasis, std::fabs(matrix.Data()[i]));
            maxTranslation = std::max({maxTranslation, std::fabs(matrix.d0), std::fabs(matrix.d1), std::fabs(matrix.d2)});
        }
        std::fprintf(stderr, "MATRIXDUMP model=%s bones=%zu maxbasis=%g maxtranslation=%g\n",
                     model.shared->name.data(), matrices.size(), maxBasis, maxTranslation);
        if (auto* complex = dynamic_cast<CModelComplex*>(model.data.get())) {
            for (std::size_t geoset = 0; geoset < complex->m_geosets.size(); ++geoset) {
                const auto& shared = model.shared->geosets[geoset];
                auto* palette = MatrixDeref(complex->m_geosets[geoset].weightedBones);
                float paletteMax = 0.0F;
                if (palette)
                    for (std::size_t group = 0; group < shared.groupMatrixCounts.size(); ++group)
                        for (const auto value : std::span<const float>(palette[group].Data(), 12U))
                            paletteMax = std::max(paletteMax, std::fabs(value));
                std::fprintf(stderr, "MATRIXDUMP g=%zu handle=%08X groups=%zu palette=%p max=%g\n",
                             geoset, complex->m_geosets[geoset].weightedBones,
                             shared.groupMatrixCounts.size(), static_cast<void*>(palette), paletteMax);
            }
        }
    }
}

void SetCollisionMatrices(CModel&, std::span<const NTempest::C34Matrix>) {}

void SetUnanimatedGeosetMatrix(CModel& model, std::uint32_t geoset)
{
    if (!model.shared || geoset >= model.shared->geosets.size()) return;
    auto* unique = GetUniqueGeoset(model, geoset);
    if (!unique || (unique->flags & 1U) != 0U) return;
    const auto& shared = model.shared->geosets[geoset];
    const auto count = static_cast<std::uint32_t>(
        shared.groupMatrixCounts.size());
    unique->weightedBones = MatrixAlloc(count);
    if (auto* weighted = MatrixDeref(unique->weightedBones)) {
        const NTempest::C34Matrix world = model.data
            ? model.data->m_modelToWorld : NTempest::C34Matrix{};
        for (std::uint32_t index = 0; index < count; ++index)
            weighted[index] = world;
    }
}

void SetUnanimatedGeosetMatrices(CModel& model)
{
    if (!model.shared) return;
    for (std::uint32_t geoset = 0; geoset < model.shared->geosets.size(); ++geoset)
        SetUnanimatedGeosetMatrix(model, geoset);
}

void SetUnanimatedCollisionMatrices(CModel&) {}
bool ModelObjectUsingLookAt(const CModel& model) { return model.data && (model.data->m_flags & kLookAt) != 0; }
bool ModelObjectUsingFaceDir(const CModel& model) { return model.data && (model.data->m_flags & kFaceDir) != 0; }
NTempest::C3Vector ModelGetObjectPosition(const CModel& model) { const auto matrix = ModelGetStandingMatrix(model); return {matrix.d0, matrix.d1, matrix.d2}; }
NTempest::C3Vector ModelGetModelSpacePivot(const CModel& model, std::uint32_t transform) { return transform < model.transforms.size() ? NTempest::C3Vector{model.transforms[transform].d0, model.transforms[transform].d1, model.transforms[transform].d2} : NTempest::C3Vector{}; }
NTempest::C3Vector ModelGetEventObjectPosition(const CModel& model, std::uint32_t transform) { return ModelGetStandingMatrix(model) * ModelGetModelSpacePivot(model, transform); }
void ModelSetSequenceFidget(CModel& model, std::uint32_t sequence, std::uint32_t count) { model.fidgetSequence = sequence; model.fidgetCount = count; }
bool ModelSetRandomSequenceFidget(CModel& model, std::uint32_t seed) { if (model.sequences.empty()) return false; return ModelSetSequence(model, seed % static_cast<std::uint32_t>(model.sequences.size())); }
std::uint32_t ModelGetNumSequenceFidgets(const CModel& model) { return model.fidgetCount; }
void ModelLockObjectSequence(CModel& model, bool lock) { if (lock) model.paused = true; }
void ModelMarkFootstepSequence(CModel& model, std::uint32_t sequence) { if (sequence < model.sequences.size()) model.sequences[sequence].flags |= 0x100U; }
bool ModelEventEmitterHasKeysThisSeq(const CModel& model, std::uint32_t sequence) { return sequence < model.sequences.size() && model.sequences[sequence].durationMs != 0; }
std::uint32_t ModelGetTotalKeys(const CModel& model) { std::uint32_t total = 0; for (const auto& sequence : model.sequences) total += sequence.durationMs != 0; return total; }
void BuildPrimBone(std::span<const NTempest::C34Matrix> boneMatrices,
                   std::span<const std::uint32_t> matrixIndices,
                   NTempest::C34Matrix& weightedMatrix)
{
    if (matrixIndices.empty()) {
        weightedMatrix = {};
        return;
    }
    const auto first = matrixIndices.front();
    weightedMatrix = first < boneMatrices.size() ? boneMatrices[first] : NTempest::C34Matrix{};
    for (std::size_t index = 1; index < matrixIndices.size(); ++index) {
        const auto bone = matrixIndices[index];
        weightedMatrix += bone < boneMatrices.size() ? boneMatrices[bone] : NTempest::C34Matrix{};
    }
    if (matrixIndices.size() > 1U)
        weightedMatrix /= static_cast<float>(matrixIndices.size());
}

void BuildPrimBones(std::span<const NTempest::C34Matrix> boneMatrices,
                    const CGeosetShared& geoset,
                    std::span<NTempest::C34Matrix> weightedMatrices)
{
    std::size_t matrixOffset = 0;
    const auto groups = std::min(weightedMatrices.size(), geoset.groupMatrixCounts.size());
    for (std::size_t group = 0; group < groups; ++group) {
        const auto count = static_cast<std::size_t>(geoset.groupMatrixCounts[group]);
        if (matrixOffset > geoset.matrices.size() || count > geoset.matrices.size() - matrixOffset) {
            weightedMatrices[group] = {};
            matrixOffset = geoset.matrices.size();
            continue;
        }
        BuildPrimBone(boneMatrices,
                      {geoset.matrices.data() + matrixOffset, count},
                      weightedMatrices[group]);
        matrixOffset += count;
    }
}
std::vector<std::uint32_t> IGetLayerIDs(const CModel& model) { std::vector<std::uint32_t> result(model.layerAlpha.size()); for (std::uint32_t i = 0; i < result.size(); ++i) result[i] = i; return result; }
void ModelAnimateLogStart() { g_animationLogging = true; }
void ModelAnimateLogStop() { g_animationLogging = false; }
void ModelAnimateLogToggle() { g_animationLogging = !g_animationLogging; }
