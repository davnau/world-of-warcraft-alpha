#include "ENGINE/Source/Gx/cgxdeviceopengl/cgxdeviceopengl.h"

#include "ENGINE/Source/Gx/cgxdeviceopengl/glextsupport.h"

#include <algorithm>
#include <cstring>

CGxDeviceOpenGl* CGxDevice::NewOpenGl()
{
    return new CGxDeviceOpenGl();
}

void CGxMemBuffer_VAR::Lock(void** destination, unsigned bytes, unsigned base)
{
    if (destination == nullptr || memory == nullptr || base + bytes > count) {
        if (destination != nullptr) {
            *destination = nullptr;
        }
        return;
    }
    *destination = static_cast<std::uint8_t*>(memory) + base;
    locked = true;
}

void CGxMemBuffer_VAR::Unlock()
{
    locked = false;
}

void CGxMemBuffer_VAR::Fence()
{
    if (glFlushVertexArrayRangeNV != nullptr) {
        glFlushVertexArrayRangeNV();
    }
}

void CGxDeviceOpenGl::DeviceReadPixels(
    const NTempest::CiRect& rect,
    std::vector<NTempest::CImVector>& pixels)
{
    NTempest::CiRect clampedRect = rect;
    ClampRectToWindow(&clampedRect);
    const int width = (std::max)(0, clampedRect.Width());
    const int height = (std::max)(0, clampedRect.Height());
    pixels.resize(static_cast<std::size_t>(width) * height);
    if (width == 0 || height == 0) {
        return;
    }
    const GLint bottom = static_cast<GLint>(DeviceCurWindow()->b) - clampedRect.maxY;
    glReadPixels(clampedRect.minX, bottom, width, height, 0x80E1, GL_UNSIGNED_BYTE, pixels.data());
    for (int top = 0; top < height / 2; ++top) {
        const int opposite = height - top - 1;
        for (int x = 0; x < width; ++x) {
            std::swap(pixels[static_cast<std::size_t>(top) * width + x],
                pixels[static_cast<std::size_t>(opposite) * width + x]);
        }
    }
}

void CGxDeviceOpenGl::DeviceReadDepths(
    const NTempest::CiRect& rect,
    std::vector<float>& depths)
{
    const int width = (std::max)(0, rect.Width());
    const int height = (std::max)(0, rect.Height());
    depths.resize(static_cast<std::size_t>(width) * height);
    if (width == 0 || height == 0) {
        return;
    }
    const GLint bottom = static_cast<GLint>(m_currentWindow.b) - rect.maxY;
    glReadPixels(rect.minX, bottom, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depths.data());
    for (int top = 0; top < height / 2; ++top) {
        const int opposite = height - top - 1;
        for (int x = 0; x < width; ++x) {
            std::swap(depths[static_cast<std::size_t>(top) * width + x],
                depths[static_cast<std::size_t>(opposite) * width + x]);
        }
    }
}

void CGxDeviceOpenGl::DeviceOverride(EGxOverride value, unsigned flags)
{
    CGxDevice::DeviceOverride(value, flags);
    if (value == GxOverride_PixelShader && flags >= CGxPixelShader::Target_nvrc
        && flags <= CGxPixelShader::Target_arbfp1) {
        m_pixelShaderTarget = static_cast<int>(flags);
        m_caps.m_pixelShaderTarget = static_cast<int>(flags);
    }
}

void CGxDeviceOpenGl::LockArrays(unsigned count)
{
    if (glExtCVA && glLockArraysEXT != nullptr) {
        glLockArraysEXT(0, static_cast<GLsizei>(count));
        m_lockedArrays = count;
    }
}

void CGxDeviceOpenGl::UnlockArrays()
{
    if (glExtCVA && m_lockedArrays != 0 && glUnlockArraysEXT != nullptr) {
        glUnlockArraysEXT();
        m_lockedArrays = 0;
    }
}

void CGxDeviceOpenGl::IAllocVAR()
{
    if (m_nvvarMem.empty()) {
        m_nvvarMem.resize(1024 * 1024);
    }
}

void CGxDeviceOpenGl::IFreeVAR()
{
    m_nvvarMem.clear();
    m_nvvarMem.shrink_to_fit();
}

void CGxDeviceOpenGl::IAllocVertexBufferVAR(unsigned frequency, unsigned bytes)
{
    (void)frequency;
    (void)bytes;
    IAllocVAR();
}

void CGxDeviceOpenGl::AllocVertexBuffer(unsigned frequency, unsigned bytes)
{
    if (frequency >= 4) {
        return;
    }
    FreeVertexBuffer(frequency);
    m_vertexBuffers[frequency] = BufCreate(bytes, 0, 0);
}

void CGxDeviceOpenGl::AllocIndexBuffer(unsigned frequency, unsigned bytes)
{
    if (frequency >= 4) {
        return;
    }
    FreeIndexBuffer(frequency);
    m_indexBuffers[frequency] = BufCreate(0, bytes, 0);
}

void CGxDeviceOpenGl::IAllocBuffers()
{
    for (unsigned frequency = 0; frequency < 4; ++frequency) {
        AllocVertexBuffer(frequency, 0);
        AllocIndexBuffer(frequency, 0);
    }
}

void CGxDeviceOpenGl::AllocBuffers()
{
    IAllocBuffers();
}

void CGxDeviceOpenGl::FreeVertexBuffer(unsigned frequency)
{
    if (frequency < 4 && m_vertexBuffers[frequency] != nullptr) {
        BufDestroy(m_vertexBuffers[frequency]);
        m_vertexBuffers[frequency] = nullptr;
    }
}

void CGxDeviceOpenGl::FreeIndexBuffer(unsigned frequency)
{
    if (frequency < 4 && m_indexBuffers[frequency] != nullptr) {
        BufDestroy(m_indexBuffers[frequency]);
        m_indexBuffers[frequency] = nullptr;
    }
}

void CGxDeviceOpenGl::FreeBuffers()
{
    for (unsigned frequency = 0; frequency < 4; ++frequency) {
        FreeVertexBuffer(frequency);
        FreeIndexBuffer(frequency);
    }
    IFreeVAR();
}

void CGxDeviceOpenGl::BufReserve(
    unsigned frequency,
    unsigned format,
    unsigned vertexCount,
    unsigned indexCount)
{
    (void)frequency;
    (void)format;
    (void)vertexCount;
    (void)indexCount;
}

void CGxDeviceOpenGl::ISetGlCaps()
{
    m_numTmus = static_cast<unsigned>(std::clamp(glExtMultiTextureCount, 1, 4));
    m_caps.m_numTmus = m_numTmus;
    GLint maxTextureSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    if (maxTextureSize > 0) {
        m_maxTextureSize = static_cast<unsigned>(maxTextureSize);
        m_caps.m_maxTextureSize = m_maxTextureSize;
    }
    m_pixelShaderTarget = glARBFragmentProgram
        ? CGxPixelShader::Target_arbfp1
        : CGxPixelShader::Target_None;
    m_caps.m_pixelShaderTarget = m_pixelShaderTarget;
    m_caps.m_colorFormat = GxCF_rgba;
    m_caps.m_texFilterAnisotropic = glExtTextureFilterAnisotropic != 0;
}
