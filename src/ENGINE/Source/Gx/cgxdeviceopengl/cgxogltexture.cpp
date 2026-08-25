#include "ENGINE/Source/Gx/cgxdeviceopengl/cgxogltexture.h"

#include "ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.h"

#include <algorithm>

namespace {

constexpr GLenum kTexture2D = 0x0DE1;
constexpr GLenum kBgra = 0x80E1;
constexpr GLenum kClampToEdge = 0x812F;
constexpr GLenum kGenerateMipmap = 0x8191;
constexpr GLenum kTextureMaxAnisotropy = 0x84FE;

GLint InternalFormat(CGxTex::Format format)
{
    switch (format) {
    case CGxTex::Rgb565: return 0x8D62;
    case CGxTex::Dxt1: return 0x83F1;
    case CGxTex::Dxt3: return 0x83F2;
    case CGxTex::Dxt5: return 0x83F3;
    case CGxTex::Argb8888:
    case CGxTex::Unknown:
    default: return GL_RGBA;
    }
}

GLint FilterMin(std::uint32_t flags)
{
    switch (flags & 7U) {
    case 0: return GL_NEAREST;
    case 1: return GL_LINEAR;
    case 2: return GL_NEAREST_MIPMAP_LINEAR;
    default: return GL_LINEAR_MIPMAP_LINEAR;
    }
}

} // namespace

void CGxDeviceOpenGl::BindTexture(CGxTex* texture, unsigned textureUnit)
{
    if (textureUnit == ~0U) {
        textureUnit = 0;
    }
    if (textureUnit >= 4) {
        return;
    }
    DsSet(1, textureUnit);
    glBindTexture(kTexture2D, texture != nullptr ? texture->glTexture : 0);
}

bool CGxDeviceOpenGl::TexCreate(
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
    if (value->glTexture == 0) {
        glGenTextures(1, &value->glTexture);
    }
    BindTexture(value, 0);
    ITexSetFlags(value);
    glTexImage2D(kTexture2D, 0, InternalFormat(format), static_cast<GLsizei>(width),
        static_cast<GLsizei>(height), 0, kBgra, GL_UNSIGNED_BYTE, nullptr);
    value->needsCreation = false;
    value->needsUpdate = false;
    return true;
}

void CGxDeviceOpenGl::TexDestroy(CGxTex* texture)
{
    if (texture == nullptr) {
        return;
    }
    if (texture->glTexture != 0) {
        glDeleteTextures(1, &texture->glTexture);
        texture->glTexture = 0;
    }
    const auto it = std::find(m_textures.begin(), m_textures.end(), texture);
    if (it != m_textures.end()) {
        m_textures.erase(it);
    }
    delete texture;
}

void CGxDeviceOpenGl::ITexSetFlags(CGxTex* texture)
{
    if (texture == nullptr) {
        return;
    }
    BindTexture(texture, 0);
    glTexParameteri(kTexture2D, GL_TEXTURE_MAG_FILTER,
        (texture->flags & 7U) == 0 ? GL_NEAREST : GL_LINEAR);
    GetError("TexFlags-mag");
    glTexParameteri(kTexture2D, GL_TEXTURE_MIN_FILTER, FilterMin(texture->flags));
    GetError("TexFlags-min");
    glTexParameteri(kTexture2D, GL_TEXTURE_WRAP_S,
        (texture->flags & 8U) != 0 ? GL_REPEAT : kClampToEdge);
    GetError("TexFlags-wrap-s");
    glTexParameteri(kTexture2D, GL_TEXTURE_WRAP_T,
        (texture->flags & 16U) != 0 ? GL_REPEAT : kClampToEdge);
    GetError("TexFlags-wrap-t");
    if (glSGISGenerateMipmap && (texture->flags & 0x40U) != 0) {
        glTexParameteri(kTexture2D, kGenerateMipmap, GL_TRUE);
        GetError("TexFlags-generate-mipmap");
    }
    if (glExtTextureFilterAnisotropic) {
        glTexParameteri(kTexture2D, kTextureMaxAnisotropy,
            static_cast<GLint>((texture->flags >> 8) & 0x1fU));
        GetError("TexFlags-anisotropy");
    }
    texture->needsFlagUpdate = false;
}

void CGxDeviceOpenGl::ITexDownload(
    CGxTex* texture,
    int width,
    int height,
    unsigned level,
    unsigned stride,
    const unsigned char* texels)
{
    if (texture == nullptr || texels == nullptr || width <= 0 || height <= 0) {
        return;
    }
    BindTexture(texture, 0);
    GetError("TexDownload-bind");
    glPixelStorei(GL_UNPACK_ROW_LENGTH, static_cast<GLint>(stride / 4));
    GetError("TexDownload-row-length");
    if (texture->needsCreation) {
        glTexImage2D(kTexture2D, level, InternalFormat(texture->format), width, height,
            0, kBgra, GL_UNSIGNED_BYTE, texels);
        GetError("TexDownload-image");
        texture->needsCreation = false;
    } else {
        glTexSubImage2D(kTexture2D, level, 0, 0, width, height,
            kBgra, GL_UNSIGNED_BYTE, texels);
        GetError("TexDownload-sub-image");
    }
    texture->needsUpdate = false;
}

void CGxDeviceOpenGl::ITexMarkAsUpdated(CGxTex* texture, unsigned textureUnit)
{
    if (texture == nullptr) {
        return;
    }
    if (texture->glTexture == 0) {
        glGenTextures(1, &texture->glTexture);
        texture->needsCreation = true;
    }
    BindTexture(texture, textureUnit);
    GetError("TexUpdate-bind");
    if (texture->needsFlagUpdate) {
        ITexSetFlags(texture);
    }
    if (texture->needsCreation || texture->needsUpdate) {
        unsigned stride = 0;
        const void* texels = nullptr;
        if (texture->userFunc) {
            texture->userFunc(GxTex_Lock, texture->width, texture->height,
                              texture->depth, 0, texture->userArg, &stride, &texels);
        }
        if (texels) {
            ITexDownload(texture, static_cast<int>(texture->width),
                         static_cast<int>(texture->height), 0,
                         stride ? stride : texture->width * 4U,
                         static_cast<const unsigned char*>(texels));
        } else if (texture->needsCreation) {
            glTexImage2D(kTexture2D, 0, InternalFormat(texture->format),
                         static_cast<GLsizei>(texture->width),
                         static_cast<GLsizei>(texture->height), 0,
                         kBgra, GL_UNSIGNED_BYTE, nullptr);
            GetError("TexUpdate-empty-image");
            texture->needsCreation = false;
            texture->needsUpdate = false;
        }
        if (texture->userFunc) {
            texture->userFunc(GxTex_Unlock, texture->width, texture->height,
                              texture->depth, 0, texture->userArg, nullptr, nullptr);
        }
    }
}

void CGxDeviceOpenGl::ITexForceRecreation()
{
    for (CGxTex* texture : m_textures) {
        if (texture != nullptr && texture->glTexture != 0) {
            glDeleteTextures(1, &texture->glTexture);
            texture->glTexture = 0;
            texture->needsCreation = true;
            texture->needsFlagUpdate = true;
        }
    }
}
