#pragma once

#include "Storm/h/SEvt.h"

struct _PARAMS {
    HWND window;
    unsigned int message;
    unsigned int wparam;
    int lparam;
    unsigned int notifycode;
    void* extra;
    int useresult;
    int result;
};

using SMSG_HANDLER = void (__stdcall*)(_PARAMS*);

int __stdcall SMsgBreakHandlerChain(_PARAMS* parameters);
int __stdcall SMsgDestroy();
int __stdcall SMsgDispatchMessage(HWND window, unsigned int message, unsigned int wparam,
    int lparam, int* useResult, int* result);
unsigned int __stdcall SMsgDoMessageLoop(int (__stdcall* idleProc)(unsigned int), int cleanupOnQuit);
HWND __stdcall SMsgGetDefaultWindow();
BOOL __stdcall SMsgGetDefaultWindowRect(RECT* rect);
WNDPROC __stdcall SMsgGetGenericWndProc(unsigned int id);
LRESULT __stdcall GenericWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam);
int __stdcall SMsgPopRegisterState(HWND window);
int __stdcall SMsgPushRegisterState(HWND window);
int __stdcall SMsgRegisterCommand(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgRegisterSysCommand(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgRegisterKeyDown(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgRegisterKeyUp(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgRegisterMessage(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgSetDefaultWindow(HWND window);
void __stdcall SMsgSetDefaultWindowRect(const RECT* rect);
int __stdcall SMsgUnregisterCommand(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgUnregisterSysCommand(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgUnregisterKeyDown(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgUnregisterKeyUp(HWND window, unsigned int id, SMSG_HANDLER handler);
int __stdcall SMsgUnregisterMessage(HWND window, unsigned int id, SMSG_HANDLER handler);

#if defined(_M_IX86)
static_assert(sizeof(_PARAMS) == 32);
#endif
