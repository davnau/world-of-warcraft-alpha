#include "ENGINE/Source/Gx/cgxdeviced3d/cgxd3dtexture.h"

#include <algorithm>

namespace {

D3DFORMAT D3dFormat(CGxTex::Format format)
{
    switch (format) {
    case CGxTex::Rgb565: return D3DFMT_R5G6B5;
    case CGxTex::Dxt1: return D3DFMT_DXT1;
    case CGxTex::Dxt3: return D3DFMT_DXT3;
    case CGxTex::Dxt5: return D3DFMT_DXT5;
    case CGxTex::Argb8888:
    case CGxTex::Unknown:
    default: return D3DFMT_A8R8G8B8;
    }
}

} // namespace

void CGxDeviceD3d::ITexForceRecreation(bool freeTextures)
{
    for (CGxTex* texture : m_textures) {
        if (texture == nullptr || texture->d3dTexture == nullptr) {
            continue;
        }
        if (freeTextures || (texture->flags & 0x80U) != 0) {
            texture->d3dTexture->Release();
            texture->d3dTexture = nullptr;
            texture->needsCreation = true;
            texture->needsUpdate = true;
        }
    }
}

bool CGxDeviceD3d::TexCreate(
    unsigned width,
    unsigned height,
    CGxTex::Format format,
    unsigned flags,
    CGxTex** texture)
{
    if (texture == nullptr) {
        return false;
    }
    if (*texture == nullptr) {
        *texture = new CGxTex();
        m_textures.push_back(*texture);
    }
    CGxTex* value = *texture;
    value->width = width;
    value->height = height;
    value->format = format;
    value->dataFormat = format;
    value->flags = flags;
    value->needsCreation = true;
    value->needsUpdate = true;
    ITexCreate(width, height, format, flags, value);
    return value->d3dTexture != nullptr;
}

void CGxDeviceD3d::TexDestroy(CGxTex* texture)
{
    if (texture == nullptr) {
        return;
    }
    if (texture->d3dTexture != nullptr) {
        texture->d3dTexture->Release();
        texture->d3dTexture = nullptr;
    }
    const auto it = std::find(m_textures.begin(), m_textures.end(), texture);
    if (it != m_textures.end()) {
        m_textures.erase(it);
    }
    delete texture;
}

void CGxDeviceD3d::ITexCreate(
    unsigned width,
    unsigned height,
    CGxTex::Format format,
    unsigned flags,
    CGxTex* texture)
{
    if (m_d3dDevice == nullptr || texture == nullptr) {
        return;
    }
    if (texture->d3dTexture != nullptr) {
        texture->d3dTexture->Release();
        texture->d3dTexture = nullptr;
    }
    IDirect3DTexture9* d3dTexture = nullptr;
    const DWORD usage = (flags & 0x80U) != 0 ? D3DUSAGE_DYNAMIC : 0;
    const D3DPOOL pool = (flags & 0x80U) != 0 ? D3DPOOL_DEFAULT : D3DPOOL_MANAGED;
    if (SUCCEEDED(m_d3dDevice->CreateTexture(width, height, 1, usage,
        D3dFormat(format), pool, &d3dTexture, nullptr))) {
        texture->d3dTexture = d3dTexture;
        texture->needsCreation = false;
    }
}

void CGxDeviceD3d::ITexUpload(
    CGxTex* texture,
    unsigned width,
    unsigned height,
    unsigned startLevel,
    unsigned endLevel)
{
    (void)width;
    (void)height;
    (void)startLevel;
    (void)endLevel;
    if (texture != nullptr) {
        texture->needsUpdate = false;
    }
}

void CGxDeviceD3d::ITexMarkAsUpdated(CGxTex* texture)
{
    if (texture == nullptr) {
        return;
    }
    if (texture->d3dTexture == nullptr) {
        ITexCreate(texture->width, texture->height, texture->format, texture->flags, texture);
    }
    texture->needsUpdate = false;
}
