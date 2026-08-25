#include "ENGINE/Source/Frame/csimpleframe.h"

#include "ENGINE/Source/Base/status.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Frame/framexml.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

namespace {
float XmlFloat(const XMLNode* node, const char* name, float fallback)
{
    if (!node) return fallback;
    const auto* value = node->GetAttributeByName(name);
    return value ? std::strtof(value, nullptr) : fallback;
}

int XmlInt(const XMLNode* node, const char* name, int fallback)
{
    if (!node) return fallback;
    const auto* value = node->GetAttributeByName(name);
    return value ? std::atoi(value) : fallback;
}

bool XmlBool(const XMLNode* node, const char* name, bool fallback)
{
    if (!node) return fallback;
    const auto* value = node->GetAttributeByName(name);
    if (!value) return fallback;
    return _stricmp(value, "true") == 0 || std::atoi(value) != 0;
}

std::string ResolveParentName(const char* value, CSimpleFrame* parent)
{
    if (!value) return {};
    constexpr char token[] = "$parent";
    if (_strnicmp(value, token, sizeof(token) - 1) != 0) return value;
    while (parent && (!parent->GetName() || !*parent->GetName())) parent = parent->ParentFrame();
    std::string name = parent ? parent->GetName() : "TOP";
    name += value + sizeof(token) - 1;
    return name;
}

unsigned DrawLayerFromString(const char* value)
{
    if (!value || !*value || _stricmp(value, "ARTWORK") == 0) return 2;
    if (_stricmp(value, "BACKGROUND") == 0) return 0;
    if (_stricmp(value, "BORDER") == 0) return 1;
    if (_stricmp(value, "OVERLAY") == 0) return 3;
    if (_stricmp(value, "HIGHLIGHT") == 0) return 4;
    return 2;
}
}

CSimpleTop* CSimpleTop::s_instance = nullptr;

CBackdropGenerator::~CBackdropGenerator()
{
    delete m_backgroundTexture;
    delete m_leftTexture;
    delete m_rightTexture;
    delete m_topTexture;
    delete m_bottomTexture;
    delete m_topLeftTexture;
    delete m_topRightTexture;
    delete m_bottomLeftTexture;
    delete m_bottomRightTexture;
}

CSimpleFrame::CSimpleFrame(CSimpleFrame* parent)
{
    if (!CSimpleTop::s_instance) CSimpleTop::s_instance = new CSimpleTop;
    m_top = CSimpleTop::s_instance;
    m_hitRect = m_rect;
    m_hitOffset = {};
    m_top->RegisterFrame(this);
    SetParent(parent);
    ShowThis();
}

CSimpleFrame::~CSimpleFrame()
{
    while (!m_regions.empty()) {
        CSimpleRegion* region = m_regions.back();
        if (!region) {
            m_regions.pop_back();
            continue;
        }
        delete region;
    }
    ClearFromSimpleRegistry();
    ClearChildrenFromSimpleRegistry();
    UnparentFrame();
    if (m_top) m_top->UnregisterFrame(this);
}

void CSimpleFrame::PreLoadXML(const XMLNode* node, CStatus*)
{
    if (node) {
        const char* value = node->GetAttributeByName("name");
        if (value && *value) {
            SetName(ResolveParentName(value, m_parent).c_str());
            AddToFrameRegistry();
        }
    }
    m_loading = true;
}

void CSimpleFrame::LoadXML(const XMLNode* node, CStatus* status)
{
    CLayoutFrame::LoadXML(node, status);
    if (!node) return;
    m_id = XmlInt(node, "id", m_id);
    m_alpha = static_cast<unsigned char>(std::clamp(XmlFloat(node, "alpha", m_alpha / 255.0f),
                                                    0.0f, 1.0f) * 255.0f + 0.5f);
    if (node->GetAttributeByName("hidden")) {
        m_shown = !XmlBool(node, "hidden", false);
        if (m_shown) ShowThis();
        else HideThis();
    }
    m_level = std::max(0, XmlInt(node, "frameLevel", m_level));
    if (node->GetAttributeByName("width")) SetWidth(XmlFloat(node, "width", GetWidth()));
    if (node->GetAttributeByName("height")) SetHeight(XmlFloat(node, "height", GetHeight()));
    if (const auto* value = node->GetAttributeByName("frameStrata")) {
        if (_stricmp(value, "BACKGROUND") == 0) m_strata = FRAME_STRATA_BACKGROUND;
        else if (_stricmp(value, "LOW") == 0) m_strata = FRAME_STRATA_LOW;
        else if (_stricmp(value, "HIGH") == 0) m_strata = FRAME_STRATA_HIGH;
        else if (_stricmp(value, "DIALOG") == 0) m_strata = FRAME_STRATA_DIALOG;
        else m_strata = std::clamp(std::atoi(value), 0, 6);
    }
    OnFrameSizeChanged();
}

void CSimpleFrame::LoadXML_Layers(const XMLNode* node, CStatus* status)
{
    if (!node) return;
    for (const XMLNode* layerNode = node->m_child; layerNode; layerNode = layerNode->m_next) {
        if (_stricmp(layerNode->m_name.c_str(), "Layer") != 0) {
            if (status) status->Add(STATUS_WARNING, "Unknown child node in %s element: %s",
                                    node->m_name.c_str(), layerNode->m_name.c_str());
            continue;
        }
        const unsigned layer = DrawLayerFromString(layerNode->GetAttributeByName("level"));
        for (const XMLNode* regionNode = layerNode->m_child; regionNode; regionNode = regionNode->m_next) {
            CSimpleRegion* region = nullptr;
            if (_stricmp(regionNode->m_name.c_str(), "Texture") == 0) {
                auto* texture = new CSimpleTexture(this);
                FrameXML_LoadTextureNode(texture, regionNode, status);
                region = texture;
            } else if (_stricmp(regionNode->m_name.c_str(), "FontString") == 0) {
                auto* string = new CSimpleFontString(this);
                FrameXML_LoadFontStringNode(string, regionNode, status);
                region = string;
            } else {
                if (status) status->Add(STATUS_WARNING, "Unknown child node in %s element: %s",
                                        layerNode->m_name.c_str(), regionNode->m_name.c_str());
            }
            if (region) {
                RemoveFrameRegion(region);
                RegisterRegion(region, layer);
            }
        }
    }
}

void CSimpleFrame::LoadXML_Scripts(const XMLNode* node, CStatus*)
{
    if (!node) return;
    const XMLNode* scripts = node;
    if (_stricmp(node->m_name.c_str(), "Scripts") != 0) {
        scripts = nullptr;
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "Scripts") == 0) { scripts = child; break; }
        }
    }
    if (!scripts) return;
    for (const XMLNode* script = scripts->m_child; script; script = script->m_next) {
        int* slot = nullptr;
        if (_stricmp(script->m_name.c_str(), "OnLoad") == 0) slot = &m_onLoad;
        else if (_stricmp(script->m_name.c_str(), "OnEvent") == 0) slot = &m_onEvent;
        else if (_stricmp(script->m_name.c_str(), "OnUpdate") == 0) slot = &m_onUpdate;
        else if (_stricmp(script->m_name.c_str(), "OnShow") == 0) slot = &m_onShow;
        else if (_stricmp(script->m_name.c_str(), "OnHide") == 0) slot = &m_onHide;
        else if (_stricmp(script->m_name.c_str(), "OnEnter") == 0) slot = &m_onEnter;
        else if (_stricmp(script->m_name.c_str(), "OnLeave") == 0) slot = &m_onLeave;
        else if (_stricmp(script->m_name.c_str(), "OnMouseDown") == 0) slot = &m_onMouseDown;
        else if (_stricmp(script->m_name.c_str(), "OnMouseUp") == 0) slot = &m_onMouseUp;
        else if (_stricmp(script->m_name.c_str(), "OnMouseWheel") == 0) slot = &m_onMouseWheel;
        else if (_stricmp(script->m_name.c_str(), "OnDragStart") == 0) slot = &m_onDragStart;
        else if (_stricmp(script->m_name.c_str(), "OnDragStop") == 0) slot = &m_onDragStop;
        else if (_stricmp(script->m_name.c_str(), "OnReceiveDrag") == 0) slot = &m_onReceiveDrag;
        else if (_stricmp(script->m_name.c_str(), "OnChar") == 0) slot = &m_onChar;
        else if (_stricmp(script->m_name.c_str(), "OnKeyDown") == 0) slot = &m_onKeyDown;
        else if (_stricmp(script->m_name.c_str(), "OnKeyUp") == 0) slot = &m_onKeyUp;
        if (slot) {
            std::string debugName = GetName() && *GetName() ? GetName() : "<unnamed>";
            debugName += ":";
            debugName += script->m_name;
            SetEventScript(*slot, script->m_value.c_str(), debugName.c_str());
        }
    }
}

void CSimpleFrame::PostLoadXML(const XMLNode*, CStatus*)
{
    m_loading = false;
    Resize();
    if (m_onLoad) FrameScript_Execute(m_onLoad, this);
    if (m_visible && m_onShow) FrameScript_Execute(m_onShow, this);
}

void CSimpleFrame::DelayedDelete()
{
    HideThis();
    if (m_top) m_top->RegisterForDelete(this);
}

CLayoutFrame* CSimpleFrame::GetLayoutFrameByName(const char* context, const char* name)
{
    return CLayoutFrame::GetLayoutFrameByName(this, context, name);
}

void CSimpleFrame::SetFrameFlag(unsigned flag, bool set)
{
    if (set) m_flags |= flag;
    else m_flags &= ~flag;
}

void CSimpleFrame::SetBeingScrolled(bool value)
{
    SetFrameFlag(0x2000u, value);
    if (value)
        for (unsigned layer = 0; layer < m_drawlayers.size(); ++layer)
            OnUpdateBatch(layer);
    for (auto* child : m_children)
        if (child) child->SetBeingScrolled(value);
}

void CSimpleFrame::SetFrameStrata(int strata)
{
    m_strata = std::clamp(strata, 0, 6);
    if (m_top) m_top->NotifyFrameLayerChanged(this);
}

void CSimpleFrame::SetFrameLevel(int level)
{
    m_level = std::max(0, level);
    if (m_top) m_top->NotifyFrameLayerChanged(this);
}

void CSimpleFrame::Raise() { if (m_top) m_top->RaiseFrame(this); }
void CSimpleFrame::Lower() { if (m_top) m_top->LowerFrame(this); }

void CSimpleFrame::SetBackdrop(CBackdropGenerator* backdrop)
{
    m_backdrop.reset(backdrop);
    if (m_backdrop) m_backdrop->SetOutput(this);
    NotifyDrawLayersChanged();
}

void CSimpleFrame::SetHighlight(bool highlight)
{
    if (highlight) EnableDrawLayer(4);
    else DisableDrawLayer(4);
}

int CSimpleFrame::SetHighlight(CStatus* texture, EGxBlend blendMode)
{
    (void)texture;
    (void)blendMode;
    return texture ? 1 : 0;
}

int CSimpleFrame::SetHighlight(CSimpleTexture* texture, EGxBlend blendMode)
{
    if (!texture) {
        SetHighlight(false);
        return 0;
    }
    texture->SetBlendMode(blendMode);
    RegisterRegion(texture, 4);
    return 1;
}

void CSimpleFrame::SetAlpha(float alpha)
{
    m_alpha = static_cast<unsigned char>(std::clamp(alpha, 0.0f, 1.0f) * 255.0f + 0.5f);
    for (auto* region : m_regions) if (region) region->SetAlpha(alpha);
}

void CSimpleFrame::EnableDrawLayer(unsigned layer)
{
    if (layer >= m_drawenabled.size()) return;
    m_drawenabled[layer] = true;
    NotifyDrawLayerChanged(nullptr);
}

void CSimpleFrame::DisableDrawLayer(unsigned layer)
{
    if (layer >= m_drawenabled.size()) return;
    m_drawenabled[layer] = false;
    NotifyDrawLayerChanged(nullptr);
}

void CSimpleFrame::RegisterRegion(CSimpleRegion* region, unsigned layer)
{
    if (!region) return;
    if (std::find(m_regions.begin(), m_regions.end(), region) == m_regions.end())
        m_regions.push_back(region);
    if (layer < m_drawlayers.size() &&
        std::find(m_drawlayers[layer].begin(), m_drawlayers[layer].end(), region) == m_drawlayers[layer].end()) {
        m_drawlayers[layer].push_back(region);
        m_batchDirty |= 1u << layer;
    }
}

void CSimpleFrame::UnregisterRegion(CSimpleRegion* region)
{
    RemoveFrameRegion(region);
}

void CSimpleFrame::AddFrameRegion(CSimpleRegion* region, unsigned layer)
{
    RegisterRegion(region, layer);
}

void CSimpleFrame::RemoveFrameRegion(CSimpleRegion* region)
{
    if (!region) return;
    m_regions.erase(std::remove(m_regions.begin(), m_regions.end(), region), m_regions.end());
    for (auto& layer : m_drawlayers)
        layer.erase(std::remove(layer.begin(), layer.end(), region), layer.end());
    NotifyDrawLayersChanged();
}

void CSimpleFrame::NotifyDrawLayerChanged(CSimpleRegion*)
{
    m_batchDirty |= 1u;
}

void CSimpleFrame::NotifyDrawLayersChanged()
{
    m_batchDirty |= 0x1fu;
}

void CSimpleFrame::AddToFrameRegistry(const char* context)
{
    if (!m_frameName.empty()) {
        SimpleFrameRegistryAddEntry(this, context, m_frameName.c_str());
        RegisterScriptObject(m_frameName.c_str());
    }
}

void CSimpleFrame::ClearFromSimpleRegistry()
{
    SimpleFrameRegistryRemoveEntry(this);
}

void CSimpleFrame::UnparentFrame()
{
    if (!m_parent) return;
    auto* parent = m_parent;
    m_parent = nullptr;
    parent->m_children.erase(std::remove(parent->m_children.begin(), parent->m_children.end(), this),
                             parent->m_children.end());
}

void CSimpleFrame::ParentFrame(CSimpleFrame* frame)
{
    SetParent(frame);
}

void CSimpleFrame::ClearChildrenFromSimpleRegistry()
{
    for (auto* child : m_children) if (child) child->ClearFromSimpleRegistry();
}

void CSimpleFrame::SetParent(CSimpleFrame* parent)
{
    if (parent == this || (parent && IsAncestor(parent))) return;
    if (parent == m_parent) return;
    const bool wasVisible = m_visible;
    if (wasVisible) HideThis();
    UnparentFrame();
    m_parent = parent;
    if (m_parent) {
        SetFrameStrata(m_parent->m_strata);
        SetFrameLevel(m_parent->m_level + 1);
        SetLayoutScale(m_parent->m_layoutScale);
        SetBeingScrolled((m_parent->m_flags & 0x2000u) != 0);
        if (std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this) == m_parent->m_children.end())
            m_parent->m_children.push_back(this);
    } else {
        SetFrameStrata(FRAME_STRATA_MEDIUM);
        SetFrameLevel(0);
        SetLayoutScale(1.0F);
        SetBeingScrolled(false);
    }
    SetDeferredResize(parent != nullptr && parent->IsResizePending());
    if (wasVisible && (!m_parent || m_parent->m_visible)) ShowThis();
}

void CSimpleFrame::SetDeferredResize(bool deferred)
{
    CLayoutFrame::SetDeferredResize(deferred);
    for (auto* child : m_children) if (child) child->SetDeferredResize(deferred);
}

void CSimpleFrame::SetLayoutScale(float scale)
{
    CLayoutFrame::SetLayoutScale(scale);
    for (auto* child : m_children) if (child) child->SetLayoutScale(scale);
}

void CSimpleFrame::HideThis()
{
    if (!m_visible) return;
    m_visible = false;
    OnLayerHide();
    for (auto* child : m_children) if (child) child->HideThis();
    NotifyDrawLayersChanged();
}

void CSimpleFrame::ShowThis()
{
    if (!m_shown || (m_parent && !m_parent->m_visible) || m_visible) return;
    m_visible = true;
    OnLayerShow();
    for (auto* child : m_children) if (child) child->ShowThis();
    NotifyDrawLayersChanged();
}

void CSimpleFrame::EnableEvent(unsigned eventMask, unsigned)
{
    m_eventmask |= eventMask;
}

void CSimpleFrame::DisableEvent(unsigned eventMask)
{
    m_eventmask &= ~eventMask;
}

void CSimpleFrame::RegisterForEvents()
{
    EnableEvent(SIMPLE_EVENT_ALL);
}

void CSimpleFrame::UnregisterForEvents()
{
    DisableEvent(SIMPLE_EVENT_ALL);
}

bool CSimpleFrame::TestHitRect(float x, float y) const
{
    const auto rect = GetHitRect();
    return m_visible && m_shown && x >= rect.l && x <= rect.r && y >= rect.t && y <= rect.b;
}

void CSimpleFrame::SetHitRect(const NTempest::CRect& rect)
{
    m_hitRect = rect;
}

void CSimpleFrame::SetHitRectInsets(const NTempest::CRect& insets)
{
    m_hitOffset = insets;
    m_hitRect = {m_rect.l + insets.l, m_rect.t + insets.t,
                 m_rect.r - insets.r, m_rect.b - insets.b};
}

NTempest::CRect CSimpleFrame::GetHitRect() const
{
    return m_hitRect.IsEmpty() ? m_rect : m_hitRect;
}

void CSimpleFrame::OnLayerShow()
{
    if (m_onShow && !m_loading) FrameScript_Execute(m_onShow, this);
}

void CSimpleFrame::OnLayerHide()
{
    if (m_onHide && !m_loading) FrameScript_Execute(m_onHide, this);
}

void CSimpleFrame::OnLayerUpdate(float elapsedSec)
{
    if (m_onUpdate && !m_loading) FrameScript_Execute(m_onUpdate, this, "%f", elapsedSec);
}

void CSimpleFrame::OnLayerTrackUpdate(float elapsedSec)
{
    for (auto* child : m_children) if (child) child->OnLayerTrackUpdate(elapsedSec);
}

void CSimpleFrame::OnUpdateBatch(unsigned layer)
{
    // CSimpleFrame::OnUpdateBatch at 0x76E440.
    if (layer < 32U) m_batchDirty |= 1U << layer;
}

void CSimpleFrame::OnFrameRender(CRenderBatch* batch, unsigned layer)
{
    if (!batch || layer >= m_drawlayers.size() || !m_drawenabled[layer]) return;
    for (auto* region : m_drawlayers[layer]) {
        if (!region || !region->IsVisible()) continue;
        if (auto* texture = dynamic_cast<CSimpleTexture*>(region)) texture->Draw(batch);
        else if (auto* text = dynamic_cast<CSimpleFontString*>(region)) text->Draw(batch);
    }
}

void CSimpleFrame::OnFrameRender()
{
    if (!m_visible || !m_shown) return;
    for (unsigned layer = 0; layer < m_batch.size(); ++layer) {
        auto& batch = m_batch[layer];
        batch.Clear();
        OnFrameRender(&batch, layer);
        batch.Finish();
        CSimpleRender::DrawBatch(&batch);
    }
    m_batchDirty = 0;
    for (auto* child : m_children)
        if (child && (child->m_flags & 0x2000u) != 0) child->OnFrameRender();
}

void CSimpleFrame::OnFrameSizeChanged()
{
    m_hitRect = m_rect;
    for (auto* region : m_regions) if (region) region->Resize();
    for (auto* child : m_children) if (child) child->Resize();
}

void CSimpleFrame::OnFrameSizeChanged(const NTempest::CRect* rect)
{
    if (rect) m_rect = *rect;
    OnFrameSizeChanged();
}

void CSimpleFrame::OnFrameSizeChanged(float width, float height)
{
    SetWidth(width);
    SetHeight(height);
}

CLayoutFrame* CSimpleTitleRegion::GetLayoutParent() const
{
    return m_parent;
}

void CSimpleFrame::OnLayerCursorEnter()
{
    if (m_tooltip) {
        m_tooltip->m_shown = true;
        m_tooltip->ShowThis();
    }
    if (!m_highlightLocked)
        EnableDrawLayer(4);
    if (m_onEnter && !m_loading)
        FrameScript_Execute(m_onEnter, this);
}
void CSimpleFrame::OnLayerCursorExit()
{
    if (m_tooltip) {
        m_tooltip->m_shown = false;
        m_tooltip->HideThis();
    }
    if (!m_highlightLocked)
        DisableDrawLayer(4);
    if (m_onLeave && !m_loading)
        FrameScript_Execute(m_onLeave, this);
}
int CSimpleFrame::OnLayerChar(CCharEvent* event)
{
    if (!event || !m_visible || !m_onChar || m_loading) return 0;
    char text[8]{};
    const unsigned codepoint = static_cast<unsigned>(event->ch);
    if (codepoint < 0x80U) {
        text[0] = static_cast<char>(codepoint);
    } else if (codepoint < 0x800U) {
        text[0] = static_cast<char>(0xC0U | (codepoint >> 6));
        text[1] = static_cast<char>(0x80U | (codepoint & 0x3FU));
    } else {
        text[0] = static_cast<char>(0xE0U | (codepoint >> 12));
        text[1] = static_cast<char>(0x80U | ((codepoint >> 6) & 0x3FU));
        text[2] = static_cast<char>(0x80U | (codepoint & 0x3FU));
    }
    FrameScript_Execute(m_onChar, this, "%s", text);
    return 1;
}

int CSimpleFrame::OnLayerKeyDown(CKeyEvent* event)
{
    if (!event || !m_visible || !m_onKeyDown || m_loading) return 0;
    char functionKey[8]{};
    const char* keyName = "UNKNOWN";
    if (event->key >= KEY_F1 && event->key <= KEY_F12) {
        std::snprintf(functionKey, sizeof(functionKey), "F%d",
                      static_cast<int>(event->key - KEY_F1 + 1));
        keyName = functionKey;
    } else if (event->key >= KEY_A && event->key <= KEY_Z) {
        functionKey[0] = static_cast<char>(event->key);
        keyName = functionKey;
    } else if (event->key >= KEY_0 && event->key <= KEY_0 + 9) {
        functionKey[0] = static_cast<char>(event->key);
        keyName = functionKey;
    } else {
        switch (event->key) {
        case KEY_ESCAPE: keyName = "ESCAPE"; break;
        case KEY_ENTER: keyName = "ENTER"; break;
        case KEY_BACKSPACE: keyName = "BACKSPACE"; break;
        case KEY_TAB: keyName = "TAB"; break;
        case KEY_SPACE: keyName = "SPACE"; break;
        case KEY_LEFT: keyName = "LEFT"; break;
        case KEY_UP: keyName = "UP"; break;
        case KEY_RIGHT: keyName = "RIGHT"; break;
        case KEY_DOWN: keyName = "DOWN"; break;
        case KEY_INSERT: keyName = "INSERT"; break;
        case KEY_DELETE: keyName = "DELETE"; break;
        case KEY_HOME: keyName = "HOME"; break;
        case KEY_END: keyName = "END"; break;
        case KEY_PAGEUP: keyName = "PAGEUP"; break;
        case KEY_PAGEDOWN: keyName = "PAGEDOWN"; break;
        default: break;
        }
    }
    FrameScript_Execute(m_onKeyDown, this, "%s", keyName);
    return 1;
}

int CSimpleFrame::OnLayerKeyDownRepeat(CKeyEvent* event) {
    return OnLayerKeyDown(event);
}

int CSimpleFrame::OnLayerKeyUp(CKeyEvent* event)
{
    if (!event || !m_visible || !m_onKeyUp || m_loading) return 0;
    char keyName[16]{};
    if (event->key >= KEY_A && event->key <= KEY_Z) {
        keyName[0] = static_cast<char>(event->key);
    } else if (event->key >= KEY_F1 && event->key <= KEY_F12) {
        std::snprintf(keyName, sizeof(keyName), "F%d",
                      static_cast<int>(event->key - KEY_F1 + 1));
    } else {
        switch (event->key) {
        case KEY_ESCAPE: strcpy_s(keyName, "ESCAPE"); break;
        case KEY_ENTER: strcpy_s(keyName, "ENTER"); break;
        case KEY_BACKSPACE: strcpy_s(keyName, "BACKSPACE"); break;
        case KEY_TAB: strcpy_s(keyName, "TAB"); break;
        case KEY_SPACE: strcpy_s(keyName, "SPACE"); break;
        case KEY_LEFT: strcpy_s(keyName, "LEFT"); break;
        case KEY_UP: strcpy_s(keyName, "UP"); break;
        case KEY_RIGHT: strcpy_s(keyName, "RIGHT"); break;
        case KEY_DOWN: strcpy_s(keyName, "DOWN"); break;
        default: strcpy_s(keyName, "UNKNOWN"); break;
        }
    }
    FrameScript_Execute(m_onKeyUp, this, "%s", keyName);
    return 1;
}
int CSimpleFrame::OnLayerMouseDown(CMouseEvent* event)
{
    if (!event) return 0;
    if ((m_lookForDrag & static_cast<unsigned>(event->button)) != 0U) {
        m_mouseDown = true;
        m_dragging = false;
        m_dragButton = event->button;
        m_clickPoint = {event->x, event->y};
    }
    if (m_onMouseDown) {
        const char* button = "UNKNOWN";
        switch (event->button) {
        case MOUSE_BUTTON_LEFT: button = "LeftButton"; break;
        case MOUSE_BUTTON_MIDDLE: button = "MiddleButton"; break;
        case MOUSE_BUTTON_RIGHT: button = "RightButton"; break;
        case MOUSE_BUTTON_XBUTTON1: button = "Button4"; break;
        case MOUSE_BUTTON_XBUTTON2: button = "Button5"; break;
        default: break;
        }
        FrameScript_Execute(m_onMouseDown, this, "%s", button);
    }
    return 0;
}

int CSimpleFrame::OnLayerMouseUp(CMouseEvent* event)
{
    if (!event) return 0;
    if ((m_lookForDrag & static_cast<unsigned>(event->button)) != 0U) {
        m_mouseDown = false;
        if (m_dragging) {
            OnDragStop(event->button);
            if (m_top && m_top->m_mouseFocus)
                m_top->m_mouseFocus->OnReceiveDrag();
            m_dragging = false;
            return 1;
        }
    }
    if (m_onMouseUp) {
        const char* button = "UNKNOWN";
        switch (event->button) {
        case MOUSE_BUTTON_LEFT: button = "LeftButton"; break;
        case MOUSE_BUTTON_MIDDLE: button = "MiddleButton"; break;
        case MOUSE_BUTTON_RIGHT: button = "RightButton"; break;
        case MOUSE_BUTTON_XBUTTON1: button = "Button4"; break;
        case MOUSE_BUTTON_XBUTTON2: button = "Button5"; break;
        default: break;
        }
        FrameScript_Execute(m_onMouseUp, this, "%s", button);
    }
    return 0;
}

int CSimpleFrame::OnLayerMouseMoveRelative(CMouseEvent*) { return 0; }

int CSimpleFrame::OnLayerMouseWheel(CMouseEvent* event)
{
    if (!event || !m_visible || !m_onMouseWheel) return 0;
    const int delta = event->wheelDistance >= 0 ? 1 : -1;
    FrameScript_Execute(m_onMouseWheel, this, "%d", delta);
    return 1;
}
void CSimpleFrame::OnDragStart(MOUSEBUTTON) {}
void CSimpleFrame::OnDragStop(MOUSEBUTTON) {}
void CSimpleFrame::OnReceiveDrag() {}
