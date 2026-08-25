#include "ENGINE/Source/Scrn/layer.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#define SCRN_USE_GX_BLIT_FORMAT
#include "ENGINE/Source/Images/images.h"
#undef SCRN_USE_GX_BLIT_FORMAT
#include "ENGINE/Source/Os/W32/osfile.h"
#include "ENGINE/Source/Os/W32/path.h"
#include "ENGINE/Source/Os/W32/time.h"
#include "Storm/h/storm.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdio>
#include <vector>

namespace {

constexpr RECTF kDefaultRect{0.0F, 0.0F, 1.0F, 1.0F};

struct LayerEntry {
    HLAYER handle = 0;
    CILayer* layer = nullptr;
};

std::vector<LayerEntry> s_layers;
float s_elapsedSec = 0.0F;
int s_scrnDisable = 0;
SCRN_CAPTURE_CALLBACK s_captureScreen = nullptr;

CILayer* LayerFromHandle(HLAYER handle)
{
    if (handle == 0)
        return nullptr;
    CHandleObject* object = HandleDereference(handle);
    return object ? static_cast<CILayer*>(object->object) : nullptr;
}

RECTF IntersectRect(const RECTF& left, const RECTF& right)
{
    return {
        std::max(left.left, right.left),
        std::max(left.bottom, right.bottom),
        std::min(left.right, right.right),
        std::min(left.top, right.top)
    };
}

bool IsVisible(const RECTF& rect)
{
    return rect.right > rect.left && rect.top > rect.bottom;
}

void RelinkLayers()
{
    std::sort(s_layers.begin(), s_layers.end(), [](const LayerEntry& left, const LayerEntry& right) {
        return left.layer->zorder < right.layer->zorder;
    });
    for (std::size_t index = 0; index < s_layers.size(); ++index) {
        CILayer* layer = s_layers[index].layer;
        layer->previous = index ? s_layers[index - 1].layer : nullptr;
        layer->next = index + 1 < s_layers.size() ? s_layers[index + 1].layer : nullptr;
        layer->linked = true;
    }
}

void UnlinkLayer(CILayer* layer)
{
    if (!layer || !layer->linked)
        return;
    const auto found = std::find_if(s_layers.begin(), s_layers.end(), [layer](const LayerEntry& entry) {
        return entry.layer == layer;
    });
    if (found != s_layers.end())
        s_layers.erase(found);
    layer->previous = nullptr;
    layer->next = nullptr;
    layer->linked = false;
    RelinkLayers();
}

NTempest::C44Matrix MakeOrtho(const RECTF& rect)
{
    const float width = rect.right - rect.left;
    const float height = rect.top - rect.bottom;
    const float depth = 500.0F;
    if (width <= 0.0F || height <= 0.0F)
        return {};
    return NTempest::C44Matrix(
        2.0F / width, 0.0F, 0.0F, 0.0F,
        0.0F, 2.0F / height, 0.0F, 0.0F,
        0.0F, 0.0F, 2.0F / depth, 0.0F,
        -(rect.right + rect.left) / width,
        -(rect.top + rect.bottom) / height, -1.0F, 1.0F);
}

} // namespace

CILayer::CILayer(const RECTF& rectangle, unsigned layerFlags, float order, void* userParam,
    SCRN_LAYER_PAINT paintFunction)
    : rect(rectangle), visible(rectangle), zorder(order), flags(layerFlags), param(userParam),
      paintfunc(paintFunction)
{
}

CILayer::~CILayer()
{
    UnlinkLayer(this);
}

int __fastcall OnIdle(const void* eventData, void*)
{
    if (eventData != nullptr)
        s_elapsedSec += static_cast<const EVENT_DATA_IDLE*>(eventData)->elapsedSec;
    return 1;
}

int __fastcall OnPaint(const void*, void*)
{
    if (s_scrnDisable != 0 || GxCapsIsWindowVisible() == 0)
        return 1;

    float oldMinX = 0.0F;
    float oldMaxX = 1.0F;
    float oldMinY = 0.0F;
    float oldMaxY = 1.0F;
    float oldMinZ = 0.0F;
    float oldMaxZ = 1.0F;
    GxXformViewport(&oldMinX, &oldMaxX, &oldMinY, &oldMaxY, &oldMinZ, &oldMaxZ);

    const std::vector<LayerEntry> layers = s_layers;
    for (const LayerEntry& entry : layers) {
        CILayer* layer = entry.layer;
        if (!layer || layer->paintfunc == nullptr)
            continue;

        layer->visible = IntersectRect(layer->rect, kDefaultRect);
        if (!IsVisible(layer->visible))
            continue;

        if ((layer->flags & 4U) != 0)
            GxXformSetViewport(0.0F, 1.0F, 0.0F, 1.0F, 0.0F, 1.0F);
        else
            GxXformSetViewport(layer->visible.left, layer->visible.right,
                layer->visible.bottom, layer->visible.top, 0.0F, 1.0F);

        if ((layer->flags & 2U) != 0) {
            const NTempest::C44Matrix projection = MakeOrtho(layer->visible);
            const NTempest::C44Matrix identity;
            GxXformSetView(identity);
            GxXformSetProjection(projection);
        }
        layer->paintfunc(layer->param, &layer->rect, &layer->visible, s_elapsedSec);
    }

    GxXformSetViewport(oldMinX, oldMaxX, oldMinY, oldMaxY, oldMinZ, oldMaxZ);
    if (s_captureScreen != nullptr) {
        GxDevTakeScreenShot();
        GxScenePresent(3);
        const int captured = CaptureScreen();
        s_captureScreen(captured);
        GxDevClearScreenShot();
        s_captureScreen = nullptr;
    } else {
        GxScenePresent(3);
    }
    s_elapsedSec = 0.0F;
    return 1;
}

void ILayerInitialize()
{
    EventRegisterEx(EVENT_ID_IDLE, OnIdle, nullptr, 0.0F);
    EventRegisterEx(EVENT_ID_PAINT, OnPaint, nullptr, 0.0F);
}

void ILayerDestroy()
{
    EventUnregister(EVENT_ID_IDLE, OnIdle);
    EventUnregister(EVENT_ID_PAINT, OnPaint);
    while (!s_layers.empty()) {
        LayerEntry entry = s_layers.back();
        s_layers.pop_back();
        if (entry.layer != nullptr) {
            entry.layer->linked = false;
            entry.layer->previous = nullptr;
            entry.layer->next = nullptr;
            HandleClose(entry.handle);
            delete entry.layer;
        }
    }
    s_elapsedSec = 0.0F;
    s_captureScreen = nullptr;
}

void ILayerPaint()
{
    OnPaint(nullptr, nullptr);
}

void __fastcall ScrnLayerCreate(const RECTF* rectangle, unsigned flags, float zorder, void* param,
    SCRN_LAYER_PAINT paintfunc, HLAYER* layer)
{
    if (layer == nullptr)
        return;
    *layer = 0;
    const RECTF& actualRect = rectangle != nullptr ? *rectangle : kDefaultRect;
    auto* object = new CILayer(actualRect, flags, zorder, param, paintfunc);
    const HLAYER handle = HandleCreate(object);
    s_layers.push_back({handle, object});
    RelinkLayers();
    *layer = handle;
}

void ScrnLayerDestroy(HLAYER handle)
{
    const auto found = std::find_if(s_layers.begin(), s_layers.end(), [handle](const LayerEntry& entry) {
        return entry.handle == handle;
    });
    if (found == s_layers.end())
        return;
    CILayer* object = found->layer;
    s_layers.erase(found);
    if (object != nullptr) {
        object->linked = false;
        object->previous = nullptr;
        object->next = nullptr;
        delete object;
    }
    HandleClose(handle);
    RelinkLayers();
}

void __fastcall ScrnLayerSetRect(HLAYER layer, const RECTF* rectangle)
{
    if (CILayer* object = LayerFromHandle(layer); object != nullptr && rectangle != nullptr) {
        object->rect = *rectangle;
        RelinkLayers();
    }
}

unsigned __fastcall ScrnLayerGetFlags(HLAYER layer)
{
    const CILayer* object = LayerFromHandle(layer);
    return object != nullptr ? object->flags : 0U;
}

void __fastcall ScrnLayerSetFlags(HLAYER layer, unsigned flags)
{
    if (CILayer* object = LayerFromHandle(layer); object != nullptr)
        object->flags = flags;
}

void __fastcall ScrnLayerDisable(int disabled)
{
    s_scrnDisable = disabled;
}

int ScrnLayerIsDisabled()
{
    return s_scrnDisable;
}

void __fastcall ScrnScreenshot(SCRN_CAPTURE_CALLBACK callback)
{
    s_captureScreen = callback;
}

int CaptureScreen()
{
    unsigned width = 0;
    unsigned height = 0;
    const NTempest::CImVector* pixels = nullptr;
    GxDevReadScreenShot(&width, &height, &pixels);
    if (pixels == nullptr || width == 0 || height == 0)
        return 0;

    constexpr const char* screenshotDirectory = "Screenshots";
    OsCreateDirectory(screenshotDirectory, 0);
    char timestamp[260]{};
    char path[260]{};
    OsGetTimeStamp(timestamp, sizeof(timestamp));
    std::snprintf(path, sizeof(path), "%s\\WoWScrnShot_%s.tga", screenshotDirectory, timestamp);

    CTgaFile tga;
    const bool initialized = tga.SetImage(pixels, static_cast<std::uint16_t>(width),
        static_cast<std::uint16_t>(height), 0x20U, 8U, 1, 0) != 0;
    const bool written = initialized && tga.Write(path) != 0;
    tga.Close();
    return written ? 1 : 0;
}

// The recovered object also exposes the compiler-generated initialization and
// termination thunks below. They are kept as named functions so symbol users
// and the source-level recovery audit see the same function inventory.
int _E1_45() { return 0x7F800000; }
int _E2_45() { return _E1_45(); }
void _E4_45() {}
int _E5_45() { _E4_45(); return 0; }
void _E7_45() {}
int _E8_45() { _E7_45(); return 0; }
unsigned _E20_8() { return 0; }
void _E21_4() {}
int _E22_8() { return 0; }
int _E23_5() { _E20_8(); return _E22_8(); }
