#include "ENGINE/Source/Frame/csimpleeditbox.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Services/textblock.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <cstdlib>

namespace {
std::string g_editClipboard;

float MeasureEditText(const CSimpleFontString* string, const char* text,
                      std::size_t length)
{
    if (!string || !text || length == 0)
        return 0.0f;

    const float scale = std::max(0.0001f, string->m_layoutScale);
    if (string->m_fontHandle) {
        float extent = 0.0f;
        TextBlockGetTextExtent(string->m_fontHandle, text,
                               static_cast<unsigned>(length),
                               string->m_fontHeight * scale, &extent,
                               string->m_spacing * scale,
                               string->m_styleFlags);
        return extent;
    }

    return static_cast<float>(length) *
           (string->m_fontHeight * 0.52f + string->m_spacing) * scale;
}
}

CSimpleEditBox::CSimpleEditBox(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
    m_string = new CSimpleFontString(this);
    RemoveFrameRegion(m_string);
    RegisterRegion(m_string, 2);
    // CSimpleEditBox::CSimpleEditBox in the recovered client forces its
    // private font string to horizontal-left (style bits 0..2 == 1).  A bare
    // CSimpleFontString defaults to centered, which put edit text in the
    // middle of the chat input and character-name boxes.
    m_string->m_styleFlags = (m_string->m_styleFlags & ~0x07U) | 0x01U;
    m_string->SetText("");
    // The recovered edit box owns a layer-3 solid texture for its caret.  It
    // is not a font glyph: using "|" changes both its baseline and advance.
    m_cursor = new CSimpleTexture(this);
    RemoveFrameRegion(m_cursor);
    RegisterRegion(m_cursor, 3);
    const NTempest::CImVector cursorColor = m_string->m_vertexColor;
    m_cursor->SetTexture(&cursorColor);
    m_cursor->Hide();
    m_cursorPos = 0;
    EnableEvent(SIMPLE_EVENT_MOUSE | SIMPLE_EVENT_KEYBOARD | SIMPLE_EVENT_CHAR);
}

CSimpleEditBox::~CSimpleEditBox()
{
    FrameScript_ReleaseFunction(m_onEnterPressed);
    FrameScript_ReleaseFunction(m_onEscapePressed);
    FrameScript_ReleaseFunction(m_onSpacePressed);
    FrameScript_ReleaseFunction(m_onTabPressed);
    FrameScript_ReleaseFunction(m_onTextChanged);
    FrameScript_ReleaseFunction(m_onTextSet);
    delete m_string;
    m_string = nullptr;
    delete m_cursor;
    m_cursor = nullptr;
}

void CSimpleEditBox::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        if (const auto* value = node->GetAttributeByName("text")) SetText(value, false);
        if (const auto* value = node->GetAttributeByName("letters")) m_textLettersMax = std::max(0, std::atoi(value));
        if (const auto* value = node->GetAttributeByName("maxLetters")) m_textLettersMax = std::max(0, std::atoi(value));
        if (const auto* value = node->GetAttributeByName("maxBytes")) m_textLengthMax = std::atoi(value);
        if (const auto* value = node->GetAttributeByName("password"))
            m_password = _stricmp(value, "true") == 0 || std::atoi(value) != 0;
        if (const auto* value = node->GetAttributeByName("multiLine"))
            m_multiline = _stricmp(value, "true") == 0 || std::atoi(value) != 0;
        if (const auto* value = node->GetAttributeByName("autoFocus"))
            m_autoFocus = _stricmp(value, "true") == 0 || std::atoi(value) != 0;
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "FontString") == 0) {
                FrameXML_LoadFontStringNode(m_string, child, status);
                if (m_cursor) {
                    const NTempest::CImVector cursorColor = m_string->m_vertexColor;
                    m_cursor->SetTexture(&cursorColor);
                }
            }
        }
    }
    UpdateSizes();
    UpdateVisibleText();
}

void CSimpleEditBox::LoadXML_Scripts(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML_Scripts(node, status);
    if (!node) return;
    const XMLNode* scripts = node;
    if (_stricmp(scripts->m_name.c_str(), "Scripts") != 0) {
        scripts = nullptr;
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "Scripts") == 0) {
                scripts = child;
                break;
            }
        }
    }
    if (!scripts) return;
    for (const XMLNode* script = scripts->m_child; script; script = script->m_next) {
        int* slot = nullptr;
        if (_stricmp(script->m_name.c_str(), "OnEnterPressed") == 0) slot = &m_onEnterPressed;
        else if (_stricmp(script->m_name.c_str(), "OnEscapePressed") == 0) slot = &m_onEscapePressed;
        else if (_stricmp(script->m_name.c_str(), "OnSpacePressed") == 0) slot = &m_onSpacePressed;
        else if (_stricmp(script->m_name.c_str(), "OnTabPressed") == 0) slot = &m_onTabPressed;
        else if (_stricmp(script->m_name.c_str(), "OnTextChanged") == 0) slot = &m_onTextChanged;
        else if (_stricmp(script->m_name.c_str(), "OnTextSet") == 0) slot = &m_onTextSet;
        if (slot) {
            const std::string debugName = m_frameName + ":" + script->m_name;
            SetEventScript(*slot, script->m_value.c_str(), debugName.c_str());
        }
    }
}

void CSimpleEditBox::SetMultiLine(int multiline) { m_multiline = multiline != 0; }
void CSimpleEditBox::SetAutoFocus(int autoFocus) { m_autoFocus = autoFocus != 0; }

void CSimpleEditBox::SetEditTextInsets(const NTempest::CRect& insets)
{
    m_editTextInset = insets;
    UpdateSizes();
}

void CSimpleEditBox::OnLayerShow() { CSimpleFrame::OnLayerShow(); if (m_autoFocus) SetKeyboardFocus(); }
void CSimpleEditBox::OnLayerHide() { CSimpleFrame::OnLayerHide(); ClearKeyboardFocus(); }
void CSimpleEditBox::OnLayerUpdate(float elapsedSec)
{
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    m_blinkElapsedTime += std::max(0.0f, elapsedSec);
    UpdateVisibleText();
    UpdateVisibleHighlight();
    UpdateVisibleCursor();
    if (m_textChangedPending) {
        m_textChangedPending = false;
        if (m_onTextChanged) FrameScript_Execute(m_onTextChanged, this);
    }
}
void CSimpleEditBox::OnLayerTrackUpdate(float elapsedSec) { CSimpleFrame::OnLayerTrackUpdate(elapsedSec); (void)elapsedSec; }
void CSimpleEditBox::OnFrameSizeChanged() { CSimpleFrame::OnFrameSizeChanged(); UpdateSizes(); }

int CSimpleEditBox::OnLayerChar(CCharEvent* event)
{
    if (!event || !m_visible || (m_top && m_top->m_keyboardFocus != this) ||
        event->ch < 32 || event->ch == 127) return 0;
    char text[2] = {static_cast<char>(event->ch), '\0'};
    Insert(text);
    if (event->ch == ' ') {
        if (m_onSpacePressed) FrameScript_Execute(m_onSpacePressed, this);
        DispatchAction(2);
    }
    return 1;
}

void CSimpleEditBox::OnLayerIme(CImeEvent* event)
{
    if (event) m_actions[5] = static_cast<int>(event->message);
    UpdateClauseInfo();
}
int CSimpleEditBox::OnLayerKeyDown(CKeyEvent* event)
{
    if (!event || !m_visible || (m_top && m_top->m_keyboardFocus != this)) return 0;
    const bool shift = (event->metaKeyState & (1u << KEY_SHIFT)) != 0;
    const bool control = (event->metaKeyState & (1u << KEY_CONTROL)) != 0;
    switch (static_cast<int>(event->key)) {
    case KEY_LEFT:
        if (control) MoveBackwardWord(shift); else MoveBackward(shift);
        break;
    case KEY_RIGHT:
        if (control) MoveForwardWord(shift); else MoveForward(shift);
        break;
    case KEY_HOME: MoveToStart(shift); break;
    case KEY_END: MoveToEnd(shift); break;
    case KEY_BACKSPACE:
        if (control) DeleteBackwardWord(); else DeleteBackward();
        break;
    case KEY_DELETE:
        if (control) DeleteForwardWord(); else DeleteForward();
        break;
    case KEY_A:
        if (control) HighlightText(0, static_cast<int>(m_text.size()));
        break;
    case 'C':
        if (control) CopyToClipboard();
        break;
    case 'V':
        if (control) PasteFromClipboard();
        break;
    case 'X':
        if (control) { CopyToClipboard(); DeleteHighlight(); }
        break;
    case KEY_ENTER:
        if (m_multiline) Insert("\n");
        else {
            if (m_onEnterPressed) FrameScript_Execute(m_onEnterPressed, this);
            DispatchAction(0);
        }
        break;
    case KEY_ESCAPE:
        if (m_onEscapePressed) FrameScript_Execute(m_onEscapePressed, this);
        DispatchAction(1);
        break;
    case KEY_TAB:
        if (m_onTabPressed) FrameScript_Execute(m_onTabPressed, this);
        DispatchAction(3);
        break;
    default: break;
    }
    return 1;
}

int CSimpleEditBox::OnLayerKeyDownRepeat(CKeyEvent* event) { return OnLayerKeyDown(event); }
int CSimpleEditBox::OnLayerKeyUp(CKeyEvent* event)
{
    if (event) m_actions[4] = static_cast<int>(event->key);
    return event && m_visible && (!m_top || m_top->m_keyboardFocus == this);
}
int CSimpleEditBox::OnLayerMouseDown(CMouseEvent* event)
{
    if (!event) return 0;
    const int baseResult = CSimpleFrame::OnLayerMouseDown(event);
    if (baseResult) return baseResult;
    m_mouseDown = true;
    SetKeyboardFocus();
    Move(ConvertCoordinateToIndex(event->x, event->y));
    m_blinkElapsedTime = 0.0f;
    return 1;
}
int CSimpleEditBox::OnLayerMouseUp(CMouseEvent* event)
{
    const int baseResult = CSimpleFrame::OnLayerMouseUp(event);
    if (baseResult) return baseResult;
    const bool handled = m_mouseDown;
    m_mouseDown = false;
    return handled ? 1 : 0;
}

void CSimpleEditBox::CreateClauseHighlight() { m_clauseHighlightVisible = true; }
void CSimpleEditBox::CreateCandidatesFrame() { m_candidatesVisible = false; }
void CSimpleEditBox::UpdateLanguageIndicator() { m_actions[3] = m_imeInputMode; }
void CSimpleEditBox::UpdateClauseInfo() { m_clauseHighlightVisible = m_imeInputMode != 0; }
void CSimpleEditBox::PopulateCandidates() { m_candidatesVisible = m_clauseHighlightVisible; }
void CSimpleEditBox::DispatchAction(unsigned action)
{
    (void)action;
}
void CSimpleEditBox::UpdateSizes()
{
    if (!m_string) return;

    // CSimpleEditBox::UpdateSizes at 0x779FB0 fixes the edit string's
    // dimensions to the inset client area, then anchors its TOPLEFT to this
    // frame's TOPLEFT.  Leaving the private font string auto-sized causes each
    // SetText/UpdateString call to throw away the left inset.
    const float layoutScale = std::max(0.0001f, m_layoutScale);
    const float width = std::max(
        0.0f,
        (m_rect.r - m_rect.l) / layoutScale -
            (m_editTextInset.l + m_editTextInset.r));
    const float height = m_multiline
        ? 0.0f
        : std::max(
              0.0f,
              (m_rect.b - m_rect.t) / layoutScale -
                  (m_editTextInset.t + m_editTextInset.b));

    m_string->m_autoWidth = false;
    m_string->m_autoHeight = false;
    m_string->ClearAllPoints();
    m_string->SetWidth(width);
    m_string->SetHeight(height);
    m_string->SetPoint(FRAMEPOINT_TOPLEFT, this, FRAMEPOINT_TOPLEFT,
                       m_editTextInset.l, -m_editTextInset.b);

    if (m_cursor) {
        // CSimpleEditBox::UpdateSizes (0x779FB0): a 0.003 layout-unit caret
        // spanning exactly the edit string's font height.
        m_cursor->ClearAllPoints();
        m_cursor->SetWidth(0.003F);
        m_cursor->SetHeight(m_string->m_fontHeight);
    }
}
void CSimpleEditBox::UpdateTextInfo() { UpdateVisibleText(); }
int CSimpleEditBox::GetNumToLen(int number) const
{
    return std::clamp(number, 0, static_cast<int>(m_text.size()));
}
int CSimpleEditBox::GetLenToNum(int length) const
{
    return std::clamp(length, 0, static_cast<int>(m_text.size()));
}
int CSimpleEditBox::NextCharOffset(int offset) const
{
    return std::min(static_cast<int>(m_text.size()), offset + 1);
}
int CSimpleEditBox::PrevCharOffset(int offset) const { return std::max(0, offset - 1); }

int CSimpleEditBox::GetOffsetToLine(int offset) const
{
    offset = std::clamp(offset, 0, static_cast<int>(m_text.size()));
    return static_cast<int>(std::count(m_text.begin(), m_text.begin() + offset, '\n'));
}

void CSimpleEditBox::GrowText(int size)
{
    if (size > m_textLengthMax) m_textLengthMax = size;
}

void CSimpleEditBox::SetText(const char* text, bool dispatch)
{
    m_text = text ? text : "";
    if (m_textLengthMax > 0 && static_cast<int>(m_text.size()) > m_textLengthMax)
        m_text.resize(static_cast<std::size_t>(m_textLengthMax));
    m_cursorPos = static_cast<int>(m_text.size());
    m_highlightLeft = m_highlightRight = m_cursorPos;
    if (m_string) m_string->SetText(m_text.c_str());
    m_textChangedPending = true;
    if (dispatch && m_onTextSet) FrameScript_Execute(m_onTextSet, this);
}

void CSimpleEditBox::Insert(int offset, const char* text)
{
    if (!text) return;
    offset = std::clamp(offset, 0, static_cast<int>(m_text.size()));
    const std::string value(text);
    if (m_textLengthMax > 0 && static_cast<int>(m_text.size() + value.size()) > m_textLengthMax) {
        const auto available = static_cast<std::size_t>(m_textLengthMax - m_text.size());
        m_text.insert(static_cast<std::size_t>(offset), value.substr(0, available));
    } else {
        m_text.insert(static_cast<std::size_t>(offset), value);
    }
    m_cursorPos = offset + static_cast<int>(std::min(value.size(), m_text.size() - static_cast<std::size_t>(offset)));
    if (m_string) m_string->SetText(m_text.c_str());
    m_textChangedPending = true;
}

void CSimpleEditBox::Insert(const char* text)
{
    DeleteHighlight();
    Insert(m_cursorPos, text);
}

void CSimpleEditBox::Delete(int count)
{
    if (count <= 0 || m_cursorPos <= 0) return;
    const auto amount = std::min(count, m_cursorPos);
    m_text.erase(static_cast<std::size_t>(m_cursorPos - amount), static_cast<std::size_t>(amount));
    m_cursorPos -= amount;
    if (m_string) m_string->SetText(m_text.c_str());
    m_textChangedPending = true;
}

void CSimpleEditBox::DeleteForward()
{
    if (m_highlightLeft != m_highlightRight) {
        DeleteHighlight();
        return;
    }
    if (m_cursorPos >= static_cast<int>(m_text.size())) return;
    m_text.erase(static_cast<std::size_t>(m_cursorPos), 1);
    if (m_string) m_string->SetText(m_text.c_str());
    m_textChangedPending = true;
}

void CSimpleEditBox::DeleteForwardWord()
{
    if (m_highlightLeft != m_highlightRight) {
        DeleteHighlight();
        return;
    }
    int end = m_cursorPos;
    while (end < static_cast<int>(m_text.size()) &&
           std::isspace(static_cast<unsigned char>(m_text[end]))) ++end;
    while (end < static_cast<int>(m_text.size()) &&
           !std::isspace(static_cast<unsigned char>(m_text[end]))) ++end;
    if (end != m_cursorPos) DeleteText(m_cursorPos, end);
}

void CSimpleEditBox::DeleteBackward()
{
    if (m_highlightLeft != m_highlightRight) {
        DeleteHighlight();
        return;
    }
    Delete(1);
}

void CSimpleEditBox::DeleteBackwardWord()
{
    if (m_highlightLeft != m_highlightRight) {
        DeleteHighlight();
        return;
    }
    while (m_cursorPos > 0 && std::isspace(static_cast<unsigned char>(m_text[m_cursorPos - 1]))) Delete(1);
    while (m_cursorPos > 0 && !std::isspace(static_cast<unsigned char>(m_text[m_cursorPos - 1]))) Delete(1);
}

void CSimpleEditBox::DeleteToStart() { Delete(m_cursorPos); }
void CSimpleEditBox::DeleteToEnd()
{
    if (m_cursorPos < static_cast<int>(m_text.size())) {
        m_text.erase(static_cast<std::size_t>(m_cursorPos));
        if (m_string) m_string->SetText(m_text.c_str());
        m_textChangedPending = true;
    }
}

void CSimpleEditBox::DeleteText(int left, int right)
{
    left = std::clamp(left, 0, static_cast<int>(m_text.size()));
    right = std::clamp(right, left, static_cast<int>(m_text.size()));
    m_text.erase(static_cast<std::size_t>(left), static_cast<std::size_t>(right - left));
    m_cursorPos = left;
    if (m_string) m_string->SetText(m_text.c_str());
    m_textChangedPending = true;
}

void CSimpleEditBox::DeleteSubstring(int left, int right) { DeleteText(left, right); }

void CSimpleEditBox::Move(int offset, bool extend)
{
    m_cursorPos = std::clamp(offset, 0, static_cast<int>(m_text.size()));
    if (!extend) m_highlightLeft = m_highlightRight = m_cursorPos;
    else m_highlightRight = m_cursorPos;
}

void CSimpleEditBox::MoveForward(bool extend) { Move(NextCharOffset(m_cursorPos), extend); }
void CSimpleEditBox::MoveForwardWord(bool extend)
{
    int pos = m_cursorPos;
    while (pos < static_cast<int>(m_text.size()) && !std::isspace(static_cast<unsigned char>(m_text[pos]))) ++pos;
    while (pos < static_cast<int>(m_text.size()) && std::isspace(static_cast<unsigned char>(m_text[pos]))) ++pos;
    Move(pos, extend);
}
void CSimpleEditBox::MoveBackward(bool extend) { Move(PrevCharOffset(m_cursorPos), extend); }
void CSimpleEditBox::MoveBackwardWord(bool extend)
{
    int pos = m_cursorPos;
    while (pos > 0 && std::isspace(static_cast<unsigned char>(m_text[pos - 1]))) --pos;
    while (pos > 0 && !std::isspace(static_cast<unsigned char>(m_text[pos - 1]))) --pos;
    Move(pos, extend);
}
void CSimpleEditBox::MoveToStart(bool extend) { Move(0, extend); }
void CSimpleEditBox::MoveToEnd(bool extend) { Move(static_cast<int>(m_text.size()), extend); }
void CSimpleEditBox::MoveLine(int direction)
{
    const int currentLine = GetOffsetToLine(m_cursorPos);
    int targetLine = std::max(0, currentLine + direction);
    int pos = 0;
    while (targetLine > 0) {
        const auto next = m_text.find('\n', static_cast<std::size_t>(pos));
        if (next == std::string::npos) { pos = static_cast<int>(m_text.size()); break; }
        pos = static_cast<int>(next + 1);
        --targetLine;
    }
    Move(pos);
}
void CSimpleEditBox::MoveForwardLine() { MoveLine(1); }
void CSimpleEditBox::MoveBackwardLine() { MoveLine(-1); }

void CSimpleEditBox::HighlightText(int left, int right)
{
    m_highlightLeft = std::clamp(std::min(left, right), 0, static_cast<int>(m_text.size()));
    m_highlightRight = std::clamp(std::max(left, right), 0, static_cast<int>(m_text.size()));
    m_cursorPos = m_highlightRight;
}
void CSimpleEditBox::StartHighlight() { m_highlightLeft = m_cursorPos; m_highlightRight = m_cursorPos; }
void CSimpleEditBox::ExtendHighlight(int offset) { HighlightText(m_highlightLeft, offset); }
void CSimpleEditBox::DeleteHighlight()
{
    if (m_highlightLeft != m_highlightRight) {
        DeleteText(m_highlightLeft, m_highlightRight);
        m_highlightLeft = m_highlightRight = m_cursorPos;
    }
}

void CSimpleEditBox::SetHistoryLines(const std::vector<std::string>& history) { m_history = history; m_curHistory = -1; }
void CSimpleEditBox::AddHistoryLine(const char* line)
{
    if (line && *line) m_history.emplace_back(line);
    m_curHistory = static_cast<int>(m_history.size());
}
void CSimpleEditBox::ForwardHistory()
{
    if (m_history.empty()) return;
    m_curHistory = std::min(static_cast<int>(m_history.size() - 1), m_curHistory + 1);
    SetText(m_history[m_curHistory].c_str());
}
void CSimpleEditBox::BackwardHistory()
{
    if (m_history.empty()) return;
    m_curHistory = std::max(0, m_curHistory - 1);
    SetText(m_history[m_curHistory].c_str());
}

int CSimpleEditBox::ConvertCoordinateToIndex(float x, float y) const
{
    if (!m_string)
        return 0;

    NTempest::CRect stringRect{};
    if (!m_string->GetRect(&stringRect))
        return 0;

    const float scale = std::max(0.0001f, m_string->m_layoutScale);
    const float lineHeight = std::max(0.0001f,
        (m_string->m_fontHeight + m_string->m_spacing) * scale);
    int line = 0;
    if (m_multiline) {
        line = std::max(0, static_cast<int>(
            (stringRect.b - std::clamp(y, stringRect.t, stringRect.b)) /
            lineHeight));
    }

    int lineStart = std::clamp(m_visiblePos, 0, static_cast<int>(m_text.size()));
    for (int current = 0; current < line; ++current) {
        const std::size_t next = m_text.find('\n', static_cast<std::size_t>(lineStart));
        if (next == std::string::npos)
            return static_cast<int>(m_text.size());
        lineStart = static_cast<int>(next + 1);
    }

    std::size_t lineEnd = m_text.find('\n', static_cast<std::size_t>(lineStart));
    if (lineEnd == std::string::npos)
        lineEnd = m_text.size();

    const float localX = std::clamp(x - stringRect.l, 0.0f,
                                    std::max(0.0f, stringRect.Width()));
    const std::string hidden = m_password ? std::string(m_text.size(), '*') : std::string();
    const char* display = m_password ? hidden.c_str() : m_text.c_str();
    float previousWidth = 0.0f;
    for (std::size_t index = static_cast<std::size_t>(lineStart);
         index < lineEnd; ++index) {
        const float width = MeasureEditText(
            m_string, display + lineStart, index - static_cast<std::size_t>(lineStart) + 1);
        if (localX < (previousWidth + width) * 0.5f)
            return static_cast<int>(index);
        previousWidth = width;
    }
    return static_cast<int>(lineEnd);
}

void CSimpleEditBox::MakeTextVisible()
{
    m_visiblePos = std::clamp(m_cursorPos, 0, static_cast<int>(m_text.size()));
    m_visibleLen = static_cast<int>(m_text.size()) - m_visiblePos;
}

void CSimpleEditBox::UpdateVisibleText()
{
    if (!m_string) return;
    if (!m_password) {
        m_string->SetText(m_text.c_str());
        return;
    }
    m_string->SetText(std::string(m_text.size(), '*').c_str());
}

void CSimpleEditBox::UpdateVisibleHighlight()
{
    m_clauseHighlightVisible = m_highlightLeft != m_highlightRight;
}

void CSimpleEditBox::UpdateVisibleCursor()
{
    const bool focused = m_shown && m_visible && m_top &&
                         m_top->m_keyboardFocus == this;
    const bool blinkOn = m_cursorBlinkSpeed <= 0.0f ||
        std::fmod(m_blinkElapsedTime, m_cursorBlinkSpeed * 2.0f) < m_cursorBlinkSpeed;
    m_cursorVisible = focused && blinkOn;
    if (!m_cursor || !m_string) return;
    if (!m_cursorVisible) {
        m_cursor->Hide();
        return;
    }

    const std::string hidden = m_password ? std::string(m_text.size(), '*') : std::string();
    const char* display = m_password ? hidden.c_str() : m_text.c_str();
    const int cursor = std::clamp(m_cursorPos, 0, static_cast<int>(m_text.size()));
    const int visible = std::clamp(m_visiblePos, 0, cursor);
    const float prefixWidth = MeasureEditText(
        m_string, display + visible, static_cast<std::size_t>(cursor - visible));

    m_cursor->ClearAllPoints();
    const FRAMEPOINT point = m_multiline
        ? FRAMEPOINT_TOPLEFT : FRAMEPOINT_LEFT;
    m_cursor->SetPoint(point, m_string, point,
                       prefixWidth / std::max(0.0001f, m_layoutScale), 0.0f);
    m_cursor->Show();
}

void CSimpleEditBox::UpdateHighlightArea()
{
    UpdateVisibleHighlight();
}

void CSimpleEditBox::CopyToClipboard() const
{
    const int left = std::clamp(std::min(m_highlightLeft, m_highlightRight), 0,
                                static_cast<int>(m_text.size()));
    const int right = std::clamp(std::max(m_highlightLeft, m_highlightRight), left,
                                 static_cast<int>(m_text.size()));
    g_editClipboard = m_text.substr(static_cast<std::size_t>(left),
                                    static_cast<std::size_t>(right - left));
}

void CSimpleEditBox::PasteFromClipboard()
{
    if (!g_editClipboard.empty()) Insert(g_editClipboard.c_str());
}
void CSimpleEditBox::SetFont(const char* font, float height)
{
    if (!m_string) m_string = new CSimpleFontString(this);
    m_string->SetFont(font, height);
}
void CSimpleEditBox::ShowCandidates() { m_candidatesVisible = true; }
void CSimpleEditBox::HideCandidates() { m_candidatesVisible = false; }
void CSimpleEditBox::SetKeyboardFocus()
{
    if (m_top) {
        if (auto* previous = dynamic_cast<CSimpleEditBox*>(m_top->m_keyboardFocus)) {
            if (previous != this && previous->m_cursor)
                previous->m_cursor->Hide();
        }
        m_top->m_keyboardFocus = this;
    }
    m_blinkElapsedTime = 0.0f;
}
void CSimpleEditBox::ClearKeyboardFocus()
{
    if (m_top && m_top->m_keyboardFocus == this)
        m_top->m_keyboardFocus = nullptr;
    if (m_cursor) m_cursor->Hide();
}
