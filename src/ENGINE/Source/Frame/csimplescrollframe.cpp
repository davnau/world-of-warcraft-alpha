#include "ENGINE/Source/Frame/frame.h"

#include <algorithm>
#include <cstdlib>

CSimpleScrollFrame::CSimpleScrollFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
}

CSimpleScrollFrame::~CSimpleScrollFrame() = default;

void CSimpleScrollFrame::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        if (const auto* value = node->GetAttributeByName("horizontalScroll"))
            m_horizontalScroll = std::strtof(value, nullptr);
        if (const auto* value = node->GetAttributeByName("verticalScroll"))
            m_verticalScroll = std::strtof(value, nullptr);
    }
    UpdateScrollChildRect();
}

void CSimpleScrollFrame::LoadXML_Scripts(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML_Scripts(node, status);
}

void CSimpleScrollFrame::SetScrollChild(CSimpleFrame* child)
{
    if (m_scrollChild == child) return;
    if (m_scrollChild) m_scrollChild->SetBeingScrolled(false);
    m_scrollChild = child;
    if (m_scrollChild) m_scrollChild->SetBeingScrolled(true);
    UpdateScrollChildRect();
}

void CSimpleScrollFrame::SetHorizontalScroll(float value)
{
    m_horizontalScroll = std::clamp(value, 0.0f, m_horizontalRange);
    UpdateScrollChild();
}

void CSimpleScrollFrame::SetVerticalScroll(float value)
{
    m_verticalScroll = std::clamp(value, 0.0f, m_verticalRange);
    UpdateScrollChild();
}

void CSimpleScrollFrame::UpdateScrollChildRect()
{
    if (!m_scrollChild) {
        m_scrollChildRect = {};
        m_horizontalRange = m_verticalRange = 0.0f;
        return;
    }
    m_scrollChildRect = m_scrollChild->m_rect;
    m_horizontalRange = std::max(0.0f, m_scrollChildRect.Width() - m_rect.Width());
    m_verticalRange = std::max(0.0f, m_scrollChildRect.Height() - m_rect.Height());
    SetHorizontalScroll(m_horizontalScroll);
    SetVerticalScroll(m_verticalScroll);
}

NTempest::CRect GetScrollChildRect(const CSimpleScrollFrame* frame)
{
    return frame ? frame->m_scrollChildRect : NTempest::CRect{};
}

void CSimpleScrollFrame::UpdateScrollChild()
{
    if (!m_scrollChild) return;
    m_scrollChild->SetRect({m_rect.l - m_horizontalScroll, m_rect.t - m_verticalScroll,
                            m_rect.l - m_horizontalScroll + m_scrollChildRect.Width(),
                            m_rect.t - m_verticalScroll + m_scrollChildRect.Height()});
}

void CSimpleScrollFrame::OnLayerUpdate(float elapsedSec)
{
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    UpdateScrollChildRect();
}

void CSimpleScrollFrame::OnFrameRender(CRenderBatch* batch, unsigned layer)
{
    CSimpleFrame::OnFrameRender(batch, layer);
    if (batch && layer == 4)
        batch->QueueCallback([this] { RenderScrollChild(); });
}

void CSimpleScrollFrame::OnFrameSizeChanged()
{
    CSimpleFrame::OnFrameSizeChanged();
    UpdateScrollChildRect();
}

void CSimpleScrollFrame::RenderScrollChild()
{
    if (m_scrollChild) m_scrollChild->OnFrameRender();
}
