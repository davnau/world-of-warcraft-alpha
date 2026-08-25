#include "ENGINE/Source/Gx/cgxdeviced3d/cgxd3dxform.h"

#include <cmath>
#include <cstring>

namespace {

D3DMATRIX ToD3dMatrix(const NTempest::C44Matrix& source)
{
    D3DMATRIX result{};
    std::memcpy(result.m, source.Data(), sizeof(result.m));
    return result;
}

} // namespace

void CGxDeviceD3d::XformSetViewport(
    float minX,
    float maxX,
    float minY,
    float maxY,
    float minZ,
    float maxZ)
{
    CGxDevice::XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    if (m_d3dDevice == nullptr) {
        return;
    }

    const NTempest::CRect* window = DeviceCurWindow();
    D3DVIEWPORT9 viewport{};
    viewport.X = static_cast<DWORD>(minX * window->r);
    viewport.Y = static_cast<DWORD>((1.0F - maxY) * window->b);
    viewport.Width = static_cast<DWORD>(maxX * window->r) - viewport.X;
    viewport.Height = static_cast<DWORD>((1.0F - minY) * window->b) - viewport.Y;
    viewport.MinZ = minZ;
    viewport.MaxZ = maxZ;
    m_d3dDevice->SetViewport(&viewport);
}

void CGxDeviceD3d::XformSetProjection(const NTempest::C44Matrix& matrix)
{
    CGxDevice::XformSetProjection(matrix);
    if (m_d3dDevice == nullptr) {
        return;
    }

    NTempest::C44Matrix projection = matrix;
    if (std::fabs(projection.d3 - 1.0F) >= 0.00000023841858F
        && std::fabs(projection.d3) >= 0.00000023841858F) {
        projection *= 1.0F / projection.d3;
    }

    if (projection.d3 == 0.0F) {
        const float nearPlane = -(projection.c3 / (projection.c2 + 1.0F));
        const float farPlane = -(projection.c3 / (projection.c2 - 1.0F));
        projection.c2 = farPlane / (farPlane - nearPlane);
        projection.c3 = farPlane * nearPlane / (nearPlane - farPlane);
    } else if (projection.c2 != 0.0F) {
        const float inverse = 1.0F / projection.c2;
        const float nearPlane = (-1.0F - projection.c3) * inverse;
        const float farPlane = (1.0F - projection.c3) * inverse;
        projection.c2 = 1.0F / (farPlane - nearPlane);
        projection.c3 = nearPlane / (nearPlane - farPlane);
    }

    if ((m_masterEnables & 0x80U) == 0 && projection.d3 != 1.0F) {
        projection.a0 *= 0.2F;
        projection.a1 *= 0.2F;
        projection.a2 *= 0.2F;
        projection.a3 *= 0.2F;
        projection.b0 *= 0.2F;
        projection.b1 *= 0.2F;
        projection.b2 *= 0.2F;
        projection.b3 *= 0.2F;
        projection.c0 *= 0.2F;
        projection.c1 *= 0.2F;
        projection.c2 *= 0.2F;
        projection.c3 *= 0.2F;
    }

    const D3DMATRIX d3dProjection = ToD3dMatrix(projection);
    m_d3dDevice->SetTransform(D3DTS_PROJECTION, &d3dProjection);
}

void CGxDeviceD3d::XformSetView(const NTempest::C44Matrix& matrix)
{
    CGxDevice::XformSetView(matrix);
    if (m_d3dDevice == nullptr) {
        return;
    }
    const D3DMATRIX d3dView = ToD3dMatrix(matrix);
    m_d3dDevice->SetTransform(D3DTS_VIEW, &d3dView);
}

void CGxDeviceD3d::IXformSetWorld()
{
    if (m_d3dDevice == nullptr) {
        return;
    }
    if (!m_worldMatrixIdentity || m_worldMatrixDirty) {
        const D3DMATRIX d3dWorld = ToD3dMatrix(m_worldMatrix);
        m_d3dDevice->SetTransform(D3DTS_WORLD, &d3dWorld);
    }
    m_worldMatrixIdentity = true;
    m_worldMatrixDirty = false;
}

void CGxDeviceD3d::IXformSetTex(unsigned textureUnit, unsigned transformMode)
{
    if (m_d3dDevice == nullptr || textureUnit >= 4) {
        return;
    }

    NTempest::C44Matrix texture = m_textureMatrices[textureUnit];
    switch (transformMode) {
    case 0:
        if (m_textureGenerationEnabled[textureUnit]) {
            texture = m_textureGenerationMatrices[textureUnit];
        }
        break;
    case 1:
    case 2:
        if (m_textureGenerationEnabled[textureUnit]) {
            texture = m_textureGenerationMatrices[textureUnit] * texture;
        }
        break;
    default:
        return;
    }

    const D3DMATRIX d3dTexture = ToD3dMatrix(texture);
    const auto transform = static_cast<D3DTRANSFORMSTATETYPE>(D3DTS_TEXTURE0 + textureUnit);
    m_d3dDevice->SetTransform(transform, &d3dTexture);
}
