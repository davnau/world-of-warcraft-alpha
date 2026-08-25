#include "ENGINE/Source/Frame/csimplemessagescrollframe.h"
#include "ENGINE/Source/Frame/framexml.h"

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>

CSimpleFontStringAttributes::CSimpleFontStringAttributes() = default;
CSimpleFontStringAttributes::~CSimpleFontStringAttributes() = default;

CSimpleMessageScrollFrameLine::~CSimpleMessageScrollFrameLine() = default;

CSimpleMessageScrollFrameDisplayNode::CSimpleMessageScrollFrameDisplayNode() = default;

CSimpleMessageScrollFrameDisplayNode::~CSimpleMessageScrollFrameDisplayNode()
{
    delete string;
    string = nullptr;
}

CSimpleMessageScrollFrameDisplayNode::CSimpleMessageScrollFrameDisplayNode(
    CSimpleMessageScrollFrameDisplayNode&& other) noexcept
    : string(other.string), line(other.line), attrib(std::move(other.attrib))
{
    other.string = nullptr;
    other.line = nullptr;
}

CSimpleMessageScrollFrameDisplayNode& CSimpleMessageScrollFrameDisplayNode::operator=(
    CSimpleMessageScrollFrameDisplayNode&& other) noexcept
{
    if (this == &other) return *this;
    delete string;
    string = other.string;
    line = other.line;
    attrib = std::move(other.attrib);
    other.string = nullptr;
    other.line = nullptr;
    return *this;
}

CSimpleMessageScrollFrame::CSimpleMessageScrollFrame(CSimpleFrame* parent)
    : CSimpleHyperlinkedFrame(parent)
{
    // Recovered CSimpleMessageScrollFrame constructor defaults.
    m_attrib.m_flags = 31;
    m_attrib.m_styleFlags = 658;
    m_attrib.m_color = NTempest::CImVector(0xffffffffU);
    m_fading = 1;
    m_fadeDuration = 3.0F;
    m_timeVisible = 10.0F;
    m_currentLine = -1;
    m_atTop = 0;
    m_atBottom = 1;
}

CSimpleMessageScrollFrame::~CSimpleMessageScrollFrame()
{
    Clear();
}

void CSimpleMessageScrollFrame::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleHyperlinkedFrame::LoadXML(node, status);
    if (node) {
        if (const auto* value = node->GetAttributeByName("maxLines")) SetMaxLines(std::atoi(value));
        if (const auto* value = node->GetAttributeByName("fade"))
            m_fading = _stricmp(value, "true") == 0 || std::atoi(value) != 0;
        if (const auto* value = node->GetAttributeByName("fadeDuration"))
            m_fadeDuration = (std::max)(0.0F, std::strtof(value, nullptr));
        if (const auto* value = node->GetAttributeByName("displayDuration"))
            m_timeVisible = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("textLength")) m_textMaxSize = std::atoi(value);

        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "FontString") != 0) continue;
            CSimpleFontString sample(nullptr);
            FrameXML_LoadFontStringNode(&sample, child, status);
            m_attrib.m_font = sample.m_font;
            m_attrib.m_fontHeight = sample.m_fontHeight;
            m_attrib.m_fontFlags = sample.m_fontFlags;
            m_attrib.m_spacing = sample.m_spacing;
            m_attrib.m_styleFlags = sample.m_styleFlags;
            m_attrib.m_color = sample.GetVertexColor();
            m_attrib.m_shadowColor = sample.m_shadowColor;
            m_attrib.m_shadowOffset = sample.m_shadowOffset;
            break;
        }
    }
    OnFrameSizeChanged();
}

void CSimpleMessageScrollFrame::SetMaxLines(int lines)
{
    m_maxMessages = std::max(0, lines);
    if (m_maxMessages > 0 && static_cast<int>(m_lines.size()) > m_maxMessages)
        m_lines.erase(m_lines.begin(), m_lines.end() - m_maxMessages);
    m_numMessages = static_cast<int>(m_lines.size());
    SetCurrentScroll(m_currentScroll);
}

void CSimpleMessageScrollFrame::SetMessageFrameInsets(const NTempest::CRect& inset)
{
    m_messageFrameInset = inset;
    OnFrameSizeChanged();
}

void CSimpleMessageScrollFrame::SetTextLength(int length) { m_textMaxSize = std::max(0, length); }
void CSimpleMessageScrollFrame::SetFading(int fading) { m_fading = fading != 0; }
void CSimpleMessageScrollFrame::SetTimeVisible(float time) { m_timeVisible = std::max(0.0f, time); }

void CSimpleMessageScrollFrame::AddMessage(const char* text,
                                           const CSimpleFontStringAttributes* attrib,
                                           float timeVisible)
{
    const bool wasAtBottom = m_atBottom != 0;
    CSimpleMessageScrollFrameLine line;
    line.string = text ? text : "";
    line.attrib = attrib ? *attrib : m_attrib;
    line.timeLeft = timeVisible > 0.0f ? timeVisible : m_timeVisible;
    line.fadeLeft = m_fadeDuration;
    if (m_maxMessages > 0 && static_cast<int>(m_lines.size()) >= m_maxMessages)
        m_lines.erase(m_lines.begin());
    m_lines.push_back(std::move(line));
    m_numMessages = static_cast<int>(m_lines.size());
    m_currentLine = m_numMessages - 1;
    SetCurrentScroll(wasAtBottom ? m_currentLine : m_currentScroll);

    if (_stricmp(GetName(), "ChatFrame") == 0) {
        std::fprintf(stderr,
                     "CHAT_LAYOUT frame=(%.6f,%.6f,%.6f,%.6f) area=(%.6f,%.6f,%.6f,%.6f) "
                     "scale=%.6f font=%s height=%.6f fading=%d duration=%.3f fade=%.3f "
                     "lines=%zu nodes=%zu displayed=%d\n",
                     m_rect.l, m_rect.t, m_rect.r, m_rect.b,
                     m_messageFrameArea.l, m_messageFrameArea.t,
                     m_messageFrameArea.r, m_messageFrameArea.b,
                     m_layoutScale, m_attrib.m_font.c_str(), m_attrib.m_fontHeight,
                     m_fading, m_timeVisible, m_fadeDuration, m_lines.size(),
                     m_displayNodes.size(), m_numDisplayed);
        for (std::size_t index = 0; index < m_lines.size(); ++index) {
            const auto& item = m_lines[index];
            std::fprintf(stderr, "CHAT_LINE index=%zu visible=%d time=%.3f fade=%.3f text=%s\n",
                         index, item.isVisible ? 1 : 0, item.timeLeft, item.fadeLeft,
                         item.string.c_str());
        }
        if (!m_displayNodes.empty() && m_displayNodes.front().string) {
            const auto* string = m_displayNodes.front().string;
            std::fprintf(stderr,
                         "CHAT_NODE rect=(%.6f,%.6f,%.6f,%.6f) scale=%.6f size=(%.6f,%.6f) "
                         "font=%s height=%.6f fontHandle=%p textHandle=%p visible=%d text=%s\n",
                         string->m_rect.l, string->m_rect.t, string->m_rect.r, string->m_rect.b,
                         string->m_layoutScale, string->m_width, string->m_height,
                         string->m_font.c_str(), string->m_fontHeight,
                         static_cast<void*>(string->m_fontHandle), static_cast<void*>(string->m_string),
                         string->IsVisible() ? 1 : 0, string->GetText());
        }
    }
}

void CSimpleMessageScrollFrame::AddMultiLine(const char* text,
                                              const CSimpleFontStringAttributes* attrib,
                                              float timeVisible)
{
    const std::string value = text ? text : "";
    std::size_t start = 0;
    while (start <= value.size()) {
        const auto end = value.find('\n', start);
        AddMessage(value.substr(start, end == std::string::npos ? std::string::npos : end - start).c_str(),
                   attrib, timeVisible);
        if (end == std::string::npos) break;
        start = end + 1;
    }
}

void CSimpleMessageScrollFrame::Clear()
{
    m_lines.clear();
    m_displayNodes.clear();
    m_numMessages = m_currentLine = m_currentScroll = m_numDisplayed = 0;
    m_atTop = m_atBottom = 1;
}

void CSimpleMessageScrollFrame::OnFrameSizeChanged()
{
    CSimpleHyperlinkedFrame::OnFrameSizeChanged();
    m_messageFrameArea = {m_rect.l + m_messageFrameInset.l, m_rect.t + m_messageFrameInset.t,
                          m_rect.r - m_messageFrameInset.r, m_rect.b - m_messageFrameInset.b};
    UpdateDisplay();
}

void CSimpleMessageScrollFrame::OnLayerUpdate(float elapsedSec)
{
    CSimpleHyperlinkedFrame::OnLayerUpdate(elapsedSec);
    for (auto& line : m_lines) {
        if (!line.isVisible || !m_fading) continue;
        if (line.timeLeft > 0.0f) {
            line.timeLeft = (std::max)(0.0F, line.timeLeft - elapsedSec);
        } else if (line.fadeLeft > 0.0F) {
            line.fadeLeft = (std::max)(0.0F, line.fadeLeft - elapsedSec);
            line.attrib.m_color.a = m_fadeDuration > 0.0F
                ? static_cast<std::uint8_t>(255.0F * line.fadeLeft / m_fadeDuration)
                : 0U;
        } else {
            if (_stricmp(GetName(), "ChatFrame") == 0)
                std::fprintf(stderr,
                             "CHAT_LINE_HIDE elapsed=%.6f time=%.3f fade=%.3f text=%s\n",
                             elapsedSec, line.timeLeft, line.fadeLeft, line.string.c_str());
            line.isVisible = false;
        }
    }
    UpdateDisplay();
}

void CSimpleMessageScrollFrame::UpdateDisplay()
{
    m_displayNodes.clear();
    if (m_numMessages == 0) {
        m_numDisplayed = 0;
        m_atTop = m_atBottom = 1;
        return;
    }
    const int first = std::clamp(m_currentScroll, 0, m_numMessages - 1);
    const float layoutScale = std::max(0.0001f, m_layoutScale);
    const float availableHeight = std::max(
        0.0f, m_messageFrameArea.Height() / layoutScale);
    const float lineHeight = std::max(
        0.0001f, m_attrib.m_fontHeight + std::max(0.0f, m_attrib.m_spacing));
    const int rows = std::max(1, static_cast<int>(availableHeight / lineHeight));
    for (int i = first; i >= 0 && static_cast<int>(m_displayNodes.size()) < rows; --i) {
        if (!m_lines[i].isVisible) continue;
        auto node = CSimpleMessageScrollFrameDisplayNode{};
        node.line = &m_lines[i];
        node.attrib = m_lines[i].attrib;
        node.string = new CSimpleFontString(this);
        RemoveFrameRegion(node.string);
        RegisterRegion(node.string, 2);
        node.string->SetLayoutScale(layoutScale);
        node.string->m_styleFlags = node.attrib.m_styleFlags;
        node.string->SetWidth(
            (std::max)(0.0F, m_messageFrameArea.Width() / layoutScale));
        node.string->SetTextLength(m_textMaxSize);
        if (m_displayNodes.empty()) {
            node.string->SetPoint(FRAMEPOINT_BOTTOMLEFT, this, FRAMEPOINT_BOTTOMLEFT,
                                  m_messageFrameInset.l, m_messageFrameInset.t);
        } else {
            node.string->SetPoint(FRAMEPOINT_BOTTOMLEFT,
                                  m_displayNodes.back().string,
                                  FRAMEPOINT_TOPLEFT, 0.0F,
                                  node.attrib.m_spacing);
        }
        node.attrib.UpdateString(node.string);
        if (node.attrib.m_shadowColor.a != 0U)
            node.string->AddShadow(node.attrib.m_shadowColor, node.attrib.m_shadowOffset);
        node.string->SetText(m_lines[i].string.c_str());
        m_displayNodes.push_back(std::move(node));
    }
    m_numDisplayed = static_cast<int>(m_displayNodes.size());
    m_atTop = first - m_numDisplayed + 1 <= 0;
    m_atBottom = first == m_currentLine;
}

void CSimpleMessageScrollFrame::ScrollMessages(int lines)
{
    SetCurrentScroll(m_currentScroll + lines);
}

void CSimpleMessageScrollFrame::PageUp()
{
    const float scale = std::max(0.0001f, m_layoutScale);
    const float lineHeight = std::max(
        0.0001f, m_attrib.m_fontHeight + std::max(0.0f, m_attrib.m_spacing));
    const int rows = std::max(
        1, static_cast<int>((m_messageFrameArea.Height() / scale) / lineHeight));
    ScrollMessages(-rows);
}

void CSimpleMessageScrollFrame::PageDown()
{
    const float scale = std::max(0.0001f, m_layoutScale);
    const float lineHeight = std::max(
        0.0001f, m_attrib.m_fontHeight + std::max(0.0f, m_attrib.m_spacing));
    const int rows = std::max(
        1, static_cast<int>((m_messageFrameArea.Height() / scale) / lineHeight));
    ScrollMessages(rows);
}

void CSimpleMessageScrollFrame::ScrollUp() { ScrollMessages(-1); }
void CSimpleMessageScrollFrame::ScrollDown() { ScrollMessages(1); }
void CSimpleMessageScrollFrame::ScrollToTop() { SetCurrentScroll(0); }
void CSimpleMessageScrollFrame::ScrollToBottom() { SetCurrentScroll(std::max(0, m_numMessages - 1)); }

void CSimpleMessageScrollFrame::UpdateNode(CSimpleMessageScrollFrameDisplayNode* node)
{
    if (!node || !node->string || !node->line) return;
    node->attrib.UpdateString(node->string);
    node->string->SetText(node->line->string.c_str());
}

void CSimpleMessageScrollFrame::RefreshMessages()
{
    for (auto& node : m_displayNodes) UpdateNode(&node);
}

void CSimpleMessageScrollFrame::RefreshHyperlinks()
{
    m_hyperlinks.clear();
}

bool CSimpleMessageScrollFrame::IsAtTop() const { return m_atTop != 0; }
bool CSimpleMessageScrollFrame::IsAtBottom() const { return m_atBottom != 0; }

void CSimpleMessageScrollFrame::SetCurrentScroll(int scroll)
{
    m_currentScroll = std::clamp(scroll, 0, std::max(0, m_numMessages - 1));
    UpdateDisplay();
}
