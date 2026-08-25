#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

void CGxDeviceOpenGl::ISceneBegin(unsigned mask)
{
    float minX, maxX, minY, maxY, minZ, maxZ;
    XformViewport(&minX, &maxX, &minY, &maxY, &minZ, &maxZ);
    XformSetViewport(0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F);
    SceneClear(mask);
    XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
}

void CGxDeviceOpenGl::SceneClear(unsigned mask)
{
    const float red = m_clearColor.r / 255.0F;
    const float green = m_clearColor.g / 255.0F;
    const float blue = m_clearColor.b / 255.0F;
    const float alpha = m_clearColor.a / 255.0F;
    glClearColor(red, green, blue, alpha);

    const unsigned oldDepthMask = m_deviceState[0];
    DsSet(0, 1, 0);
    GLbitfield clearMask = 0;
    if (mask & 1u) clearMask |= GL_COLOR_BUFFER_BIT;
    if (mask & 2u) clearMask |= GL_DEPTH_BUFFER_BIT;
    glClear(clearMask);
    DsSet(0, oldDepthMask, 0);
}

void CGxDeviceOpenGl::ScenePresent(unsigned mask)
{
    const int takeScreenShot = m_scrShotClick;
    CGxDevice::ScenePresent(mask);
    if ((CGxDevice::m_hwState.m_masterEnables
         & (1U << GxMasterEnable_DoubleBuffering)) != 0)
        wglSwapLayerBuffers(m_hdc, WGL_SWAP_MAIN_PLANE);
    else
        glFinish();
    GetError("ScenePresent");
    if (takeScreenShot)
        DeviceScreenShot();
    ISceneBegin(mask);
}
