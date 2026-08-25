#include "ENGINE/Source/Gx/cgxdeviced3d11/cgxdeviced3d11.h"

#include <algorithm>
#include <cstring>

namespace {

template <class T>
T ReadMember(const T* values, unsigned stride, unsigned index, const T& fallback)
{
    if (values == nullptr) return fallback;
    if (stride == 0) return *values;
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(values);
    return *reinterpret_cast<const T*>(bytes + static_cast<std::size_t>(stride) * index);
}

unsigned NextCapacity(unsigned current, unsigned needed)
{
    unsigned capacity = std::max(256U, current);
    while (capacity < needed && capacity <= 0x40000000U) capacity *= 2U;
    return std::max(capacity, needed);
}

} // namespace

void CGxDeviceD3d11::PrimLockAndProcessVertexPtrs(
    unsigned vertexCount,
    const NTempest::C3Vector* positions,
    unsigned positionStride,
    const NTempest::C3Vector* normals,
    unsigned normalStride,
    const NTempest::CImVector* colors,
    unsigned colorStride,
    const std::uint8_t* bones,
    unsigned boneStride,
    const NTempest::C2Vector* tex0,
    unsigned tex0Stride,
    const NTempest::C2Vector* tex1,
    unsigned tex1Stride)
{
    CGxDevice::PrimLockAndProcessVertexPtrs(vertexCount, positions, positionStride,
        normals, normalStride, colors, colorStride, bones, boneStride,
        tex0, tex0Stride, tex1, tex1Stride);
    m_vertices.clear();
    if (positions == nullptr || vertexCount == 0) return;

    const NTempest::C3Vector defaultPosition{};
    const NTempest::C3Vector defaultNormal{0.0F, 1.0F, 0.0F};
    const NTempest::CImVector defaultColor{0xFFFFFFFFU};
    const NTempest::C2Vector defaultTexCoord{};
    m_vertices.resize(vertexCount);
    for (unsigned index = 0; index < vertexCount; ++index) {
        NTempest::C3Vector position = ReadMember(positions, positionStride,
            index, defaultPosition);
        NTempest::C3Vector normal = ReadMember(normals, normalStride,
            index, defaultNormal);
        const NTempest::CImVector color = ReadMember(colors, colorStride,
            index, defaultColor);
        const NTempest::C2Vector coordinate0 = ReadMember(tex0, tex0Stride,
            index, defaultTexCoord);
        const NTempest::C2Vector coordinate1 = ReadMember(tex1, tex1Stride,
            index, defaultTexCoord);

        if (m_vertexShader == GxVS_Skin && m_bones != nullptr && m_boneCount != 0) {
            unsigned boneIndex = bones != nullptr
                ? ReadMember<std::uint8_t>(bones, boneStride, index, 0U) : 0U;
            boneIndex = std::min(boneIndex, m_boneCount - 1U);
            const NTempest::C34Matrix& matrix = m_bones[boneIndex];
            const NTempest::C3Vector transformedPosition{
                matrix.a0 * position.x + matrix.b0 * position.y + matrix.c0 * position.z + matrix.d0,
                matrix.a1 * position.x + matrix.b1 * position.y + matrix.c1 * position.z + matrix.d1,
                matrix.a2 * position.x + matrix.b2 * position.y + matrix.c2 * position.z + matrix.d2,
            };
            const NTempest::C3Vector transformedNormal{
                matrix.a0 * normal.x + matrix.b0 * normal.y + matrix.c0 * normal.z,
                matrix.a1 * normal.x + matrix.b1 * normal.y + matrix.c1 * normal.z,
                matrix.a2 * normal.x + matrix.b2 * normal.y + matrix.c2 * normal.z,
            };
            position = transformedPosition;
            normal = transformedNormal;
        }

        Vertex& vertex = m_vertices[index];
        vertex.position[0] = position.x;
        vertex.position[1] = position.y;
        vertex.position[2] = position.z;
        vertex.normal[0] = normal.x;
        vertex.normal[1] = normal.y;
        vertex.normal[2] = normal.z;
        vertex.color[0] = color.r / 255.0F;
        vertex.color[1] = color.g / 255.0F;
        vertex.color[2] = color.b / 255.0F;
        vertex.color[3] = color.a / 255.0F;
        vertex.tex0[0] = coordinate0.x;
        vertex.tex0[1] = coordinate0.y;
        vertex.tex1[0] = coordinate1.x;
        vertex.tex1[1] = coordinate1.y;
    }
}

std::vector<std::uint16_t> CGxDeviceD3d11::ConvertIndices(
    EGxPrim primitive, unsigned count, const std::uint16_t* indices) const
{
    if (primitive == GxPrim_TriangleFan) {
        std::vector<std::uint16_t> converted;
        if (count < 3) return converted;
        converted.reserve(static_cast<std::size_t>(count - 2U) * 3U);
        const auto at = [indices](unsigned index) {
            return indices != nullptr ? indices[index] : static_cast<std::uint16_t>(index);
        };
        for (unsigned index = 1; index + 1 < count; ++index) {
            converted.push_back(at(0));
            converted.push_back(at(index));
            converted.push_back(at(index + 1));
        }
        return converted;
    }
    if (indices != nullptr) return {indices, indices + count};
    std::vector<std::uint16_t> generated(count);
    for (unsigned index = 0; index < count; ++index)
        generated[index] = static_cast<std::uint16_t>(index);
    return generated;
}

void CGxDeviceD3d11::PrimLockIndexPtr(
    EGxPrim primitive, unsigned count, const std::uint16_t* indices)
{
    CGxDevice::PrimLockIndexPtr(primitive, count, indices);
    m_indices = ConvertIndices(primitive, count, indices);
    m_topology = PrimitiveTopology(primitive);
}

D3D11_PRIMITIVE_TOPOLOGY CGxDeviceD3d11::PrimitiveTopology(EGxPrim primitive) const
{
    switch (primitive) {
    case GxPrim_Points: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
    case GxPrim_Lines: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case GxPrim_LineStrip: return D3D11_PRIMITIVE_TOPOLOGY_LINESTRIP;
    case GxPrim_TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case GxPrim_TriangleFan:
    case GxPrim_Triangles:
    default: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    }
}

bool CGxDeviceD3d11::UploadVertices(const std::vector<Vertex>& vertices)
{
    if (vertices.empty() || m_device == nullptr || m_context == nullptr) return false;
    const unsigned bytes = static_cast<unsigned>(vertices.size() * sizeof(Vertex));
    if (m_dynamicVertexBuffer == nullptr || bytes > m_vertexCapacity) {
        m_vertexCapacity = NextCapacity(m_vertexCapacity, bytes);
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = m_vertexCapacity;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_dynamicVertexBuffer.Reset();
        if (FAILED(m_device->CreateBuffer(&desc, nullptr, &m_dynamicVertexBuffer))) return false;
    }
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_context->Map(m_dynamicVertexBuffer.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;
    std::memcpy(mapped.pData, vertices.data(), bytes);
    m_context->Unmap(m_dynamicVertexBuffer.Get(), 0);
    return true;
}

bool CGxDeviceD3d11::UploadIndices(const std::vector<std::uint16_t>& indices)
{
    if (indices.empty() || m_device == nullptr || m_context == nullptr) return false;
    const unsigned bytes = static_cast<unsigned>(indices.size() * sizeof(std::uint16_t));
    if (m_dynamicIndexBuffer == nullptr || bytes > m_indexCapacity) {
        m_indexCapacity = NextCapacity(m_indexCapacity, bytes);
        D3D11_BUFFER_DESC desc{};
        desc.ByteWidth = m_indexCapacity;
        desc.Usage = D3D11_USAGE_DYNAMIC;
        desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        m_dynamicIndexBuffer.Reset();
        if (FAILED(m_device->CreateBuffer(&desc, nullptr, &m_dynamicIndexBuffer))) return false;
    }
    D3D11_MAPPED_SUBRESOURCE mapped{};
    if (FAILED(m_context->Map(m_dynamicIndexBuffer.Get(), 0,
        D3D11_MAP_WRITE_DISCARD, 0, &mapped))) return false;
    std::memcpy(mapped.pData, indices.data(), bytes);
    m_context->Unmap(m_dynamicIndexBuffer.Get(), 0);
    return true;
}

void CGxDeviceD3d11::Draw(
    const std::vector<Vertex>& vertices,
    const std::vector<std::uint16_t>& indices,
    EGxPrim primitive)
{
    if (!UploadVertices(vertices) || !UploadIndices(indices)) return;
    ApplyPipeline();
    const UINT stride = sizeof(Vertex);
    const UINT offset = 0;
    ID3D11Buffer* vertexBuffer = m_dynamicVertexBuffer.Get();
    m_context->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
    m_context->IASetIndexBuffer(m_dynamicIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
    m_context->IASetPrimitiveTopology(PrimitiveTopology(primitive));
    m_context->DrawIndexed(static_cast<UINT>(indices.size()), 0, 0);
}

void CGxDeviceD3d11::PrimDrawElements()
{
    CGxDevice::PrimDrawElements();
    Draw(m_vertices, m_indices, m_primType);
}

void CGxDeviceD3d11::PrimUnlockIndexPtr()
{
    m_indices.clear();
    CGxDevice::PrimUnlockIndexPtr();
}

void CGxDeviceD3d11::PrimUnlockVertexPtrs()
{
    m_vertices.clear();
    CGxDevice::PrimUnlockVertexPtrs();
}

void CGxDeviceD3d11::PrimPointSize(float size)
{
    CGxDevice::PrimPointSize(size);
}

void CGxDeviceD3d11::PrimLineWidth(float width)
{
    CGxDevice::PrimLineWidth(width);
}

void CGxDeviceD3d11::BufRender(const CGxBatch* batches, unsigned count)
{
    CGxBuf* buffer = m_bufLocked;
    CGxDevice::BufRender(batches, count);
    if (buffer == nullptr || batches == nullptr || buffer->vertexStride == 0
        || buffer->vertexData.empty()) return;

    const unsigned availableVertices = static_cast<unsigned>(
        buffer->vertexData.size() / buffer->vertexStride);
    const unsigned vertexCount = std::min(buffer->vertexCount, availableVertices);
    std::vector<Vertex> vertices(vertexCount);
    const unsigned positionOffset = GxVertexMemberOffset(buffer->vertexFormat, GxVM_Position);
    const unsigned normalOffset = GxVertexMemberOffset(buffer->vertexFormat, GxVM_Normal);
    const unsigned colorOffset = GxVertexMemberOffset(buffer->vertexFormat, GxVM_Color);
    const unsigned tex0Offset = GxVertexMemberOffset(buffer->vertexFormat, GxVM_Texture0);
    const unsigned tex1Offset = GxVertexMemberOffset(buffer->vertexFormat, GxVM_Texture1);
    for (unsigned index = 0; index < vertexCount; ++index) {
        const auto* source = buffer->vertexData.data()
            + static_cast<std::size_t>(index) * buffer->vertexStride;
        Vertex& vertex = vertices[index];
        if (positionOffset != ~0U) {
            const auto* value = reinterpret_cast<const NTempest::C3Vector*>(source + positionOffset);
            vertex.position[0] = value->x; vertex.position[1] = value->y; vertex.position[2] = value->z;
        }
        if (normalOffset != ~0U) {
            const auto* value = reinterpret_cast<const NTempest::C3Vector*>(source + normalOffset);
            vertex.normal[0] = value->x; vertex.normal[1] = value->y; vertex.normal[2] = value->z;
        }
        if (colorOffset != ~0U) {
            const auto* value = reinterpret_cast<const NTempest::CImVector*>(source + colorOffset);
            vertex.color[0] = value->r / 255.0F; vertex.color[1] = value->g / 255.0F;
            vertex.color[2] = value->b / 255.0F; vertex.color[3] = value->a / 255.0F;
        }
        if (tex0Offset != ~0U) {
            const auto* value = reinterpret_cast<const NTempest::C2Vector*>(source + tex0Offset);
            vertex.tex0[0] = value->x; vertex.tex0[1] = value->y;
        }
        if (tex1Offset != ~0U) {
            const auto* value = reinterpret_cast<const NTempest::C2Vector*>(source + tex1Offset);
            vertex.tex1[0] = value->x; vertex.tex1[1] = value->y;
        }
    }

    const auto* allIndices = reinterpret_cast<const std::uint16_t*>(buffer->indexData.data());
    const unsigned availableIndices = static_cast<unsigned>(
        buffer->indexData.size() / sizeof(std::uint16_t));
    for (unsigned batchIndex = 0; batchIndex < count; ++batchIndex) {
        const CGxBatch& batch = batches[batchIndex];
        if (batch.m_start >= availableIndices) continue;
        const unsigned indexCount = std::min(batch.m_count, availableIndices - batch.m_start);
        auto indices = ConvertIndices(batch.m_primType, indexCount,
            allIndices + batch.m_start);
        Draw(vertices, indices, batch.m_primType);
    }
}
