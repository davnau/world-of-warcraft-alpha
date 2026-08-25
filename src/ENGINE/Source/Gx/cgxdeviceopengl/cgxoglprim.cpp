#include "ENGINE/Source/Gx/cgxdeviceopengl/cgxoglprim.h"
#include "ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.h"

#include <algorithm>
#include <cstring>

namespace {
template<class T>
T ReadVertexMember(const T* values, unsigned stride, unsigned index)
{
    if (!values) return T{};
    if (stride == 0) return *values;
    const auto* bytes = reinterpret_cast<const std::uint8_t*>(values);
    return *reinterpret_cast<const T*>(bytes + static_cast<std::size_t>(index) * stride);
}
}

bool CGxBufOgl::LockVB(void** data, unsigned bytes, unsigned offset)
{
    if (data == nullptr) {
        return false;
    }
    if (offset + bytes > vertexData.size()) {
        vertexData.resize(offset + bytes);
    }
    *data = vertexData.data() + offset;
    vertexLocked = true;
    return true;
}

bool CGxBufOgl::LockIB(void** data, unsigned bytes, unsigned offset)
{
    if (data == nullptr) {
        return false;
    }
    if (offset + bytes > indexData.size()) {
        indexData.resize(offset + bytes);
    }
    *data = indexData.data() + offset;
    indexLocked = true;
    return true;
}

void CGxBufOgl::UnlockVB() { vertexLocked = false; }
void CGxBufOgl::UnlockIB() { indexLocked = false; }

CGxBufOgl* CGxDeviceOpenGl::BufCreate(unsigned vertexBytes, unsigned indexBytes, unsigned vertexStride)
{
    auto* buffer = new CGxBufOgl();
    buffer->vertexStride = vertexStride;
    buffer->vertexData.resize(vertexBytes);
    buffer->indexData.resize(indexBytes);
    m_buffers.push_back(buffer);
    return buffer;
}

bool CGxDeviceOpenGl::BufLock(
    CGxBufOgl* buffer,
    bool indexBuffer,
    void** data,
    unsigned bytes,
    unsigned offset)
{
    return buffer != nullptr && (indexBuffer
        ? buffer->LockIB(data, bytes, offset)
        : buffer->LockVB(data, bytes, offset));
}

void CGxDeviceOpenGl::BufUnlock(CGxBufOgl* buffer, bool indexBuffer)
{
    if (buffer == nullptr) {
        return;
    }
    indexBuffer ? buffer->UnlockIB() : buffer->UnlockVB();
}

void CGxDeviceOpenGl::BufRender(CGxBufOgl* buffer, unsigned primitiveCount)
{
    if (buffer == nullptr || buffer->vertexData.empty()) {
        return;
    }
    glEnableClientState(GL_VERTEX_ARRAY);
    glVertexPointer(3, GL_FLOAT, buffer->vertexStride, buffer->vertexData.data());
    if (!buffer->indexData.empty()) {
        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(primitiveCount * 3),
            GL_UNSIGNED_SHORT, buffer->indexData.data());
    } else {
        glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(primitiveCount * 3));
    }
}

void CGxDeviceOpenGl::BufDestroy(CGxBufOgl* buffer)
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

void CGxDeviceOpenGl::IPrimSetupNormal(unsigned stride, const void* normals)
{
    if (normals != nullptr) {
        DsSet(Ds_NormalArray, 1);
        glNormalPointer(GL_FLOAT, stride, normals);
    } else {
        DsSet(Ds_NormalArray, 0);
    }
}

void CGxDeviceOpenGl::IPrimSetupColor(
    unsigned stride,
    const NTempest::CImVector* colors,
    unsigned count,
    bool convert)
{
    if (colors == nullptr) {
        DsSet(Ds_ColorArray, 0);
        IStateSetColorSource(Cs_Material);
        return;
    }

    if (stride == 0) {
        DsSet(Ds_ColorArray, 0);
        IStateSetColorSource(Cs_Constant);
        IStateSetColorSourceColor(Cs_Constant, *colors);
        return;
    }

    const NTempest::CImVector* glColors = colors;
    unsigned glStride = stride;
    if (convert) {
        m_primColors.resize(count);
        for (unsigned index = 0; index < count; ++index) {
            const auto source = ReadVertexMember(colors, stride, index);
            auto& destination = m_primColors[index];
            destination.b = source.r;
            destination.g = source.g;
            destination.r = source.b;
            destination.a = source.a;
        }
        glColors = m_primColors.data();
        glStride = sizeof(NTempest::CImVector);
    }

    glColorPointer(4, GL_UNSIGNED_BYTE, static_cast<GLsizei>(glStride), glColors);
    DsSet(Ds_ColorArray, 1);
    IStateSetColorSource(Cs_Array);
}

void CGxDeviceOpenGl::IPrimSetupTexCoord(
    unsigned textureUnit,
    unsigned stride,
    const void* coordinates)
{
    if (textureUnit >= m_caps.m_numTmus || textureUnit >= 4) {
        return;
    }
    DsSet(Ds_ActiveTexture, textureUnit);
    if (coordinates != nullptr) {
        DsSet(Ds_TextureArray0 + textureUnit, 1);
        glTexCoordPointer(2, GL_FLOAT, static_cast<GLsizei>(stride), coordinates);
    } else {
        DsSet(Ds_TextureArray0 + textureUnit, 0);
    }
}

void CGxDeviceOpenGl::PrimLockAndProcessVertexPtrs(
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
    CGxDevice::PrimLockAndProcessVertexPtrs(
        vertexCount, positions, positionStride, normals, normalStride, colors,
        colorStride, bones, boneStride, tex0, tex0Stride, tex1, tex1Stride);

    static const NTempest::C3Vector genericNormal{0.0F, 1.0F, 0.0F};
    static const NTempest::CImVector genericColor{0xFFFFFFFFU};
    static const NTempest::C2Vector genericTexCoord{0.0F, 0.0F};

    const NTempest::C3Vector* sourceNormal = normals ? normals : &genericNormal;
    const NTempest::CImVector* sourceColor = colors ? colors : &genericColor;
    const NTempest::C2Vector* sourceTex0 = tex0 ? tex0 : &genericTexCoord;
    const NTempest::C2Vector* sourceTex1 = tex1 ? tex1 : &genericTexCoord;

    if (glVertexArrayRangeNV != nullptr) {
        DsSet(Ds_NVVAR, 0);
    }

    if (m_vertexShader == GxVS_PassThru) {
        glVertexPointer(3, GL_FLOAT, static_cast<GLsizei>(positionStride), positions);
        if (normalStride != 0) {
            IPrimSetupNormal(normalStride, sourceNormal);
        } else {
            IPrimSetupNormal(0, nullptr);
            glNormal3fv(&sourceNormal->x);
        }
    } else if (m_vertexShader == GxVS_Skin) {
        m_primVertices.resize(static_cast<std::size_t>(vertexCount) * 3U);
        m_primNormals.resize(vertexCount);
        for (unsigned index = 0; index < vertexCount; ++index) {
            const auto position = ReadVertexMember(positions, positionStride, index);
            const auto normal = ReadVertexMember(sourceNormal, normalStride, index);
            const unsigned bone = bones ? ReadVertexMember(bones, boneStride, index) : 0U;
            const NTempest::C34Matrix& matrix = m_bones[bone];
            m_primVertices[index * 3U + 0U] = matrix.a0 * position.x + matrix.b0 * position.y + matrix.c0 * position.z + matrix.d0;
            m_primVertices[index * 3U + 1U] = matrix.a1 * position.x + matrix.b1 * position.y + matrix.c1 * position.z + matrix.d1;
            m_primVertices[index * 3U + 2U] = matrix.a2 * position.x + matrix.b2 * position.y + matrix.c2 * position.z + matrix.d2;
            m_primNormals[index].x = matrix.a0 * normal.x + matrix.b0 * normal.y + matrix.c0 * normal.z;
            m_primNormals[index].y = matrix.a1 * normal.x + matrix.b1 * normal.y + matrix.c1 * normal.z;
            m_primNormals[index].z = matrix.a2 * normal.x + matrix.b2 * normal.y + matrix.c2 * normal.z;
        }
        glVertexPointer(3, GL_FLOAT, sizeof(NTempest::C3Vector), m_primVertices.data());
        IPrimSetupNormal(sizeof(NTempest::C3Vector), m_primNormals.data());
    } else {
        return;
    }

    switch (m_vertexBufferFormat) {
    case GxVBF_PN:
        IPrimSetupColor(0, nullptr, vertexCount, true);
        IPrimSetupTexCoord(0, 0, nullptr);
        IPrimSetupTexCoord(1, 0, nullptr);
        break;
    case GxVBF_PNC:
    case GxVBF_PC:
        IPrimSetupColor(colorStride, sourceColor, vertexCount, true);
        IPrimSetupTexCoord(0, 0, nullptr);
        IPrimSetupTexCoord(1, 0, nullptr);
        break;
    case GxVBF_PNT0:
        IPrimSetupColor(0, nullptr, vertexCount, true);
        IPrimSetupTexCoord(0, tex0Stride, sourceTex0);
        IPrimSetupTexCoord(1, 0, nullptr);
        break;
    case GxVBF_PNCT0:
    case GxVBF_PCT0:
        IPrimSetupColor(colorStride, sourceColor, vertexCount, true);
        IPrimSetupTexCoord(0, tex0Stride, sourceTex0);
        IPrimSetupTexCoord(1, 0, nullptr);
        break;
    case GxVBF_PNT0T1:
    case GxVBF_PT0T1:
        IPrimSetupColor(0, nullptr, vertexCount, true);
        IPrimSetupTexCoord(0, tex0Stride, sourceTex0);
        IPrimSetupTexCoord(1, tex1Stride, sourceTex1);
        break;
    case GxVBF_PNCT0T1:
        IPrimSetupColor(colorStride, sourceColor, vertexCount, true);
        IPrimSetupTexCoord(0, tex0Stride, sourceTex0);
        IPrimSetupTexCoord(1, tex1Stride, sourceTex1);
        break;
    default:
        break;
    }

    LockArrays(vertexCount);
}

void CGxDeviceOpenGl::PrimLockIndexPtr(EGxPrim primitive, unsigned count, const unsigned short* indices)
{
    CGxDevice::PrimLockIndexPtr(primitive, count, indices);
    m_primIndicesPtr = indices;
}

void CGxDeviceOpenGl::PrimDrawElements()
{
    CGxDevice::PrimDrawElements();
    GetError("PrimDraw-pre");
    IStateSync();
    GetError("PrimDraw-state");
    static constexpr GLenum primitiveConversion[GxPrims_Last] = {
        GL_POINTS, GL_LINES, GL_LINE_STRIP, GL_TRIANGLES,
        GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN
    };
    glDrawElements(primitiveConversion[m_primType], static_cast<GLsizei>(m_primIndexCount),
        GL_UNSIGNED_SHORT, m_primIndicesPtr);
    GetError("PrimDraw-draw");
    if ((m_hwState.m_masterEnables & (1U << GxMasterEnable_DoubleBuffering)) == 0) {
        glFlush();
    }
}

void CGxDeviceOpenGl::PrimUnlockIndexPtr()
{
    m_primIndicesPtr = nullptr;
    CGxDevice::PrimUnlockIndexPtr();
}
void CGxDeviceOpenGl::PrimUnlockVertexPtrs()
{
    CGxDevice::PrimUnlockVertexPtrs();
    UnlockArrays();
}
void CGxDeviceOpenGl::PrimPointSize(float size) { glPointSize(size); }
void CGxDeviceOpenGl::PrimLineWidth(float width) { glLineWidth(width); }
