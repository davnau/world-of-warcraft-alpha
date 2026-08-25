#include "ENGINE/Source/Frame/frame.h"

#include <cctype>

CSimpleHTML::CSimpleHTML(CSimpleFrame* parent)
    : CSimpleFrame(parent)
{
}

CSimpleHTML::~CSimpleHTML()
{
    ClearContent();
}

void CSimpleHTML::LoadXML(const XMLNode* node, CStatus* status)
{
    CSimpleFrame::LoadXML(node, status);
    if (!node) return;
    if (const auto* value = node->GetAttributeByName("text")) SetText(value);
    else if (!node->m_value.empty()) SetText(node->m_value.c_str());
}

void CSimpleHTML::SetText(const char* text)
{
    m_text = text ? text : "";
    ClearContent();
    ParseBODY(m_text);
}

void CSimpleHTML::ClearContent()
{
    for (auto* region : m_content) {
        RemoveFrameRegion(region);
        delete region;
    }
    m_content.clear();
}

void CSimpleHTML::ParseBODY(std::string_view body)
{
    std::size_t cursor = 0;
    while (cursor < body.size()) {
        const auto open = body.find('<', cursor);
        if (open == std::string_view::npos) {
            AddText(body.substr(cursor));
            break;
        }
        if (open > cursor) AddText(body.substr(cursor, open - cursor));
        const auto close = body.find('>', open);
        if (close == std::string_view::npos) break;
        const auto tag = body.substr(open + 1, close - open - 1);
        if (tag.find("IMG") != std::string_view::npos || tag.find("img") != std::string_view::npos)
            ParseIMG(tag);
        cursor = close + 1;
    }
}

void CSimpleHTML::ParseP(std::string_view paragraph) { AddText(paragraph); }

void CSimpleHTML::ParseIMG(std::string_view image)
{
    auto* texture = new CSimpleTexture(this);
    const auto source = image.find("src=");
    if (source != std::string_view::npos) {
        auto value = image.substr(source + 4);
        if (!value.empty() && (value.front() == '\'' || value.front() == '"')) value.remove_prefix(1);
        if (!value.empty() && (value.back() == '\'' || value.back() == '"')) value.remove_suffix(1);
        texture->SetTexture(std::string(value).c_str());
    }
    m_content.push_back(texture);
}

void CSimpleHTML::AddText(std::string_view text)
{
    if (text.empty()) return;
    auto* string = new CSimpleFontString(this);
    string->SetText(std::string(text).c_str());
    m_content.push_back(string);
}
