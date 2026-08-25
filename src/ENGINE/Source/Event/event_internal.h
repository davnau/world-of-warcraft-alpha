#pragma once
#include "ENGINE/Source/Event/event.h"
#include <atomic>
#include <chrono>
#include <deque>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>
#include <list>

class CSBasePriorityQueue;
class CSBasePriority {
public:
    virtual ~CSBasePriority()=default;virtual int Compare(const CSBasePriority* other)const=0;
    void Relink();void Unlink();CSBasePriorityQueue* m_queue=nullptr;unsigned m_index=0;
};
class CSBasePriorityQueue {
public:
    explicit CSBasePriorityQueue(unsigned linkOffset=0):m_linkOffset(linkOffset){}~CSBasePriorityQueue();
    CSBasePriority* Link(unsigned index)const;void SetLink(unsigned index);void Enqueue(void* value);void Remove(unsigned index);void* Dequeue();
    std::vector<void*> m_data;unsigned m_linkOffset;
private:friend class CSBasePriority;void Repair(unsigned index);CSBasePriority* Priority(void* value)const;
};

struct EvtMessage { EVENTID id;std::vector<std::uint8_t> data; };
struct EvtHandler { EventHandler func;void* param;float priority;int marker; };
struct EvtTimer { unsigned id;std::chrono::steady_clock::time_point target;float timeout;EventHandler handler;void* param;EventGuidHandler guidHandler;std::uint64_t guidParam;void* guidParam2; };
struct EvtContext { std::recursive_mutex mutex;unsigned currTime=0;unsigned lastIdleTime=0;unsigned idleTime=500;unsigned initialIdleTime=500;unsigned flags=0;bool interactive=true,closed=false,destroyed=false;std::deque<EvtMessage> queue;std::unordered_map<unsigned,std::list<EvtHandler>> handlers;std::unordered_map<unsigned,EvtTimer> timers;unsigned keyState[KEY_LAST+1]{};unsigned buttonState=0;EventHandler initializeHandler=nullptr,idleHandler=nullptr,destroyHandler=nullptr;void* callContext=nullptr;int IsCurrentContext() const; };
struct EvtThread { unsigned m_threadSlot=0,m_threadCount=1,m_weightTotal=0,m_weightAvg=0,m_contextCount=0,m_rebalance=0;std::vector<HEVENTCONTEXT> m_contextQueue; };
struct HEVENTCONTEXT__ { unsigned id;EvtContext* context; };
namespace EventInternal {extern std::recursive_mutex mutex;extern std::vector<std::unique_ptr<HEVENTCONTEXT__>> contexts;extern std::atomic<unsigned> nextContextId;extern std::atomic<unsigned> nextTimerId;extern std::atomic<bool> shutdown;extern thread_local HEVENTCONTEXT current;EvtContext* Resolve(HEVENTCONTEXT);HEVENTCONTEXT CurrentOr(HEVENTCONTEXT);unsigned TimeMs();int Dispatch(HEVENTCONTEXT,EVENTID,const void*,unsigned);}

unsigned SchedulerThreadProc(void*);int SynthesizeInitialize(HEVENTCONTEXT);void SynthesizeDestroy(HEVENTCONTEXT);void SynthesizeIdle(HEVENTCONTEXT);void SynthesizePoll(HEVENTCONTEXT);void SynthesizePaint(HEVENTCONTEXT);unsigned InitializeSchedulerThread(unsigned slot);void DestroySchedulerThread(unsigned slot);void DetachContextFromThread(EvtThread*,HEVENTCONTEXT);HEVENTCONTEXT GetNextContext(unsigned index);void* GetWakeEvent(unsigned index);void PutContext(HEVENTCONTEXT);HEVENTCONTEXT AttachContextToThread(HEVENTCONTEXT,EvtThread*);unsigned __stdcall ShutdownThreadProc(void*);
