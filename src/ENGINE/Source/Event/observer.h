#pragma once
#include "ENGINE/Source/Event/event.h"
#include <memory>
#include <list>
#include <unordered_map>
#include <vector>

class CObserver;
class EventReg {
public:
    struct EVENTCALLBACKREG { EventHandler callback;void* param; };
    struct EVENTDISPATCHREG { CObserver* pObserver;int expectedEventId; };
    EventReg();~EventReg();
    void RegisterCallback(EventHandler,void*);void RegisterEvent(int,CObserver*);void UnregisterCallback(EventHandler);void UnregisterEvent(CObserver*);
    void CleanupCallbacks();void CleanupEvents();int IsCallbackRegistered(EventHandler)const;int IsEventRegistered(CObserver*)const;int DispatchCallback(CEvent*);int DispatchEvent(CEvent*);
    unsigned flags=0;std::list<EVENTCALLBACKREG> callbackList;std::list<EVENTDISPATCHREG> dispatchList;
};
class EventRegistry {
public:
    EventReg* InternalNew(unsigned id);void InternalDelete(EventReg* reg);EventReg* Find(unsigned id);
private:std::unordered_map<unsigned,std::unique_ptr<EventReg>> m_regs;
};
class CObserver : public TRefCnt {
public:
    CObserver();virtual ~CObserver();virtual int OnEvent(const CEvent* event);
    void ClearRegistry();EventRegistry* GetRegistry(int create=1);EventReg* GetEventReg(unsigned eventId,int create=1);
    int DispatchEvent(CEvent* event);int DispatchEvent(unsigned id,CEvent* event);
    void RegisterCallback(unsigned id,EventHandler callback,void* param=nullptr);void RegisterEvent(unsigned id,int expectedEventId,CObserver* observer);
    void UnregisterCallback(unsigned id,EventHandler callback);void UnregisterEvent(unsigned id,CObserver* observer);
    int IsEventRegistered(unsigned id);EventReg* IsEventRegisteredBy(unsigned id,CObserver* observer);
private:std::unique_ptr<EventRegistry> m_registry;
};
void ObserverInitialize();void ObserverDestroy();
