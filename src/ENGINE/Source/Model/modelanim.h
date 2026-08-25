#pragma once
#include "ENGINE/Source/Model/model.h"
#include <span>

void ModelAnimateInitialize();
void ModelAnimateDestroy();
void ModelAdvanceTime(CModel& model, std::uint32_t elapsedMs);
void ModelAnimate(CModel& model, std::uint32_t elapsedMs);
void ModelAnimateAttached(CModel& model, std::uint32_t elapsedMs);
void ModelAnimateCameras(CModel& model, std::uint32_t elapsedMs);
void ModelPauseTime(CModel& model, bool pause);
void ModelSetTimeScale(CModel& model, float scale);
float ModelGetTimeScale(const CModel& model);
void ModelSetObjectTimeScale(CModel& model, float scale);
float ModelGetObjectTimeScale(const CModel& model);
bool ModelSetSequence(CModel& model, std::uint32_t sequence);
bool ModelForceSequenceTime(CModel& model, std::uint32_t sequence, std::uint32_t timeMs);
void ModelForceCurrentSequenceTime(CModel& model, std::uint32_t timeMs);
std::uint32_t ModelGetPrimarySequence(const CModel& model);
std::uint32_t ModelGetSequenceTime(const CModel& model);
std::uint32_t ModelGetNumSequences(const CModel& model);
std::uint32_t ModelGetSequenceDuration(const CModel& model, std::uint32_t sequence);
float ModelGetSequenceMoveSpeed(const CModel& model, std::uint32_t sequence);
const char* ModelGetSequenceName(const CModel& model, std::uint32_t sequence);
bool ModelHasSequenceId(const CModel& model, std::uint32_t id);
int ModelMatchSequence(const CModel& model, const char* name);
float ModelGetPrimarySequenceCompletion(const CModel& model);
void ModelResetGlobalSequenceTimes(CModel& model);
void ModelSetSeqFinishedHandler(CModel& model, std::function<void(CModel&)> callback);
void ModelSetEventCallback(CModel& model,
                           std::function<void(const char*, const NTempest::C3Vector&)> callback);
void ModelProcessEvents(CModel& model, std::uint32_t oldTime, std::uint32_t newTime);
void UpdateEmitters(CModel& model, float elapsedSec);
void ApplyWorldTransforms(CModel& model);
void ModelApplyObjectLookAt(CModel& model, const NTempest::C3Vector& target);
void ModelRemoveObjectLookAt(CModel& model);
void ModelApplyObjectFaceDir(CModel& model, const NTempest::C3Vector& direction);
void ModelApplyObjectFaceDir(CModel& model, std::uint32_t objectId,
                             const NTempest::C3Vector& direction);
void ModelRemoveObjectFaceDir(CModel& model);
NTempest::C34Matrix ModelGetStandingMatrix(const CModel& model);
void ModelForceStandingMatrix(CModel& model, const NTempest::C34Matrix& matrix);
void IModelAnimate(CModel& model, std::uint32_t elapsedMs);
void IModelAnimateBounds(CModel& model);
void IModelProcessEvents(CModel& model, std::uint32_t oldTime, std::uint32_t newTime);
NTempest::C34Matrix IModelGetStandingMatrix(const CModel& model);
void IModelGetStandingBasis(const CModel& model, NTempest::C3Vector& right,
                            NTempest::C3Vector& forward, NTempest::C3Vector& up);
std::uint32_t GetTransformListIndex(CModel& model, std::uint32_t requested);
NTempest::C34Matrix* GetTransformPtr(CModel& model, std::uint32_t index);
void ReleaseTransformListIndex(CModel& model, std::uint32_t index);
std::uint32_t GetLayerIndex(CModel& model, std::uint32_t requested);
float* GetLayerPtr(CModel& model, std::uint32_t index);
void ReleaseLayerIndex(CModel& model, std::uint32_t index);
float GetLayerAlpha(const CModel& model, std::uint32_t layer);
void SetLayerAlpha(CModel& model, std::uint32_t layer, float alpha);
void SetGeosetMatrix(CModel& model, std::uint32_t geoset,
                     std::span<const NTempest::C34Matrix> boneMatrices);
void SetGeosetMatrices(CModel& model, std::span<const NTempest::C34Matrix> matrices);
void SetCollisionMatrices(CModel& model, std::span<const NTempest::C34Matrix> matrices);
void SetUnanimatedGeosetMatrix(CModel& model, std::uint32_t geoset);
void SetUnanimatedGeosetMatrices(CModel& model);
void SetUnanimatedCollisionMatrices(CModel& model);
bool ModelObjectUsingLookAt(const CModel& model);
bool ModelObjectUsingFaceDir(const CModel& model);
NTempest::C3Vector ModelGetObjectPosition(const CModel& model);
NTempest::C3Vector ModelGetModelSpacePivot(const CModel& model, std::uint32_t transform);
NTempest::C3Vector ModelGetEventObjectPosition(const CModel& model, std::uint32_t transform);
void ModelSetSequenceFidget(CModel& model, std::uint32_t sequence, std::uint32_t count);
bool ModelSetRandomSequenceFidget(CModel& model, std::uint32_t seed);
std::uint32_t ModelGetNumSequenceFidgets(const CModel& model);
void ModelLockObjectSequence(CModel& model, bool lock);
void ModelMarkFootstepSequence(CModel& model, std::uint32_t sequence);
bool ModelEventEmitterHasKeysThisSeq(const CModel& model, std::uint32_t sequence);
std::uint32_t ModelGetTotalKeys(const CModel& model);
void BuildPrimBone(std::span<const NTempest::C34Matrix> boneMatrices,
                   std::span<const std::uint32_t> matrixIndices,
                   NTempest::C34Matrix& weightedMatrix);
void BuildPrimBones(std::span<const NTempest::C34Matrix> boneMatrices,
                    const CGeosetShared& geoset,
                    std::span<NTempest::C34Matrix> weightedMatrices);
std::vector<std::uint32_t> IGetLayerIDs(const CModel& model);
void ModelAnimateLogStart();
void ModelAnimateLogStop();
void ModelAnimateLogToggle();
