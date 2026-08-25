#include "ENGINE/Source/Frame/csimplebutton.h"
#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Frame/framescript.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace {
constexpr float kDdcPerPixel = 0.0009765625f * 0.80000001f;

CSimpleTexture* LoadTexture(const XMLNode* node, CSimpleFrame* parent, CStatus* status)
{
    auto* texture = new CSimpleTexture(parent);
    FrameXML_LoadTextureNode(texture, node, status);
    return texture;
}

CSimpleFontString* LoadString(const XMLNode* node, CSimpleFrame* parent, CStatus* status)
{
    auto* string = new CSimpleFontString(parent);
    FrameXML_LoadFontStringNode(string, node, status);
    return string;
}

bool LoadDimensions(const XMLNode* node, NTempest::C2Vector& value)
{
    if (!node || !node->m_child) return false;
    const XMLNode* dimensions = node->m_child;
    const bool absolute = _stricmp(dimensions->m_name.c_str(), "AbsDimension") == 0;
    const bool relative = _stricmp(dimensions->m_name.c_str(), "RelDimension") == 0;
    if (!absolute && !relative) return false;
    const float scale = absolute ? kDdcPerPixel : 1.0f;
    if (const char* x = dimensions->GetAttributeByName("x"))
        value.x = std::strtof(x, nullptr) * scale;
    if (const char* y = dimensions->GetAttributeByName("y"))
        value.y = std::strtof(y, nullptr) * scale;
    return true;
}
}

CSimpleButton::CSimpleButton(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
    Enable(true);
    EnableEvent(SIMPLE_EVENT_MOUSE);
}

CSimpleButton::~CSimpleButton()
{
    m_text = nullptr;
    m_disabledText = nullptr;
    m_highlightText = nullptr;
    m_textures.fill(nullptr);
    m_activeTexture = nullptr;
}

void CSimpleButton::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "NormalTexture") == 0)
                SetStateTexture(BUTTONSTATE_NORMAL, LoadTexture(child, this, status));
            else if (_stricmp(child->m_name.c_str(), "PushedTexture") == 0)
                SetStateTexture(BUTTONSTATE_PUSHED, LoadTexture(child, this, status));
            else if (_stricmp(child->m_name.c_str(), "DisabledTexture") == 0)
                SetStateTexture(BUTTONSTATE_DISABLED, LoadTexture(child, this, status));
            else if (_stricmp(child->m_name.c_str(), "HighlightTexture") == 0) {
                CSimpleTexture* texture = LoadTexture(child, this, status);
                RemoveFrameRegion(texture);
                SetHighlight(texture, texture->m_alphamode);
            } else if (_stricmp(child->m_name.c_str(), "NormalText") == 0 ||
                       _stricmp(child->m_name.c_str(), "Text") == 0)
                SetText(LoadString(child, this, status));
            else if (_stricmp(child->m_name.c_str(), "DisabledText") == 0)
            {
                CSimpleFontString* string = LoadString(child, this, status);
                RemoveFrameRegion(string);
                SetDisabledText(string);
            }
            else if (_stricmp(child->m_name.c_str(), "HighlightText") == 0) {
                CSimpleFontString* string = LoadString(child, this, status);
                RemoveFrameRegion(string);
                SetHighlightText(string);
            } else if (_stricmp(child->m_name.c_str(), "PushedTextOffset") == 0) {
                NTempest::C2Vector offset{};
                if (LoadDimensions(child, offset)) SetPressedOffset(offset);
            }
        }
    }
}

void CSimpleButton::LoadXML_Scripts(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML_Scripts(node, status);
    if (!node) return;
    for (const XMLNode* child = node->m_child; child; child = child->m_next) {
        if (_stricmp(child->m_name.c_str(), "OnClick") == 0) {
            SetEventScript(m_onClick, child->m_value.c_str(), "CSimpleButton:OnClick");
        }
    }
}

void CSimpleButton::LockHighlight(int lock)
{
    m_highlightLocked = lock != 0;
    if (m_highlightLocked) EnableDrawLayer(4);
    else if (m_top && m_top->m_mouseFocus != this) DisableDrawLayer(4);
    UpdateTextState(m_state);
}

void CSimpleButton::SetText(CSimpleFontString* text)
{
    if (m_text) {
        m_text->Hide();
        m_text->SetFrame(nullptr);
    }
    if (text) {
        text->SetFrame(this);
        RemoveFrameRegion(text);
        text->Hide();
        RegisterRegion(text, 2);
    }
    m_text = text;
    UpdateTextState(m_state);
}

void CSimpleButton::SetDisabledText(CSimpleFontString* text)
{
    if (m_disabledText) {
        m_disabledText->Hide();
        m_disabledText->SetFrame(nullptr);
    }
    if (text) {
        text->SetFrame(this);
        RemoveFrameRegion(text);
        text->Hide();
        RegisterRegion(text, 2);
    }
    m_disabledText = text;
    UpdateTextState(m_state);
}

void CSimpleButton::SetHighlightText(CSimpleFontString* text)
{
    if (m_highlightText) {
        m_highlightText->Hide();
        m_highlightText->SetFrame(nullptr);
    }
    if (text) {
        text->SetFrame(this);
        RemoveFrameRegion(text);
        text->Hide();
        RegisterRegion(text, 4);
    }
    m_highlightText = text;
    UpdateTextState(m_state);
}

void CSimpleButton::SetTextString(const char* text)
{
    if (m_text && text) m_text->SetText(text);
}
void CSimpleButton::SetDisabledTextString(const char* text)
{
    if (m_disabledText && text) m_disabledText->SetText(text);
}
void CSimpleButton::SetHighlightTextString(const char* text)
{
    if (m_highlightText && text) m_highlightText->SetText(text);
}
void CSimpleButton::SetTextColor(const NTempest::CImVector& color) { if (m_text) m_text->SetTextColor(color); }
void CSimpleButton::SetDisabledTextColor(const NTempest::CImVector& color) { if (m_disabledText) m_disabledText->SetTextColor(color); }
void CSimpleButton::SetHighlightTextColor(const NTempest::CImVector& color) { if (m_highlightText) m_highlightText->SetTextColor(color); }
void CSimpleButton::SetPressedOffset(const NTempest::C2Vector& offset) { m_pressedOffset = offset; }

bool CSimpleButton::SetStateTexture(CSimpleButtonState state, const char* texture)
{
    if (state >= NUM_BUTTONSTATES) return false;
    if (m_textures[state]) return m_textures[state]->SetTexture(texture);

    auto* stateTexture = new CSimpleTexture(nullptr);
    if (!stateTexture->SetTexture(texture)) {
        delete stateTexture;
        return false;
    }
    stateTexture->SetAllPoints(this);
    SetStateTexture(state, stateTexture);
    return true;
}

void CSimpleButton::SetStateTexture(CSimpleButtonState state, CSimpleTexture* texture)
{
    if (state >= NUM_BUTTONSTATES) return;
    if (m_textures[state]) {
        if (m_activeTexture == m_textures[state]) m_activeTexture = nullptr;
        m_textures[state]->Hide();
        m_textures[state]->SetFrame(nullptr);
    }
    if (texture) {
        texture->SetFrame(this);
        RemoveFrameRegion(texture);
        texture->Hide();
        RegisterRegion(texture, 2);
    }
    m_textures[state] = texture;
    if (texture && state == m_state) {
        m_activeTexture = texture;
        texture->Show();
    }
}

void CSimpleButton::Enable(int enabled)
{
    if (enabled) {
        if (m_state == BUTTONSTATE_DISABLED) SetButtonState(BUTTONSTATE_NORMAL);
    } else if (m_state != BUTTONSTATE_DISABLED) {
        DisableDrawLayer(4);
        SetButtonState(BUTTONSTATE_DISABLED);
    }
}

void CSimpleButton::OnLayerHide()
{
    if (m_state != BUTTONSTATE_DISABLED && !m_stateLocked) SetButtonState(BUTTONSTATE_NORMAL);
    CSimpleFrame::OnLayerHide();
}

int CSimpleButton::OnLayerMouseDown(CMouseEvent* event)
{
    const int baseResult = CSimpleFrame::OnLayerMouseDown(event);
    if (baseResult || !event || m_state == BUTTONSTATE_DISABLED) return baseResult;
    const unsigned button = static_cast<unsigned>(event->button);
    if (((button | (button << 8)) & m_clickAction) == 0 ||
        !TestHitRect(event->x, event->y)) return 0;
    m_mouseDown = true;
    m_dragButton = event->button;
    if ((button & m_clickAction) != 0) OnClick(event->button);
    if (!m_stateLocked && m_state != BUTTONSTATE_DISABLED)
        SetButtonState(BUTTONSTATE_PUSHED);
    return 1;
}

int CSimpleButton::OnLayerMouseUp(CMouseEvent* event)
{
    const int baseResult = CSimpleFrame::OnLayerMouseUp(event);
    if (baseResult || !event || m_state == BUTTONSTATE_DISABLED) return baseResult;
    int handled = 0;
    if (m_state == BUTTONSTATE_PUSHED) {
        handled = 1;
        const unsigned button = static_cast<unsigned>(event->button);
        if (((button << 8) & m_clickAction) != 0 && TestHitRect(event->x, event->y))
            OnClick(event->button);
        if (!m_stateLocked && m_state != BUTTONSTATE_DISABLED)
            SetButtonState(BUTTONSTATE_NORMAL);
    }
    m_mouseDown = false;
    return handled;
}

void CSimpleButton::OnDragStart(MOUSEBUTTON button)
{
    if (m_state != BUTTONSTATE_DISABLED && !m_stateLocked)
        SetButtonState(BUTTONSTATE_NORMAL);
    CSimpleFrame::OnDragStart(button);
}

void CSimpleButton::OnLayerCursorEnter()
{
    if (m_state != BUTTONSTATE_DISABLED) {
        CSimpleFrame::OnLayerCursorEnter();
        UpdateTextState(m_state);
    }
    if (m_trackHandler) m_trackHandler(*this, true);
}

void CSimpleButton::OnLayerCursorExit()
{
    if (m_state != BUTTONSTATE_DISABLED) {
        CSimpleFrame::OnLayerCursorExit();
        UpdateTextState(m_state);
    }
    if (m_trackHandler) m_trackHandler(*this, false);
}

void CSimpleButton::OnClick(MOUSEBUTTON button)
{
    if (m_state == BUTTONSTATE_DISABLED) return;
    CSimpleButtonClickEvent event;
    event.button = button;
    if (m_clickHandler) m_clickHandler(*this);
    if (m_onClick) {
        const char* buttonName = "UNKNOWN";
        switch (button) {
        case MOUSE_BUTTON_LEFT: buttonName = "LeftButton"; break;
        case MOUSE_BUTTON_MIDDLE: buttonName = "MiddleButton"; break;
        case MOUSE_BUTTON_RIGHT: buttonName = "RightButton"; break;
        case MOUSE_BUTTON_XBUTTON1: buttonName = "Button4"; break;
        case MOUSE_BUTTON_XBUTTON2: buttonName = "Button5"; break;
        default: break;
        }
        FrameScript_Execute(m_onClick, this, "%s", buttonName);
    }
}

void CSimpleButton::SetClickAction(unsigned action) { m_clickAction = action; }
void CSimpleButton::RegisterClick(std::function<void(CSimpleButton&)> handler) { m_clickHandler = std::move(handler); }
void CSimpleButton::RegisterTrack(std::function<void(CSimpleButton&, bool)> handler)
{
    m_trackHandler = std::move(handler);
}

void CSimpleButton::SetButtonState(CSimpleButtonState state, bool lock)
{
    if (state >= NUM_BUTTONSTATES) return;
    m_stateLocked = lock;
    if (state != m_state) {
        if (m_activeTexture && m_textures[state]) {
            m_activeTexture->Hide();
            m_activeTexture = nullptr;
        }
        if (m_textures[state]) {
            m_activeTexture = m_textures[state];
            m_activeTexture->Show();
        }
        UpdateTextState(state);
        m_state = state;
    }
}

void CSimpleButton::UpdateTextState(CSimpleButtonState state)
{
    CSimpleFontString* text = (m_drawenabled[4] && m_highlightText) ? m_highlightText : m_text;
    if (text) {
        if (state == BUTTONSTATE_PUSHED)
            text->SetJustificationOffset(m_pressedOffset);
        else
            text->SetJustificationOffset(NTempest::C2Vector{});
    }

    if (state == BUTTONSTATE_DISABLED) {
        if (m_disabledText && !m_disabledText->m_visible) {
            if (text) text->Hide();
            m_disabledText->Show();
        }
        return;
    }

    if (text && !text->m_visible) {
        if (m_disabledText) m_disabledText->Hide();
        if (text == m_text) {
            if (m_highlightText) m_highlightText->Hide();
        } else if (m_text) {
            m_text->Hide();
        }
        text->Show();
    }
}
