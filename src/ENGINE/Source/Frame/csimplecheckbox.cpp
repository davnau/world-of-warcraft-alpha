#include "ENGINE/Source/Frame/csimplebutton.h"
#include "ENGINE/Source/Frame/framexml.h"

#include <cstdlib>
#include <cstring>

CSimpleCheckbox::CSimpleCheckbox(CSimpleFrame* parent)
    : CSimpleButton(parent)
{
    m_checked = false;
}

CSimpleCheckbox::~CSimpleCheckbox()
{
    SetCheckedTexture(static_cast<CSimpleTexture*>(nullptr));
    SetDisabledCheckedTexture(static_cast<CSimpleTexture*>(nullptr));
}

void CSimpleCheckbox::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleButton::LoadXML(node, status);
    if (node) {
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "CheckedTexture") == 0) {
                auto* texture = new CSimpleTexture(this);
                FrameXML_LoadTextureNode(texture, child, status);
                SetCheckedTexture(texture);
            } else if (_stricmp(child->m_name.c_str(), "DisabledCheckedTexture") == 0) {
                auto* texture = new CSimpleTexture(this);
                FrameXML_LoadTextureNode(texture, child, status);
                SetDisabledCheckedTexture(texture);
            }
        }
        if (const auto* value = node->GetAttributeByName("checked"))
            SetChecked(std::atoi(value) != 0 || _stricmp(value, "true") == 0, 1);
    }
    SetChecked(m_checked, 1);
}

void CSimpleCheckbox::SetCheckedTexture(const char* texture)
{
    if (m_checkedTexture) {
        m_checkedTexture->SetTexture(texture);
        return;
    }

    auto* checkedTexture = new CSimpleTexture(nullptr);
    if (!checkedTexture->SetTexture(texture)) {
        delete checkedTexture;
        return;
    }
    checkedTexture->SetAllPoints(this);
    checkedTexture->SetBlendMode(GxBlend_Add);
    SetCheckedTexture(checkedTexture);
}

void CSimpleCheckbox::SetCheckedTexture(CSimpleTexture* texture)
{
    if (m_checkedTexture == texture) {
        SetChecked(m_checked, 1);
        return;
    }
    delete m_checkedTexture;
    m_checkedTexture = texture;
    if (texture) {
        texture->SetFrame(this);
        RemoveFrameRegion(texture);
        RegisterRegion(texture, 3);
    }
    SetChecked(m_checked, 1);
}

void CSimpleCheckbox::SetDisabledCheckedTexture(const char* texture)
{
    if (m_disabledTexture) {
        m_disabledTexture->SetTexture(texture);
        return;
    }

    auto* disabledTexture = new CSimpleTexture(nullptr);
    if (!disabledTexture->SetTexture(texture)) {
        delete disabledTexture;
        return;
    }
    disabledTexture->SetAllPoints(this);
    disabledTexture->SetBlendMode(GxBlend_Add);
    SetDisabledCheckedTexture(disabledTexture);
}

void CSimpleCheckbox::SetDisabledCheckedTexture(CSimpleTexture* texture)
{
    if (m_disabledTexture == texture) {
        SetChecked(m_checked, 1);
        return;
    }
    delete m_disabledTexture;
    m_disabledTexture = texture;
    if (texture) {
        texture->SetFrame(this);
        RemoveFrameRegion(texture);
        RegisterRegion(texture, 3);
    }
    SetChecked(m_checked, 1);
}

void CSimpleCheckbox::Enable(int enabled)
{
    CSimpleButton::Enable(enabled != 0);
    SetChecked(m_checked, 1);
}

void CSimpleCheckbox::SetChecked(int state, int force)
{
    const bool checked = state != 0;
    if (!force && checked == m_checked) return;
    m_checked = checked;
    if (m_checkedTexture) m_checkedTexture->Hide();
    if (m_disabledTexture) m_disabledTexture->Hide();
    if (!m_checked) return;

    if (m_disabledTexture && m_state == BUTTONSTATE_DISABLED)
        m_disabledTexture->Show();
    else if (m_checkedTexture)
        m_checkedTexture->Show();
}

void CSimpleCheckbox::OnClick(MOUSEBUTTON click)
{
    SetChecked(!m_checked, 0);
    CSimpleButton::OnClick(click);
}
