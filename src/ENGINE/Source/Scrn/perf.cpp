#include "ENGINE/Source/Scrn/perf.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Os/W32/debugging.h"
#include "ENGINE/Source/Os/W32/ossystem.h"
#include "ENGINE/Source/Os/W32/ossound.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "Storm/h/SMem.h"
#include "Storm/h/SThread.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdio>
#include <cstring>
#include <iterator>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace {

constexpr unsigned kPerfValueCount = 20;
constexpr unsigned kPerfAverageSamples = 8;
constexpr unsigned kRingBufferSize = 0x10000;

struct ActivityMode {
    const char* title;
    double (*get)(ACTIVITY);
};

double ActivityTimeMilliseconds(ACTIVITY activity)
{
    return ActivityGetTime(activity) * 1000.0;
}

double ActivityTimePercent(ACTIVITY activity)
{
    return ActivityGetTimePercent(activity);
}

double ActivityCallCount(ACTIVITY activity)
{
    return static_cast<double>(ActivityGetCalls(activity));
}

constexpr std::array<ActivityMode, 3> s_activityModes{{
    {"milliseconds", ActivityTimeMilliseconds},
    {"percent", ActivityTimePercent},
    {"calls", ActivityCallCount}
}};

std::array<float, kPerfValueCount> s_avgPerfTime{};
std::array<float, kPerfValueCount * kPerfAverageSamples> s_avgPerfAccum{};
std::array<float, kPerfValueCount> s_peakPerfTime{};
std::array<unsigned, kPerfValueCount> s_peakSustain{};
unsigned s_avgPerfIndex = 0;
unsigned s_activityMode = 0;
ACTIVITY s_enabled = ACTIVITY_ANIMATE;
float s_perfFontHeight = 0.0F;
HLAYER s_layer = 0;
HTEXTFONT s_perfFont = nullptr;
void (__fastcall *s_textFunction)(char*, int, void*) = nullptr;
void* s_textFunctionParam = nullptr;

std::array<std::uint8_t, kRingBufferSize> s_profRingBuffer{};
unsigned s_profRingHead = 0;
unsigned s_profRingTail = 0;
std::mutex s_ringMutex;

NETADDR s_profAddr{};
HNETCONN s_profConn = nullptr;
std::mutex s_profLock;
std::condition_variable s_remoteCondition;
bool s_addrResolved = false;
bool s_addrFailed = false;
bool s_hasProfServer = false;
bool s_hasProfConn = false;
PERF_REMOTE_MODE s_remoteMode = PERF_REMOTE_MODE__ONPAINT;
bool s_loggedFull = false;
std::array<char, 64> s_hostNameList{};
std::atomic<bool> s_remoteStop{false};
std::thread s_remoteThread;
int* s_profReferences = nullptr;
std::int64_t s_profRingFrame = 0;

constexpr std::array<const char*, kPerfValueCount> s_perfPrompts{{
    "event", "idle", "paint", "animate", "animate events", "animate sequence",
    "mouse", "net", "event paint", "poll", "frame manager", "object manager",
    "lighting", "model", "perf display", "program 0", "program 1", "program 2",
    "program 3", "other"
}};

const char* ApiName()
{
    switch (GxDevApi()) {
    case GxApi_OpenGl: return "OpenGL";
    case GxApi_Direct3d: return "Direct3D 11";
    default: return "unknown 3D API";
    }
}

unsigned PerfCounter(EGxPerfCounter counter)
{
    return GxPerfCounter(counter);
}

void CopyText(char* destination, std::size_t capacity, const char* source)
{
    if (destination == nullptr || capacity == 0)
        return;
    std::snprintf(destination, capacity, "%s", source != nullptr ? source : "");
}

void SendBytes(const std::uint8_t* data, unsigned bytes)
{
    if (data == nullptr || bytes == 0)
        return;
    std::lock_guard lock(s_profLock);
    if (s_profConn != nullptr)
        OsTcpConnSend(s_profConn, data, bytes);
}

bool RingHasData()
{
    std::lock_guard lock(s_ringMutex);
    return s_profRingHead != s_profRingTail;
}

} // namespace

void __stdcall HookRenderBegin(void*)
{
    ActivityBegin(ACTIVITY_RENDER);
}

void __stdcall HookRenderEnd(void*)
{
    ActivityEnd(ACTIVITY_RENDER);
}

void IPerfDestroy()
{
    if (s_layer != 0) {
        ScrnLayerDestroy(s_layer);
        s_layer = 0;
    }
    s_perfFont = nullptr;
    IPerfRemoteLoggingShutdown();
}

void IPerfInitialize()
{
    if (s_layer != 0)
        IPerfDestroy();
    ScrnLayerCreate(nullptr, 3U, 2.0F, nullptr, PaintFunc, &s_layer);
    s_perfFont = ScrnGetStockFont(STOCK_PERFFONT);
    s_perfFontHeight = ScrnGetStockFontHeight(STOCK_PERFFONT);
}

void __fastcall PaintFunc(void*, const RECTF* visible, const RECTF*, float)
{
    // ScrnInitialize follows the recovered startup order: the perf layer is
    // created before stock fonts. Resolve the handle lazily on the first
    // paint so that order remains intact without leaving the overlay fontless.
    if (s_perfFont == nullptr) {
        s_perfFont = ScrnGetStockFont(STOCK_PERFFONT);
        s_perfFontHeight = ScrnGetStockFontHeight(STOCK_PERFFONT);
    }
    std::array<float, kPerfValueCount> perfTime{};
    IPerfGetPerfTimes(perfTime.data());

    if (s_enabled == ACTIVITY_ANIMATE) {
        if (ScrnPerfRemoteGetMode() == PERF_REMOTE_MODE__ONPAINT)
            IPerfRemoteQueuePerfData(perfTime.data());
        return;
    }

    ActivityBegin(ACTIVITY_PERFDISPLAY);
    const unsigned frameCount = PerfCounter(GxPerf_FrameNum);
    const unsigned vertices = PerfCounter(GxPerf_Vertices);
    const unsigned primitives = PerfCounter(GxPerf_Primitives);
    const unsigned batches = PerfCounter(GxPerf_Batches);
    const unsigned textures = PerfCounter(GxPerf_Textures);
    const unsigned textureBytes = PerfCounter(GxPerf_TextureBytes);
    const unsigned textureUpdates = PerfCounter(GxPerf_TexUploads);
    const unsigned textureUpdateBytes = PerfCounter(GxPerf_TexUploadBytes);
    const unsigned binds = PerfCounter(GxPerf_TexBinds);
    const unsigned bindBytes = PerfCounter(GxPerf_TexBindBytes);

    if (ScrnPerfRemoteGetMode() == PERF_REMOTE_MODE__ONPAINT)
        IPerfRemoteQueuePerfData(perfTime.data());

    ++s_avgPerfIndex;
    if (s_avgPerfIndex >= kPerfAverageSamples)
        s_avgPerfIndex = 0;
    for (unsigned index = 0; index < kPerfValueCount; ++index) {
        const float current = perfTime[index];
        float& slot = s_avgPerfAccum[index * kPerfAverageSamples + s_avgPerfIndex];
        slot = current;
        float total = 0.0F;
        for (unsigned sample = 0; sample < kPerfAverageSamples; ++sample)
            total += s_avgPerfAccum[index * kPerfAverageSamples + sample];
        s_avgPerfTime[index] = total / static_cast<float>(kPerfAverageSamples);
        if (current > s_peakPerfTime[index]) {
            s_peakPerfTime[index] = current;
            s_peakSustain[index] = 64;
        } else if (s_peakSustain[index] != 0) {
            --s_peakSustain[index];
        } else {
            s_peakPerfTime[index] = current * 0.1F + s_peakPerfTime[index] * 0.9F;
        }
    }

    unsigned memoryAllocated = 0;
    unsigned memoryCommitted = 0;
    _SMEMHEAPDETAILS memoryDetails{sizeof(memoryDetails)};
    HSHEAP__* nextHeap = nullptr;
    while (SMemFindNextHeap(nextHeap, &nextHeap, &memoryDetails) != 0) {
        memoryAllocated += memoryDetails.allocatedbytes;
        memoryCommitted += memoryDetails.committedbytes;
    }

    const char* title = s_activityModes[s_activityMode].title;
    std::array<char, 512> customText{};
    char buffer[11][512]{};
    std::snprintf(buffer[0], sizeof(buffer[0]),
        "%3u FPS, %5.1f/%5.1fMB mem,  Displaying: %s", frameCount,
        static_cast<double>(memoryAllocated) / (1024.0 * 1024.0),
        static_cast<double>(memoryCommitted) / (1024.0 * 1024.0), title);
    std::snprintf(buffer[1], sizeof(buffer[1]), "%2u batches, %5u verts, %5u prims, %s",
        batches, vertices, primitives, ApiName());
    std::snprintf(buffer[2], sizeof(buffer[2]), "%6u VB, %6u IB", vertices, primitives);
    std::snprintf(buffer[3], sizeof(buffer[3]),
        "tex:%3u, %4.2fMB   ren:%3u, %3.3fMB   upd:%3u, %2.4fMB", textures,
        static_cast<double>(textureBytes) / (1024.0 * 1024.0), binds,
        static_cast<double>(bindBytes) / (1024.0 * 1024.0), textureUpdates,
        static_cast<double>(textureUpdateBytes) / (1024.0 * 1024.0));
    std::snprintf(buffer[4], sizeof(buffer[4]),
        "%6.1f idle @%4.0f, %6.1f evnt @%4.0f, %6.1f snd  @%4.0f, %6.1f pnt  @%4.0f",
        s_avgPerfTime[1], s_peakPerfTime[1], s_avgPerfTime[0], s_peakPerfTime[0],
        s_avgPerfTime[13], s_peakPerfTime[13], s_avgPerfTime[2], s_peakPerfTime[2]);
    std::snprintf(buffer[5], sizeof(buffer[5]),
        "%6.1f fmgr @%4.0f, %6.1f wrld @%4.0f, %6.1f lght @%4.0f, %6.1f modl @%4.0f",
        s_avgPerfTime[11], s_peakPerfTime[11], s_avgPerfTime[12], s_peakPerfTime[12],
        s_avgPerfTime[8], s_peakPerfTime[8], s_avgPerfTime[6], s_peakPerfTime[6]);
    std::snprintf(buffer[6], sizeof(buffer[6]),
        "%6.1f part @%4.0f, %6.1f rndr @%4.0f, %6.1f omgr @%4.0f",
        s_avgPerfTime[7], s_peakPerfTime[7], s_avgPerfTime[9], s_peakPerfTime[9],
        s_avgPerfTime[10], s_peakPerfTime[10]);
    std::snprintf(buffer[7], sizeof(buffer[7]),
        "%6.1f anim @%4.0f, %6.1f aevt @%4.0f, %6.1f aseq @%4.0f",
        s_avgPerfTime[3], s_peakPerfTime[3], s_avgPerfTime[4], s_peakPerfTime[4],
        s_avgPerfTime[5], s_peakPerfTime[5]);
    std::snprintf(buffer[8], sizeof(buffer[8]),
        "%6.1f p0   @%4.0f, %6.1f p1   @%4.0f, %6.1f p2   @%4.0f, %6.1f p3   @%4.0f",
        s_avgPerfTime[15], s_peakPerfTime[15], s_avgPerfTime[16], s_peakPerfTime[16],
        s_avgPerfTime[17], s_peakPerfTime[17], s_avgPerfTime[18], s_peakPerfTime[18]);
    std::snprintf(buffer[9], sizeof(buffer[9]),
        "%6.1f perf @%4.0f, %6.1f othr @%4.0f, %6.1f totl @%4.0f,   miles %u%%",
        s_avgPerfTime[14], s_peakPerfTime[14], s_avgPerfTime[19], s_peakPerfTime[19],
        perfTime[0], s_peakPerfTime[0], Sound::SndGetCPUPerformance());
    if (s_textFunction != nullptr)
        s_textFunction(customText.data(), static_cast<int>(customText.size()), s_textFunctionParam);
    CopyText(buffer[10], sizeof(buffer[10]), customText.data());

    if (s_perfFont != nullptr && visible != nullptr) {
        const NTempest::CImVector color(0xFFFFFFFFU);
        for (unsigned index = 0; index < std::size(buffer); ++index) {
            const float y = visible->top - static_cast<float>(index + 2) * 0.025F;
            const NTempest::C3Vector position(visible->left + 0.002F, y, 0.0F);
            HTEXTBLOCK text = TextBlockCreate(s_perfFont, buffer[index], &color, &position,
                1.0F, 0.0F, 0.0F, 0x54U, 0.0F, -0.45F);
            TextBlockRender(text);
            TextBlockDestroy(text);
        }
    }
    ActivityEnd(ACTIVITY_PERFDISPLAY);
}

void __fastcall IPerfGetPerfTimes(float* perfTime)
{
    if (perfTime == nullptr)
        return;
    const auto getter = s_activityModes[s_activityMode].get;
    const auto get = [getter](unsigned index) {
        return static_cast<float>(getter(static_cast<ACTIVITY>(index)));
    };
    perfTime[3] = get(ACTIVITY_ANIMATE);
    perfTime[4] = get(ACTIVITY_ANIMEVENTS);
    perfTime[5] = get(ACTIVITY_ANIMSEQEND);
    perfTime[6] = get(ACTIVITY_MODEL);
    perfTime[7] = get(ACTIVITY_PARTICLE);
    perfTime[8] = get(ACTIVITY_LIGHTING);
    perfTime[9] = get(ACTIVITY_RENDER);
    perfTime[10] = get(ACTIVITY_OBJMGR);
    perfTime[11] = get(ACTIVITY_FRAMEMANAGER);
    perfTime[12] = get(ACTIVITY_WORLD);
    perfTime[13] = get(ACTIVITY_SOUND);
    perfTime[14] = get(ACTIVITY_PERFDISPLAY);
    perfTime[15] = get(ACTIVITY_PROG0);
    perfTime[16] = get(ACTIVITY_PROG1);
    perfTime[17] = get(ACTIVITY_PROG2);
    perfTime[18] = get(ACTIVITY_PROG3);
    perfTime[19] = get(ACTIVITY_OTHER);
    perfTime[0] = get(ACTIVITY_EVENTHANDLERS) + get(ACTIVITY_EVENTKEYANDCHAR)
        + get(ACTIVITY_EVENTMOUSE) + get(ACTIVITY_EVENTPOLL);
    perfTime[1] = get(ACTIVITY_EVENTIDLE);
    perfTime[2] = get(ACTIVITY_EVENTPAINT);
    ActivityResetTimes();
}

void IPerfResetTimePeaks()
{
    s_avgPerfAccum.fill(0.0F);
    s_avgPerfTime.fill(0.0F);
    s_peakPerfTime.fill(0.0F);
    s_peakSustain.fill(0);
    s_avgPerfIndex = 0;
}

void __fastcall ScrnPerfEnable(ACTIVITY activity)
{
    s_enabled = activity;
}

ACTIVITY ScrnPerfIsEnabled()
{
    return s_enabled;
}

void ScrnPerfSetTextFunction(void (__fastcall *function)(char*, int, void*), void* param)
{
    s_textFunction = function;
    s_textFunctionParam = param;
}

void ScrnPerfToggleDisplayedValues()
{
    s_activityMode = (s_activityMode + 1U) % static_cast<unsigned>(s_activityModes.size());
    IPerfResetTimePeaks();
}

void ScrnPerfResetTimePeaks()
{
    IPerfResetTimePeaks();
}

void __stdcall IPerfHostAddrProc(const NETADDR* addresses, unsigned long addressCount, void*)
{
    {
        std::lock_guard lock(s_profLock);
        if (addresses != nullptr && addressCount != 0) {
            s_profAddr = addresses[0];
            s_addrResolved = true;
        } else {
            s_addrFailed = true;
        }
    }
    s_remoteCondition.notify_all();
}

int __stdcall ChatEventProc(HNETCONN connection, const NETCONNADDR*, NETNOTE note,
    void*, const void*, unsigned long, unsigned long*)
{
    std::lock_guard lock(s_profLock);
    if (note == NETNOTE_CONNECT) {
        if (s_profConn != nullptr)
            OsNetConnFreeHandle(s_profConn);
        s_profConn = OsNetConnCopyHandle(connection);
        s_hasProfConn = s_profConn != nullptr;
        s_remoteCondition.notify_all();
        return 1;
    }
    if (note == NETNOTE_DISCONNECT) {
        if (s_profConn != nullptr)
            OsNetConnFreeHandle(s_profConn);
        s_profConn = nullptr;
        s_hasProfConn = false;
    }
    if (note == NETNOTE_CANTCONNECT)
        s_remoteCondition.notify_all();
    return note == NETNOTE_DISCONNECT ? 1 : 0;
}

unsigned __fastcall IPerfRemoteLoggingIni()
{
    std::array<char, 64> host{};
    if (!SRegLoadString("Internal", "EnginePerfServ", 0, host.data(), host.size()))
        return 0;
    if (host[0] == '\0')
        return 0;
    s_hostNameList = host;
    s_addrResolved = false;
    s_addrFailed = false;
    if (OsNetGetHostAddrs(s_hostNameList.data(), 0x17EBU, IPerfHostAddrProc, nullptr) == 0)
        return 0;
    {
        std::unique_lock lock(s_profLock);
        s_remoteCondition.wait_for(lock, std::chrono::seconds(2), [] {
            return s_addrResolved || s_addrFailed || s_remoteStop.load();
        });
        if (!s_addrResolved || s_remoteStop.load())
            return 0;
    }
    unsigned short port = 0;
    const unsigned long address = OsNetAddrGetAddress(&s_profAddr, &port);
    std::uint8_t hello = 0x8E;
    OsTcpConnect(address, port, ChatEventProc, nullptr, &hello, sizeof(hello));
    {
        std::unique_lock lock(s_profLock);
        s_remoteCondition.wait_for(lock, std::chrono::seconds(2), [] {
            return s_hasProfConn || s_remoteStop.load();
        });
    }
    if (!s_hasProfConn)
        return 0;
    s_hasProfServer = true;

    char userName[16]{};
    char computerName[32]{};
    unsigned userNameSize = sizeof(userName);
    unsigned computerNameSize = sizeof(computerName);
    OsGetUserName(userName, &userNameSize);
    OsGetComputerName(computerName, &computerNameSize);
    std::array<std::uint8_t, 260> message{};
    message[0] = 0xAB;
    message[1] = 0;
    const std::size_t userLength = std::strlen(userName);
    const std::size_t computerLength = std::strlen(computerName);
    const std::size_t payloadLength = std::min<std::size_t>(userLength + computerLength + 2, 255);
    *reinterpret_cast<std::uint16_t*>(message.data() + 2) = static_cast<std::uint16_t>(payloadLength + 4);
    std::memcpy(message.data() + 4, userName, std::min(userLength, payloadLength));
    if (userLength + 1 < payloadLength)
        std::memcpy(message.data() + 5 + userLength, computerName,
            std::min(computerLength, payloadLength - userLength - 1));
    SendBytes(message.data(), static_cast<unsigned>(payloadLength + 4));
    IPerfRemoteQueuePerfLabels();
    return 1;
}

void __fastcall IPerfGetFromBuffer(const std::uint8_t** buffer1, unsigned* length1,
    const std::uint8_t** buffer2, unsigned* length2)
{
    if (buffer1) *buffer1 = nullptr;
    if (length1) *length1 = 0;
    if (buffer2) *buffer2 = nullptr;
    if (length2) *length2 = 0;
    std::lock_guard lock(s_ringMutex);
    if (s_profRingHead == s_profRingTail)
        return;
    if (kRingBufferSize - s_profRingTail < sizeof(std::uint32_t))
        s_profRingTail = 0;
    std::uint32_t payloadBytes = 0;
    std::memcpy(&payloadBytes, s_profRingBuffer.data() + s_profRingTail, sizeof(payloadBytes));
    if (payloadBytes < sizeof(std::uint32_t) + sizeof(SimplePerfHdr) ||
        payloadBytes > kRingBufferSize) {
        s_profRingTail = s_profRingHead;
        return;
    }
    const unsigned start = s_profRingTail + sizeof(std::uint32_t);
    const unsigned payloadLength = payloadBytes - sizeof(std::uint32_t);
    const unsigned contiguous = std::min<unsigned>(payloadLength, kRingBufferSize - start);
    if (buffer1) *buffer1 = s_profRingBuffer.data() + start;
    if (length1) *length1 = contiguous;
    if (contiguous < payloadBytes) {
        if (buffer2) *buffer2 = s_profRingBuffer.data();
        if (length2) *length2 = payloadLength - contiguous;
    }
}

void __cdecl IPerfRemoteSendQueued()
{
    for (;;) {
        const std::uint8_t* buffer1 = nullptr;
        const std::uint8_t* buffer2 = nullptr;
        unsigned length1 = 0;
        unsigned length2 = 0;
        unsigned payloadBytes = 0;
        {
            std::lock_guard lock(s_ringMutex);
            if (s_profRingHead == s_profRingTail)
                break;
            unsigned tail = s_profRingTail;
            if (kRingBufferSize - tail < sizeof(std::uint32_t))
                tail = s_profRingTail = 0;
            std::memcpy(&payloadBytes, s_profRingBuffer.data() + tail, sizeof(payloadBytes));
        }
        IPerfGetFromBuffer(&buffer1, &length1, &buffer2, &length2);
        if ((buffer1 == nullptr || length1 == 0) && (buffer2 == nullptr || length2 == 0))
            break;
        if (length1 != 0)
            SendBytes(buffer1, length1);
        if (length2 != 0)
            SendBytes(buffer2, length2);
        std::lock_guard lock(s_ringMutex);
        s_profRingTail = (s_profRingTail + payloadBytes) % kRingBufferSize;
    }
}

unsigned __stdcall IPerfRemoteLoggingThread(void*)
{
    if (IPerfRemoteLoggingIni() == 0)
        return 0;
    while (!s_remoteStop.load()) {
        if (RingHasData())
            IPerfRemoteSendQueued();
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    IPerfRemoteSendQueued();
    return 0;
}

int __fastcall IPerfRemoteSpaceAvail(unsigned bytes)
{
    std::lock_guard lock(s_ringMutex);
    const unsigned used = s_profRingHead >= s_profRingTail
        ? s_profRingHead - s_profRingTail
        : kRingBufferSize - (s_profRingTail - s_profRingHead);
    const unsigned freeBytes = kRingBufferSize - used - 1;
    if (bytes + 3U < freeBytes) {
        s_loggedFull = false;
        return 1;
    }
    s_loggedFull = true;
    return 0;
}

void __fastcall IPerfRemoteInitSimpleHdr(SimplePerfHdr* header, unsigned packetSize, std::uint16_t id)
{
    if (header == nullptr)
        return;
    header->signature = 0xAB;
    header->message = 1;
    header->len = static_cast<std::uint16_t>(std::min(packetSize, 0xFFFBU));
    const double clocks = static_cast<double>(OsGetAsyncTimeClocks());
    header->timestamp = static_cast<std::int64_t>(OsGetAsyncClocksDivisor() * clocks * 1000000.0);
    header->frame = s_profRingFrame++;
    header->updateCount = 1;
    header->id = id;
    header->size = 0;
}

void __fastcall IPerfRemoteQueueOnRingBuffer(const void* data, unsigned bytes)
{
    if (data == nullptr || bytes < sizeof(std::uint32_t) || bytes > kRingBufferSize)
        return;
    std::lock_guard lock(s_ringMutex);
    const unsigned used = s_profRingHead >= s_profRingTail
        ? s_profRingHead - s_profRingTail
        : kRingBufferSize - (s_profRingTail - s_profRingHead);
    if (bytes >= kRingBufferSize - used)
        return;
    auto copyToRing = [](unsigned& head, const std::uint8_t* source, unsigned count) {
        const unsigned first = std::min(count, kRingBufferSize - head);
        std::memcpy(s_profRingBuffer.data() + head, source, first);
        if (first < count)
            std::memcpy(s_profRingBuffer.data(), source + first, count - first);
        head = (head + count) % kRingBufferSize;
    };
    copyToRing(s_profRingHead, static_cast<const std::uint8_t*>(data), bytes);
    s_remoteCondition.notify_all();
}

void __fastcall IPerfRemoteQueuePerfData(const float* perfTime)
{
    if (!s_hasProfServer || !s_hasProfConn || perfTime == nullptr || !IPerfRemoteSpaceAvail(112))
        return;
    struct Packet {
        std::uint32_t messageSize;
        SimplePerfHdr header;
        float values[kPerfValueCount];
    } packet{};
    packet.messageSize = sizeof(packet);
    IPerfRemoteInitSimpleHdr(&packet.header, packet.messageSize - sizeof(packet.messageSize), 1);
    packet.header.size = sizeof(packet.values);
    std::memcpy(packet.values, perfTime, sizeof(packet.values));
    IPerfRemoteQueueOnRingBuffer(&packet, packet.messageSize);
}

void IPerfRemoteQueuePerfLabels()
{
    if (!s_hasProfServer || !s_hasProfConn)
        return;
    std::array<char, 1024> labels{};
    unsigned length = 0;
    for (const char* prompt : s_perfPrompts) {
        const unsigned promptLength = static_cast<unsigned>(std::strlen(prompt));
        if (length + promptLength + 1 >= labels.size())
            break;
        std::memcpy(labels.data() + length, prompt, promptLength);
        length += promptLength;
        labels[length++] = '\0';
    }
    const unsigned packetBytes = sizeof(SimplePerfHdr) + length;
    const unsigned totalBytes = sizeof(std::uint32_t) + packetBytes;
    if (!IPerfRemoteSpaceAvail(totalBytes))
        return;
    std::vector<std::uint8_t> packet(totalBytes);
    *reinterpret_cast<std::uint32_t*>(packet.data()) = totalBytes;
    auto* header = reinterpret_cast<SimplePerfHdr*>(packet.data() + sizeof(std::uint32_t));
    IPerfRemoteInitSimpleHdr(header, packetBytes, 2);
    header->size = static_cast<std::uint16_t>(length);
    std::memcpy(packet.data() + sizeof(std::uint32_t) + sizeof(SimplePerfHdr), labels.data(), length);
    IPerfRemoteQueueOnRingBuffer(packet.data(), totalBytes);
}

void __fastcall IPerfRemoteQueueLogString(const char* text)
{
    if (!s_hasProfServer || !s_hasProfConn || text == nullptr)
        return;
    const unsigned textLength = std::min<unsigned>(static_cast<unsigned>(std::strlen(text)), 255U);
    const unsigned packetBytes = sizeof(SimplePerfHdr) + textLength + 1;
    const unsigned totalBytes = sizeof(std::uint32_t) + packetBytes;
    if (!IPerfRemoteSpaceAvail(totalBytes))
        return;
    std::vector<std::uint8_t> packet(totalBytes);
    *reinterpret_cast<std::uint32_t*>(packet.data()) = totalBytes;
    auto* header = reinterpret_cast<SimplePerfHdr*>(packet.data() + sizeof(std::uint32_t));
    IPerfRemoteInitSimpleHdr(header, packetBytes, 3);
    header->size = static_cast<std::uint16_t>(textLength + 1);
    std::memcpy(packet.data() + sizeof(std::uint32_t) + sizeof(SimplePerfHdr), text, textLength);
    packet[sizeof(std::uint32_t) + sizeof(SimplePerfHdr) + textLength] = '\0';
    IPerfRemoteQueueOnRingBuffer(packet.data(), totalBytes);
}

int IPerfRemoteLoggingStartup(int* references)
{
    if (references == nullptr)
        return 0;
    const int previous = *references;
    ++*references;
    if (previous != 0)
        return 1;
    s_remoteStop = false;
    if (!SRegLoadString("Internal", "EnginePerfServ", 0, s_hostNameList.data(), s_hostNameList.size()) ||
        s_hostNameList[0] == '\0') {
        --*references;
        return 0;
    }
    s_profReferences = references;
    OsNetInitialize(0, 3);
    {
        std::lock_guard lock(s_ringMutex);
        s_profRingHead = 0;
        s_profRingTail = 0;
        s_profRingFrame = 0;
        s_loggedFull = false;
    }
    s_remoteThread = std::thread([] { IPerfRemoteLoggingThread(nullptr); });
    return 1;
}

int __fastcall IPerfRemoteLoggingShutdown()
{
    if (s_profReferences == nullptr)
        return 1;
    if (*s_profReferences > 0)
        --*s_profReferences;
    if (*s_profReferences != 0)
        return 1;
    s_profReferences = nullptr;
    s_hasProfServer = false;
    s_hasProfConn = false;
    s_remoteStop = true;
    s_remoteCondition.notify_all();
    if (s_remoteThread.joinable())
        s_remoteThread.join();
    {
        std::lock_guard lock(s_profLock);
        if (s_profConn != nullptr)
            OsNetConnFreeHandle(s_profConn);
        s_profConn = nullptr;
    }
    OsNetDestroy(3);
    return 1;
}

int ScrnPerfRemoteStartup(int* references)
{
    return IPerfRemoteLoggingStartup(references);
}

int ScrnPerfRemoteShutdown()
{
    return IPerfRemoteLoggingShutdown();
}

void __fastcall ScrnPerfRemoteSetMode(PERF_REMOTE_MODE mode)
{
    s_remoteMode = mode < PERF_REMOTE_MODE_MAX ? mode : PERF_REMOTE_MODE__ONPAINT;
}

PERF_REMOTE_MODE __fastcall ScrnPerfRemoteGetMode()
{
    return s_remoteMode;
}

void __fastcall ScrnPerfRemoteLogString(const char* text)
{
    IPerfRemoteQueueLogString(text);
}

void ScrnPerfRemoteLogPerfCounters()
{
    std::array<float, kPerfValueCount> values{};
    IPerfGetPerfTimes(values.data());
    IPerfRemoteQueuePerfData(values.data());
}

int _E1_47() { return 0x7F800000; }
int _E2_47() { return _E1_47(); }
void _E4_47() {}
int _E5_47() { _E4_47(); return 0; }
void _E7_47() {}
int _E8_47() { _E7_47(); return 0; }
int _E20_9() { s_ringMutex.lock(); s_ringMutex.unlock(); return 0; }
static int _E21_9() { return 0; }
int _E22_9() { return 0; }
int _E23_6() { return _E22_9(); }
SCritSect* _E25_8() { static SCritSect lock; return &lock; }
void _E26_7() {}
int _E27_3() { _E26_7(); return 0; }
void _E28_4() {}
