#include "ENGINE/Source/Frame/cframepoint.h"

double CFramePointRelative::X(float scale) const
{
    if (!m_relative)
        return CFramePoint::UNDEFINED;
    NTempest::CRect rect;
    if (!m_relative->GetRect(&rect))
        return CFramePoint::UNDEFINED;
    if (m_relative->IsAttachmentOrigin()) {
        rect.r -= rect.l;
        rect.l = 0.0F;
    }
    switch (m_framePoint) {
    case FRAMEPOINT_TOPLEFT:
    case FRAMEPOINT_LEFT:
    case FRAMEPOINT_BOTTOMLEFT: return rect.l + scale * m_offset.x;
    case FRAMEPOINT_TOP:
    case FRAMEPOINT_CENTER:
    case FRAMEPOINT_BOTTOM: return (rect.l + rect.r) * 0.5F + scale * m_offset.x;
    case FRAMEPOINT_TOPRIGHT:
    case FRAMEPOINT_RIGHT:
    case FRAMEPOINT_BOTTOMRIGHT: return rect.r + scale * m_offset.x;
    default: return CFramePoint::UNDEFINED;
    }
}

double CFramePointRelative::Y(float scale) const
{
    if (!m_relative)
        return CFramePoint::UNDEFINED;
    NTempest::CRect rect;
    if (!m_relative->GetRect(&rect))
        return CFramePoint::UNDEFINED;
    if (m_relative->IsAttachmentOrigin()) {
        rect.b -= rect.t;
        rect.t = 0.0F;
    }
    switch (m_framePoint) {
    case FRAMEPOINT_TOPLEFT:
    case FRAMEPOINT_TOP:
    case FRAMEPOINT_TOPRIGHT: return rect.b + scale * m_offset.y;
    case FRAMEPOINT_LEFT:
    case FRAMEPOINT_CENTER:
    case FRAMEPOINT_RIGHT: return (rect.t + rect.b) * 0.5F + scale * m_offset.y;
    case FRAMEPOINT_BOTTOMLEFT:
    case FRAMEPOINT_BOTTOM:
    case FRAMEPOINT_BOTTOMRIGHT: return rect.t + scale * m_offset.y;
    default: return CFramePoint::UNDEFINED;
    }
}
