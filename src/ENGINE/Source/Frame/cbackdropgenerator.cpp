#include "ENGINE/Source/Frame/frame.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>

namespace {
void SetTexture(CSimpleTexture*& target, CSimpleFrame* output, const std::string& file)
{
    if (file.empty()) return;
    if (!target) target = new CSimpleTexture(output);
    target->SetTexture(file.c_str());
}

void SetRect(CSimpleTexture* texture, float l, float t, float r, float b)
{
    if (texture) texture->SetRect({l, t, r, b});
}
}

CBackdropGenerator::CBackdropGenerator()
{
    m_cornerSize = 0.025f;
    m_color = NTempest::CImVector(0xffffffffu);
    m_borderColor = NTempest::CImVector(0xffffffffu);
}

void CBackdropGenerator::LoadXML(const XMLNode* node, CStatus*)
{
    if (!node) return;
    if (const auto* value = node->GetAttributeByName("bgFile")) m_background = value;
    if (const auto* value = node->GetAttributeByName("edgeFile")) m_border = value;
    if (const auto* value = node->GetAttributeByName("tile"))
        m_tileBackground = (_stricmp(value, "true") == 0 || std::atoi(value) != 0);
    for (const XMLNode* child = node->m_child; child; child = child->m_next) {
        if (_stricmp(child->m_name.c_str(), "TileSize") == 0 && !child->m_value.empty())
            m_backgroundSize = std::strtof(child->m_value.c_str(), nullptr);
        else if (_stricmp(child->m_name.c_str(), "EdgeSize") == 0 && !child->m_value.empty())
            m_cornerSize = std::strtof(child->m_value.c_str(), nullptr);
        else if (_stricmp(child->m_name.c_str(), "BackgroundInsets") == 0) {
            if (const auto* value = child->GetAttributeByName("left")) m_leftInset = std::strtof(value, nullptr);
            if (const auto* value = child->GetAttributeByName("right")) m_rightInset = std::strtof(value, nullptr);
            if (const auto* value = child->GetAttributeByName("top")) m_topInset = std::strtof(value, nullptr);
            if (const auto* value = child->GetAttributeByName("bottom")) m_bottomInset = std::strtof(value, nullptr);
        }
    }
    m_pieces = 0xffu;
}

void CBackdropGenerator::SetOutput(CSimpleFrame* output)
{
    if (!output) return;
    SetTexture(m_backgroundTexture, output, m_background);
    SetTexture(m_leftTexture, output, m_border);
    SetTexture(m_rightTexture, output, m_border);
    SetTexture(m_topTexture, output, m_border);
    SetTexture(m_bottomTexture, output, m_border);
    SetTexture(m_topLeftTexture, output, m_border);
    SetTexture(m_topRightTexture, output, m_border);
    SetTexture(m_bottomLeftTexture, output, m_border);
    SetTexture(m_bottomRightTexture, output, m_border);
    Generate(&output->m_rect);
}

void CBackdropGenerator::Generate(const NTempest::CRect* rect)
{
    if (!rect) return;
    const float width = std::max(0.0f, rect->Width());
    const float height = std::max(0.0f, rect->Height());
    const float corner = std::clamp(m_cornerSize, 0.0f, std::min(width, height) * 0.5f);
    const float left = rect->l + m_leftInset;
    const float right = rect->r - m_rightInset;
    const float top = rect->t + m_topInset;
    const float bottom = rect->b - m_bottomInset;
    SetRect(m_backgroundTexture, left, top, right, bottom);
    SetRect(m_topLeftTexture, rect->l, rect->t, rect->l + corner, rect->t + corner);
    SetRect(m_topRightTexture, rect->r - corner, rect->t, rect->r, rect->t + corner);
    SetRect(m_bottomLeftTexture, rect->l, rect->b - corner, rect->l + corner, rect->b);
    SetRect(m_bottomRightTexture, rect->r - corner, rect->b - corner, rect->r, rect->b);
    SetRect(m_leftTexture, rect->l, top, rect->l + corner, bottom);
    SetRect(m_rightTexture, rect->r - corner, top, rect->r, bottom);
    SetRect(m_topTexture, left, rect->t, right, rect->t + corner);
    SetRect(m_bottomTexture, left, rect->b - corner, right, rect->b);
    SetVertexColor(&m_color);
    SetBorderVertexColor(&m_borderColor);
}

void CBackdropGenerator::SetVertexColor(const NTempest::CImVector* color)
{
    if (!color) return;
    m_color = *color;
    for (auto* texture : {m_backgroundTexture}) if (texture) texture->SetVertexColor(m_color);
}

void CBackdropGenerator::GetVertexColor(NTempest::CImVector* color) const
{
    if (color) *color = m_color;
}

void CBackdropGenerator::SetBorderVertexColor(const NTempest::CImVector* color)
{
    if (!color) return;
    m_borderColor = *color;
    for (auto* texture : {m_leftTexture, m_rightTexture, m_topTexture, m_bottomTexture,
                          m_topLeftTexture, m_topRightTexture, m_bottomLeftTexture,
                          m_bottomRightTexture})
        if (texture) texture->SetVertexColor(m_borderColor);
}

void CBackdropGenerator::GetBorderVertexColor(NTempest::CImVector* color) const
{
    if (color) *color = m_borderColor;
}
