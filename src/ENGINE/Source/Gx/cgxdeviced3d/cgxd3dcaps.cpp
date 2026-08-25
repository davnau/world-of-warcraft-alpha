#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

void CGxDeviceD3d::CapsWindowSize(NTempest::CRect* destination) const
{
    if (destination)
        *destination = *DeviceCurWindow();
}

void CGxDeviceD3d::CapsWindowSizeInScreenCoords(NTempest::CRect* destination) const
{
    if (!destination)
        return;
    if (!IDevIsWindowed() || !m_hwnd) {
        *destination = *DeviceCurWindow();
        return;
    }

    RECT windowRect{0, 0,
        static_cast<LONG>(DeviceCurWindow()->r - DeviceCurWindow()->l),
        static_cast<LONG>(DeviceCurWindow()->b - DeviceCurWindow()->t)};
    MapWindowPoints(m_hwnd, nullptr, reinterpret_cast<POINT*>(&windowRect), 2);
    destination->l = static_cast<float>(windowRect.left);
    destination->t = static_cast<float>(windowRect.top);
    destination->r = static_cast<float>(windowRect.right);
    destination->b = static_cast<float>(windowRect.bottom);
}

int CGxDeviceD3d::CapsIsWindowVisible() const
{
    if (IDevIsWindowed())
        return m_windowVisible && !m_d3dNeedsReset;
    return !m_d3dNeedsReset;
}
