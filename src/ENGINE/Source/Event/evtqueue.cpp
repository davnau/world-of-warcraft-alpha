#include "ENGINE/Source/Event/event_internal.h"

#include <algorithm>
#include <cstring>
#include <iterator>

namespace EventInternal {
std::recursive_mutex mutex;
std::vector<std::unique_ptr<HEVENTCONTEXT__>> contexts;
std::atomic<unsigned> nextContextId{1};
std::atomic<unsigned> nextTimerId{1};
std::atomic<bool> shutdown{false};
thread_local HEVENTCONTEXT current=nullptr;
EvtContext* Resolve(HEVENTCONTEXT h){return h?h->context:(current?current->context:nullptr);}
HEVENTCONTEXT CurrentOr(HEVENTCONTEXT h){return h?h:current;}
unsigned TimeMs(){return static_cast<unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count());}
int Dispatch(HEVENTCONTEXT h,EVENTID id,const void* data,unsigned){
    auto* context=Resolve(h);if(!context)return 0;
    std::unique_lock lock(context->mutex);
    auto listIt=context->handlers.find(static_cast<unsigned>(id));if(listIt==context->handlers.end())return 0;
    auto& handlers=listIt->second;handlers.push_front({nullptr,nullptr,0.0f,1});auto marker=handlers.begin();int handled=0;
    for(;;){
        auto next=std::next(marker);if(next==handlers.end())break;
        handlers.splice(marker,handlers,next);const EvtHandler entry=*std::prev(marker);
        if(!entry.func||entry.marker)continue;
        if(entry.func(data,entry.param))handled=1;else break;
    }
    handlers.erase(marker);return handled;
}
}

void IEvtQueueInitialize(){}
void IEvtQueueDestroy(){
    std::lock_guard lock(EventInternal::mutex);
    for(auto& handle:EventInternal::contexts)if(handle&&handle->context){std::lock_guard contextLock(handle->context->mutex);handle->context->queue.clear();handle->context->handlers.clear();}
}
int IEvtQueueCheckSyncKeyState(HEVENTCONTEXT h,KEY key){auto*c=EventInternal::Resolve(h);return c&&key>=0&&key<=KEY_LAST&&c->keyState[key]!=0;}
int IEvtQueueCheckSyncMouseState(HEVENTCONTEXT h,MOUSEBUTTON button){auto*c=EventInternal::Resolve(h);return c&&(c->buttonState&static_cast<unsigned>(button))!=0;}
void IEvtQueueDispatch(EVENTID id,const void* data,unsigned bytes){if(id==EVENT_ID_CAPTURECHANGED||id<0||id>=EVENTIDS)return;EVENTID actual=id;UpdateSyncState(&actual,data,EventInternal::current);EventInternal::Dispatch(EventInternal::current,actual,data,bytes);}
int IEvtQueueHasMessages(HEVENTCONTEXT h){auto*c=EventInternal::Resolve(h);if(!c)return 0;std::lock_guard lock(c->mutex);return!c->queue.empty();}
int IEvtQueueDispatchNext(HEVENTCONTEXT h){
    h=EventInternal::CurrentOr(h);auto*c=EventInternal::Resolve(h);if(!c)return 0;EvtMessage message;bool remaining=false;
    {std::lock_guard lock(c->mutex);if(c->queue.empty())return 0;message=std::move(c->queue.front());c->queue.pop_front();remaining=!c->queue.empty();}
    auto previous=EventInternal::current;EventInternal::current=h;IEvtQueueDispatch(message.id,message.data.empty()?nullptr:message.data.data(),static_cast<unsigned>(message.data.size()));EventInternal::current=previous;return remaining?1:0;
}
void IEvtQueueDispatchAll(HEVENTCONTEXT h){
    h=EventInternal::CurrentOr(h);auto*c=EventInternal::Resolve(h);if(!c)return;std::deque<EvtMessage> pending;
    {std::lock_guard lock(c->mutex);pending.swap(c->queue);}
    auto previous=EventInternal::current;EventInternal::current=h;for(auto&message:pending)IEvtQueueDispatch(message.id,message.data.empty()?nullptr:message.data.data(),static_cast<unsigned>(message.data.size()));EventInternal::current=previous;
}
int EventQueuePost(HEVENTCONTEXT h,EVENTID id,const void*data,unsigned bytes){
    h=EventInternal::CurrentOr(h);auto*c=EventInternal::Resolve(h);if(!c||id<0||id>=EVENTIDS||(!data&&bytes))return 0;EvtMessage message;message.id=id;message.data.resize(bytes);if(bytes)std::memcpy(message.data.data(),data,bytes);std::lock_guard lock(c->mutex);c->queue.push_back(std::move(message));return 1;
}
int EventQueueScan(EventScanner scanner,void*param){
    auto*c=EventInternal::Resolve(EventInternal::current);if(!c||!scanner)return 0;std::deque<EvtMessage> pending;{std::lock_guard lock(c->mutex);pending.swap(c->queue);}for(auto&message:pending)scanner(message.id,message.data.empty()?nullptr:message.data.data(),param);{std::lock_guard lock(c->mutex);std::deque<EvtMessage> merged;merged.insert(merged.end(),pending.begin(),pending.end());merged.insert(merged.end(),c->queue.begin(),c->queue.end());c->queue.swap(merged);}return static_cast<int>(pending.size());
}
EvtMessage*MessageAlloc(unsigned bytes){auto*m=new EvtMessage{};m->data.resize(bytes);return m;}
void MessageFree(EvtMessage*m){delete m;}
void IEvtQueuePost(HEVENTCONTEXT h,EVENTID id,const void*data,unsigned bytes){EventQueuePost(h,id,data,bytes);}
void IEvtQueueScan(HEVENTCONTEXT h,EventScanner scanner,void*param){
    auto*c=EventInternal::Resolve(h);if(!c||!scanner)return;std::deque<EvtMessage>pending;{std::lock_guard lock(c->mutex);pending.swap(c->queue);}for(auto&message:pending)scanner(message.id,message.data.empty()?nullptr:message.data.data(),param);{std::lock_guard lock(c->mutex);std::deque<EvtMessage>merged;merged.insert(merged.end(),pending.begin(),pending.end());merged.insert(merged.end(),c->queue.begin(),c->queue.end());c->queue.swap(merged);}
}
void IEvtQueueRegister(HEVENTCONTEXT h,EVENTID id,EventHandler handler,void*param,float priority){
    auto*c=EventInternal::Resolve(h);if(!c||!handler||id<0||id>=EVENTIDS)return;std::lock_guard lock(c->mutex);auto&handlers=c->handlers[static_cast<unsigned>(id)];EvtHandler entry{handler,param,priority,0};auto pos=std::find_if(handlers.begin(),handlers.end(),[&](const EvtHandler&e){return!e.marker&&e.priority<=priority;});handlers.insert(pos,entry);
}
void IEvtQueueUnregister(HEVENTCONTEXT h,EVENTID id,EventHandler handler,void*param,unsigned flags){
    auto*c=EventInternal::Resolve(h);if(!c)return;std::lock_guard lock(c->mutex);for(unsigned current=0;current<EVENTIDS;++current){if((flags&1)&&current!=static_cast<unsigned>(id))continue;auto listIt=c->handlers.find(current);if(listIt==c->handlers.end())continue;auto&handlers=listIt->second;for(auto it=handlers.begin();it!=handlers.end();){const bool match=!it->marker&&(!(flags&2)||it->func==handler)&&(!(flags&4)||it->param==param);if(match)it=handlers.erase(it);else++it;}}}
void ResetSyncState(HEVENTCONTEXT h){auto*c=EventInternal::Resolve(h);if(!c)return;std::lock_guard lock(c->mutex);std::fill(std::begin(c->keyState),std::end(c->keyState),0);c->buttonState=0;}
void UpdateSyncKeyState(HEVENTCONTEXT h,KEY key,EVENTID*id){auto*c=EventInternal::Resolve(h);if(!c||!id||key<0||key>KEY_LAST)return;std::lock_guard lock(c->mutex);const bool wasDown=c->keyState[key]!=0;if(*id==EVENT_ID_KEYDOWN){if(wasDown)*id=EVENT_ID_KEYDOWN_REPEATING;c->keyState[key]=1;}else if(*id==EVENT_ID_KEYUP)c->keyState[key]=0;}
void UpdateSyncMouseState(HEVENTCONTEXT h,MOUSEBUTTON button,unsigned down){auto*c=EventInternal::Resolve(h);if(!c)return;std::lock_guard lock(c->mutex);if(down)c->buttonState|=static_cast<unsigned>(button);else c->buttonState&=~static_cast<unsigned>(button);}
void UpdateSyncState(EVENTID*id,const void*data,HEVENTCONTEXT h){if(!id)return;if(*id==EVENT_ID_FOCUS){ResetSyncState(h);return;}if(!data)return;if(*id==EVENT_ID_KEYDOWN||*id==EVENT_ID_KEYDOWN_REPEATING||*id==EVENT_ID_KEYUP){const auto*key=static_cast<const EVENT_DATA_KEY*>(data);UpdateSyncKeyState(h,key->key,id);}else if(*id==EVENT_ID_MOUSEDOWN||*id==EVENT_ID_MOUSEUP){const auto*mouse=static_cast<const EVENT_DATA_MOUSE*>(data);UpdateSyncMouseState(h,mouse->button,*id==EVENT_ID_MOUSEDOWN);}}
