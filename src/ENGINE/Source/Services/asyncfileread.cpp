#include "ENGINE/Source/Services/asyncfileread.h"

#include "Storm/h/storm.h"

#include <algorithm>
#include <condition_variable>
#include <deque>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {
std::mutex s_queueLock;
std::condition_variable s_queueEvent;
std::deque<CAsyncObject*> s_asyncFileReadList;
std::deque<CAsyncObject*> s_asyncFileReadPostList;
std::vector<std::unique_ptr<CAsyncObject>> s_objects;
std::vector<ASYNCFILEREAD_HANDLER> s_handlers;
std::thread s_asyncReadThread;
CAsyncObject* s_asyncCurrentObject = nullptr;
CAsyncObject* s_asyncWaitObject = nullptr;
bool s_initialized = false;
bool s_shutdown = false;

void Remove(std::deque<CAsyncObject*>& queue, CAsyncObject* object) {
    queue.erase(std::remove(queue.begin(), queue.end(), object), queue.end());
}

void Validate(const CAsyncObject* object) {
    if (!object) throw std::invalid_argument("async object");
    if (!object->file) throw std::invalid_argument("async object file");
    if (!object->buffer) throw std::invalid_argument("async object buffer");
    if (!object->size) throw std::invalid_argument("async object size");
    if (!object->userPostloadCallback) throw std::invalid_argument("async object postload callback");
}

void Worker() {
    for (;;) {
        CAsyncObject* object = nullptr;
        {
            std::unique_lock lock(s_queueLock);
            s_queueEvent.wait(lock, [] { return s_shutdown || !s_asyncFileReadList.empty(); });
            if (s_shutdown && s_asyncFileReadList.empty()) return;
            object = s_asyncFileReadList.front();
            s_asyncFileReadList.pop_front();
            object->queued = false;
            s_asyncCurrentObject = object;
        }
        if (object->critSect) object->critSect->Enter();
        SFile::SetFilePointer(object->file, static_cast<int>(object->offset), nullptr, FILE_BEGIN);
        SFile::Read(object->file, object->buffer, object->size, nullptr, nullptr, nullptr);
        if (object->critSect) object->critSect->Leave();
        {
            std::lock_guard lock(s_queueLock);
            object->completed = true;
            s_asyncFileReadPostList.push_back(object);
            s_asyncCurrentObject = nullptr;
        }
        s_queueEvent.notify_all();
    }
}
}

void AsyncFileReadInitialize() {
    std::lock_guard lock(s_queueLock);
    if (s_initialized) return;
    s_shutdown = false;
    s_asyncCurrentObject = nullptr;
    s_asyncWaitObject = nullptr;
    s_initialized = true;
    s_asyncReadThread = std::thread(Worker);
}

unsigned __stdcall AsyncFileReadThread(void*) { Worker(); return 0; }

void AsyncFileReadDestroy() {
    {
        std::lock_guard lock(s_queueLock);
        if (!s_initialized) return;
        s_shutdown = true;
    }
    s_queueEvent.notify_all();
    if (s_asyncReadThread.joinable()) s_asyncReadThread.join();
    AsyncFileReadPollHandler(nullptr);
    std::lock_guard lock(s_queueLock);
    s_asyncFileReadList.clear();
    s_asyncFileReadPostList.clear();
    s_objects.clear();
    s_handlers.clear();
    s_asyncCurrentObject = nullptr;
    s_asyncWaitObject = nullptr;
    s_initialized = false;
}

void AsyncFileReadAddHandler(ASYNCFILEREAD_HANDLER handler) {
    if (!handler) return;
    std::lock_guard lock(s_queueLock);
    if (std::find(s_handlers.begin(), s_handlers.end(), handler) == s_handlers.end()) s_handlers.push_back(handler);
}

CAsyncObject* AsyncFileReadCreateObject() {
    if (!s_initialized) AsyncFileReadInitialize();
    auto object = std::make_unique<CAsyncObject>();
    CAsyncObject* result = object.get();
    std::lock_guard lock(s_queueLock);
    s_objects.push_back(std::move(object));
    return result;
}

void AsyncFileReadDestroyObject(CAsyncObject* object) {
    if (!object) throw std::invalid_argument("async object");
    AsyncFileReadWait(object);
    std::lock_guard lock(s_queueLock);
    Remove(s_asyncFileReadList, object);
    Remove(s_asyncFileReadPostList, object);
    s_objects.erase(std::remove_if(s_objects.begin(), s_objects.end(), [object](const auto& item) { return item.get() == object; }), s_objects.end());
}

void AsyncFileReadObject(CAsyncObject* object) {
    Validate(object);
    std::lock_guard lock(s_queueLock);
    if (object->isLoaded) throw std::logic_error("async object is already loaded");
    Remove(s_asyncFileReadList, object);
    object->queued = true;
    object->completed = false;
    if (object == s_asyncWaitObject && object->canReorder) s_asyncFileReadList.push_front(object);
    else s_asyncFileReadList.push_back(object);
    s_queueEvent.notify_one();
}

void AsyncFileReadWait(CAsyncObject* object) {
    if (!object) throw std::invalid_argument("async object");
    for (;;) {
        {
            std::lock_guard lock(s_queueLock);
            if (object->isLoaded || (!object->queued && !object->completed && object != s_asyncCurrentObject)) return;
            s_asyncWaitObject = object;
            if (object->queued && object->canReorder) {
                Remove(s_asyncFileReadList, object);
                s_asyncFileReadList.push_front(object);
                s_queueEvent.notify_one();
            }
        }
        AsyncFileReadPollHandler(nullptr);
        std::this_thread::yield();
    }
}

void AsyncFileReadWaitAll() {
    while (AsyncFileReadIsReading()) {
        AsyncFileReadPollHandler(nullptr);
        std::this_thread::yield();
    }
}

std::uint8_t AsyncFileReadIsReading() {
    std::lock_guard lock(s_queueLock);
    return s_asyncCurrentObject || !s_asyncFileReadList.empty() || !s_asyncFileReadPostList.empty();
}

int AsyncFileReadPollHandler(void*) {
    std::vector<ASYNCFILEREAD_HANDLER> handlers;
    {
        std::lock_guard lock(s_queueLock);
        handlers = s_handlers;
    }
    for (const auto handler : handlers) handler();
    for (;;) {
        CAsyncObject* object = nullptr;
        {
            std::lock_guard lock(s_queueLock);
            if (s_asyncFileReadPostList.empty()) break;
            object = s_asyncFileReadPostList.front();
            s_asyncFileReadPostList.pop_front();
            object->completed = false;
            object->isLoaded = 1;
            if (object == s_asyncWaitObject) s_asyncWaitObject = nullptr;
        }
        object->userPostloadCallback(object->userArg);
    }
    return 1;
}
