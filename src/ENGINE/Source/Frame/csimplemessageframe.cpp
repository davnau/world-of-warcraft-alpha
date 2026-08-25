#include "ENGINE/Source/Frame/csimplemessageframe.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

MessageData::~MessageData() = default;

CSimpleMessageFrameLineNode::CSimpleMessageFrameLineNode() = default;

CSimpleMessageFrameLineNode::~CSimpleMessageFrameLineNode()
{
    delete string;
    string = nullptr;
}

CSimpleMessageFrameLine::CSimpleMessageFrameLine() = default;

CSimpleMessageFrameLine::~CSimpleMessageFrameLine()
{
    delete stringNode;
    stringNode = nullptr;
}

CSimpleMessageFrameLine::CSimpleMessageFrameLine(CSimpleMessageFrameLine&& other) noexcept
    : offsetX(other.offsetX), offsetY(other.offsetY), stringNode(other.stringNode)
{
    other.stringNode = nullptr;
}

CSimpleMessageFrameLine& CSimpleMessageFrameLine::operator=(CSimpleMessageFrameLine&& other) noexcept
{
    if (this == &other) return *this;
    delete stringNode;
    offsetX = other.offsetX;
    offsetY = other.offsetY;
    stringNode = other.stringNode;
    other.stringNode = nullptr;
    return *this;
}

CSimpleMessageFrame::CSimpleMessageFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
}

CSimpleMessageFrame::~CSimpleMessageFrame()
{
    Clear();
}

void CSimpleMessageFrame::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        if (const auto* value = node->GetAttributeByName("maxLines")) m_rows = std::max(0, std::atoi(value));
        if (const auto* value = node->GetAttributeByName("textLength")) m_textMaxSize = std::max(0, std::atoi(value));
        if (const auto* value = node->GetAttributeByName("fadeDuration")) m_fadeDuration = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("insertMode"))
            m_insertMode = _stricmp(value, "TOP") == 0 ? INSERT_AT_TOP : INSERT_AT_BOTTOM;
    }
    OnFrameSizeChanged();
}

void CSimpleMessageFrame::SetMessageFrameInsets(const NTempest::CRect& inset)
{
    m_messageFrameInset = inset;
    OnFrameSizeChanged();
}

void CSimpleMessageFrame::SetTextLength(int length) { m_textMaxSize = std::max(0, length); }
void CSimpleMessageFrame::SetInsertMode(SimpleMessageFrameInsertMode mode) { m_insertMode = mode; }

void CSimpleMessageFrame::AddMessage(const char* text, const NTempest::CImVector& color,
                                     float timeVisible, bool permanent)
{
    MessageData data{ text ? text : "", color, timeVisible, permanent };
    AddPendingMessage(data);
}

void CSimpleMessageFrame::Clear()
{
    m_lines.clear();
    m_pendingMessages.clear();
    m_numVisible = 0;
}

void CSimpleMessageFrame::ClearPending() { m_pendingMessages.clear(); }

void CSimpleMessageFrame::OnFrameSizeChanged()
{
    CSimpleFrame::OnFrameSizeChanged();
    m_messageFrameArea = {m_rect.l + m_messageFrameInset.l, m_rect.t + m_messageFrameInset.t,
                          m_rect.r - m_messageFrameInset.r, m_rect.b - m_messageFrameInset.b};
}

void CSimpleMessageFrame::OnLayerUpdate(float elapsedSec)
{
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    while (!m_pendingMessages.empty()) {
        auto message = std::move(m_pendingMessages.front());
        m_pendingMessages.erase(m_pendingMessages.begin());
        auto* node = new CSimpleMessageFrameLineNode;
        node->color = message.color;
        node->timeLeft = message.timeVisible;
        node->fadeLeft = m_fadeDuration;
        node->permanent = message.permanent;
        auto* string = new CSimpleFontString(this);
        string->SetText(message.text.c_str());
        string->SetTextColor(message.color);
        node->string = string;
        CSimpleMessageFrameLine line;
        line.stringNode = node;
        if (m_insertMode == INSERT_AT_TOP) m_lines.insert(m_lines.begin(), std::move(line));
        else m_lines.push_back(std::move(line));
    }
    for (auto& line : m_lines) {
        if (!line.stringNode || line.stringNode->permanent) continue;
        line.stringNode->timeLeft -= elapsedSec;
        if (line.stringNode->timeLeft <= 0.0f) HideLineNode(line.stringNode);
    }
}

void CSimpleMessageFrame::AddPendingMessage(const MessageData& message) { m_pendingMessages.push_back(message); }
void CSimpleMessageFrame::ScrollMessages(int lines)
{
    if (lines > 0) std::rotate(m_lines.begin(), m_lines.begin() + std::min<int>(lines, m_lines.size()), m_lines.end());
}
void CSimpleMessageFrame::HideLineNode(CSimpleMessageFrameLineNode* node)
{
    if (node && node->string) node->string->Hide();
}
void CSimpleMessageFrame::ShowLineNode(CSimpleMessageFrameLineNode* node)
{
    if (node && node->string) node->string->Show();
}
