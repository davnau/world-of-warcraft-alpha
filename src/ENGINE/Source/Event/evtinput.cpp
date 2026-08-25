#include "ENGINE/Source/Event/event_internal.h"
#include "ENGINE/Source/Base/coordinate.h"

#include <algorithm>
#include <cstdint>
#include <windows.h>

void __cdecl OsInputInitialize();
void __cdecl OsInputDestroy();
int __fastcall OsInputGet(int*,int*,int*,int*,int*);
void OsInputSetMouseMode(int);
void OsInputGetMousePosition(int*,int*);
void OsInputSetMousePosition(int,int);
UINT __stdcall OsInputGetCodePage();
BOOL OsGetDefaultWindowRect(RECT*);
DWORD __stdcall OsGetAsyncTimeMs();

namespace {

enum OSINPUT : unsigned {
    OS_INPUT_CAPTURE_CHANGED=0,OS_INPUT_CHAR=1,OS_INPUT_STRING=2,OS_INPUT_IME=3,
    OS_INPUT_SIZE=4,OS_INPUT_CLOSE=5,OS_INPUT_FOCUS=6,OS_INPUT_KEY_DOWN=7,
    OS_INPUT_KEY_UP=8,OS_INPUT_MOUSE_DOWN=9,OS_INPUT_MOUSE_MOVE=10,
    OS_INPUT_MOUSE_WHEEL=11,OS_INPUT_MOUSE_MOVE_RELATIVE=12,OS_INPUT_MOUSE_UP=13,
    OS_INPUT_SHUTDOWN=14
};

struct OS_INPUT_EVENT {OSINPUT type;int param0,param1,param2,param3;};

MOUSEMODE mouseMode=MOUSE_MODE_NORMAL;
unsigned mouseHoldButton=0;
unsigned buttonState=0;
unsigned metaKeyState=0;
EventRect boundingRect{0.0f,0.0f,0.0f,0.0f};
int (__fastcall*confirmCloseCallback)(void*)=nullptr;
void* confirmCloseParam=nullptr;

unsigned GenerateMouseFlags(){return mouseMode==MOUSE_MODE_RELATIVE?2u:0u;}

void GetWindowRectOrDefault(RECT* rect){
    if(!rect||!OsGetDefaultWindowRect(rect)||rect->right<=rect->left||rect->bottom<=rect->top){
        if(rect)*rect=RECT{0,0,1024,768};
    }
}

}

void PostClose();
void PostMouseUp(EVENTID,MOUSEBUTTON,int,int,unsigned,unsigned);
void CheckMouseModeState();
void PostMouseModeChanged(MOUSEMODE);

void IEvtInputInitialize(){
    OsInputInitialize();
    mouseMode=MOUSE_MODE_NORMAL;
    mouseHoldButton=0;
    boundingRect={0.0f,0.0f,0.0f,0.0f};
    ResetAsyncState();
}

void IEvtInputDestroy(){OsInputDestroy();ResetAsyncState();}

int ResetAsyncState(){buttonState=0;metaKeyState=0;return 0;}

void PostCaptureChanged(){
    while(buttonState){
        const unsigned bit=buttonState&(~buttonState+1u);
        PostMouseUp(EVENT_ID_MOUSEUP,static_cast<MOUSEBUTTON>(bit),0,0,1u,OsGetAsyncTimeMs());
    }
}
void PostChar(int ch,unsigned repeat){EVENT_DATA_CHAR event{ch,metaKeyState,repeat};IEvtQueueDispatch(EVENT_ID_CHAR,&event,sizeof(event));}
void PostString(const std::uint16_t* chars,unsigned count){if(!chars)return;for(unsigned i=0;i<count;++i)PostChar(static_cast<int>(chars[i]),1);}
void PostIme(unsigned message,unsigned wParam,unsigned lParam){EVENT_DATA_IME event{message,wParam,lParam,OsInputGetCodePage()};IEvtQueueDispatch(EVENT_ID_IME,&event,sizeof(event));}
void PostSize(int width,int height){EVENT_DATA_SIZE event{width,height};IEvtQueueDispatch(EVENT_ID_SIZE,&event,sizeof(event));}
void PostFocus(int focus){ResetAsyncState();EVENT_DATA_FOCUS event{focus};IEvtQueueDispatch(EVENT_ID_FOCUS,&event,sizeof(event));CheckMouseModeState();}
void PostKeyDown(KEY key,unsigned repeat,unsigned time){if(key<=KEY_ALT)metaKeyState|=1u<<static_cast<unsigned>(key);EVENT_DATA_KEY event{key,metaKeyState,repeat,time};IEvtQueueDispatch(EVENT_ID_KEYDOWN,&event,sizeof(event));}
void PostKeyUp(KEY key,unsigned repeat,unsigned time){if(key<=KEY_ALT)metaKeyState&=~(1u<<static_cast<unsigned>(key));EVENT_DATA_KEY event{key,metaKeyState,repeat,time};IEvtQueueDispatch(EVENT_ID_KEYUP,&event,sizeof(event));}
void PostMouseDown(MOUSEBUTTON button,int x,int y,unsigned time){buttonState|=static_cast<unsigned>(button);EVENT_DATA_MOUSE event{mouseMode,button,buttonState,metaKeyState,GenerateMouseFlags(),0.0f,0.0f,0,time};ConvertPosition(x,y,&event.x,&event.y);IEvtQueueDispatch(EVENT_ID_MOUSEDOWN,&event,sizeof(event));}
void PostMouseMove(int x,int y,unsigned time){EVENT_DATA_MOUSE event{mouseMode,MOUSE_BUTTON_NONE,buttonState,metaKeyState,GenerateMouseFlags(),0.0f,0.0f,0,time};ConvertPosition(x,y,&event.x,&event.y);IEvtQueueDispatch(EVENT_ID_MOUSEMOVE,&event,sizeof(event));}
void PostMouseMoveRelative(int x,int y,unsigned time){EVENT_DATA_MOUSE event{mouseMode,MOUSE_BUTTON_NONE,buttonState,metaKeyState,GenerateMouseFlags(),static_cast<float>(x),static_cast<float>(y),0,time};IEvtQueueDispatch(EVENT_ID_MOUSEMOVE_RELATIVE,&event,sizeof(event));}
void PostMouseWheel(int distance,int x,int y,unsigned time){EVENT_DATA_MOUSE event{mouseMode,MOUSE_BUTTON_NONE,buttonState,metaKeyState,GenerateMouseFlags(),0.0f,0.0f,distance,time};ConvertPosition(x,y,&event.x,&event.y);IEvtQueueDispatch(EVENT_ID_MOUSEWHEEL,&event,sizeof(event));}

int IEvtInputProcess(int* shutdown,HEVENTCONTEXT context){
    if(!shutdown)return 0;
    *shutdown=0;
    context=EventInternal::CurrentOr(context);
    auto previous=EventInternal::current;
    EventInternal::current=context;

    int type=0,param0=0,param1=0,param2=0,param3=0;
    int processed=0;
    while(OsInputGet(&type,&param0,&param1,&param2,&param3)){
        processed=1;
        OS_INPUT_EVENT input{static_cast<OSINPUT>(type),param0,param1,param2,param3};
        ProcessInput(context,static_cast<EVENTID>(type),&input,sizeof(input),shutdown);
    }
    EventInternal::current=previous;
    return processed;
}

void ProcessInput(HEVENTCONTEXT,EVENTID id,const void* data,unsigned bytes,int* shutdown){
    if(!data||bytes<sizeof(OS_INPUT_EVENT))return;
    const auto& input=*static_cast<const OS_INPUT_EVENT*>(data);
    switch(static_cast<unsigned>(id)){
    case OS_INPUT_CAPTURE_CHANGED:
        PostCaptureChanged();
        break;
    case OS_INPUT_CHAR:
        PostChar(input.param0,1);
        break;
    case OS_INPUT_STRING:
        PostString(reinterpret_cast<const std::uint16_t*>(static_cast<std::uintptr_t>(static_cast<unsigned>(input.param0))),static_cast<unsigned>(input.param1));
        break;
    case OS_INPUT_IME:
        PostIme(static_cast<unsigned>(input.param0),static_cast<unsigned>(input.param1),static_cast<unsigned>(input.param2));
        break;
    case OS_INPUT_SIZE:
        PostSize(input.param0,input.param1);
        break;
    case OS_INPUT_CLOSE:
        if(ConfirmClose()){PostClose();if(shutdown)*shutdown=1;}
        break;
    case OS_INPUT_FOCUS:
        PostFocus(input.param0);
        break;
    case OS_INPUT_KEY_DOWN:
        PostKeyDown(static_cast<KEY>(input.param0),static_cast<unsigned>(input.param1),static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_KEY_UP:
        PostKeyUp(static_cast<KEY>(input.param0),static_cast<unsigned>(input.param1),static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_MOUSE_DOWN:
        PostMouseDown(static_cast<MOUSEBUTTON>(input.param0),input.param1,input.param2,static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_MOUSE_MOVE:
        PostMouseMove(input.param0,input.param1,static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_MOUSE_WHEEL:
        PostMouseWheel(input.param0,input.param1,input.param2,static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_MOUSE_MOVE_RELATIVE:
        PostMouseMoveRelative(input.param0,input.param1,static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_MOUSE_UP:
        PostMouseUp(EVENT_ID_MOUSEUP,static_cast<MOUSEBUTTON>(input.param0),input.param1,input.param2,0,static_cast<unsigned>(input.param3));
        break;
    case OS_INPUT_SHUTDOWN:
        if(ConfirmClose()&&shutdown)*shutdown=1;
        break;
    default:break;
    }
}

int ConfirmClose(){return confirmCloseCallback?confirmCloseCallback(confirmCloseParam):1;}
void PostClose(){EventInitiateShutdown();}
void IEvtInputSetConfirmCloseCallback(int (__fastcall* callback)(void*),void* param){confirmCloseCallback=callback;confirmCloseParam=param;}

void ConvertPosition(int clientX,int clientY,float* x,float* y){
    int boundedX=clientX,boundedY=clientY;
    if(boundingRect.right!=boundingRect.left&&boundingRect.bottom!=boundingRect.top){
        if(boundedX<=boundingRect.left||boundedX>=boundingRect.right||boundedY<=boundingRect.top||boundedY>=boundingRect.bottom){
            boundedX=static_cast<int>(std::clamp(static_cast<float>(boundedX),boundingRect.left+1.0f,boundingRect.right-1.0f));
            boundedY=static_cast<int>(std::clamp(static_cast<float>(boundedY),boundingRect.top+1.0f,boundingRect.bottom-1.0f));
            OsInputSetMousePosition(boundedX,boundedY);
        }
    }
    RECT rect{};GetWindowRectOrDefault(&rect);
    const float width=static_cast<float>(rect.right-rect.left),height=static_cast<float>(rect.bottom-rect.top);
    if(x)*x=(static_cast<float>(boundedX-rect.left))/width;
    if(y)*y=1.0f-(static_cast<float>(boundedY-rect.top))/height;
}

void UnconvertPosition(float x,float y,int* clientX,int* clientY){
    RECT rect{};GetWindowRectOrDefault(&rect);
    const int left=static_cast<int>(rect.left),top=static_cast<int>(rect.top),right=static_cast<int>(rect.right),bottom=static_cast<int>(rect.bottom);
    int convertedX=left+static_cast<int>((right-left)*x);
    int convertedY=top+static_cast<int>((bottom-top)*y);
    convertedX=std::clamp(convertedX,left,right-1);
    convertedY=std::clamp(convertedY,top,bottom-1);
    if(clientX)*clientX=convertedX;
    if(clientY)*clientY=rect.bottom-convertedY-rect.top;
}

void IEvtInputGetMousePosition(float* x,float* y){
    int clientX=0,clientY=0;OsInputGetMousePosition(&clientX,&clientY);
    float ndcX=0.0f,ndcY=0.0f;ConvertPosition(clientX,clientY,&ndcX,&ndcY);NDCToDDC(x,y,ndcX,ndcY);
}
void IEvtInputSetMousePosition(float x,float y){
    float ndcX=0.0f,ndcY=0.0f;DDCToNDC(&ndcX,&ndcY,x,y);int clientX=0,clientY=0;UnconvertPosition(ndcX,ndcY,&clientX,&clientY);OsInputSetMousePosition(clientX,clientY);
}
void IEvtInputSetMouseBoundingRect(const EventRect* rect){
    if(!rect){boundingRect={0.0f,0.0f,0.0f,0.0f};return;}
    float left=0.0f,top=0.0f,right=0.0f,bottom=0.0f;DDCToNDC(&left,&top,rect->left,rect->top);DDCToNDC(&right,&bottom,rect->right,rect->bottom);int leftPx=0,topPx=0,rightPx=0,bottomPx=0;UnconvertPosition(left,top,&leftPx,&topPx);UnconvertPosition(right,bottom,&rightPx,&bottomPx);boundingRect={static_cast<float>(leftPx),static_cast<float>(topPx),static_cast<float>(rightPx),static_cast<float>(bottomPx)};
}

void EventSetConfirmCloseCallback(int (__fastcall* callback)(void*),void* param){IEvtInputSetConfirmCloseCallback(callback,param);}
void EventSetMouseMode(MOUSEMODE mode,unsigned holdButton){IEvtInputSetMouseMode(mode,holdButton);}
void EventInputGetMousePosition(float* x,float* y){IEvtInputGetMousePosition(x,y);}
void EventInputSetMousePosition(float x,float y){IEvtInputSetMousePosition(x,y);}
void EventSetMouseBoundingRect(const EventRect* rect){IEvtInputSetMouseBoundingRect(rect);}

void PostMouseModeChanged(MOUSEMODE mode){
    mouseMode=mode;EVENT_DATA_MOUSE event{};event.mode=mode;event.buttonState=buttonState;event.metaKeyState=metaKeyState;event.flags=GenerateMouseFlags();IEvtQueueDispatch(EVENT_ID_MOUSEMODE_CHANGED,&event,sizeof(event));
}
void IEvtInputSetMouseMode(MOUSEMODE mode,unsigned holdButton){
    if(mode<0||mode>=MOUSE_MODES)return;
    if((holdButton&buttonState)!=holdButton)return;
    mouseHoldButton=holdButton;
    if(mode!=mouseMode){OsInputSetMouseMode(static_cast<int>(mode));PostMouseModeChanged(mode);}
}
void CheckMouseModeState(){if(mouseHoldButton&&!(mouseHoldButton&buttonState))EventSetMouseMode(MOUSE_MODE_NORMAL,0);}

void PostMouseUp(EVENTID id,MOUSEBUTTON button,int x,int y,unsigned flags,unsigned time){
    buttonState&=~static_cast<unsigned>(button);EVENT_DATA_MOUSE event{mouseMode,button,buttonState,metaKeyState,flags|GenerateMouseFlags(),0.0f,0.0f,0,time};ConvertPosition(x,y,&event.x,&event.y);IEvtQueueDispatch(id,&event,sizeof(event));CheckMouseModeState();
}
