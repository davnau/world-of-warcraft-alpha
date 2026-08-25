#include "ENGINE/Source/Model/collisiondata.h"
#include "ENGINE/Source/Tempest/tempest_intersect.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <limits>
#include <stdexcept>

namespace {
struct CollisionHeader {
    std::uint32_t magic;
    std::uint32_t vertexCount;
    std::uint32_t indexCount;
};
constexpr std::uint32_t kCollisionMagic = 0x4C4C4F43U; // COLL

NTempest::C3Vector TransformPoint(const NTempest::C34Matrix& m, const NTempest::C3Vector& v)
{
    return m * v;
}
}

std::shared_ptr<CCollisionData> CollisionDataCreate(
    std::span<const NTempest::C3Vector> vertices,
    std::span<const std::uint16_t> indices)
{
    if (indices.size() % 3U != 0U)
        throw std::invalid_argument("collision indices must contain complete triangles");
    for (const auto index : indices)
        if (index >= vertices.size())
            throw std::out_of_range("collision index exceeds vertex count");

    auto collision = std::make_shared<CCollisionData>();
    collision->vertices.assign(vertices.begin(), vertices.end());
    collision->indices.assign(indices.begin(), indices.end());
    if (!collision->vertices.empty())
        collision->extents.Bounding(collision->vertices.data(), static_cast<unsigned>(collision->vertices.size()));
    ComputeSurfaceNormals(*collision);
    return collision;
}

std::shared_ptr<CCollisionData> CollisionDataCreate(const std::uint8_t* fileBytes, std::size_t byteCount)
{
    if (!fileBytes || byteCount < sizeof(CollisionHeader)) return {};
    CollisionHeader header{};
    std::memcpy(&header, fileBytes, sizeof(header));
    if (header.magic != kCollisionMagic) return {};
    const std::size_t vertexBytes = std::size_t(header.vertexCount) * sizeof(NTempest::C3Vector);
    const std::size_t indexBytes = std::size_t(header.indexCount) * sizeof(std::uint16_t);
    if (vertexBytes > byteCount - sizeof(header) || indexBytes > byteCount - sizeof(header) - vertexBytes)
        return {};
    const auto* vertices = reinterpret_cast<const NTempest::C3Vector*>(fileBytes + sizeof(header));
    const auto* indices = reinterpret_cast<const std::uint16_t*>(fileBytes + sizeof(header) + vertexBytes);
    try {
        return CollisionDataCreate({vertices, header.vertexCount}, {indices, header.indexCount});
    } catch (...) {
        return {};
    }
}

void ComputeSurfaceNormals(CCollisionData& collision)
{
    collision.surfaceNormals.clear();
    collision.surfaceNormals.reserve(collision.indices.size() / 3U);
    for (std::size_t i = 0; i + 2U < collision.indices.size(); i += 3U) {
        const auto& a = collision.vertices[collision.indices[i]];
        const auto& b = collision.vertices[collision.indices[i + 1U]];
        const auto& c = collision.vertices[collision.indices[i + 2U]];
        auto normal = NTempest::C3Vector::Cross(b - a, c - a);
        if (normal.SquaredMag() > std::numeric_limits<float>::epsilon()) normal.Normalize();
        collision.surfaceNormals.push_back(normal);
    }
}

bool TriangleIsClippedOut(const NTempest::C3Vector* triVerts, const NTempest::CAaBox& clip)
{
    if (!triVerts) return true;
    for (unsigned axis = 0; axis < 3; ++axis) {
        if (triVerts[0][axis] < clip.b[axis] && triVerts[1][axis] < clip.b[axis] && triVerts[2][axis] < clip.b[axis]) return true;
        if (triVerts[0][axis] > clip.t[axis] && triVerts[1][axis] > clip.t[axis] && triVerts[2][axis] > clip.t[axis]) return true;
    }
    return false;
}

void CollisionDataAddFacets(const CCollisionData& collision, std::vector<NTempest::CFacet>& facets,
                            const NTempest::C34Matrix* transform, const NTempest::CAaBox* clip)
{
    for (std::size_t i = 0; i + 2U < collision.indices.size(); i += 3U) {
        NTempest::C3Vector triangle[3] = {
            collision.vertices[collision.indices[i]],
            collision.vertices[collision.indices[i + 1U]],
            collision.vertices[collision.indices[i + 2U]],
        };
        if (transform)
            for (auto& vertex : triangle) vertex = TransformPoint(*transform, vertex);
        if (clip && TriangleIsClippedOut(triangle, *clip)) continue;
        NTempest::CFacet facet;
        facet.Set(triangle[0], triangle[1], triangle[2]);
        facets.push_back(facet);
    }
}

void ModelAddCollisionFacets(const CModel& model, std::vector<NTempest::CFacet>& facets,
                             const NTempest::CAaBox* clip)
{
    if (!model.shared || !model.shared->collision) return;
    const auto* transform = model.data ? &model.data->m_modelToWorld : nullptr;
    CollisionDataAddFacets(*model.shared->collision, facets, transform, clip);
}

bool CollisionDataVectorIntersect(const CCollisionData& collision, const NTempest::C3Vector& start,
                                  const NTempest::C3Vector& delta, float& distance,
                                  std::uint32_t* surfaceIndex)
{
    bool hit = false;
    float nearest = std::numeric_limits<float>::max();
    for (std::size_t i = 0; i + 2U < collision.indices.size(); i += 3U) {
        NTempest::CFacet facet;
        facet.Set(collision.vertices[collision.indices[i]], collision.vertices[collision.indices[i + 1U]],
                  collision.vertices[collision.indices[i + 2U]]);
        float t = 0.0F;
        if (NTempest::Intersect(facet, start, delta, t) && t >= 0.0F && t <= 1.0F && t < nearest) {
            nearest = t;
            if (surfaceIndex) *surfaceIndex = static_cast<std::uint32_t>(i / 3U);
            hit = true;
        }
    }
    if (hit) distance = nearest;
    return hit;
}

bool ModelCollisionVectorIntersect(const CModel& model, const NTempest::C3Vector& start,
                                   const NTempest::C3Vector& delta, ModelIntersectResult& result)
{
    if (!model.shared || !model.shared->collision) return false;
    float distance = 0.0F;
    std::uint32_t surface = 0;
    if (!CollisionDataVectorIntersect(*model.shared->collision, start, delta, distance, &surface)) return false;
    result.hit = true;
    result.distance = distance;
    result.index = surface;
    result.point = start + delta * distance;
    if (surface < model.shared->collision->surfaceNormals.size())
        result.normal = model.shared->collision->surfaceNormals[surface];
    return true;
}

bool ModelGetCollisionExtents(const CModel& model, NTempest::CAaBox& extents)
{
    if (!model.shared || !model.shared->collision) return false;
    extents = model.shared->collision->extents;
    return true;
}

void ModelShowCollision(CModel& model, bool show)
{
    if (!model.data) return;
    if (show) model.data->m_flags |= 0x1000U; else model.data->m_flags &= ~0x1000U;
}
void ModelShowCollisionAaBox(CModel& model, bool show)
{
    if (!model.data) return;
    if (show) model.data->m_flags |= 0x2000U; else model.data->m_flags &= ~0x2000U;
}
void ModelShowModel(CModel& model, bool show)
{
    if (!model.data) return;
    if (show) model.data->m_flags &= ~0x1U; else model.data->m_flags |= 0x1U;
}
