#pragma once

#include "ENGINE/Source/Model/model.h"
#include "ENGINE/Source/Tempest/cfacet.h"

#include <span>

std::shared_ptr<CCollisionData> CollisionDataCreate(
    std::span<const NTempest::C3Vector> vertices,
    std::span<const std::uint16_t> indices);
std::shared_ptr<CCollisionData> CollisionDataCreate(const std::uint8_t* fileBytes, std::size_t byteCount);
void ComputeSurfaceNormals(CCollisionData& collision);
void CollisionDataAddFacets(const CCollisionData& collision,
                            std::vector<NTempest::CFacet>& facets,
                            const NTempest::C34Matrix* transform = nullptr,
                            const NTempest::CAaBox* clip = nullptr);
void ModelAddCollisionFacets(const CModel& model, std::vector<NTempest::CFacet>& facets,
                             const NTempest::CAaBox* clip = nullptr);
bool TriangleIsClippedOut(const NTempest::C3Vector* triVerts, const NTempest::CAaBox& clip);
bool CollisionDataVectorIntersect(const CCollisionData& collision,
                                  const NTempest::C3Vector& start,
                                  const NTempest::C3Vector& delta,
                                  float& distance,
                                  std::uint32_t* surfaceIndex = nullptr);
bool ModelCollisionVectorIntersect(const CModel& model,
                                   const NTempest::C3Vector& start,
                                   const NTempest::C3Vector& delta,
                                   ModelIntersectResult& result);
bool ModelGetCollisionExtents(const CModel& model, NTempest::CAaBox& extents);
void ModelShowCollision(CModel& model, bool show);
void ModelShowCollisionAaBox(CModel& model, bool show);
void ModelShowModel(CModel& model, bool show);
