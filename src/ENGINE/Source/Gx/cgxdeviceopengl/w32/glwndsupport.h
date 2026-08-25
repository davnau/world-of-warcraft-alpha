#pragma once

#include "ENGINE/Source/Gx/gxapi/gxformat.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

HGLRC AttachGlContext(HWND window, const CGxFormat* format);
int SetupPixelFormat(HDC dc, const CGxFormat* format);
void RemoveGlContext(HGLRC context);
