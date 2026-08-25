#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"

void CGxDeviceOpenGl::CapsWindowSize(NTempest::CRect* destination) const
{
    if (destination)
        *destination = *DeviceCurWindow();
}

int CGxDeviceOpenGl::CapsIsWindowVisible() const
{
    const auto* window = DeviceCurWindow();
    return window && (window->r - window->l) * (window->b - window->t) > 1.0F;
}
