#include "ENGINE/Source/Event/cinputobserver.h"
#include "ENGINE/Source/Base/coordinate.h"

namespace {
CInputObserver* input=nullptr;

void Reg(EVENTID queueId,unsigned observerId,EventHandler handler,float priority,CObserver* observer){
    if(!observer)return;
    CInputObserver* inputObserver=CInputObserver::GetInputObserver();
    if(!inputObserver->IsEventRegistered(observerId))
        EventRegisterEx(queueId,handler,inputObserver,priority);
    inputObserver->RegisterEvent(observerId,static_cast<int>(observerId),observer);
}

void Unreg(EVENTID queueId,unsigned observerId,EventHandler handler,CObserver* observer){
    CInputObserver* inputObserver=CInputObserver::GetInputObserver();
    inputObserver->UnregisterEvent(observerId,observer);
    if(!inputObserver->IsEventRegistered(observerId))
        EventUnregisterEx(queueId,handler,inputObserver,~0u);
}

template<class E,class D>int Send(unsigned id,const void*d){if(!d)return 0;E event;event.id=id;event.param=nullptr;static_cast<D&>(event)=*static_cast<const D*>(d);return CInputObserver::GetInputObserver()->DispatchEvent(&event)|1;}template<>int Send<CMouseEvent,EVENT_DATA_MOUSE>(unsigned id,const void*d){if(!d)return 0;CMouseEvent event;event.id=id;event.param=nullptr;static_cast<EVENT_DATA_MOUSE&>(event)=*static_cast<const EVENT_DATA_MOUSE*>(d);NDCToDDC(&event.x,&event.y,event.x,event.y);return CInputObserver::GetInputObserver()->DispatchEvent(&event)|1;}
}
void InputObserverInitialize(){}void InputObserverDestroy(){delete input;input=nullptr;}CInputObserver*CInputObserver::GetInputObserver(){if(!input)input=new CInputObserver;return input;}
#define ON(Name,Class,Data,Id) int __fastcall CInputObserver::On##Name(const void*d,void*){return Send<Class,Data>(Id,d);}
ON(Char,CCharEvent,EVENT_DATA_CHAR,INPUT_EVENT_CHAR) ON(KeyDown,CKeyEvent,EVENT_DATA_KEY,INPUT_EVENT_KEYDOWN) ON(KeyRepeat,CKeyEvent,EVENT_DATA_KEY,INPUT_EVENT_KEYDOWN_REPEATING) ON(KeyUp,CKeyEvent,EVENT_DATA_KEY,INPUT_EVENT_KEYUP) ON(MouseDown,CMouseEvent,EVENT_DATA_MOUSE,INPUT_EVENT_MOUSEDOWN) ON(MouseUp,CMouseEvent,EVENT_DATA_MOUSE,INPUT_EVENT_MOUSEUP) ON(MouseMove,CMouseEvent,EVENT_DATA_MOUSE,INPUT_EVENT_MOUSEMOVE) ON(MouseWheel,CMouseEvent,EVENT_DATA_MOUSE,INPUT_EVENT_MOUSEWHEEL) ON(MouseMoveRelative,CMouseEvent,EVENT_DATA_MOUSE,INPUT_EVENT_MOUSEMOVE_RELATIVE) ON(MouseModeChanged,CMouseEvent,EVENT_DATA_MOUSE,INPUT_EVENT_MOUSEMODE_CHANGED) ON(Ime,CImeEvent,EVENT_DATA_IME,INPUT_EVENT_IME) ON(WindowSize,CSizeEvent,EVENT_DATA_SIZE,INPUT_EVENT_SIZE) ON(WindowFocus,CFocusEvent,EVENT_DATA_FOCUS,INPUT_EVENT_FOCUS)
#undef ON
#define REG(Name,QueueId,ObserverId,Handler,Priority) void CInputObserver::Register##Name(CObserver*o){Reg(QueueId,ObserverId,Handler,Priority,o);}void CInputObserver::Unregister##Name(CObserver*o){Unreg(QueueId,ObserverId,Handler,o);}
REG(KeyDown,EVENT_ID_KEYDOWN,INPUT_EVENT_KEYDOWN,OnKeyDown,0.0f)
REG(KeyDownRepeating,EVENT_ID_KEYDOWN_REPEATING,INPUT_EVENT_KEYDOWN_REPEATING,OnKeyRepeat,-1.0f)
REG(KeyUp,EVENT_ID_KEYUP,INPUT_EVENT_KEYUP,OnKeyUp,-1.0f)
REG(Char,EVENT_ID_CHAR,INPUT_EVENT_CHAR,OnChar,-1.0f)
REG(MouseDown,EVENT_ID_MOUSEDOWN,INPUT_EVENT_MOUSEDOWN,OnMouseDown,1.0f)
REG(MouseUp,EVENT_ID_MOUSEUP,INPUT_EVENT_MOUSEUP,OnMouseUp,-1.0f)
REG(MouseMove,EVENT_ID_MOUSEMOVE,INPUT_EVENT_MOUSEMOVE,OnMouseMove,1.0f)
REG(MouseWheel,EVENT_ID_MOUSEWHEEL,INPUT_EVENT_MOUSEWHEEL,OnMouseWheel,-1.0f)
REG(MouseMoveRelative,EVENT_ID_MOUSEMOVE_RELATIVE,INPUT_EVENT_MOUSEMOVE_RELATIVE,OnMouseMoveRelative,1.0f)
REG(MouseModeChanged,EVENT_ID_MOUSEMODE_CHANGED,INPUT_EVENT_MOUSEMODE_CHANGED,OnMouseModeChanged,-1.0f)
REG(Ime,EVENT_ID_IME,INPUT_EVENT_IME,OnIme,0.0f)
REG(WindowSize,EVENT_ID_SIZE,INPUT_EVENT_SIZE,OnWindowSize,0.0f)
REG(WindowFocus,EVENT_ID_FOCUS,INPUT_EVENT_FOCUS,OnWindowFocus,0.0f)
#undef REG
void CInputObserver::SetMouseMode(MOUSEMODE m,unsigned b){EventSetMouseMode(m,b);}
