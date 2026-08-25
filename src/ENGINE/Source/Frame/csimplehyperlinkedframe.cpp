#include "ENGINE/Source/Frame/frame.h"

CSimpleHyperlinkButton::CSimpleHyperlinkButton(CSimpleFrame* parent)
    : CSimpleButton(parent)
{
}

CSimpleHyperlinkButton::~CSimpleHyperlinkButton() = default;

void CSimpleHyperlinkButton::SetHyperlink(const char* hyperlink)
{
    m_hyperlink = hyperlink ? hyperlink : "";
}

void CSimpleHyperlinkButton::OnLayerCursorEnter()
{
    CSimpleButton::OnLayerCursorEnter();
    if (m_owner) m_owner->OnHyperlinkEnter(this);
}

void CSimpleHyperlinkButton::OnLayerCursorExit()
{
    CSimpleButton::OnLayerCursorExit();
    if (m_owner) m_owner->OnHyperlinkLeave(this);
}

void CSimpleHyperlinkButton::OnClick(MOUSEBUTTON button)
{
    if (m_owner) m_owner->OnHyperlinkClick(this);
    CSimpleButton::OnClick(button);
}

CSimpleHyperlinkedFrame::CSimpleHyperlinkedFrame(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
}

CSimpleHyperlinkedFrame::~CSimpleHyperlinkedFrame()
{
    m_hyperlinks.clear();
}

void CSimpleHyperlinkedFrame::LoadXML_Scripts(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML_Scripts(node, status);
}

void CSimpleHyperlinkedFrame::OnHyperlinkEnter(CSimpleHyperlinkButton*) {}
void CSimpleHyperlinkedFrame::OnHyperlinkLeave(CSimpleHyperlinkButton*) {}
void CSimpleHyperlinkedFrame::OnHyperlinkClick(CSimpleHyperlinkButton*) {}

CSimpleHyperlinkButton* CSimpleHyperlinkedFrame::CreateHyperlinkButton(const char* hyperlink)
{
    auto button = std::make_unique<CSimpleHyperlinkButton>(this);
    button->m_owner = this;
    button->SetHyperlink(hyperlink);
    auto* result = button.get();
    m_hyperlinks.push_back(std::move(button));
    return result;
}

void CSimpleHyperlinkedFrame::ReleaseHyperlinkButton(CSimpleHyperlinkButton* button)
{
    m_hyperlinks.erase(std::remove_if(m_hyperlinks.begin(), m_hyperlinks.end(),
        [button](const auto& item) { return item.get() == button; }), m_hyperlinks.end());
}
