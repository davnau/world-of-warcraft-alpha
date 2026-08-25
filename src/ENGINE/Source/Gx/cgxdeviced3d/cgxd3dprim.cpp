#include "ENGINE/Source/Gx/cgxdeviced3d/cgxd3dprim.h"

#include <algorithm>
#include <cstring>

CGxBufD3d::~CGxBufD3d()
{
    Release();
}

bool CGxBufD3d::Create(void* rawDevice, unsigned vertexBytes, unsigned indexBytes)
{
    IDirect3DDevice9* device = static_cast<IDirect3DDevice9*>(rawDevice);
    vertexData.resize(vertexBytes);
    indexData.resize(indexBytes);
    if (device == nullptr) {
        return true;
    }

    IDirect3DVertexBuffer9* vertices = nullptr;
    if (vertexBytes != 0 && FAILED(device->CreateVertexBuffer(
        vertexBytes, D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vertices, nullptr))) {
        return false;
    }
    IDirect3DIndexBuffer9* indices = nullptr;
    if (indexBytes != 0 && FAILED(device->CreateIndexBuffer(
        indexBytes, D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, D3DPOOL_MANAGED, &indices, nullptr))) {
        if (vertices != nullptr) {
            vertices->Release();
        }
        return false;
    }
    vertexBuffer = vertices;
    indexBuffer = indices;
    return true;
}

void CGxBufD3d::Release()
{
    if (vertexBuffer != nullptr) {
        static_cast<IDirect3DVertexBuffer9*>(vertexBuffer)->Release();
        vertexBuffer = nullptr;
    }
    if (indexBuffer != nullptr) {
        static_cast<IDirect3DIndexBuffer9*>(indexBuffer)->Release();
        indexBuffer = nullptr;
    }
}

bool CGxBufD3d::LockVB(void** data, unsigned bytes, unsigned offset)
{
    if (data == nullptr) {
        return false;
    }
    if (vertexBuffer != nullptr) {
        if (FAILED(static_cast<IDirect3DVertexBuffer9*>(vertexBuffer)->Lock(
            offset, bytes, data, D3DLOCK_DISCARD))) {
            return false;
        }
    } else {
        if (offset + bytes > vertexData.size()) {
            vertexData.resize(offset + bytes);
        }
        *data = vertexData.data() + offset;
    }
    vertexLocked = true;
    return true;
}

bool CGxBufD3d::LockIB(void** data, unsigned bytes, unsigned offset)
{
    if (data == nullptr) {
        return false;
    }
    if (indexBuffer != nullptr) {
        if (FAILED(static_cast<IDirect3DIndexBuffer9*>(indexBuffer)->Lock(
            offset, bytes, data, D3DLOCK_DISCARD))) {
            return false;
        }
    } else {
        if (offset + bytes > indexData.size()) {
            indexData.resize(offset + bytes);
        }
        *data = indexData.data() + offset;
    }
    indexLocked = true;
    return true;
}

void CGxBufD3d::UnlockVB()
{
    if (vertexLocked && vertexBuffer != nullptr) {
        static_cast<IDirect3DVertexBuffer9*>(vertexBuffer)->Unlock();
    }
    vertexLocked = false;
}

void CGxBufD3d::UnlockIB()
{
    if (indexLocked && indexBuffer != nullptr) {
        static_cast<IDirect3DIndexBuffer9*>(indexBuffer)->Unlock();
    }
    indexLocked = false;
}

bool CGxBufD3d::VBLValid() const { return vertexBuffer != nullptr || !vertexData.empty(); }
bool CGxBufD3d::IBValid() const { return indexBuffer != nullptr || !indexData.empty(); }
void CGxBufD3d::SetVBL(void* buffer) { vertexBuffer = buffer; }
void CGxBufD3d::SetVB(void* buffer) { vertexBuffer = buffer; }
void CGxBufD3d::SetIB(void* buffer) { indexBuffer = buffer; }
void CGxBufD3d::UnsetVB() { vertexBuffer = nullptr; }
void CGxBufD3d::UnsetIB() { indexBuffer = nullptr; }

void CGxDeviceD3d::BufReserve(unsigned frequency, unsigned format, unsigned vertexCount, unsigned indexCount)
{
    (void)frequency;
    (void)format;
    (void)vertexCount;
    (void)indexCount;
}

CGxBufD3d* CGxDeviceD3d::BufCreate(unsigned vertexBytes, unsigned indexBytes, unsigned vertexStride)
{
    auto* buffer = new CGxBufD3d();
    buffer->vertexStride = vertexStride;
    if (!buffer->Create(m_d3dDevice, vertexBytes, indexBytes)) {
        delete buffer;
        return nullptr;
    }
    m_buffers.push_back(buffer);
    return buffer;
}

bool CGxDeviceD3d::BufLock(
    CGxBufD3d* buffer,
    bool indexBuffer,
    void** data,
    unsigned bytes,
    unsigned offset)
{
    return buffer != nullptr && (indexBuffer
        ? buffer->LockIB(data, bytes, offset)
        : buffer->LockVB(data, bytes, offset));
}

void CGxDeviceD3d::BufUnlock(CGxBufD3d* buffer, bool indexBuffer)
{
    if (buffer == nullptr) {
        return;
    }
    indexBuffer ? buffer->UnlockIB() : buffer->UnlockVB();
}

void CGxDeviceD3d::BufRender(CGxBufD3d* buffer, unsigned primitiveCount)
{
    if (m_d3dDevice == nullptr || buffer == nullptr) {
        return;
    }
    if (buffer->vertexBuffer != nullptr) {
        m_d3dDevice->SetStreamSource(0, static_cast<IDirect3DVertexBuffer9*>(buffer->vertexBuffer),
            0, buffer->vertexStride);
    }
    if (buffer->indexBuffer != nullptr) {
        m_d3dDevice->SetIndices(static_cast<IDirect3DIndexBuffer9*>(buffer->indexBuffer));
    }
    if (buffer->indexBuffer != nullptr) {
        m_d3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0,
            buffer->vertexCount, 0, primitiveCount);
    } else {
        m_d3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, primitiveCount);
    }
}

void CGxDeviceD3d::BufDestroy(CGxBufD3d* buffer)
{
    if (buffer == nullptr) {
        return;
    }
    const auto it = std::find(m_buffers.begin(), m_buffers.end(), buffer);
    if (it != m_buffers.end()) {
        m_buffers.erase(it);
    }
    delete buffer;
}

void CGxDeviceD3d::IPrimSetupPos(const float* positions, unsigned count, unsigned stride)
{
    PrimLockAndProcessVertexPtrs(positions, count, stride);
}

void CGxDeviceD3d::PrimLockAndProcessVertexPtrs(const void* vertices, unsigned count, unsigned stride)
{
    if (vertices == nullptr || count == 0 || stride == 0) {
        m_primVertices.clear();
        return;
    }
    m_primVertexStride = stride;
    m_primVertices.resize((static_cast<std::size_t>(count) * stride) / sizeof(float));
    std::memcpy(m_primVertices.data(), vertices, static_cast<std::size_t>(count) * stride);
}

void CGxDeviceD3d::IPrimProcessVertexPtrs(const void* vertices, unsigned count, unsigned stride)
{
    PrimLockAndProcessVertexPtrs(vertices, count, stride);
}

void CGxDeviceD3d::IPrimProcessIndexPtrs(const unsigned short* indices, unsigned count)
{
    if (indices == nullptr) {
        m_primIndices.clear();
        return;
    }
    m_primIndices.assign(indices, indices + count);
}

void CGxDeviceD3d::PrimLockIndexPtr(EGxPrim primitive, unsigned count, const unsigned short* indices)
{
    CGxDevice::PrimLockIndexPtr(primitive, count, indices);
    IPrimProcessIndexPtrs(indices, count);
}

void CGxDeviceD3d::PrimDrawElements()
{
    if (m_d3dDevice == nullptr || m_primVertices.empty() || m_primIndices.empty()) {
        return;
    }
    m_d3dDevice->SetFVF(D3DFVF_XYZ);
    const unsigned stride = m_primVertexStride == 0 ? sizeof(float) * 3 : m_primVertexStride;
    m_d3dDevice->DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0,
        static_cast<UINT>(m_primVertices.size() * sizeof(float) / stride),
        static_cast<UINT>(m_primIndices.size() / 3), m_primIndices.data(),
        D3DFMT_INDEX16, m_primVertices.data(), stride);
}

void CGxDeviceD3d::PrimUnlockIndexPtr() { m_primIndices.clear(); CGxDevice::PrimUnlockIndexPtr(); }
void CGxDeviceD3d::PrimUnlockVertexPtrs() { m_primVertices.clear(); CGxDevice::PrimUnlockVertexPtrs(); }
