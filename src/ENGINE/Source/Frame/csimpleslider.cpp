#include "ENGINE/Source/Frame/frame.h"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>

CSimpleSlider::CSimpleSlider(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
    EnableEvent(SIMPLE_EVENT_MOUSE);
}

CSimpleSlider::~CSimpleSlider() = default;

void CSimpleSlider::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        const auto* minValue = node->GetAttributeByName("minValue");
        const auto* maxValue = node->GetAttributeByName("maxValue");
        if (minValue || maxValue)
            SetMinMaxValues(minValue ? std::strtof(minValue, nullptr) : m_minValue,
                            maxValue ? std::strtof(maxValue, nullptr) : m_maxValue);
        if (const auto* value = node->GetAttributeByName("value")) SetValue(std::strtof(value, nullptr));
        if (const auto* value = node->GetAttributeByName("valueStep")) SetValueStep(std::strtof(value, nullptr));
        if (const auto* value = node->GetAttributeByName("orientation"))
            m_orientation = _stricmp(value, "VERTICAL") == 0 ? SLIDER_VERTICAL : SLIDER_HORIZONTAL;
    }
    OnFrameSizeChanged();
}

void CSimpleSlider::LoadXML_Scripts(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML_Scripts(node, status);
}

void CSimpleSlider::SetThumbTexture(CSimpleTexture* texture)
{
    if (m_thumbTexture) RemoveFrameRegion(m_thumbTexture);
    m_thumbTexture = texture;
    if (texture) RegisterRegion(texture, 0);
}

void CSimpleSlider::SetOrientation(CSimpleSliderOrientation orientation)
{
    m_orientation = orientation;
    OnFrameSizeChanged();
}

void CSimpleSlider::SetMinMaxValues(float min, float max)
{
    if (min > max) std::swap(min, max);
    m_minValue = min;
    m_maxValue = max;
    SetValue(m_value);
}

void CSimpleSlider::SetValue(float value)
{
    const float range = m_maxValue - m_minValue;
    value = std::clamp(value, m_minValue, m_maxValue);
    if (m_valueStep > 0.0f && range > 0.0f)
        value = m_minValue + std::round((value - m_minValue) / m_valueStep) * m_valueStep;
    m_value = std::clamp(value, m_minValue, m_maxValue);
    OnFrameSizeChanged();
}

void CSimpleSlider::SetValueStep(float step)
{
    m_valueStep = std::max(0.0f, step);
    SetValue(m_value);
}

void CSimpleSlider::OnLayerUpdate(float elapsedSec)
{
    (void)elapsedSec;
    if (m_thumbTexture) m_thumbTexture->Show();
}

void CSimpleSlider::OnLayerTrackUpdate(float elapsedSec) { OnLayerUpdate(elapsedSec); }

void CSimpleSlider::OnFrameSizeChanged()
{
    CSimpleFrame::OnFrameSizeChanged();
    if (!m_thumbTexture || m_maxValue <= m_minValue) return;
    const float fraction = (m_value - m_minValue) / (m_maxValue - m_minValue);
    auto rect = m_thumbTexture->m_rect;
    if (m_orientation == SLIDER_HORIZONTAL) {
        const float travel = std::max(0.0f, m_rect.Width() - rect.Width());
        rect.l = m_rect.l + travel * fraction;
        rect.r = rect.l + m_thumbTexture->GetWidth();
        rect.t = m_rect.t;
        rect.b = m_rect.b;
    } else {
        const float travel = std::max(0.0f, m_rect.Height() - rect.Height());
        rect.t = m_rect.t + travel * fraction;
        rect.b = rect.t + m_thumbTexture->GetHeight();
        rect.l = m_rect.l;
        rect.r = m_rect.r;
    }
    m_thumbTexture->SetRect(rect);
}

int CSimpleSlider::OnLayerMouseDown(CMouseEvent* event)
{
    if (!event || event->button != MOUSE_BUTTON_LEFT)
        return CSimpleFrame::OnLayerMouseDown(event);
    m_draggingThumb = true;
    const float extent = m_orientation == SLIDER_HORIZONTAL ? m_rect.Width() : m_rect.Height();
    if (extent > 0.0f) {
        const float coordinate = m_orientation == SLIDER_HORIZONTAL ? event->x - m_rect.l : event->y - m_rect.t;
        SetValue(m_minValue + std::clamp(coordinate / extent, 0.0f, 1.0f) * (m_maxValue - m_minValue));
    }
    return CSimpleFrame::OnLayerMouseDown(event);
}

int CSimpleSlider::OnLayerMouseUp(CMouseEvent* event)
{
    if (event && event->button == MOUSE_BUTTON_LEFT) m_draggingThumb = false;
    return CSimpleFrame::OnLayerMouseUp(event);
}
