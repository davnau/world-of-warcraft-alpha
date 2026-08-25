#pragma once

#include "ENGINE/Source/Gx/cgxdevice/cgxdevice.h"
#include "ENGINE/Source/Gx/cgxdeviceopengl/w32/glwndsupport.h"

bool WindowClassCreate(CGxDeviceOpenGl::WindowProc windowProc);
void WindowClassDestroy(ATOM& atom);
void WindowDestroy(HWND& window);
HWND WindowCreate(CGxDeviceOpenGl* device, const CGxFormat& format);
bool IsGlDisplayModeGood(const DEVMODEA& mode);
