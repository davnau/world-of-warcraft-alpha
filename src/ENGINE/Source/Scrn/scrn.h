#pragma once

#include "ENGINE/Source/Base/activity.h"
#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Base/handle.h"
#include "ENGINE/Source/Event/event.h"
#include "ENGINE/Source/Services/textblock.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Os/W32/ostcp.h"

#include <cstdint>

using HLAYER = HOBJECT;

enum SCRNSTOCK : std::int32_t {
    STOCK_SYSFONT = 0,
    STOCK_PERFFONT = 1,
    SCRNSTOCKOBJECTS = 2
};

enum PERF_REMOTE_MODE : std::int32_t {
    PERF_REMOTE_MODE__ONPAINT = 0,
    PERF_REMOTE_MODE__ADVANCESYNCGAMETIME = 1,
    PERF_REMOTE_MODE_MAX = 2
};

using SCRN_LAYER_PAINT = void (__fastcall *)(void*, const RECTF*, const RECTF*, float);
using SCRN_CAPTURE_CALLBACK = void (__fastcall *)(int);

// A layer is kept in z-order by the Scrn implementation and is referenced by
// the same opaque handle type used by the rest of the recovered client.
class CILayer {
public:
    CILayer(const RECTF& rectangle, unsigned layerFlags, float order, void* userParam,
        SCRN_LAYER_PAINT paintFunction);
    ~CILayer();

    CILayer(const CILayer&) = delete;
    CILayer& operator=(const CILayer&) = delete;

    RECTF rect{};
    RECTF visible{};
    float zorder = 0.0F;
    unsigned flags = 0;
    void* param = nullptr;
    SCRN_LAYER_PAINT paintfunc = nullptr;
    CILayer* previous = nullptr;
    CILayer* next = nullptr;
    bool linked = false;
};

// Scrn lifecycle and the event-facing layer renderer.
void ScrnInitialize(int consoleInitialized);
void ScrnDestroy();
void ScrnPaint();
void ILayerInitialize();
void ILayerDestroy();
void ILayerPaint();
int __fastcall OnIdle(const void* eventData, void* param);
int __fastcall OnPaint(const void* eventData, void* param);

// Layer API exported to the client and UI modules.
void __fastcall ScrnLayerCreate(const RECTF* rectangle, unsigned flags, float zorder, void* param,
    SCRN_LAYER_PAINT paintfunc, HLAYER* layer);
void ScrnLayerDestroy(HLAYER layer);
void __fastcall ScrnLayerSetRect(HLAYER layer, const RECTF* rectangle);
unsigned __fastcall ScrnLayerGetFlags(HLAYER layer);
void __fastcall ScrnLayerSetFlags(HLAYER layer, unsigned flags);
void __fastcall ScrnLayerDisable(int disabled);
int ScrnLayerIsDisabled();
void __fastcall ScrnScreenshot(SCRN_CAPTURE_CALLBACK callback);
int CaptureScreen();

// Stock font API.
void IStockInitialize();
void IStockDestroy();
HTEXTFONT __fastcall ScrnGetStockFont(SCRNSTOCK stockID);
void ScrnSetStockFont(SCRNSTOCK stockID, const char* fontTexturePath);
void CreateSysFont(SCRNSTOCK stockID, const char* fontFilePath);
float __fastcall ScrnGetStockFontHeight(SCRNSTOCK stockID);

// Performance overlay and render hooks.
void __stdcall HookRenderBegin(void* param);
void __stdcall HookRenderEnd(void* param);
void IPerfInitialize();
void IPerfDestroy();
void __fastcall PaintFunc(void* param, const RECTF* visible, const RECTF* rectangle, float elapsed);
void __fastcall IPerfGetPerfTimes(float* perfTime);
void IPerfResetTimePeaks();
void __fastcall ScrnPerfEnable(ACTIVITY activity);
ACTIVITY ScrnPerfIsEnabled();
void ScrnPerfSetTextFunction(void (__fastcall *function)(char*, int, void*), void* param);
void ScrnPerfToggleDisplayedValues();
void ScrnPerfResetTimePeaks();

// Remote performance logging.  These names are retained from the DIA records
// because the profiling service connects to them by symbol/API name.
void __stdcall IPerfHostAddrProc(const NETADDR* addresses, unsigned long addressCount, void* user);
int __stdcall ChatEventProc(HNETCONN connection, const NETCONNADDR* address, NETNOTE note,
    void* user, const void* data, unsigned long bytes, unsigned long* bytesProcessed);
unsigned __fastcall IPerfRemoteLoggingIni();
void __fastcall IPerfGetFromBuffer(const std::uint8_t** buffer1, unsigned* length1,
    const std::uint8_t** buffer2, unsigned* length2);
void __cdecl IPerfRemoteSendQueued();
unsigned __stdcall IPerfRemoteLoggingThread(void* param);
int __fastcall IPerfRemoteSpaceAvail(unsigned bytes);

#pragma pack(push, 1)
struct SimplePerfHdr {
    std::uint8_t signature = 0;
    std::uint8_t message = 0;
    std::uint16_t len = 0;
    std::int64_t timestamp = 0;
    std::int64_t frame = 0;
    std::uint32_t updateCount = 0;
    std::uint16_t id = 0;
    std::uint16_t size = 0;
};
#pragma pack(pop)

static_assert(sizeof(SimplePerfHdr) == 28, "SimplePerfHdr layout must remain packed.");

void __fastcall IPerfRemoteInitSimpleHdr(SimplePerfHdr* header, unsigned packetSize, std::uint16_t id);
void __fastcall IPerfRemoteQueueOnRingBuffer(const void* data, unsigned bytes);
void __fastcall IPerfRemoteQueuePerfData(const float* perfTime);
void IPerfRemoteQueuePerfLabels();
void __fastcall IPerfRemoteQueueLogString(const char* text);
int IPerfRemoteLoggingStartup(int* references);
int __fastcall IPerfRemoteLoggingShutdown();
int ScrnPerfRemoteStartup(int* references);
int ScrnPerfRemoteShutdown();
void __fastcall ScrnPerfRemoteSetMode(PERF_REMOTE_MODE mode);
PERF_REMOTE_MODE __fastcall ScrnPerfRemoteGetMode();
void __fastcall ScrnPerfRemoteLogString(const char* text);
void ScrnPerfRemoteLogPerfCounters();
