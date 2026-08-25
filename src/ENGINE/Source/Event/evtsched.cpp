#include "ENGINE/Source/Event/event_internal.h"

#include <algorithm>
#include <chrono>
#include <thread>

CSBasePriority* CSBasePriorityQueue::Priority(void* value)const{return value?reinterpret_cast<CSBasePriority*>(static_cast<unsigned char*>(value)+m_linkOffset):nullptr;}
CSBasePriority* CSBasePriorityQueue::Link(unsigned index)const{return index<m_data.size()?Priority(m_data[index]):nullptr;}
void CSBasePriorityQueue::SetLink(unsigned index){if(auto* link=Link(index)){link->m_queue=this;link->m_index=index;}}
void CSBasePriorityQueue::Repair(unsigned index){
    if(index>=m_data.size())return;
    while(index){const unsigned parent=(index-1)/2;if(Link(parent)->Compare(Link(index)))break;std::swap(m_data[parent],m_data[index]);SetLink(parent);SetLink(index);index=parent;}
    for(;;){const unsigned left=index*2+1;if(left>=m_data.size())break;unsigned best=left;const unsigned right=left+1;if(right<m_data.size()&&Link(right)->Compare(Link(left)))best=right;if(Link(index)->Compare(Link(best)))break;std::swap(m_data[index],m_data[best]);SetLink(index);SetLink(best);index=best;}
}
void CSBasePriorityQueue::Enqueue(void* value){if(!value)return;m_data.push_back(value);SetLink(static_cast<unsigned>(m_data.size()-1));Repair(static_cast<unsigned>(m_data.size()-1));}
void CSBasePriorityQueue::Remove(unsigned index){if(index>=m_data.size())return;if(auto* link=Link(index)){link->m_queue=nullptr;link->m_index=0;}if(index+1==m_data.size()){m_data.pop_back();return;}m_data[index]=m_data.back();m_data.pop_back();SetLink(index);Repair(index);}
CSBasePriorityQueue::~CSBasePriorityQueue(){while(!m_data.empty())Remove(static_cast<unsigned>(m_data.size()-1));}
void CSBasePriority::Relink(){if(m_queue)m_queue->Repair(m_index);}
void CSBasePriority::Unlink(){if(m_queue)m_queue->Remove(m_index);}

void IEvtSchedulerInitialize(unsigned){EventInternal::shutdown=false;}

HEVENTCONTEXT IEvtSchedulerCreateContext(EventHandler initializeHandler,EventHandler idleHandler,EventHandler destroyHandler,unsigned idleTime,char){
    auto context=std::make_unique<EvtContext>();
    context->initializeHandler=initializeHandler;context->idleHandler=idleHandler;context->destroyHandler=destroyHandler;
    context->idleTime=idleTime?idleTime:1;context->initialIdleTime=context->idleTime;context->currTime=EventInternal::TimeMs();context->lastIdleTime=context->currTime;context->interactive=initializeHandler!=nullptr;
    auto handle=std::make_unique<HEVENTCONTEXT__>();handle->id=EventInternal::nextContextId++;handle->context=context.release();HEVENTCONTEXT result=handle.get();
    {std::lock_guard lock(EventInternal::mutex);EventInternal::contexts.push_back(std::move(handle));}
    if(initializeHandler)IEvtQueueRegister(result,EVENT_ID_INITIALIZE,initializeHandler,nullptr,1000.0f);
    if(destroyHandler)IEvtQueueRegister(result,EVENT_ID_DESTROY,destroyHandler,nullptr,1000.0f);
    return result;
}

int IEvtSchedulerIsContextInteractive(HEVENTCONTEXT h){auto*c=EventInternal::Resolve(h);return c&&c->interactive&&!c->closed&&!c->destroyed;}

int SynthesizeInitialize(HEVENTCONTEXT h){
    auto*c=EventInternal::Resolve(h);if(!c)return 0;{std::lock_guard lock(c->mutex);if(c->flags&1u)return 0;c->flags|=1u;c->lastIdleTime=c->currTime;}
    auto previous=EventInternal::current;EventInternal::current=h;IEvtQueueDispatch(EVENT_ID_INITIALIZE,nullptr,0);EventInternal::current=previous;return 1;
}

void SynthesizeDestroy(HEVENTCONTEXT h){
    auto*c=EventInternal::Resolve(h);if(!c)return;auto previous=EventInternal::current;EventInternal::current=h;
    if(!(c->flags&1u))SynthesizeInitialize(h);
    {std::lock_guard lock(c->mutex);if(c->destroyed){EventInternal::current=previous;return;}c->closed=true;}
    IEvtQueueDispatch(EVENT_ID_CLOSE,nullptr,0);IEvtQueueDispatchAll(h);IEvtQueueDispatch(EVENT_ID_DESTROY,nullptr,0);{std::lock_guard lock(c->mutex);c->destroyed=true;c->flags|=2u;}EventInternal::current=previous;
}

void SynthesizeIdle(HEVENTCONTEXT h){
    auto*c=EventInternal::Resolve(h);if(!c||c->closed||c->destroyed)return;const unsigned now=EventInternal::TimeMs();unsigned previous;
    {std::lock_guard lock(c->mutex);previous=c->lastIdleTime;c->lastIdleTime=now;c->currTime=now;}
    EVENT_DATA_IDLE data{static_cast<float>(static_cast<std::int32_t>(now-previous))*0.001f,now};IEvtQueueDispatch(EVENT_ID_IDLE,&data,sizeof(data));
    if(c->idleHandler)c->idleHandler(&data,nullptr);
}
void SynthesizePoll(HEVENTCONTEXT h){auto*c=EventInternal::Resolve(h);if(c&&!c->closed&&!c->destroyed)IEvtQueueDispatch(EVENT_ID_POLL,nullptr,0);}
void SynthesizePaint(HEVENTCONTEXT h){auto*c=EventInternal::Resolve(h);if(c&&!c->closed&&!c->destroyed)IEvtQueueDispatch(EVENT_ID_PAINT,nullptr,0);}

static bool ProcessContext(HEVENTCONTEXT h){
    auto*c=EventInternal::Resolve(h);if(!c||c->destroyed)return false;EventInternal::current=h;bool work=false;
    {std::lock_guard lock(c->mutex);c->currTime=EventInternal::TimeMs();}
    work|=SynthesizeInitialize(h)!=0;IEvtTimerDispatch(h);
    int inputShutdown=0;work|=IEvtInputProcess(&inputShutdown,h)!=0;if(inputShutdown)EventPostCloseEx(h);
    SynthesizePoll(h);const bool hadMessages=IEvtQueueHasMessages(h)!=0;work|=hadMessages;IEvtQueueDispatchAll(h);SynthesizeIdle(h);SynthesizePaint(h);
    if(c->closed)SynthesizeDestroy(h);EventInternal::current=nullptr;return work;
}

void IEvtSchedulerProcess(){
    while(!EventInternal::shutdown){
        std::vector<HEVENTCONTEXT> list;{std::lock_guard lock(EventInternal::mutex);for(auto&handle:EventInternal::contexts)if(handle&&handle->context&&!handle->context->destroyed)list.push_back(handle.get());}
        if(list.empty())break;bool work=false;for(auto h:list)work|=ProcessContext(h);if(!work)std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}

void IEvtSchedulerShutdown(){EventInternal::shutdown=true;}
void IEvtSchedulerDestroy(){
    IEvtSchedulerShutdown();std::vector<HEVENTCONTEXT> list;{std::lock_guard lock(EventInternal::mutex);for(auto&handle:EventInternal::contexts)if(handle&&handle->context)list.push_back(handle.get());}
    for(auto h:list)if(auto*c=EventInternal::Resolve(h);c&&!c->destroyed)SynthesizeDestroy(h);
    std::lock_guard lock(EventInternal::mutex);for(auto&handle:EventInternal::contexts)if(handle&&handle->context){delete handle->context;handle->context=nullptr;}EventInternal::contexts.clear();EventInternal::current=nullptr;
}

void EventProcessStart(void*){}
void EventProcessOnce(){
    const bool previous=EventInternal::shutdown.exchange(true);EventInternal::shutdown=false;std::vector<HEVENTCONTEXT>list;{std::lock_guard lock(EventInternal::mutex);for(auto&handle:EventInternal::contexts)if(handle&&handle->context&&!handle->context->destroyed)list.push_back(handle.get());}for(auto h:list)ProcessContext(h);EventInternal::shutdown=previous;
}
void EventProcessDone(){}
unsigned SchedulerThreadProc(void*){IEvtSchedulerProcess();return 0;}
unsigned InitializeSchedulerThread(unsigned slot){return slot;}
void DestroySchedulerThread(unsigned){}
void DetachContextFromThread(EvtThread* thread,HEVENTCONTEXT h){if(!thread)return;std::erase(thread->m_contextQueue,h);thread->m_contextCount=static_cast<unsigned>(thread->m_contextQueue.size());}
HEVENTCONTEXT GetNextContext(unsigned index){std::lock_guard lock(EventInternal::mutex);return index<EventInternal::contexts.size()?EventInternal::contexts[index].get():nullptr;}
void* GetWakeEvent(unsigned){return nullptr;}
void PutContext(HEVENTCONTEXT){}
HEVENTCONTEXT AttachContextToThread(HEVENTCONTEXT h,EvtThread* thread){if(thread&&h){thread->m_contextQueue.push_back(h);thread->m_contextCount=static_cast<unsigned>(thread->m_contextQueue.size());}return h;}
unsigned __stdcall ShutdownThreadProc(void*){IEvtSchedulerShutdown();return 0;}
