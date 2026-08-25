#include "ENGINE/Source/Frame/frame.h"

#include "ENGINE/Source/Base/coordinate.h"
#include "ENGINE/Source/Scrn/scrn.h"
#include "ENGINE/Source/Services/camera.h"

#include <algorithm>

namespace {
void DispatchTo(CSimpleFrame* frame, const std::function<void(CSimpleFrame*)>& callback)
{
    if (frame && frame->m_visible && frame->m_shown) callback(frame);
}

CMouseEvent MakeMouseEvent(const EVENT_DATA_MOUSE* data, unsigned id, bool convertPosition)
{
    CMouseEvent event;
    event.id = id;
    event.param = nullptr;
    if (data) static_cast<EVENT_DATA_MOUSE&>(event) = *data;
    if (data && convertPosition)
        NDCToDDC(&event.x, &event.y, data->x, data->y);
    return event;
}

int __fastcall TopOnChar(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    CCharEvent event;
    event.id = INPUT_EVENT_CHAR;
    static_cast<EVENT_DATA_CHAR&>(event) = *static_cast<const EVENT_DATA_CHAR*>(data);
    return top->OnChar(&event) ? 0 : 1;
}

int __fastcall TopOnIme(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    CImeEvent event;
    event.id = INPUT_EVENT_IME;
    static_cast<EVENT_DATA_IME&>(event) = *static_cast<const EVENT_DATA_IME*>(data);
    top->OnIme(&event);
    return top->m_keyboardFocus ? 0 : 1;
}

int __fastcall TopOnKeyDown(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    CKeyEvent event;
    event.id = INPUT_EVENT_KEYDOWN;
    static_cast<EVENT_DATA_KEY&>(event) = *static_cast<const EVENT_DATA_KEY*>(data);
    return top->OnKeyDown(&event) ? 0 : 1;
}

int __fastcall TopOnKeyUp(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    CKeyEvent event;
    event.id = INPUT_EVENT_KEYUP;
    static_cast<EVENT_DATA_KEY&>(event) = *static_cast<const EVENT_DATA_KEY*>(data);
    return top->OnKeyUp(&event) ? 0 : 1;
}

int __fastcall TopOnKeyRepeat(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    CKeyEvent event;
    event.id = INPUT_EVENT_KEYDOWN_REPEATING;
    static_cast<EVENT_DATA_KEY&>(event) = *static_cast<const EVENT_DATA_KEY*>(data);
    return top->OnKeyDownRepeat(&event) ? 0 : 1;
}

int __fastcall TopOnMouseMove(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    auto event = MakeMouseEvent(static_cast<const EVENT_DATA_MOUSE*>(data),
                                INPUT_EVENT_MOUSEMOVE, true);
    top->OnMouseMove(&event);
    return top->m_mouseFocus ? 0 : 1;
}

int __fastcall TopOnMouseMoveRelative(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    auto event = MakeMouseEvent(static_cast<const EVENT_DATA_MOUSE*>(data),
                                INPUT_EVENT_MOUSEMOVE_RELATIVE, true);
    top->OnMouseMoveRelative(&event);
    return top->m_mouseCapture ? 0 : 1;
}

int __fastcall TopOnMouseDown(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    auto event = MakeMouseEvent(static_cast<const EVENT_DATA_MOUSE*>(data),
                                INPUT_EVENT_MOUSEDOWN, true);
    top->OnMouseDown(&event);
    return top->m_mouseCapture ? 0 : 1;
}

int __fastcall TopOnMouseUp(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    auto event = MakeMouseEvent(static_cast<const EVENT_DATA_MOUSE*>(data),
                                INPUT_EVENT_MOUSEUP, true);
    const bool captured = top->m_mouseCapture != nullptr;
    top->OnMouseUp(&event);
    return captured ? 0 : 1;
}

int __fastcall TopOnMouseWheel(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    if (!top || !data) return 1;
    auto event = MakeMouseEvent(static_cast<const EVENT_DATA_MOUSE*>(data),
                                INPUT_EVENT_MOUSEWHEEL, true);
    return top->OnMouseWheel(&event) ? 0 : 1;
}

int __fastcall TopOnDisplaySizeChanged(const void* data, void* param)
{
    auto* top = static_cast<CSimpleTop*>(param);
    const auto* size = static_cast<const EVENT_DATA_SIZE*>(data);
    if (!top || !size) return 1;
    top->OnDisplaySizeChanged(size->w, size->h);
    return 1;
}
}

void __fastcall PaintCursor(void*, const RECTF*, const RECTF*, float) {
    if (CSimpleTop::s_instance) CSimpleTop::s_instance->DrawCursor();
}
void __fastcall PaintScreen(void*, const RECTF*, const RECTF*, float elapsedSec) {
    if (!CSimpleTop::s_instance) return;
    CSimpleTop::s_instance->OnLayerUpdate(elapsedSec);
    CSimpleTop::s_instance->OnLayerRender();
}

CLayoutFrame* CLayoutFrame::GetLayoutParent() const
{
    return nullptr;
}

CLayoutFrame* CSimpleFrame::GetLayoutParent() const
{
    return m_parent ? static_cast<CLayoutFrame*>(m_parent)
                    : static_cast<CLayoutFrame*>(m_top);
}

CSimpleFrame* CSimpleFrame::GetToplevelFrame() const
{
    const CSimpleFrame* frame = this;
    while (frame && frame->m_parent) frame = frame->m_parent;
    return const_cast<CSimpleFrame*>(frame);
}

bool CSimpleFrame::IsAncestor(const CSimpleFrame* frame) const
{
    for (auto* parent = frame; parent; parent = parent->m_parent)
        if (parent == this) return true;
    return false;
}

CFrameStrata::~CFrameStrata()
{
    for (auto* node : nodes) delete node;
    nodes.clear();
}

bool CFrameStrata::FrameOccluded(const CSimpleFrame* frame) const
{
    if (!frame) return false;
    const auto rect = frame->GetHitRect();
    for (const auto* node : nodes) {
        if (!node || !node->frame || node->frame == frame) continue;
        const auto other = node->frame->GetHitRect();
        if (other.l <= rect.l && other.t <= rect.t && other.r >= rect.r && other.b >= rect.b &&
            node->frame->m_shown && node->frame->m_visible)
            return true;
    }
    return false;
}

void CFrameStrata::CheckOcclusion()
{
    nodes.erase(std::remove(nodes.begin(), nodes.end(), nullptr), nodes.end());
}

CSimpleTop::CSimpleTop()
{
    if (!s_instance) s_instance = this;
    ScrnLayerCreate(nullptr, 0, 1.0f, nullptr, PaintScreen, &m_screenLayer);
    ScrnLayerCreate(nullptr, 0, 8.0f, nullptr, PaintCursor, &m_cursorLayer);
    NDCToDDC(&m_rect.r, &m_rect.b, 1.0f, 1.0f);
    m_width = m_rect.r;
    m_height = m_rect.b;
    EnableEvents();
}

CSimpleTop::~CSimpleTop()
{
    DisableEvents();
    if (m_cursorLayer) { ScrnLayerDestroy(m_cursorLayer); m_cursorLayer = 0; }
    if (m_screenLayer) { ScrnLayerDestroy(m_screenLayer); m_screenLayer = 0; }
    m_frames.clear();
    if (s_instance == this) s_instance = nullptr;
}

void CSimpleTop::EnumerateFrames(int (*callback)(CSimpleFrame*, void*), void* param)
{
    if (!callback) return;
    for (auto* frame : m_frames)
        if (frame && callback(frame, param) == 0) break;
}

void CSimpleTop::ValidateDeletedFrame(CSimpleFrame* frame)
{
    if (!frame) return;
    m_frames.erase(std::remove(m_frames.begin(), m_frames.end(), frame), m_frames.end());
    if (m_mouseFocus == frame) m_mouseFocus = nullptr;
    if (m_mouseCapture == frame) m_mouseCapture = nullptr;
    if (m_keyboardFocus == frame) m_keyboardFocus = nullptr;
    if (m_keyboardFallback == frame) m_keyboardFallback = nullptr;
    for (auto& capture : m_keyCapture)
        if (capture == frame) capture = nullptr;
}

void CSimpleTop::RegisterFrame(CSimpleFrame* frame)
{
    if (frame && std::find(m_frames.begin(), m_frames.end(), frame) == m_frames.end())
        m_frames.push_back(frame);
}

void CSimpleTop::UnregisterFrame(CSimpleFrame* frame) { ValidateDeletedFrame(frame); }

void CSimpleTop::NotifyFrameMovedOrResized(CSimpleFrame* frame)
{
    if (frame) frame->Resize();
}

void CSimpleTop::NotifyFrameLayerChanged(CSimpleFrame*)
{
    std::stable_sort(m_frames.begin(), m_frames.end(), [](const auto* left, const auto* right) {
        if (left->m_strata != right->m_strata) return left->m_strata < right->m_strata;
        return left->m_level < right->m_level;
    });
}

void CSimpleTop::RegisterForEvent(CSimpleFrame* frame, unsigned eventId)
{
    if (frame) frame->EnableEvent(eventId);
}

void CSimpleTop::UnregisterForEvent(CSimpleFrame* frame, unsigned eventId)
{
    if (frame) frame->DisableEvent(eventId);
}

void CSimpleTop::RegisterForDelete(CSimpleFrame* frame) { ValidateDeletedFrame(frame); }

void CSimpleTop::SetCursor(float x, float y) { m_cursor = {x, y}; }

void CSimpleTop::RaiseFrame(CSimpleFrame* frame)
{
    if (!frame) return;
    auto it = std::find(m_frames.begin(), m_frames.end(), frame);
    if (it != m_frames.end()) {
        auto value = *it;
        m_frames.erase(it);
        m_frames.push_back(value);
    }
}

void CSimpleTop::LowerFrame(CSimpleFrame* frame)
{
    if (!frame) return;
    auto it = std::find(m_frames.begin(), m_frames.end(), frame);
    if (it != m_frames.end()) {
        auto value = *it;
        m_frames.erase(it);
        m_frames.insert(m_frames.begin(), value);
    }
}

void CSimpleTop::StartMoveOrResizeFrame(CSimpleFrame* frame, MOUSEBUTTON button)
{
    m_moveFrame = frame;
    m_moveButton = button;
    if (frame) m_cursor = frame->m_clickPoint;
}

void CSimpleTop::MoveOrResizeFrame(float x, float y)
{
    if (!m_moveFrame) return;
    const float dx = x - m_cursor.x;
    const float dy = y - m_cursor.y;
    m_moveFrame->DragBy(dx, dy);
    SetCursor(x, y);
}

void CSimpleTop::StopMoveOrResizeFrame()
{
    m_moveFrame = nullptr;
    m_moveButton = MOUSE_BUTTON_NONE;
}

void CSimpleTop::OnLayerUpdate(float elapsedSec)
{
    if (!m_eventsEnabled) return;
    for (auto* frame : m_frames) DispatchTo(frame, [elapsedSec](auto* item) { item->OnLayerUpdate(elapsedSec); });
}

void CSimpleTop::OnLayerRender()
{
    const NTempest::C2Vector origin{m_rect.l, m_rect.t};
    CameraSetupScreenProjection(&m_rect, &origin, 0.0f);

    std::stable_sort(m_frames.begin(), m_frames.end(), [](const auto* left, const auto* right) {
        if (!left || !right) return left != nullptr;
        if (left->m_strata != right->m_strata) return left->m_strata < right->m_strata;
        return left->m_level < right->m_level;
    });

    static bool reportedGameHud = false;
    if (!reportedGameHud && SimpleFrameRegistryGetEntry(nullptr, "ActionButton1")) {
        static constexpr const char* names[] = {
            "UIParent", "WorldFrame", "PlayerFrame", "TargetFrame",
            "MainMenuBar", "MainMenuBarArtFrame", "MainMenuExpBar",
            "ActionButton1", "ActionButton2", "ActionButton3",
            "MinimapCluster", "Minimap"
        };
        std::fprintf(stderr, "HUD_LAYOUT top=(%.6f,%.6f,%.6f,%.6f) frames=%zu\n",
                     m_rect.l, m_rect.t, m_rect.r, m_rect.b, m_frames.size());
        for (const char* name : names) {
            CSimpleFrame* frame = SimpleFrameRegistryGetEntry(nullptr, name);
            if (!frame) {
                std::fprintf(stderr, "HUD_FRAME name=%s missing=1\n", name);
                continue;
            }
            NTempest::CRect rect{};
            const bool hasRect = frame->GetRect(&rect);
            std::fprintf(stderr,
                         "HUD_FRAME name=%s shown=%d visible=%d strata=%d level=%d "
                         "rect=%d(%.6f,%.6f,%.6f,%.6f) parent=%s regions=%zu "
                         "layers=%zu,%zu,%zu,%zu,%zu\n",
                         name, frame->m_shown ? 1 : 0, frame->m_visible ? 1 : 0,
                         frame->m_strata, frame->m_level, hasRect ? 1 : 0,
                         rect.l, rect.t, rect.r, rect.b,
                         frame->m_parent && frame->m_parent->GetName()
                             ? frame->m_parent->GetName() : "<none>",
                         frame->m_regions.size(), frame->m_drawlayers[0].size(),
                         frame->m_drawlayers[1].size(), frame->m_drawlayers[2].size(),
                         frame->m_drawlayers[3].size(), frame->m_drawlayers[4].size());
        }
        std::fflush(stderr);
        reportedGameHud = true;
    }

    std::size_t first = 0;
    while (first < m_frames.size()) {
        if (!m_frames[first]) { ++first; continue; }
        const int strata = m_frames[first]->m_strata;
        const int level = m_frames[first]->m_level;
        std::size_t last = first + 1;
        while (last < m_frames.size() && m_frames[last] &&
               m_frames[last]->m_strata == strata && m_frames[last]->m_level == level)
            ++last;

        for (unsigned layer = 0; layer < 5; ++layer) {
            CRenderBatch batch;
            for (std::size_t i = first; i < last; ++i) {
                auto* frame = m_frames[i];
                if (frame->m_visible && frame->m_shown && (frame->m_flags & 0x2000u) == 0) {
                    frame->OnFrameRender(&batch, layer);
                }
            }
            batch.Finish();
            CSimpleRender::DrawBatch(&batch);
        }
        first = last;
    }
}

void CSimpleTop::EnableEvents()
{
    if (m_eventsEnabled) return;
    EventRegisterEx(EVENT_ID_CHAR, TopOnChar, this, 1.0f);
    EventRegisterEx(EVENT_ID_IME, TopOnIme, this, 1.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN, TopOnKeyDown, this, 1.0f);
    EventRegisterEx(EVENT_ID_KEYUP, TopOnKeyUp, this, 1.0f);
    EventRegisterEx(EVENT_ID_KEYDOWN_REPEATING, TopOnKeyRepeat, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEMOVE, TopOnMouseMove, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, TopOnMouseMoveRelative, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEDOWN, TopOnMouseDown, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEUP, TopOnMouseUp, this, 1.0f);
    EventRegisterEx(EVENT_ID_MOUSEWHEEL, TopOnMouseWheel, this, 1.0f);
    EventRegisterEx(EVENT_ID_SIZE, TopOnDisplaySizeChanged, this, 1.0f);
    m_eventsEnabled = true;
}

void CSimpleTop::DisableEvents()
{
    if (!m_eventsEnabled) return;
    EventUnregisterEx(EVENT_ID_CHAR, TopOnChar, this, ~0u);
    EventUnregisterEx(EVENT_ID_IME, TopOnIme, this, ~0u);
    EventUnregisterEx(EVENT_ID_KEYDOWN, TopOnKeyDown, this, ~0u);
    EventUnregisterEx(EVENT_ID_KEYUP, TopOnKeyUp, this, ~0u);
    EventUnregisterEx(EVENT_ID_KEYDOWN_REPEATING, TopOnKeyRepeat, this, ~0u);
    EventUnregisterEx(EVENT_ID_MOUSEMOVE, TopOnMouseMove, this, ~0u);
    EventUnregisterEx(EVENT_ID_MOUSEMOVE_RELATIVE, TopOnMouseMoveRelative, this, ~0u);
    EventUnregisterEx(EVENT_ID_MOUSEDOWN, TopOnMouseDown, this, ~0u);
    EventUnregisterEx(EVENT_ID_MOUSEUP, TopOnMouseUp, this, ~0u);
    EventUnregisterEx(EVENT_ID_MOUSEWHEEL, TopOnMouseWheel, this, ~0u);
    EventUnregisterEx(EVENT_ID_SIZE, TopOnDisplaySizeChanged, this, ~0u);
    m_eventsEnabled = false;
}
void CSimpleTop::DrawCursor() {}

int CSimpleTop::OnChar(CCharEvent* event)
{
    if (!event) return 0;
    if (m_keyboardFocus && m_keyboardFocus->OnLayerChar(event)) return 1;
    for (auto it = m_frames.rbegin(); it != m_frames.rend(); ++it) {
        CSimpleFrame* frame = *it;
        if (!frame || frame == m_keyboardFocus ||
            (frame->m_eventmask & SIMPLE_EVENT_KEYBOARD) == 0U) continue;
        if (frame->OnLayerChar(event)) return 1;
    }
    return 0;
}

void CSimpleTop::OnIme(CImeEvent* event)
{
    if (auto* edit = dynamic_cast<CSimpleEditBox*>(m_keyboardFocus)) edit->OnLayerIme(event);
}

int CSimpleTop::OnKeyDown(CKeyEvent* event)
{
    if (!event || event->key < 0 || event->key >= KEY_LAST) return 0;
    if (m_keyboardFocus && m_keyboardFocus->OnLayerKeyDown(event)) {
        m_keyCapture[static_cast<unsigned>(event->key)] = m_keyboardFocus;
        return 1;
    }
    for (auto it = m_frames.rbegin(); it != m_frames.rend(); ++it) {
        CSimpleFrame* frame = *it;
        if (!frame || frame == m_keyboardFocus ||
            (frame->m_eventmask & SIMPLE_EVENT_KEYBOARD) == 0U) continue;
        if (frame->OnLayerKeyDown(event)) {
            m_keyCapture[static_cast<unsigned>(event->key)] = frame;
            return 1;
        }
    }
    return 0;
}

int CSimpleTop::OnKeyUp(CKeyEvent* event)
{
    if (!event || event->key < 0 || event->key >= KEY_LAST) return 0;
    CSimpleFrame*& capture = m_keyCapture[static_cast<unsigned>(event->key)];
    if (capture) {
        CSimpleFrame* frame = capture;
        capture = nullptr;
        frame->OnLayerKeyUp(event);
        return 1;
    }
    if (event->key == KEY_PRINTSCREEN) return OnKeyDown(event);
    return 0;
}

int CSimpleTop::OnKeyDownRepeat(CKeyEvent* event)
{
    if (!event || event->key < 0 || event->key >= KEY_LAST) return 0;
    if (CSimpleFrame* capture = m_keyCapture[static_cast<unsigned>(event->key)])
        return capture->OnLayerKeyDownRepeat(event);
    return 0;
}

void CSimpleTop::OnMouseMove(CMouseEvent* event)
{
    if (!event) return;
    SetCursor(event->x, event->y);
    CSimpleFrame* nextFocus = nullptr;
    for (auto it = m_frames.rbegin(); it != m_frames.rend(); ++it) {
        if (*it && (((*it)->m_eventmask & SIMPLE_EVENT_MOUSE) != 0) &&
            (*it)->TestHitRect(event->x, event->y)) {
            nextFocus = *it;
            break;
        }
    }
    if (nextFocus != m_mouseFocus) {
        if (m_mouseFocus) m_mouseFocus->OnLayerCursorExit();
        m_mouseFocus = nextFocus;
        if (m_mouseFocus) m_mouseFocus->OnLayerCursorEnter();
    }
}

void CSimpleTop::OnMouseMoveRelative(CMouseEvent* event)
{
    if (m_mouseCapture) m_mouseCapture->OnLayerMouseMoveRelative(event);
}

void CSimpleTop::OnMouseDown(CMouseEvent* event)
{
    if (!event) return;
    OnMouseMove(event);
    if (m_mouseFocus) {
        m_mouseCapture = m_mouseFocus;
        m_mouseCapture->OnLayerMouseDown(event);
    }
}

void CSimpleTop::OnMouseUp(CMouseEvent* event)
{
    if (!event) return;
    if (m_mouseCapture) m_mouseCapture->OnLayerMouseUp(event);
    if (event->buttonState == 0) m_mouseCapture = nullptr;
}

int CSimpleTop::OnMouseWheel(CMouseEvent* event)
{
    if (!event) return 0;
    for (auto it = m_frames.rbegin(); it != m_frames.rend(); ++it) {
        CSimpleFrame* frame = *it;
        if (!frame || !frame->m_visible || !frame->m_shown ||
            (frame->m_eventmask & SIMPLE_EVENT_MOUSE) == 0U ||
            !frame->TestHitRect(event->x, event->y))
            continue;
        if (frame->OnLayerMouseWheel(event)) return 1;
    }
    return 0;
}

void CSimpleTop::OnDisplaySizeChanged(int width, int height)
{
    CoordinateSetDisplaySize(static_cast<float>(width), static_cast<float>(height));
    NDCToDDC(&m_rect.r, &m_rect.b, 1.0f, 1.0f);
    m_width = m_rect.r;
    m_height = m_rect.b;
    for (auto* frame : m_frames)
        if (frame && !frame->m_parent) frame->Resize();
}

void CFrameStrataNode::BuildBatches()
{
    for (auto& batch : batches) batch.Clear();
    if (!frame) return;
    for (unsigned layer = 0; layer < batches.size(); ++layer) {
        frame->OnFrameRender(&batches[layer], layer);
        batches[layer].Finish();
    }
}
