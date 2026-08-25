#pragma once
#include "ENGINE/Source/Model/collisiondata.h"
#include "ENGINE/Source/Model/modelcreate.h"

std::shared_ptr<CModel> CollisionDataCreateModel(const CCollisionData& collision);
void CreateCollisionDisplayMesh(const CCollisionData& collision, CGeosetShared& mesh);
void CreateCollisionDisplayNormals(const CCollisionData& collision, CGeosetShared& normals);
void BuildDisplayBox(const NTempest::CAaBox& box, CGeosetShared& mesh);
std::shared_ptr<CModel> CollisionDataRenderAABox(const NTempest::CAaBox& box);
std::shared_ptr<CModel> CreateSimpleModel(const CGeosetShared& geoset, const char* name);
void CollisionDataAABoxRenderCallback(CModel& model, const NTempest::C34Matrix& basis);
