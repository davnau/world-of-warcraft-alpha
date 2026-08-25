#pragma once

#include <cstdint>

enum EVENTID : std::int32_t { EVENT_ID_CAPTURECHANGED,EVENT_ID_CHAR,EVENT_ID_FOCUS,EVENT_ID_CLOSE,EVENT_ID_DESTROY,EVENT_ID_IDLE,EVENT_ID_POLL,EVENT_ID_INITIALIZE,EVENT_ID_KEYDOWN,EVENT_ID_KEYUP,EVENT_ID_KEYDOWN_REPEATING,EVENT_ID_MOUSEDOWN,EVENT_ID_MOUSEMOVE,EVENT_ID_MOUSEMOVE_RELATIVE,EVENT_ID_MOUSEUP,EVENT_ID_MOUSEMODE_CHANGED,EVENT_ID_MOUSEWHEEL,EVENT_ID_PAINT,EVENT_ID_NET_DATA,EVENT_ID_NET_CONNECT,EVENT_ID_NET_DISCONNECT,EVENT_ID_NET_CANTCONNECT,EVENT_ID_NET_DESTROY,EVENT_ID_CONSOLE_INPUT,EVENT_ID_ENGINENET,EVENT_ID_BATTLENET,EVENT_ID_WOW_Q_IDLE,EVENT_ID_IME,EVENT_ID_SIZE,EVENTIDS };
enum KEY : std::int32_t {
    KEY_NONE = -1,
    KEY_SHIFT = 0,
    KEY_CONTROL = 1,
    KEY_ALT = 2,
    KEY_SPACE = 0x20,
    KEY_0 = 0x30,
    KEY_A = 0x41,
    KEY_Z = 0x5A,
    KEY_GRAVE = 0x100,
    KEY_NUMPAD0 = 0x101,
    KEY_NUMPAD1,
    KEY_NUMPAD2,
    KEY_NUMPAD3,
    KEY_NUMPAD4,
    KEY_NUMPAD5,
    KEY_NUMPAD6,
    KEY_NUMPAD7,
    KEY_NUMPAD8,
    KEY_NUMPAD9,
    KEY_NUMPADPLUS,
    KEY_NUMPADMINUS,
    KEY_NUMPADMULTIPLY,
    KEY_NUMPADDIVIDE,
    KEY_NUMPADDECIMAL,
    KEY_EQUALS,
    KEY_MINUS,
    KEY_LEFTBRACKET,
    KEY_RIGHTBRACKET,
    KEY_SLASH,
    KEY_BACKSLASH,
    KEY_SEMICOLON,
    KEY_APOSTROPHE,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_ESCAPE = 0x200,
    KEY_ENTER,
    KEY_BACKSPACE,
    KEY_TAB,
    KEY_LEFT,
    KEY_UP,
    KEY_RIGHT,
    KEY_DOWN,
    KEY_INSERT,
    KEY_DELETE,
    KEY_HOME,
    KEY_END,
    KEY_PAGEUP,
    KEY_PAGEDOWN,
    KEY_CAPSLOCK,
    KEY_NUMLOCK,
    KEY_SCROLLLOCK,
    KEY_PAUSE,
    KEY_PRINTSCREEN,
    KEY_F1 = 0x300,
    KEY_F12 = 0x30B,
    KEY_LAST = 0x30C
};
enum MOUSEMODE : std::int32_t { MOUSE_MODE_NORMAL,MOUSE_MODE_RELATIVE,MOUSE_MODES };
enum MOUSEBUTTON : std::int32_t { MOUSE_BUTTON_NONE=0,MOUSE_BUTTON_LEFT=1,MOUSE_BUTTON_MIDDLE=2,MOUSE_BUTTON_RIGHT=4,MOUSE_BUTTON_XBUTTON1=8,MOUSE_BUTTON_XBUTTON2=16,MOUSE_BUTTON_ALL=-1 };

struct EVENT_DATA_CHAR { int ch;unsigned metaKeyState,repeat; };
struct EVENT_DATA_KEY { KEY key;unsigned metaKeyState,repeat,time; };
struct EVENT_DATA_MOUSE { MOUSEMODE mode;MOUSEBUTTON button;unsigned buttonState,metaKeyState,flags;float x,y;int wheelDistance;unsigned time; };
struct EVENT_DATA_TIMER { float elapsedSec;unsigned currTime; };
struct EVENT_DATA_FOCUS { int focus; };struct EVENT_DATA_IME { unsigned message,wParam,lParam,codepage; };struct EVENT_DATA_SIZE { int w,h; };struct EVENT_DATA_IDLE { float elapsedSec;unsigned time; };
struct TRefCnt { virtual ~TRefCnt()=default;unsigned m_refcnt=0; };
struct CEvent: TRefCnt { unsigned id;void* param; CEvent(unsigned eventId=0,void* eventParam=nullptr):id(eventId),param(eventParam){} };
enum INPUTEVENTID : unsigned { INPUT_EVENT_SIZE=0x40040064u,INPUT_EVENT_FOCUS=0x40040065u,INPUT_EVENT_MOUSEDOWN=0x400500C8u,INPUT_EVENT_MOUSEUP=0x400500C9u,INPUT_EVENT_MOUSEMOVE=0x400500CAu,INPUT_EVENT_MOUSEMOVE_RELATIVE=0x400500CBu,INPUT_EVENT_MOUSEMODE_CHANGED=0x400500CCu,INPUT_EVENT_MOUSEWHEEL=0x400500CDu,INPUT_EVENT_KEYDOWN=0x40060064u,INPUT_EVENT_KEYDOWN_REPEATING=0x40060065u,INPUT_EVENT_KEYUP=0x40060066u,INPUT_EVENT_CHAR=0x40060067u,INPUT_EVENT_IME=0x40060068u };
struct CCharEvent:CEvent,EVENT_DATA_CHAR{};struct CKeyEvent:CEvent,EVENT_DATA_KEY{};struct CMouseEvent:CEvent,EVENT_DATA_MOUSE{};struct CImeEvent:CEvent,EVENT_DATA_IME{};struct CSizeEvent:CEvent,EVENT_DATA_SIZE{};struct CFocusEvent:CEvent,EVENT_DATA_FOCUS{};
struct HEVENTCONTEXT__;using HEVENTCONTEXT=HEVENTCONTEXT__*;
struct EventRect { float left,top,right,bottom; };
using EventHandler=int (__fastcall*)(const void*,void*);using EventGuidHandler=int (__fastcall*)(const void*,std::uint64_t,void*);using EventScanner=void (__fastcall*)(EVENTID,const void*,void*);

void EventInitialize(unsigned netServer=0,unsigned schedulerThreads=0);void EventDestroy();void EventDoMessageLoop();void EventInitiateShutdown();
HEVENTCONTEXT EventCreateContextEx(EventHandler initializeHandler,EventHandler idleHandler,EventHandler destroyHandler,unsigned idleTime,char debugFlags=0);
void EventCreateContext(EventHandler initializeHandler,EventHandler idleHandler,EventHandler destroyHandler);
int EventIsContextInteractive(HEVENTCONTEXT context=nullptr);HEVENTCONTEXT EventGetCurrentContext();void EventSetContextIdleTime(unsigned idleTime,HEVENTCONTEXT context=nullptr);int EventGetContextIdleTime(HEVENTCONTEXT context=nullptr);
int EventIsButtonDown(unsigned button);int EventIsKeyDown(unsigned key);void EventPostClose(HEVENTCONTEXT context=nullptr);void EventPostCloseEx(HEVENTCONTEXT context);
int EventQueuePost(HEVENTCONTEXT context,EVENTID id,const void* data,unsigned bytes);int EventQueueScan(EventScanner scanner,void* param);
void EventRegister(EventHandler handler);void EventRegisterEx(unsigned id,EventHandler handler,void* param=nullptr,float priority=0);void EventUnregister(EVENTID id,EventHandler handler);void EventUnregisterEx(unsigned id,EventHandler handler,void* param=nullptr,unsigned flags=~0u);
void EventSetConfirmCloseCallback(int (__fastcall* callback)(void*),void* param);int EventInputProcess(HEVENTCONTEXT context=nullptr);
unsigned EventSetTimer(EventHandler handler,void* param,float timeout);unsigned EventSetTimer(unsigned timeoutMs,EventHandler handler,void* param);unsigned EventSetTimer(EventGuidHandler handler,std::uint64_t param,void* param2,float timeout);unsigned EventSetTimer(unsigned timeoutMs,EventGuidHandler handler,std::uint64_t param,void* param2);
unsigned EventSetTimerAbsolute(unsigned triggerTime,EventHandler handler,void* param);unsigned EventSetTimerAbsolute(unsigned triggerTime,EventGuidHandler handler,std::uint64_t param,void* param2);
void EventKillTimer(unsigned timerId,EventHandler handler=nullptr,const char* functionName=nullptr);double EventGetRemainingTime(unsigned timerId);
void EventSetMouseMode(MOUSEMODE mode,unsigned holdButton=0);void EventInputGetMousePosition(float* x,float* y);void EventInputSetMousePosition(float x,float y);void EventSetMouseBoundingRect(const EventRect* rect);

void IEvtQueueInitialize();void IEvtQueueDestroy();int IEvtQueueCheckSyncKeyState(HEVENTCONTEXT,KEY);int IEvtQueueCheckSyncMouseState(HEVENTCONTEXT,MOUSEBUTTON);void IEvtQueueDispatch(EVENTID,const void*,unsigned);int IEvtQueueHasMessages(HEVENTCONTEXT);int IEvtQueueDispatchNext(HEVENTCONTEXT);void IEvtQueueDispatchAll(HEVENTCONTEXT);
struct EvtMessage;EvtMessage* MessageAlloc(unsigned bytes);void MessageFree(EvtMessage*);void IEvtQueuePost(HEVENTCONTEXT,EVENTID,const void*,unsigned);void IEvtQueueScan(HEVENTCONTEXT,EventScanner,void*);void IEvtQueueRegister(HEVENTCONTEXT,EVENTID,EventHandler,void*,float=0);void IEvtQueueUnregister(HEVENTCONTEXT,EVENTID,EventHandler,void* param=nullptr,unsigned flags=~0u);void ResetSyncState(HEVENTCONTEXT);void UpdateSyncKeyState(HEVENTCONTEXT,KEY,EVENTID*);void UpdateSyncMouseState(HEVENTCONTEXT,MOUSEBUTTON,unsigned);void UpdateSyncState(EVENTID*,const void*,HEVENTCONTEXT);
void IEvtInputInitialize();void IEvtInputDestroy();int IEvtInputProcess(int* shutdown,HEVENTCONTEXT);void IEvtInputSetMouseMode(MOUSEMODE,unsigned);void IEvtInputGetMousePosition(float*,float*);void IEvtInputSetMousePosition(float,float);void IEvtInputSetMouseBoundingRect(const EventRect*);
void IEvtInputSetConfirmCloseCallback(int (__fastcall*)(void*),void*);int ResetAsyncState();int ConfirmClose();void ConvertPosition(int clientX,int clientY,float* x,float* y);void UnconvertPosition(float x,float y,int* clientX,int* clientY);
void ProcessInput(HEVENTCONTEXT,EVENTID,const void*,unsigned,int* shutdown);
void IEvtSchedulerInitialize(unsigned threads=0);void IEvtSchedulerDestroy();void IEvtSchedulerProcess();void IEvtSchedulerShutdown();HEVENTCONTEXT IEvtSchedulerCreateContext(EventHandler,EventHandler,EventHandler,unsigned,char);int IEvtSchedulerIsContextInteractive(HEVENTCONTEXT);
unsigned IEvtTimerSet(HEVENTCONTEXT,float,EventHandler,void*,EventGuidHandler=nullptr,std::uint64_t=0,void* guidParam2=nullptr);unsigned IEvtTimerSetAbsolute(HEVENTCONTEXT,unsigned,EventHandler,void*,EventGuidHandler=nullptr,std::uint64_t=0,void* guidParam2=nullptr);void IEvtTimerKill(HEVENTCONTEXT,unsigned,EventHandler=nullptr,const char* functionName=nullptr);double IEvtTimerGetRemaining(HEVENTCONTEXT,unsigned);unsigned IEvtTimerGetNextTime(HEVENTCONTEXT,unsigned);void IEvtTimerDispatch(HEVENTCONTEXT);
