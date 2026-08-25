#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

#include <cmath>

namespace {
unsigned g_clearAnimationTick = 0;
}

void CGxDeviceD3d::ISceneBegin(unsigned mask)
{
    if ((m_masterEnables & 0x20u) != 0) {
        float minX, maxX, minY, maxY, minZ, maxZ;
        XformViewport(&minX, &maxX, &minY, &maxY, &minZ, &maxZ);
        XformSetViewport(0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F);
        SceneClear(mask);
        XformSetViewport(minX, maxX, minY, maxY, minZ, maxZ);
    }
    if (m_d3dDevice && m_d3dDevice->BeginScene() == D3D_OK)
        m_inScene = true;
}

void CGxDeviceD3d::ISceneEnd()
{
    if (m_inScene) {
        if (m_d3dDevice)
            m_d3dDevice->EndScene();
        m_inScene = false;
    }
}

void CGxDeviceD3d::SceneClear(unsigned mask)
{
    CGxDevice::SceneClear(mask);
    D3DCOLOR clearColor = static_cast<D3DCOLOR>(m_clearColor);
    if ((m_masterEnables & 0x80u) == 0) {
        const double phase = static_cast<double>(g_clearAnimationTick++) * 3.141592653589793 * 0.0078125;
        clearColor = D3DCOLOR_XRGB(
            static_cast<unsigned>((std::sin(phase * 3.0) + 1.0) * 127.5),
            static_cast<unsigned>((std::sin(phase * 5.0) + 1.0) * 127.5),
            static_cast<unsigned>((std::sin(phase * 7.0) + 1.0) * 127.5));
    }
    if (m_d3dDevice) {
        DWORD flags = 0;
        if (mask & 1u) flags |= D3DCLEAR_TARGET;
        if (mask & 2u) flags |= D3DCLEAR_ZBUFFER;
        if (mask & 4u) flags |= D3DCLEAR_STENCIL;
        m_d3dDevice->Clear(0, nullptr, flags, clearColor, 1.0F, 0);
    }
}

void CGxDeviceD3d::ScenePresent(unsigned mask)
{
    CGxDevice::ScenePresent(mask);
    ISceneEnd();
    if (m_scrShotClick)
        DeviceScreenShot();
    if (m_d3dDevice)
        m_d3dDevice->Present(nullptr, nullptr, nullptr, nullptr);
    ISceneBegin(0);
}
