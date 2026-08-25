#include "ENGINE/Source/Frame/frame.h"

#include <algorithm>

CSimpleRegion::CSimpleRegion(CSimpleFrame* parent)
    : m_frame(parent)
{
    if (parent) parent->RegisterRegion(this);
}

CSimpleRegion::~CSimpleRegion()
{
    if (m_frame) m_frame->RemoveFrameRegion(this);
    m_frame = nullptr;
}

CLayoutFrame* CSimpleRegion::GetLayoutParent() const { return m_frame; }
const char* CSimpleRegion::GetName() const { return m_name.c_str(); }
void CSimpleRegion::SetName(const char* name) { m_name = name ? name : ""; }
void CSimpleRegion::Show() { m_visible = true; }
void CSimpleRegion::Hide() { m_visible = false; }
bool CSimpleRegion::IsVisible() const { return m_visible && m_alpha > 0.0f; }
void CSimpleRegion::SetAlpha(float alpha)
{
    m_alpha = std::clamp(alpha, 0.0f, 1.0f);
    OnGxColorChanged();
}
float CSimpleRegion::GetAlpha() const { return m_alpha; }
void CSimpleRegion::SetVertexColor(const NTempest::CImVector& color)
{
    m_vertexColor = color;
    OnGxColorChanged();
}
const NTempest::CImVector& CSimpleRegion::GetVertexColor() const { return m_vertexColor; }

void CSimpleRegion::OnGxColorChanged()
{
    if (m_frame) m_frame->NotifyDrawLayerChanged(this);
}

void CSimpleRegion::SetFrame(CSimpleFrame* frame)
{
    if (m_frame == frame) return;
    if (m_frame) m_frame->RemoveFrameRegion(this);
    m_frame = frame;
    if (m_frame) m_frame->RegisterRegion(this);
    OnRegionChanged();
}

void CSimpleRegion::OnRegionChanged()
{
    if (m_frame) m_frame->NotifyDrawLayerChanged(this);
}
