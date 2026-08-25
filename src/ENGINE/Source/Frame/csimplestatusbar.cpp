#include "ENGINE/Source/Frame/csimplestatusbar.h"
#include "ENGINE/Source/Frame/framexml.h"
#include "ENGINE/Source/Frame/framescript.h"

#include <algorithm>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <string>

namespace {
unsigned DrawLayerFromString(const char* value)
{
    if (!value || !*value || _stricmp(value, "ARTWORK") == 0) return 2U;
    if (_stricmp(value, "BACKGROUND") == 0) return 0U;
    if (_stricmp(value, "BORDER") == 0) return 1U;
    if (_stricmp(value, "OVERLAY") == 0) return 3U;
    if (_stricmp(value, "HIGHLIGHT") == 0) return 4U;
    return 2U;
}

std::uint8_t ColorChannel(const char* value, std::uint8_t fallback)
{
    if (!value || !*value) return fallback;
    const float channel = std::clamp(std::strtof(value, nullptr), 0.0F, 1.0F);
    return static_cast<std::uint8_t>(channel * 255.0F + 0.5F);
}

bool IsUnitStatusBar(const char* name)
{
    return name && (_strnicmp(name, "PlayerFrame", 11) == 0 ||
                    _strnicmp(name, "TargetFrame", 11) == 0 ||
                    _stricmp(name, "MainMenuExpBar") == 0);
}
}

CSimpleStatusBar::CSimpleStatusBar(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
}

CSimpleStatusBar::~CSimpleStatusBar() = default;

void CSimpleStatusBar::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (node) {
        const unsigned layer = DrawLayerFromString(node->GetAttributeByName("drawLayer"));
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "BarTexture") == 0) {
                auto* texture = new CSimpleTexture(this);
                FrameXML_LoadTextureNode(texture, child, status);
                SetBarTexture(texture, layer);
            } else if (_stricmp(child->m_name.c_str(), "BarColor") == 0) {
                NTempest::CImVector color(0xffffffffU);
                color.r = ColorChannel(child->GetAttributeByName("r"), color.r);
                color.g = ColorChannel(child->GetAttributeByName("g"), color.g);
                color.b = ColorChannel(child->GetAttributeByName("b"), color.b);
                color.a = ColorChannel(child->GetAttributeByName("a"), color.a);
                SetStatusBarColor(color);
            }
        }
        const auto* minValue = node->GetAttributeByName("minValue");
        const auto* maxValue = node->GetAttributeByName("maxValue");
        if (minValue || maxValue)
            SetMinMaxValues(minValue ? std::strtof(minValue, nullptr) : m_minValue,
                            maxValue ? std::strtof(maxValue, nullptr) : m_maxValue);
        const char* value = node->GetAttributeByName("defaultValue");
        if (!value) value = node->GetAttributeByName("value");
        if (value && *value) SetValue(std::strtof(value, nullptr));
    }
    OnLayerUpdate(0.0f);
}

void CSimpleStatusBar::LoadXML_Scripts(const XMLNode* node, CStatus* status)
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
        if (_stricmp(script->m_name.c_str(), "OnValueChanged") != 0) continue;
        const std::string debugName = m_frameName + ":OnValueChanged";
        SetEventScript(m_onValueChanged, script->m_value.c_str(), debugName.c_str());
    }
}

void CSimpleStatusBar::SetStatusBarColor(const NTempest::CImVector& color)
{
    if (m_barTexture) m_barTexture->SetVertexColor(color);
}

bool CSimpleStatusBar::SetBarTexture(const char* texture, int)
{
    if (m_barTexture) return m_barTexture->SetTexture(texture);
    auto* region = new CSimpleTexture(nullptr);
    if (!region->SetTexture(texture)) {
        delete region;
        return false;
    }
    region->SetAllPoints(this);
    SetBarTexture(region, 2U);
    return true;
}

void CSimpleStatusBar::SetBarTexture(CSimpleTexture* texture, unsigned layer)
{
    if (texture == m_barTexture) return;
    if (m_barTexture) {
        CSimpleTexture* old = m_barTexture;
        m_barTexture = nullptr;
        old->SetFrame(nullptr);
        delete old;
    }
    m_barTexture = texture;
    if (texture) {
        texture->SetFrame(this);
        RemoveFrameRegion(texture);
        RegisterRegion(texture, layer);
        if (std::none_of(texture->m_pointSet.begin(), texture->m_pointSet.end(),
                         [](bool set) { return set; })) {
            texture->SetAllPoints(this);
        }
        texture->m_TexCoordModifiesPosition = true;
        m_changed = true;
    }
}

void CSimpleStatusBar::SetMinMaxValues(float min, float max)
{
    if (min > max) std::swap(min, max);
    const bool hadValue = m_valueSet;
    m_minValue = min;
    m_maxValue = max;
    m_rangeSet = true;
    m_changed = true;
    // The original clears its VALUE_SET flag before reclamping an existing
    // value.  That deliberately fires OnValueChanged when only the range
    // changes (TextStatusBar.lua relies on it to reshow the XP/action bar).
    if (hadValue) {
        m_valueSet = false;
        SetValue(m_value);
    }
    if (IsUnitStatusBar(GetName())) {
        std::fprintf(stderr, "STATUSBAR_RANGE name=%s min=%.3f max=%.3f texture=%s\n",
                     GetName(), min, max,
                     m_barTexture ? m_barTexture->m_texture.c_str() : "<none>");
    }
}

void CSimpleStatusBar::SetValue(float value)
{
    const float clamped = std::clamp(value, m_minValue, m_maxValue);
    if (m_valueSet && clamped == m_value) return;
    m_value = clamped;
    m_valueSet = true;
    m_changed = true;
    if (m_onValueChanged) FrameScript_Execute(m_onValueChanged, this, "%f", clamped);
    if (IsUnitStatusBar(GetName())) {
        std::fprintf(stderr,
                     "STATUSBAR_VALUE name=%s value=%.3f range=(%.3f,%.3f) "
                     "onValue=%d shown=%d visible=%d\n",
                     GetName(), clamped, m_minValue, m_maxValue,
                     m_onValueChanged, m_shown ? 1 : 0, m_visible ? 1 : 0);
    }
}

double CSimpleStatusBar::GetAnimValue() const
{
    const float range = m_maxValue - m_minValue;
    return range > 0.0F ? (m_value - m_minValue) / range : 0.0;
}

void CSimpleStatusBar::OnLayerUpdate(float elapsedSec)
{
    CSimpleFrame::OnLayerUpdate(elapsedSec);
    if (!m_barTexture || !m_rangeSet || !m_valueSet) return;
    const float amount = std::clamp(static_cast<float>(GetAnimValue()), 0.0F, 1.0F);
    m_barTexture->SetTexCoord(0.0F, amount, 0.0F, 1.0F);
    m_changed = false;
}
