#include "ENGINE/Source/Model/collisiondisplay.h"
#include "ENGINE/Source/Model/mdlsgmodel.h"
#include "ENGINE/Source/Model/modelrender.h"

void CreateCollisionDisplayMesh(const CCollisionData& collision, CGeosetShared& mesh)
{
    mesh.position = collision.vertices; mesh.primitiveVertices = collision.indices;
    mesh.normal.assign(mesh.position.size(), {});
    for (std::size_t i = 0; i + 2U < collision.indices.size(); i += 3U) {
        const auto normal = i / 3U < collision.surfaceNormals.size() ? collision.surfaceNormals[i / 3U] : NTempest::C3Vector{};
        mesh.normal[collision.indices[i]] += normal; mesh.normal[collision.indices[i + 1U]] += normal; mesh.normal[collision.indices[i + 2U]] += normal;
    }
    for (auto& normal : mesh.normal) if (normal.SquaredMag() > 0.0F) normal.Normalize();
    mesh.primitive = {{4U, 0U, static_cast<std::uint32_t>(mesh.primitiveVertices.size())}};
}

void CreateCollisionDisplayNormals(const CCollisionData& collision, CGeosetShared& normals)
{
    normals.position.clear(); normals.primitiveVertices.clear();
    for (std::size_t i = 0; i + 2U < collision.indices.size(); i += 3U) {
        const auto center = (collision.vertices[collision.indices[i]] + collision.vertices[collision.indices[i + 1U]] + collision.vertices[collision.indices[i + 2U]]) / 3.0F;
        const auto normal = i / 3U < collision.surfaceNormals.size() ? collision.surfaceNormals[i / 3U] : NTempest::C3Vector{};
        const auto base = static_cast<std::uint16_t>(normals.position.size()); normals.position.push_back(center); normals.position.push_back(center + normal * 0.25F);
        normals.primitiveVertices.push_back(base); normals.primitiveVertices.push_back(base + 1U);
    }
    normals.primitive = {{1U, 0U, static_cast<std::uint32_t>(normals.primitiveVertices.size())}};
}

std::shared_ptr<CModel> CollisionDataCreateModel(const CCollisionData& collision)
{
    auto model = IModelCreateSimpleEmpty("CollisionDisplay"); auto complex = std::make_unique<CModelComplex>();
    CGeosetShared mesh; CreateCollisionDisplayMesh(collision, mesh); model->shared->geosets.push_back(std::move(mesh));
    BuildAllGeosets(*complex, *model->shared); model->data = std::move(complex); model->shared->collision = std::make_shared<CCollisionData>(collision); IModelComputeBounds(*model->shared); return model;
}

void BuildDisplayBox(const NTempest::CAaBox& box, CGeosetShared& mesh)
{
    CreateBoxGeometry(box, mesh.position, mesh.primitiveVertices); mesh.primitive = {{4U, 0U, static_cast<std::uint32_t>(mesh.primitiveVertices.size())}};
}

std::shared_ptr<CModel> CollisionDataRenderAABox(const NTempest::CAaBox& box)
{
    auto model = IModelCreateSimpleEmpty("CollisionAABox"); auto complex = std::make_unique<CModelComplex>(); CGeosetShared mesh; BuildDisplayBox(box, mesh);
    model->shared->geosets.push_back(std::move(mesh)); BuildAllGeosets(*complex, *model->shared); model->data = std::move(complex); model->shared->bounds.extent = box; model->shared->bounds.sphere.c = (box.b + box.t) * 0.5F; model->shared->bounds.sphere.r = ComputeBoundingRadius({model->shared->geosets[0].position}, model->shared->bounds.sphere.c); return model;
}

std::shared_ptr<CModel> CreateSimpleModel(const CGeosetShared& geoset, const char* name)
{
    auto model = IModelCreateSimpleEmpty(name); auto complex = std::make_unique<CModelComplex>();
    model->shared->geosets.push_back(geoset); BuildAllGeosets(*complex, *model->shared); model->data = std::move(complex); IModelComputeBounds(*model->shared); return model;
}

void CollisionDataAABoxRenderCallback(CModel& model, const NTempest::C34Matrix& basis)
{
    if (model.data) model.data->m_modelToWorld = basis;
    ModelAddToScene(model, 0);
}
