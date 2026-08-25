#include "ENGINE/Source/Frame/csimplerender.h"
#include "ENGINE/Source/Frame/framescript.h"
#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "ENGINE/Source/Services/texture.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <utility>

namespace {
constexpr float kDdcPerPixel = 0.0009765625f * 0.80000001f;

std::string ResolveRegionName(const char* value, CSimpleFrame* frame)
{
    if (!value) return {};
    constexpr char token[] = "$parent";
    if (_strnicmp(value, token, sizeof(token) - 1) != 0) return value;
    while (frame && (!frame->GetName() || !*frame->GetName())) frame = frame->ParentFrame();
    std::string name = frame ? frame->GetName() : "TOP";
    name += value + sizeof(token) - 1;
    return name;
}

const XMLNode* ChildByName(const XMLNode* node, const char* name)
{
    if (!node || !name) return nullptr;
    for (const XMLNode* child = node->m_child; child; child = child->m_next)
        if (_stricmp(child->m_name.c_str(), name) == 0) return child;
    return nullptr;
}

bool XmlBool(const char* value)
{
    return value && (_stricmp(value, "true") == 0 || _stricmp(value, "yes") == 0 ||
                     std::strcmp(value, "1") == 0);
}

bool LoadXmlValue(const XMLNode* node, float& result)
{
    if (!node) return false;
    const XMLNode* valueNode = node->m_child ? node->m_child : node;
    const char* value = valueNode->GetAttributeByName("val");
    if (!value || !*value) return false;
    const bool absolute = _stricmp(valueNode->m_name.c_str(), "AbsValue") == 0;
    const bool relative = _stricmp(valueNode->m_name.c_str(), "RelValue") == 0;
    if (!absolute && !relative && valueNode != node) return false;
    result = std::strtof(value, nullptr) * (absolute ? kDdcPerPixel : 1.0f);
    return true;
}

bool LoadXmlDimensions(const XMLNode* node, NTempest::C2Vector& result)
{
    if (!node) return false;
    const XMLNode* dimensions = node->m_child ? node->m_child : node;
    const bool absolute = _stricmp(dimensions->m_name.c_str(), "AbsDimension") == 0;
    const bool relative = _stricmp(dimensions->m_name.c_str(), "RelDimension") == 0;
    if (!absolute && !relative) return false;
    const float scale = absolute ? kDdcPerPixel : 1.0f;
    if (const char* value = dimensions->GetAttributeByName("x"))
        result.x = std::strtof(value, nullptr) * scale;
    if (const char* value = dimensions->GetAttributeByName("y"))
        result.y = std::strtof(value, nullptr) * scale;
    return true;
}

std::uint8_t ColorChannel(const char* value, std::uint8_t fallback)
{
    if (!value || !*value) return fallback;
    return static_cast<std::uint8_t>(std::clamp(std::strtof(value, nullptr), 0.0f, 1.0f) * 255.0f + 0.5f);
}

bool LoadXmlColor(const XMLNode* node, NTempest::CImVector& color)
{
    if (!node) return false;
    color.r = ColorChannel(node->GetAttributeByName("r"), color.r);
    color.g = ColorChannel(node->GetAttributeByName("g"), color.g);
    color.b = ColorChannel(node->GetAttributeByName("b"), color.b);
    color.a = ColorChannel(node->GetAttributeByName("a"), color.a ? color.a : 255);
    return true;
}

unsigned StringToJustify(const char* value)
{
    if (!value) return 0;
    if (_stricmp(value, "LEFT") == 0) return 1;
    if (_stricmp(value, "CENTER") == 0) return 2;
    if (_stricmp(value, "RIGHT") == 0) return 4;
    if (_stricmp(value, "TOP") == 0) return 8;
    if (_stricmp(value, "MIDDLE") == 0 || _stricmp(value, "CENTER") == 0) return 16;
    if (_stricmp(value, "BOTTOM") == 0) return 32;
    return 0;
}
}

CRenderBatch::CRenderBatch() = default;
CRenderBatch::~CRenderBatch() = default;

void SortByTexture(CRenderBatch* batch)
{
    if (!batch) return;
    std::stable_sort(batch->m_texturelist.begin(), batch->m_texturelist.end(),
                     [](const CSimpleBatchedTexture& left, const CSimpleBatchedTexture& right) {
                         return left.texture < right.texture;
                     });
}

void CSimpleRender::DrawBatch(CRenderBatch* batch)
{
    if (!batch) return;
    batch->Finish();

    if (!batch->m_texturelist.empty()) {
        // CSimpleRender::s_indices at 0x8AF8D0 in the original image.
        static constexpr std::uint16_t indices[] = {0, 1, 2, 3};
        static const NTempest::C3Vector normal{0.0f, 0.0f, 1.0f};
        GxVertexShaderSelect(GxVS_PassThru);
        GxRsPush();
        GxRsSet(GxRs_Lighting, 0);
        GxRsSet(GxRs_Fog, 0);
        GxRsSet(GxRs_DepthTest, 0);
        GxRsSet(GxRs_DepthWrite, 0);
        // Screen-space quads have no meaningful back face.  Model materials
        // may leave either winding selected; the recovered UI pass relies on
        // these quads being rendered independently of that 3D state.
        GxRsSet(GxRs_Culling, 0);
        GxRsSet(GxRs_MatDiffuse, NTempest::CImVector{0xffffffffU});
        GxRsSet(GxRs_TexBlend0, static_cast<int>(GxTexBlend_Mod));
        GxRsSet(GxRs_TexGen0, 0);
        GxRsSet(GxRs_TextureShader0, 0);
        GxRsSet(GxRs_Texture1, static_cast<CGxTex*>(nullptr));
        GxRsSet(GxRs_Texture2, static_cast<CGxTex*>(nullptr));
        GxRsSet(GxRs_Texture3, static_cast<CGxTex*>(nullptr));

        CGxTex* activeTexture = nullptr;
        for (const auto& item : batch->m_texturelist) {
            if (!item.texture) continue;
            CGxTex* texture = item.texture->GetTexture();
            if (!texture) continue;
            if (texture != activeTexture) {
                activeTexture = texture;
                GxRsSet(GxRs_Texture0, texture);
            }
            GxRsSet(GxRs_Blend, item.texture->m_alphamode);
            std::array<NTempest::C3Vector, 4> positions = item.texture->m_position;
            if (item.rect.l != item.texture->m_rect.l || item.rect.t != item.texture->m_rect.t ||
                item.rect.r != item.texture->m_rect.r || item.rect.b != item.texture->m_rect.b) {
                positions = {NTempest::C3Vector(item.rect.l, item.rect.b, 0.0f),
                             NTempest::C3Vector(item.rect.l, item.rect.t, 0.0f),
                             NTempest::C3Vector(item.rect.r, item.rect.b, 0.0f),
                             NTempest::C3Vector(item.rect.r, item.rect.t, 0.0f)};
            }
            NTempest::CImVector color = item.texture->GetVertexColor();
            color.a = static_cast<std::uint8_t>(std::clamp(item.alpha, 0.0f, 1.0f) * color.a + 0.5f);
            GxPrimLockVertexPtrs(4, positions.data(), sizeof(NTempest::C3Vector),
                                 &normal, 0, &color, 0, nullptr, 0,
                                 item.texture->m_texCoord.data(), sizeof(NTempest::C2Vector),
                                 nullptr, 0);
            GxPrimDrawElements(GxPrim_TriangleStrip, 4, indices);
            GxPrimUnlockVertexPtrs();
        }
        GxRsPop();
    }

    for (CSimpleFontString* string : batch->m_strings) {
        if (string && string->m_string) TextBlockRender(string->m_string);
    }

    // The recovered callback list is traversed after both texture and font
    // batches.  Nodes remain attached until CRenderBatch::Clear rebuilds it.
    for (auto& callback : batch->m_callbacks)
        if (callback.callback) callback.callback();
}

void CRenderBatch::QueueTexture(CSimpleTexture* texture, const NTempest::CRect* rect)
{
    if (!texture) return;
    CSimpleBatchedTexture item;
    item.texture = texture;
    item.rect = rect ? *rect : texture->m_rect;
    item.alpha = texture->GetAlpha();
    m_texturelist.push_back(item);
    ++m_count;
}

void CRenderBatch::QueueFontString(CSimpleFontString* string)
{
    if (!string) return;
    m_strings.push_back(string);
    ++m_count;
}

void CRenderBatch::QueueCallback(std::function<void()> callback)
{
    if (callback) {
        m_callbacks.push_back({std::move(callback)});
        ++m_count;
    }
}

void CRenderBatch::Finish()
{
    SortByTexture(this);
}

void CRenderBatch::Clear()
{
    m_count = 0;
    m_texturelist.clear();
    m_strings.clear();
    m_callbacks.clear();
}

CSimpleTexture::CSimpleTexture(CSimpleFrame* parent)
    : CSimpleRegion(parent)
{
    m_texCoord = {NTempest::C2Vector(0.0f, 0.0f), NTempest::C2Vector(0.0f, 1.0f),
                  NTempest::C2Vector(1.0f, 0.0f), NTempest::C2Vector(1.0f, 1.0f)};
}

CSimpleTexture::~CSimpleTexture()
{
    ClearFromSimpleRegistry();
    if (m_textureHandle) TextureRelease(m_textureHandle);
}

CLayoutFrame* CSimpleTexture::GetLayoutFrameByName(const char* context, const char* name)
{
    return CLayoutFrame::GetLayoutFrameByName(this, context, name);
}

void CSimpleTexture::PreLoadXML(const XMLNode* node, CStatus*)
{
    if (!node) return;
    const char* value = node->GetAttributeByName("name");
    if (value && *value) {
        SetName(ResolveRegionName(value, m_frame).c_str());
        AddToRegistry();
    }
}

void CSimpleTexture::LoadXML(const XMLNode* node, CStatus* status)
{
    CLayoutFrame::LoadXML(node, status);
    if (node) {
        if (const char* hidden = node->GetAttributeByName("hidden")) {
            if (XmlBool(hidden)) Hide(); else Show();
        }
        // CSimpleTexture::LoadXML at 0x76F9E0 walks the texture children before
        // loading the file.  In particular, the 2003 FrameXML uses TexCoords to
        // select strips from UI atlases such as UI-MainMenuBar-Dwarf.
        bool uvWrapping = false;
        for (const XMLNode* child = node->m_child; child; child = child->m_next) {
            if (_stricmp(child->m_name.c_str(), "TexCoords") == 0) {
                float left = 0.0f;
                float right = 1.0f;
                float top = 0.0f;
                float bottom = 1.0f;
                if (const char* value = child->GetAttributeByName("left"))
                    left = std::strtof(value, nullptr);
                if (const char* value = child->GetAttributeByName("right"))
                    right = std::strtof(value, nullptr);
                if (const char* value = child->GetAttributeByName("top"))
                    top = std::strtof(value, nullptr);
                if (const char* value = child->GetAttributeByName("bottom"))
                    bottom = std::strtof(value, nullptr);
                SetTexCoord(left, right, top, bottom);
                uvWrapping = left < 0.0f || left > 1.0f ||
                    right < 0.0f || right > 1.0f ||
                    top < 0.0f || top > 1.0f ||
                    bottom < 0.0f || bottom > 1.0f;
            } else if (_stricmp(child->m_name.c_str(), "Color") == 0) {
                NTempest::CImVector color{};
                if (LoadXmlColor(child, color)) SetTexture(&color);
            }
        }
        if (const auto* value = node->GetAttributeByName("file"))
            SetTexture(value, uvWrapping ? 1 : 0);
        const char* value = node->GetAttributeByName("alphaMode");
        if (!value || !*value) value = node->GetAttributeByName("blendMode");
        if (value && *value) {
            if (_stricmp(value, "DISABLE") == 0 || _stricmp(value, "OPAQUE") == 0)
                SetBlendMode(EGxBlend::GxBlend_Opaque);
            else if (_stricmp(value, "ALPHAKEY") == 0)
                SetBlendMode(EGxBlend::GxBlend_AlphaKey);
            else if (_stricmp(value, "BLEND") == 0 || _stricmp(value, "ALPHA") == 0)
                SetBlendMode(EGxBlend::GxBlend_Alpha);
            else if (_stricmp(value, "ADD") == 0)
                SetBlendMode(EGxBlend::GxBlend_Add);
            else if (_stricmp(value, "MOD") == 0)
                SetBlendMode(EGxBlend::GxBlend_Mod);
            else if (_stricmp(value, "MOD2X") == 0)
                SetBlendMode(EGxBlend::GxBlend_Mod2x);
            else if (_stricmp(value, "MODADD") == 0)
                SetBlendMode(EGxBlend::GxBlend_ModAdd);
            else if (_stricmp(value, "INVSRCALPHAADD") == 0)
                SetBlendMode(EGxBlend::GxBlend_InvSrcAlphaAdd);
        }
    }
    OnFrameSizeChanged();
}

void CSimpleTexture::PostLoadXML(const XMLNode*, CStatus*)
{
    if (m_frame && std::none_of(m_pointSet.begin(), m_pointSet.end(), [](bool set) { return set; }))
        SetAllPoints(m_frame);
}

void CSimpleTexture::AddToRegistry(const char* context)
{
    if (!m_name.empty()) {
        SimpleTextureRegistryAddEntry(this, context, m_name.c_str());
        RegisterScriptObject(m_name.c_str());
    }
}

bool CSimpleTexture::SetTexture(const char* texture, int flags)
{
    if (m_textureHandle) {
        TextureRelease(m_textureHandle);
        m_textureHandle = nullptr;
    }
    m_texture = texture ? texture : "";
    if (m_texture.empty()) {
        OnRegionChanged();
        return true;
    }
    const CGxTexFlags gxFlags(GxTex_Linear, flags != 0, flags != 0,
                              false, false, false, 1);
    m_textureHandle = TextureCreate(m_texture.c_str(), nullptr, gxFlags.bits, 0);
    OnRegionChanged();
    return m_textureHandle != nullptr;
}

void CSimpleTexture::SetTexture(void* texture)
{
    if (m_textureHandle) TextureRelease(m_textureHandle);
    m_textureHandle = static_cast<CTexture*>(texture);
    if (m_textureHandle) ++m_textureHandle->refCount;
    m_texture = m_textureHandle ? TextureGetFilename(m_textureHandle) : "";
    OnRegionChanged();
}

void CSimpleTexture::SetTexture(const NTempest::CImVector* color)
{
    if (m_textureHandle)
        TextureRelease(m_textureHandle);
    const NTempest::CImVector solid = color
        ? *color : NTempest::CImVector{0xffffffffU};
    m_textureHandle = TextureCreateSolid(&solid);
    m_texture.clear();
    m_vertexColor = NTempest::CImVector{0xffffffffU};
    OnRegionChanged();
}

void CSimpleTexture::SetBlendMode(EGxBlend mode) { m_alphamode = mode; }

void CSimpleTexture::SetTexCoord(const NTempest::C2Vector* coords)
{
    if (!coords) return;
    std::copy_n(coords, 4, m_texCoord.begin());
    if (m_TexCoordModifiesPosition) {
        NTempest::CRect rect{};
        if (GetRect(&rect)) {
            TexCorrectRect(&rect);
            m_position = {NTempest::C3Vector(rect.l, rect.b, 0.0F),
                          NTempest::C3Vector(rect.l, rect.t, 0.0F),
                          NTempest::C3Vector(rect.r, rect.b, 0.0F),
                          NTempest::C3Vector(rect.r, rect.t, 0.0F)};
        }
    }
}

void CSimpleTexture::SetTexCoord(float left, float right, float top, float bottom)
{
    const std::array<NTempest::C2Vector, 4> coords{
        NTempest::C2Vector(left, top), NTempest::C2Vector(left, bottom),
        NTempest::C2Vector(right, top), NTempest::C2Vector(right, bottom)};
    SetTexCoord(coords.data());
}

void CSimpleTexture::TexCorrectRect(NTempest::CRect* rect) const
{
    if (!rect) return;
    const float left = rect->l;
    const float top = rect->t;
    const float width = rect->r - rect->l;
    const float height = rect->b - rect->t;
    rect->l = left + width * m_texCoord[0].x;
    rect->r = left + width * m_texCoord[2].x;
    rect->t = top + height * m_texCoord[0].y;
    rect->b = top + height * m_texCoord[1].y;
}

void CSimpleTexture::SetPosition(const NTempest::C3Vector* positions)
{
    if (positions) std::copy_n(positions, 4, m_position.begin());
}

void CSimpleTexture::OnFrameSizeChanged()
{
    NTempest::CRect rect = m_rect;
    if (m_TexCoordModifiesPosition) TexCorrectRect(&rect);
    m_position[0] = {rect.l, rect.b, 0.0f};
    m_position[1] = {rect.l, rect.t, 0.0f};
    m_position[2] = {rect.r, rect.b, 0.0f};
    m_position[3] = {rect.r, rect.t, 0.0f};
}

void CSimpleTexture::Draw(CRenderBatch* batch)
{
    if (!batch || !IsVisible() || !m_textureHandle) return;
    batch->QueueTexture(this);
}

void CSimpleTexture::ClearFromSimpleRegistry()
{
    SimpleTextureRegistryRemoveEntry(this);
}

const char* CSimpleTexture::GetName() const
{
    return CSimpleRegion::GetName();
}

CGxTex* CSimpleTexture::GetTexture() const
{
    return m_textureHandle ? TextureGetGxTex(m_textureHandle, 1, nullptr) : nullptr;
}

float CSimpleTexture::GetWidth() const
{
    const float width = CLayoutFrame::GetWidth();
    if (width != 0.0f || !m_textureHandle) return width;
    unsigned pixels = 0;
    TextureGetDimensions(m_textureHandle, &pixels, nullptr);
    return static_cast<float>(pixels) * 0.0009765625f * 0.80000001f;
}

float CSimpleTexture::GetHeight() const
{
    const float height = CLayoutFrame::GetHeight();
    if (height != 0.0f || !m_textureHandle) return height;
    unsigned pixels = 0;
    TextureGetDimensions(m_textureHandle, nullptr, &pixels);
    return static_cast<float>(pixels) * 0.0009765625f * 0.80000001f;
}

void CSimpleFontStringAttributes::UpdateString(CSimpleFontString* string) const
{
    if (!string) return;
    string->SetFont(m_font.c_str(), m_fontHeight, m_fontFlags);
    string->SetTextColor(m_color);
    string->SetSpacing(m_spacing);
}

void CSimpleFontStringAttributes::CopyFlags(const CSimpleFontStringAttributes& other)
{
    m_flags = other.m_flags;
    m_fontFlags = other.m_fontFlags;
    m_styleFlags = other.m_styleFlags;
}

CSimpleFontStringAttributes& CSimpleFontStringAttributes::operator=(
    const CSimpleFontStringAttributes& other)
{
    if (this == &other) return *this;
    m_flags = other.m_flags;
    m_font = other.m_font;
    m_fontHeight = other.m_fontHeight;
    m_fontFlags = other.m_fontFlags;
    m_spacing = other.m_spacing;
    m_styleFlags = other.m_styleFlags;
    m_color = other.m_color;
    m_shadowColor = other.m_shadowColor;
    m_shadowOffset = other.m_shadowOffset;
    return *this;
}

CSimpleFontString::CSimpleFontString(CSimpleFrame* parent)
    : CSimpleRegion(parent)
{
}

CSimpleFontString::~CSimpleFontString()
{
    if (m_string) TextBlockDestroy(m_string);
    if (m_fontHandle) TextBlockReleaseFont(m_fontHandle);
    SimpleFontStringRegistryRemoveEntry(this);
}

CLayoutFrame* CSimpleFontString::GetLayoutFrameByName(const char* context, const char* name)
{
    return CLayoutFrame::GetLayoutFrameByName(this, context, name);
}

void CSimpleFontString::PreLoadXML(const XMLNode* node, CStatus*)
{
    if (!node) return;
    const char* value = node->GetAttributeByName("name");
    if (value && *value) {
        SetName(ResolveRegionName(value, m_frame).c_str());
        AddToRegistry();
    }
}

void CSimpleFontString::LoadXML(const XMLNode* node, CStatus* status)
{
    CLayoutFrame::LoadXML(node, status);
    if (!node) return;
    if (const XMLNode* size = ChildByName(node, "Size")) {
        NTempest::C2Vector dimensions{};
        if (LoadXmlDimensions(size, dimensions)) {
            // A zero FontString dimension means "measure this axis from the
            // text".  GlueDialogText is the canonical case: it has a fixed
            // 300-pixel width and a zero height so the dialog grows to fit
            // the status message.
            m_autoWidth = dimensions.x == 0.0f;
            m_autoHeight = dimensions.y == 0.0f;
        }
    }

    if (const char* value = node->GetAttributeByName("hidden")) {
        if (XmlBool(value)) Hide(); else Show();
    }

    if (const char* font = node->GetAttributeByName("font"); font && *font) {
        float height = 0.0f;
        if (const XMLNode* heightNode = ChildByName(node, "FontHeight")) LoadXmlValue(heightNode, height);
        unsigned fontFlags = 0;
        if (const char* outline = node->GetAttributeByName("outline")) {
            if (_stricmp(outline, "NORMAL") == 0) fontFlags = 1;
            else if (_stricmp(outline, "THICK") == 0) fontFlags = 5;
        }
        if (XmlBool(node->GetAttributeByName("monochrome"))) fontFlags |= 8;
        if (height > 0.0f) SetFont(font, height, fontFlags);
    }

    if (const char* value = node->GetAttributeByName("spacing"); value && *value)
        SetSpacing(std::strtof(value, nullptr) * kDdcPerPixel);
    if (const char* value = node->GetAttributeByName("bytes"); value && *value)
        SetTextLength(std::atoi(value));
    if (const char* value = node->GetAttributeByName("text"); value && *value) {
        const char* translated = FrameScript_GetText(value, -1, GENDER_NOT_APPLICABLE);
        SetText(translated && *translated ? translated : value);
    }

    if (const unsigned justify = StringToJustify(node->GetAttributeByName("justifyV")))
        m_styleFlags = (m_styleFlags & ~0x38U) | (justify & 0x38U);
    if (const unsigned justify = StringToJustify(node->GetAttributeByName("justifyH")))
        m_styleFlags = (m_styleFlags & ~0x07U) | (justify & 0x07U);
    if (const char* value = node->GetAttributeByName("wrapOnSpaces"); value && *value) {
        if (XmlBool(value)) m_styleFlags |= 0x1000U;
        else m_styleFlags &= ~0x1000U;
    }

    for (const XMLNode* child = node->m_child; child; child = child->m_next) {
        if (_stricmp(child->m_name.c_str(), "Color") == 0) {
            NTempest::CImVector color{0xffffffffU};
            if (LoadXmlColor(child, color)) SetVertexColor(color);
        } else if (_stricmp(child->m_name.c_str(), "Shadow") == 0) {
            NTempest::CImVector color{0xff000000U};
            NTempest::C2Vector offset{kDdcPerPixel, -kDdcPerPixel};
            if (const XMLNode* colorNode = ChildByName(child, "Color")) LoadXmlColor(colorNode, color);
            if (const XMLNode* offsetNode = ChildByName(child, "Offset")) LoadXmlDimensions(offsetNode, offset);
            AddShadow(color, offset);
        }
    }
    UpdateString();
}

void CSimpleFontString::PostLoadXML(const XMLNode*, CStatus*)
{
    if (m_frame && std::none_of(m_pointSet.begin(), m_pointSet.end(), [](bool set) { return set; })) {
        const FRAMEPOINT point = (m_styleFlags & 1U) ? FRAMEPOINT_LEFT :
                                 (m_styleFlags & 4U) ? FRAMEPOINT_RIGHT : FRAMEPOINT_CENTER;
        SetPoint(point, m_frame, point, 0.0f, 0.0f);
    }
}

void CSimpleFontString::AddToRegistry(const char* context)
{
    if (!m_name.empty()) {
        SimpleFontStringRegistryAddEntry(this, context, m_name.c_str());
        RegisterScriptObject(m_name.c_str());
    }
}

void CSimpleFontString::SetFont(const char* font, float height, unsigned flags)
{
    if (m_string) {
        TextBlockDestroy(m_string);
        m_string = nullptr;
    }
    if (m_fontHandle) {
        TextBlockReleaseFont(m_fontHandle);
        m_fontHandle = nullptr;
    }
    m_font = font ? font : "";
    m_fontHeight = std::max(0.0f, height);
    m_fontFlags = flags;
    if (!m_font.empty() && m_fontHeight > 0.0f)
        m_fontHandle = TextBlockGenerateFont(m_font.c_str(), static_cast<int>(m_fontFlags),
                                             m_fontHeight * m_layoutScale);
    UpdateString();
    Resize();
}

void CSimpleFontString::SetTextHeight(float height)
{
    m_fontHeight = std::max(0.0f, height);
    UpdateString();
    Resize();
}

void CSimpleFontString::SetTextLength(int length)
{
    m_textMaxSize = std::max(0, length);
    if (m_textMaxSize && static_cast<int>(m_text.size()) > m_textMaxSize)
        m_text.resize(static_cast<std::size_t>(m_textMaxSize));
    UpdateString();
    Resize();
}

void CSimpleFontString::SetText(const char* text)
{
    m_text = text ? text : "";
    if (m_textMaxSize && static_cast<int>(m_text.size()) > m_textMaxSize)
        m_text.resize(static_cast<std::size_t>(m_textMaxSize));
    UpdateString();
    Resize();
}

const char* CSimpleFontString::GetText() const { return m_text.c_str(); }

void CSimpleFontString::SetTextColor(const NTempest::CImVector& color)
{
    m_vertexColor = color;
    if (m_string) TextBlockUpdateColor(m_string, &m_vertexColor);
}
void CSimpleFontString::OnGxColorChanged() { UpdateString(); }
void CSimpleFontString::SetSpacing(float spacing) { m_spacing = spacing; UpdateString(); Resize(); }

void CSimpleFontString::AddShadow(const NTempest::CImVector& color,
                                  const NTempest::C2Vector& offset)
{
    m_shadowColor = color;
    m_shadowOffset = offset;
    m_styleFlags |= 0x100U;
    UpdateString();
}

void CSimpleFontString::RemoveShadow()
{
    m_shadowColor = {};
    m_shadowOffset = {};
    m_styleFlags &= ~0x100U;
    UpdateString();
}

void CSimpleFontString::SetAlphaGradient(int start, int length)
{
    m_alphaGradientStart = start;
    m_alphaGradientLength = std::max(0, length);
}

void CSimpleFontString::UpdateString()
{
    if (m_string) {
        TextBlockDestroy(m_string);
        m_string = nullptr;
    }

    const auto lines = static_cast<float>(std::count(m_text.begin(), m_text.end(), '\n') + 1);
    m_cachedWidth = 0.0f;
    if (m_fontHandle && !m_text.empty()) {
        std::size_t begin = 0;
        while (begin <= m_text.size()) {
            const std::size_t end = m_text.find('\n', begin);
            const std::size_t count = end == std::string::npos ? m_text.size() - begin : end - begin;
            float extent = 0.0f;
            TextBlockGetTextExtent(m_fontHandle, m_text.c_str() + begin,
                                   static_cast<unsigned>(count), m_fontHeight * m_layoutScale,
                                   &extent, m_spacing * m_layoutScale, m_styleFlags);
            m_cachedWidth = std::max(m_cachedWidth, extent / m_layoutScale);
            if (end == std::string::npos) break;
            begin = end + 1;
        }
    } else {
        m_cachedWidth = static_cast<float>(m_text.size()) * (m_fontHeight * 0.52f + m_spacing);
    }
    if (m_fontHandle && !m_text.empty()) {
        const float blockWidth = CLayoutFrame::GetWidth() * m_layoutScale;
        m_cachedHeight = static_cast<float>(TextBlockGetWrappedTextHeight(
            m_fontHandle, m_text.c_str(), m_fontHeight * m_layoutScale,
            blockWidth, m_spacing * m_layoutScale, m_styleFlags) / m_layoutScale);
    } else {
        m_cachedHeight = m_text.empty()
            ? 0.0f
            : lines * m_fontHeight + (lines - 1.0f) * m_spacing;
    }
    if (m_autoWidth) m_width = m_cachedWidth;
    if (m_autoHeight) m_height = m_cachedHeight;

    if (m_fontHandle && !m_text.empty() && !m_rect.IsEmpty()) {
        const NTempest::C3Vector position{m_rect.l + m_justificationOffset.x * m_layoutScale,
                                          m_rect.t + m_justificationOffset.y * m_layoutScale, 0.0f};
        m_string = TextBlockCreate(m_fontHandle, m_text.c_str(), &m_vertexColor, &position,
                                   m_fontHeight * m_layoutScale, m_rect.Width(), m_rect.Height(),
                                   m_styleFlags, 0.0f, m_spacing * m_layoutScale);
        if (m_string && (m_styleFlags & 0x100U))
            TextBlockAddShadow(m_string, &m_shadowOffset, m_shadowColor);
        if (m_string && m_alphaGradientStart >= 0)
            TextBlockSetGradient(m_string, m_alphaGradientStart, m_alphaGradientLength);
    }
}

float CSimpleFontString::GetStringWidth() const { return m_cachedWidth; }
float CSimpleFontString::GetWidth() const
{
    const float width = CLayoutFrame::GetWidth();
    return width != 0.0f ? width : m_cachedWidth;
}
float CSimpleFontString::GetStringHeight() const { return m_cachedHeight; }
float CSimpleFontString::GetHeight() const
{
    const float height = CLayoutFrame::GetHeight();
    return height != 0.0f ? height : m_cachedHeight;
}
float CSimpleFontString::GetTextWidth() const { return m_cachedWidth; }

void CSimpleFontString::WrapText(float width)
{
    if (width <= 0.0f) return;
    const auto chars = GetNumCharsWithinWidth(width);
    if (chars > 0 && chars < static_cast<int>(m_text.size())) m_text.insert(static_cast<std::size_t>(chars), 1, '\n');
    UpdateString();
}

int CSimpleFontString::GetNumCharsWithinWidth(float width) const
{
    if (width <= 0.0f) return 0;
    const float advance = std::max(kDdcPerPixel, m_fontHeight * 0.52f) + m_spacing;
    return std::min(static_cast<int>(m_text.size()), static_cast<int>(width / advance));
}

int CSimpleFontString::GetNumCharsWithinWidthFromEnd(float width) const
{
    return GetNumCharsWithinWidth(width);
}

void CSimpleFontString::SetJustificationOffset(const NTempest::C2Vector& offset)
{
    m_justificationOffset = offset;
}

void CSimpleFontString::Draw(CRenderBatch* batch)
{
    if (batch && IsVisible() && !m_text.empty()) batch->QueueFontString(this);
}

void CSimpleFontString::ClearFromSimpleRegistry()
{
    SimpleFontStringRegistryRemoveEntry(this);
}

void CSimpleFontString::SetLayoutScale(float scale)
{
    CLayoutFrame::SetLayoutScale(scale);
    if (m_fontHandle) {
        TextBlockReleaseFont(m_fontHandle);
        m_fontHandle = TextBlockGenerateFont(m_font.c_str(), static_cast<int>(m_fontFlags),
                                             m_fontHeight * m_layoutScale);
    }
    UpdateString();
}

void CSimpleFontString::OnFrameSizeChanged() { UpdateString(); }

const char* CSimpleFontString::GetName() const
{
    return CSimpleRegion::GetName();
}
