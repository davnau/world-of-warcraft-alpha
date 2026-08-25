#include "Storm/h/SMsg.h"

#include <algorithm>
#include <mutex>
#include <vector>

namespace {
constexpr unsigned int MSG_MESSAGE = 0x534D5347U;
constexpr unsigned int MSG_COMMAND = 0x534D5348U;
constexpr unsigned int MSG_SYSCOMMAND = 0x534D5349U;
constexpr unsigned int MSG_KEYDOWN = 0x534D534AU;
constexpr unsigned int MSG_KEYUP = 0x534D534BU;

std::recursive_mutex s_messageLock;
std::vector<HWND> s_windows;
HWND s_defaultWindow = nullptr;
RECT s_defaultRect{};

unsigned int WindowKey(HWND window) { return static_cast<unsigned int>(reinterpret_cast<std::uintptr_t>(window)); }
void AddWindow(HWND window) {
    std::lock_guard lock(s_messageLock);
    if (std::find(s_windows.begin(), s_windows.end(), window) == s_windows.end()) s_windows.push_back(window);
}
void DeleteWindowRecord(HWND window) {
    std::lock_guard lock(s_messageLock);
    for (unsigned int type : {MSG_MESSAGE, MSG_COMMAND, MSG_SYSCOMMAND, MSG_KEYDOWN, MSG_KEYUP})
        SEvtUnregisterType(type, WindowKey(window));
    s_windows.erase(std::remove(s_windows.begin(), s_windows.end(), window), s_windows.end());
    if (s_defaultWindow == window) { s_defaultWindow = nullptr; s_defaultRect = {}; }
}
int Register(unsigned int type, HWND window, unsigned int id, SMSG_HANDLER handler) {
    if (!handler) return 0;
    AddWindow(window);
    return SEvtRegisterHandler(type, WindowKey(window), id, nullptr, reinterpret_cast<SEVT_HANDLER>(handler));
}
int Unregister(unsigned int type, HWND window, unsigned int id, SMSG_HANDLER handler) {
    return SEvtUnregisterHandler(type, WindowKey(window), id, reinterpret_cast<SEVT_HANDLER>(handler));
}
} // namespace

int __stdcall SMsgBreakHandlerChain(_PARAMS* parameters) { return SEvtBreakHandlerChain(parameters); }
int __stdcall SMsgDestroy() {
    std::lock_guard lock(s_messageLock);
    for (HWND window : s_windows)
        for (unsigned int type : {MSG_MESSAGE, MSG_COMMAND, MSG_SYSCOMMAND, MSG_KEYDOWN, MSG_KEYUP})
            SEvtUnregisterType(type, WindowKey(window));
    s_windows.clear(); s_defaultWindow = nullptr; s_defaultRect = {};
    return 1;
}

int __stdcall SMsgDispatchMessage(HWND window, unsigned int message, unsigned int wparam,
    int lparam, int* useResult, int* result) {
    if (useResult) *useResult = 0;
    if (result) *result = 0;
    _PARAMS parameters{window, message, wparam, lparam, message == WM_COMMAND ? HIWORD(wparam) : 0U,
        nullptr, 0, 0};
    HWND targets[] = {window, s_defaultWindow};
    for (unsigned int targetIndex = 0; targetIndex < 2; ++targetIndex) {
        HWND target = targets[targetIndex];
        if (targetIndex == 1 && (!target || target == window)) continue;
        const unsigned int key = WindowKey(target);
        SEvtDispatch(MSG_MESSAGE, key, message, &parameters);
        switch (message) {
        case WM_KEYDOWN: SEvtDispatch(MSG_KEYDOWN, key, wparam, &parameters); break;
        case WM_KEYUP: SEvtDispatch(MSG_KEYUP, key, wparam, &parameters); break;
        case WM_COMMAND: SEvtDispatch(MSG_COMMAND, key, LOWORD(wparam), &parameters); break;
        case WM_SYSCOMMAND: SEvtDispatch(MSG_SYSCOMMAND, key, wparam, &parameters); break;
        default: break;
        }
    }
    if (message == WM_NCDESTROY) DeleteWindowRecord(window);
    if (useResult) *useResult = parameters.useresult;
    if (result) *result = parameters.result;
    return 1;
}

unsigned int __stdcall SMsgDoMessageLoop(int (__stdcall* idleProc)(unsigned int), int cleanupOnQuit) {
    MSG message{};
    unsigned int idleCount = 0;
    for (;;) {
        while (!PeekMessageA(&message, nullptr, 0, 0, PM_NOREMOVE)) {
            if (!idleProc || !idleProc(idleCount++)) break;
        }
        const BOOL status = GetMessageA(&message, nullptr, 0, 0);
        if (status <= 0) break;
        TranslateMessage(&message);
        DispatchMessageA(&message);
        idleCount = 0;
    }
    if (cleanupOnQuit) SMsgDestroy();
    return static_cast<unsigned int>(message.wParam);
}

HWND __stdcall SMsgGetDefaultWindow() { return s_defaultWindow; }
BOOL __stdcall SMsgGetDefaultWindowRect(RECT* rect) {
    if (!rect) return FALSE;
    if (s_defaultRect.left || s_defaultRect.top || s_defaultRect.right || s_defaultRect.bottom) {
        *rect = s_defaultRect; return TRUE;
    }
    return s_defaultWindow ? GetClientRect(s_defaultWindow, rect) : FALSE;
}
WNDPROC __stdcall SMsgGetGenericWndProc(unsigned int id) { return id == 0 ? GenericWndProc : nullptr; }
LRESULT __stdcall GenericWndProc(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
    int useResult = 0, result = 0;
    SMsgDispatchMessage(window, message, static_cast<unsigned int>(wparam), static_cast<int>(lparam), &useResult, &result);
    return useResult ? result : DefWindowProcA(window, message, wparam, lparam);
}

int __stdcall SMsgPushRegisterState(HWND window) {
    for (unsigned int type : {MSG_MESSAGE, MSG_COMMAND, MSG_SYSCOMMAND, MSG_KEYDOWN, MSG_KEYUP}) SEvtPushState(type, WindowKey(window));
    return 1;
}
int __stdcall SMsgPopRegisterState(HWND window) {
    for (unsigned int type : {MSG_MESSAGE, MSG_COMMAND, MSG_SYSCOMMAND, MSG_KEYDOWN, MSG_KEYUP}) SEvtPopState(type, WindowKey(window));
    return 1;
}
int __stdcall SMsgRegisterCommand(HWND w,unsigned int id,SMSG_HANDLER h){return Register(MSG_COMMAND,w,id,h);}
int __stdcall SMsgRegisterSysCommand(HWND w,unsigned int id,SMSG_HANDLER h){return Register(MSG_SYSCOMMAND,w,id,h);}
int __stdcall SMsgRegisterKeyDown(HWND w,unsigned int id,SMSG_HANDLER h){return Register(MSG_KEYDOWN,w,id,h);}
int __stdcall SMsgRegisterKeyUp(HWND w,unsigned int id,SMSG_HANDLER h){return Register(MSG_KEYUP,w,id,h);}
int __stdcall SMsgRegisterMessage(HWND w,unsigned int id,SMSG_HANDLER h){return Register(MSG_MESSAGE,w,id,h);}
int __stdcall SMsgSetDefaultWindow(HWND window) { s_defaultWindow=window; AddWindow(window); return 1; }
void __stdcall SMsgSetDefaultWindowRect(const RECT* rect) { if(rect)s_defaultRect=*rect; }
int __stdcall SMsgUnregisterCommand(HWND w,unsigned int id,SMSG_HANDLER h){return Unregister(MSG_COMMAND,w,id,h);}
int __stdcall SMsgUnregisterSysCommand(HWND w,unsigned int id,SMSG_HANDLER h){return Unregister(MSG_SYSCOMMAND,w,id,h);}
int __stdcall SMsgUnregisterKeyDown(HWND w,unsigned int id,SMSG_HANDLER h){return Unregister(MSG_KEYDOWN,w,id,h);}
int __stdcall SMsgUnregisterKeyUp(HWND w,unsigned int id,SMSG_HANDLER h){return Unregister(MSG_KEYUP,w,id,h);}
int __stdcall SMsgUnregisterMessage(HWND w,unsigned int id,SMSG_HANDLER h){return Unregister(MSG_MESSAGE,w,id,h);}
