#ifndef NOMINMAX
#define NOMINMAX
#endif

#include "ENGINE/Source/Os/W32/input.h"
#include <windowsx.h>
#include <deque>
#include <mutex>
#include <algorithm>

int OsGuiProcessMessage(MSG* message);
namespace {
std::deque<OS_INPUT_EVENT> s_queue;
std::mutex s_lock;
HWND s_window = nullptr;
WNDPROC s_previous = nullptr;
POINT s_saved{}, s_mouseCenter{};
int s_relative = 0;
int s_screenWindow = 1;
unsigned s_buttonState = 0;
bool s_releasingCapture = false;
}
void __cdecl OsInputInitialize(){std::lock_guard lock(s_lock);s_queue.clear();}void __cdecl OsInputDestroy(){std::lock_guard lock(s_lock);s_queue.clear();s_window=nullptr;s_previous=nullptr;}void OsQueuePut(OSINPUT t,int a,int b,int c,int d){std::lock_guard lock(s_lock);s_queue.push_back({t,a,b,c,d});if(s_queue.size()>1024)s_queue.pop_front();}int OsQueueGet(OS_INPUT_EVENT*e){std::lock_guard lock(s_lock);if(s_queue.empty()||!e)return 0;*e=s_queue.front();s_queue.pop_front();return 1;}

int __fastcall OsInputGet(int* type,int* param0,int* param1,int* param2,int* param3){
    OS_INPUT_EVENT event{};
    if(!OsQueueGet(&event)){
        MSG message{};
        while(PeekMessageA(&message,nullptr,0,0,PM_REMOVE)){
            if(message.message==WM_QUIT){
                event={OS_INPUT_SHUTDOWN,0,0,0,0};
                break;
            }
            OsGuiProcessMessage(&message);
            // The recovered OsInputGet applies MSG.time to every event queued
            // while this Windows message is dispatched. Camera and movement
            // code use the same GetTickCount time domain.
            OsQueueSetParam(3, static_cast<int>(message.time));
            if(OsQueueGet(&event))break;
        }
        if(event.type==OS_INPUT_CAPTURE_CHANGED&&event.param0==0&&event.param1==0&&event.param2==0&&event.param3==0)return 0;
    }
    if(type)*type=event.type;if(param0)*param0=event.param0;if(param1)*param1=event.param1;if(param2)*param2=event.param2;if(param3)*param3=event.param3;return 1;
}

void OsQueueSetParam(int index,int value){std::lock_guard lock(s_lock);if(index<0||index>=4)return;for(auto& event:s_queue)(&event.param0)[index]=value;}void OsInputNotifyScreenResize(int x,int y){OsQueuePut(OS_INPUT_SIZE,x,y,0,0);}void OsInputSetScreenIsWindow(int v){s_screenWindow=v;}BOOL CenterMouse(){if(!s_window)return FALSE;RECT r{};GetClientRect(s_window,&r);s_mouseCenter={(r.right-r.left)/2,(r.bottom-r.top)/2};ClientToScreen(s_window,&s_mouseCenter);return SetCursorPos(s_mouseCenter.x,s_mouseCenter.y);}BOOL RestoreMouse(){return SetCursorPos(s_saved.x,s_saved.y);}void OsInputSetMouseMode(int mode){if(mode==s_relative)return;if(mode==OS_MOUSE_MODE_RELATIVE){GetCursorPos(&s_saved);s_relative=1;CenterMouse();}else{s_relative=0;RestoreMouse();}}void OsInputGetMousePosition(int*x,int*y){POINT p{};GetCursorPos(&p);if(s_window)ScreenToClient(s_window,&p);if(x)*x=p.x;if(y)*y=p.y;}void OsInputSetMousePosition(int x,int y){POINT p{x,y};if(s_window)ClientToScreen(s_window,&p);SetCursorPos(p.x,p.y);}UINT __stdcall OsInputGetCodePage(){return GetACP();}WNDPROC OsSetWindowProc(HWND w,WNDPROC p){s_window=w;s_previous=p;return reinterpret_cast<WNDPROC>(SetWindowLongPtrA(w,GWLP_WNDPROC,reinterpret_cast<LONG_PTR>(p)));}HWND OsInputGetWindow(){return s_window;}
int ConvertKeyCode(int key){
    if((key>=48&&key<=57)||(key>=65&&key<=90))return key;
    if(key>=VK_F1&&key<=VK_F12)return key+656;
    switch(key){
    case VK_BACK:return 514;
    case VK_TAB:return 515;
    case VK_RETURN:return 513;
    case VK_SHIFT:return 0;
    case VK_CONTROL:return 1;
    case VK_MENU:return 2;
    case VK_PAUSE:return 529;
    case VK_CAPITAL:return 526;
    case VK_ESCAPE:return 512;
    case VK_SPACE:return 32;
    case VK_PRIOR:return 524;
    case VK_NEXT:return 525;
    case VK_END:return 523;
    case VK_HOME:return 522;
    case VK_LEFT:return 516;
    case VK_UP:return 517;
    case VK_RIGHT:return 518;
    case VK_DOWN:return 519;
    case VK_SNAPSHOT:return 530;
    case VK_INSERT:return 520;
    case VK_DELETE:return 521;
    case VK_NUMPAD0:return 257;
    case VK_NUMPAD1:return 258;
    case VK_NUMPAD2:return 259;
    case VK_NUMPAD3:return 260;
    case VK_NUMPAD4:return 261;
    case VK_NUMPAD5:return 262;
    case VK_NUMPAD6:return 263;
    case VK_NUMPAD7:return 264;
    case VK_NUMPAD8:return 265;
    case VK_NUMPAD9:return 266;
    case VK_MULTIPLY:return 269;
    case VK_ADD:return 267;
    case VK_SUBTRACT:return 268;
    case VK_DECIMAL:return 271;
    case VK_DIVIDE:return 270;
    case VK_NUMLOCK:return 527;
    case VK_SCROLL:return 528;
    case 186:return 278;
    case 187:return 272;
    case 188:return 280;
    case 189:return 273;
    case 190:return 281;
    case 191:return 276;
    case 192:return 256;
    case 219:return 274;
    case 220:return 277;
    case 221:return 275;
    case 222:return 279;
    default:return -1;
    }
}
int ConvertButton(UINT msg,WPARAM w,MOUSEBUTTON*b){if(!b)return 0;switch(msg){case WM_LBUTTONDOWN:case WM_LBUTTONUP:*b=MOUSE_BUTTON_LEFT;break;case WM_MBUTTONDOWN:case WM_MBUTTONUP:*b=MOUSE_BUTTON_MIDDLE;break;case WM_RBUTTONDOWN:case WM_RBUTTONUP:*b=MOUSE_BUTTON_RIGHT;break;case WM_XBUTTONDOWN:case WM_XBUTTONUP:*b=GET_XBUTTON_WPARAM(w)==XBUTTON1?MOUSE_BUTTON_XBUTTON1:MOUSE_BUTTON_XBUTTON2;break;default:*b=MOUSE_BUTTON_NONE;return 0;}return 1;}
LRESULT CALLBACK OsWindowProc(HWND w,UINT m,WPARAM wp,LPARAM lp){
    s_window=w;
    MOUSEBUTTON button=MOUSE_BUTTON_NONE;
    switch(m){
    case WM_CLOSE:
        OsQueuePut(OS_INPUT_CLOSE,0,0,0,0);
        return 0;
    case WM_DESTROY:
        OsQueuePut(OS_INPUT_SHUTDOWN,0,0,0,0);
        break;
    case WM_SETFOCUS:
        OsQueuePut(OS_INPUT_FOCUS,1,0,0,0);
        break;
    case WM_KILLFOCUS:
        OsQueuePut(OS_INPUT_FOCUS,0,0,0,0);
        break;
    case WM_SIZE:
        OsQueuePut(OS_INPUT_SIZE,LOWORD(lp),HIWORD(lp),0,0);
        break;
    case WM_CHAR:
        OsQueuePut(OS_INPUT_CHAR,int(wp),0,0,0);
        break;
    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        OsQueuePut(OS_INPUT_KEY_DOWN,ConvertKeyCode(int(wp)),LOWORD(lp),0,0);
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        OsQueuePut(OS_INPUT_KEY_UP,ConvertKeyCode(int(wp)),LOWORD(lp),0,0);
        break;
    case WM_MOUSEMOVE:
        if(s_relative){
            POINT p{};
            GetCursorPos(&p);
            if(p.x!=s_mouseCenter.x||p.y!=s_mouseCenter.y){
                OsQueuePut(OS_INPUT_MOUSE_MOVE_RELATIVE,p.x-s_mouseCenter.x,
                           p.y-s_mouseCenter.y,0,0);
                CenterMouse();
            }
        }else{
            OsQueuePut(OS_INPUT_MOUSE_MOVE,GET_X_LPARAM(lp),GET_Y_LPARAM(lp),0,0);
        }
        break;
    case WM_MOUSEWHEEL:
        OsQueuePut(OS_INPUT_MOUSE_WHEEL,GET_WHEEL_DELTA_WPARAM(wp),
                   GET_X_LPARAM(lp),GET_Y_LPARAM(lp),0);
        break;
    case WM_CAPTURECHANGED:
        if(!s_releasingCapture){
            s_buttonState=0;
            POINT p{};
            GetCursorPos(&p);
            ScreenToClient(w,&p);
            OsQueuePut(OS_INPUT_CAPTURE_CHANGED,p.x,p.y,0,0);
        }
        break;
    default:
        if(ConvertButton(m,wp,&button)){
            const bool down=m==WM_LBUTTONDOWN||m==WM_MBUTTONDOWN||
                            m==WM_RBUTTONDOWN||m==WM_XBUTTONDOWN;
            if(down){
                if(s_buttonState==0)SetCapture(w);
                s_buttonState|=static_cast<unsigned>(button);
            }else{
                s_buttonState&=~static_cast<unsigned>(button);
                if(s_buttonState==0){
                    s_releasingCapture=true;
                    ReleaseCapture();
                    s_releasingCapture=false;
                }
            }
            OsQueuePut(down?OS_INPUT_MOUSE_DOWN:OS_INPUT_MOUSE_UP,button,
                       GET_X_LPARAM(lp),GET_Y_LPARAM(lp),0);
            return (m==WM_XBUTTONDOWN||m==WM_XBUTTONUP)?TRUE:0;
        }
    }
    return s_previous?CallWindowProcA(s_previous,w,m,wp,lp):
                      DefWindowProcA(w,m,wp,lp);
}
void __fastcall SaveMouse(const POINT* point,POINT* saved){if(saved)*saved=point?*point:POINT{};}
BOOL OsGetDefaultWindowRect(RECT* rect){if(!rect||!s_window)return FALSE;return GetClientRect(s_window,rect);}
