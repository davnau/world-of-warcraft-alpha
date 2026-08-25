#pragma once

#include <cstdint>

class SFile;
struct SCritSect;

struct CAsyncObject {
    SFile* file = nullptr;
    unsigned offset = 0;
    void* buffer = nullptr;
    unsigned size = 0;
    void* userArg = nullptr;
    void (__fastcall* userPostloadCallback)(void*) = nullptr;
    SCritSect* critSect = nullptr;
    std::uint8_t isLoaded = 0;
    std::uint8_t canReorder = 1;

    // Runtime bookkeeping follows the recovered payload fields. It is not part
    // of the preserved 32-bit serialized data contract.
    bool queued = false;
    bool completed = false;
};

using ASYNCFILEREAD_HANDLER = void (__fastcall*)();

void AsyncFileReadInitialize();
unsigned __stdcall AsyncFileReadThread(void* parameter);
void AsyncFileReadDestroy();
void AsyncFileReadAddHandler(ASYNCFILEREAD_HANDLER handler);
CAsyncObject* AsyncFileReadCreateObject();
void AsyncFileReadDestroyObject(CAsyncObject* object);
void AsyncFileReadObject(CAsyncObject* object);
void AsyncFileReadWait(CAsyncObject* object);
void AsyncFileReadWaitAll();
std::uint8_t AsyncFileReadIsReading();
int AsyncFileReadPollHandler(void* eventData);
