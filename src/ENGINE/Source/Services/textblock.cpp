#include "ENGINE/Source/Services/textblock.h"

#include "ENGINE/Source/Gx/gxapi/gxapi.h"
#include "Storm/h/storm.h"

#include <GL/gl.h>

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstring>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string_view>
#include <unordered_map>
#include <vector>

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

struct CGxStringRenderData {
    struct Line {
        CGxDevice* device = nullptr;
        CGxTex* texture = nullptr;
        std::vector<std::uint8_t> pixels;
        unsigned width = 0;
        unsigned height = 0;
        float ascent = 0.0F;
        float extent = 0.0F;

        ~Line();
    };

    CGxDevice* device = nullptr;
    std::vector<std::unique_ptr<Line>> lines;
};

namespace {
constexpr float kDdcPerPixel = 0.0009765625F * 0.80000001F;

struct PrivateFontResource {
    std::string name;
    std::vector<unsigned char> bytes;
    HANDLE handle = nullptr;
};

struct GlFontLists {
    HGLRC context = nullptr;
    std::string key;
    GLuint base = 0;
};

std::mutex s_fontLock;
std::unordered_map<std::string, std::unique_ptr<FONTHASHOBJ>> s_fontHash;
std::vector<PrivateFontResource> s_privateFonts;
std::vector<GlFontLists> s_glFonts;

std::string Lower(std::string value)
{
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    return value;
}

const char* FontFamily(const std::string& filename)
{
    const std::string lower = Lower(filename);
    if (lower.find("frizqt__") != std::string::npos) return "Friz Quadrata TT";
    if (lower.find("arialn") != std::string::npos) return "Arial Narrow";
    if (lower.find("skurri") != std::string::npos) return "Skurri";
    if (lower.find("morpheus") != std::string::npos) return "Morpheus";
    return "Arial";
}

void EnsureFontResource(const char* filename)
{
    if (!filename || !*filename) return;
    const std::string key = Lower(filename);
    if (std::any_of(s_privateFonts.begin(), s_privateFonts.end(),
                    [&](const PrivateFontResource& item) { return item.name == key; }))
        return;

    PrivateFontResource resource;
    resource.name = key;
    void* data = nullptr;
    unsigned bytes = 0;
    if (SFile::LoadFile(filename, &data, &bytes, 0, nullptr) && data && bytes) {
        resource.bytes.assign(static_cast<const unsigned char*>(data),
                              static_cast<const unsigned char*>(data) + bytes);
        SFile::Unload(data);
        DWORD fonts = 0;
        resource.handle = AddFontMemResourceEx(resource.bytes.data(),
                                               static_cast<DWORD>(resource.bytes.size()),
                                               nullptr, &fonts);
    }
    s_privateFonts.push_back(std::move(resource));
}

int PixelHeight(float height)
{
    return std::max(1, static_cast<int>(std::lround(height / kDdcPerPixel)));
}

HFONT CreateGdiFont(const CGxFont& font, float height)
{
    const int pixels = PixelHeight(height);
    return CreateFontA(-pixels, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE,
                       ANSI_CHARSET, OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS,
                       (font.flags & 8U) ? NONANTIALIASED_QUALITY : ANTIALIASED_QUALITY,
                       DEFAULT_PITCH | FF_DONTCARE, FontFamily(font.name));
}

float MeasureTextPixels(const CGxFont& font, const char* text, unsigned bytes, float height)
{
    if (!text || bytes == 0) return 0.0F;
    HDC dc = GetDC(nullptr);
    HFONT gdiFont = CreateGdiFont(font, height);
    HGDIOBJ previous = gdiFont ? SelectObject(dc, gdiFont) : nullptr;
    SIZE extent{};
    const BOOL measured = GetTextExtentPoint32A(dc, text, static_cast<int>(bytes), &extent);
    if (previous) SelectObject(dc, previous);
    if (gdiFont) DeleteObject(gdiFont);
    ReleaseDC(nullptr, dc);
    return measured ? static_cast<float>(extent.cx) : bytes * PixelHeight(height) * 0.52F;
}

GLuint GetGlFontLists(const CGxFont& font, float height)
{
    const HGLRC context = wglGetCurrentContext();
    if (!context) return 0;
    const std::string key = Lower(font.name) + ':' + std::to_string(font.flags) + ':' +
                            std::to_string(PixelHeight(height));
    for (const GlFontLists& item : s_glFonts)
        if (item.context == context && item.key == key) return item.base;

    HDC dc = wglGetCurrentDC();
    if (!dc) return 0;
    HFONT gdiFont = CreateGdiFont(font, height);
    if (!gdiFont) return 0;
    HGDIOBJ previous = SelectObject(dc, gdiFont);
    const GLuint base = glGenLists(256);
    const BOOL created = base != 0 && wglUseFontBitmapsA(dc, 0, 256, base);
    SelectObject(dc, previous);
    DeleteObject(gdiFont);
    if (!created) {
        if (base) glDeleteLists(base, 256);
        return 0;
    }
    s_glFonts.push_back({context, key, base});
    return base;
}

void DrawGlyphLine(GLuint base, const char* text, unsigned bytes, float x, float y,
                   const NTempest::CImVector& color)
{
    if (!base || !text || !bytes) return;
    glColor4ub(color.r, color.g, color.b, color.a);
    glRasterPos3f(x, y, 0.0F);
    glListBase(base);
    glCallLists(static_cast<GLsizei>(bytes), GL_UNSIGNED_BYTE, text);
}

void DrawStyledLine(const CGxString& string, GLuint base, const char* text, unsigned bytes,
                    float x, float y)
{
    const unsigned outline = string.font ? string.font->flags & 5U : 0U;
    if (outline) {
        NTempest::CImVector outlineColor{0xff000000U};
        outlineColor.a = string.color.a;
        const int radius = (outline & 4U) ? 2 : 1;
        for (int oy = -radius; oy <= radius; ++oy) {
            for (int ox = -radius; ox <= radius; ++ox) {
                if (ox == 0 && oy == 0) continue;
                DrawGlyphLine(base, text, bytes, x + ox * kDdcPerPixel,
                              y + oy * kDdcPerPixel, outlineColor);
            }
        }
    }
    if (string.shadow)
        DrawGlyphLine(base, text, bytes, x + string.shadowOffset.x,
                      y + string.shadowOffset.y, string.shadowColor);
    DrawGlyphLine(base, text, bytes, x, y, string.color);
}

void UpdateTextLineTexture(EGxTexCommand operation, unsigned, unsigned, unsigned,
                           unsigned mipLevel, void* user, unsigned* stride,
                           const void** texels)
{
    auto* line = static_cast<CGxStringRenderData::Line*>(user);
    if ((operation != GxTex_Lock && operation != GxTex_Latch) || !line || mipLevel != 0)
        return;
    if (stride) *stride = line->width * 4U;
    if (texels) *texels = line->pixels.data();
}

std::unique_ptr<CGxStringRenderData::Line> RasterizeTextLine(
    const CGxString& string, std::string_view text)
{
    auto line = std::make_unique<CGxStringRenderData::Line>();
    line->device = g_theGxDevicePtr;
    if (text.empty() || !string.font || !line->device) return line;

    HDC dc = CreateCompatibleDC(nullptr);
    HFONT gdiFont = CreateGdiFont(*string.font, string.fontHeight);
    if (!dc || !gdiFont) {
        if (gdiFont) DeleteObject(gdiFont);
        if (dc) DeleteDC(dc);
        return line;
    }

    HGDIOBJ previousFont = SelectObject(dc, gdiFont);
    const int characterExtra = static_cast<int>(std::lround(string.charSpacing / kDdcPerPixel));
    SetTextCharacterExtra(dc, characterExtra);
    SIZE extent{};
    TEXTMETRICA metrics{};
    GetTextExtentPoint32A(dc, text.data(), static_cast<int>(text.size()), &extent);
    GetTextMetricsA(dc, &metrics);
    line->width = std::max(1L, extent.cx);
    line->height = std::max(1L, metrics.tmHeight);
    line->ascent = static_cast<float>(metrics.tmAscent) * kDdcPerPixel;
    line->extent = static_cast<float>(extent.cx) * kDdcPerPixel;

    BITMAPINFO bitmapInfo{};
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = static_cast<LONG>(line->width);
    bitmapInfo.bmiHeader.biHeight = -static_cast<LONG>(line->height);
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 32;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    void* dibPixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(dc, &bitmapInfo, DIB_RGB_COLORS, &dibPixels, nullptr, 0);
    if (bitmap && dibPixels) {
        HGDIOBJ previousBitmap = SelectObject(dc, bitmap);
        std::memset(dibPixels, 0, static_cast<std::size_t>(line->width) * line->height * 4U);
        SetBkMode(dc, TRANSPARENT);
        SetTextColor(dc, RGB(255, 255, 255));
        SetTextAlign(dc, TA_LEFT | TA_TOP | TA_NOUPDATECP);
        TextOutA(dc, 0, 0, text.data(), static_cast<int>(text.size()));
        GdiFlush();

        const auto* source = static_cast<const std::uint8_t*>(dibPixels);
        line->pixels.resize(static_cast<std::size_t>(line->width) * line->height * 4U);
        for (std::size_t pixel = 0; pixel < line->pixels.size(); pixel += 4) {
            const std::uint8_t coverage = std::max({source[pixel], source[pixel + 1],
                                                    source[pixel + 2]});
            line->pixels[pixel] = 255;
            line->pixels[pixel + 1] = 255;
            line->pixels[pixel + 2] = 255;
            line->pixels[pixel + 3] = coverage;
        }
        SelectObject(dc, previousBitmap);
    }

    SelectObject(dc, previousFont);
    if (bitmap) DeleteObject(bitmap);
    DeleteObject(gdiFont);
    DeleteDC(dc);

    if (!line->pixels.empty()
        && GxTexCreate(line->width, line->height, GxTex_Argb8888,
                       CGxTexFlags(GxTex_Linear, false, false, false, false, false, 1),
                       line.get(), UpdateTextLineTexture, &line->texture)) {
        GxTexUpdate(line->texture, nullptr, true);
    }
    return line;
}

std::shared_ptr<CGxStringRenderData> BuildStringRenderData(const CGxString& string)
{
    auto data = std::make_shared<CGxStringRenderData>();
    data->device = g_theGxDevicePtr;
    std::string_view remaining = string.text;
    for (;;) {
        const std::size_t newline = remaining.find('\n');
        data->lines.push_back(RasterizeTextLine(string, remaining.substr(0, newline)));
        if (newline == std::string_view::npos) break;
        remaining.remove_prefix(newline + 1);
    }
    return data;
}

void DrawTexturedLine(const CGxStringRenderData::Line& line, float x, float baseline,
                      const NTempest::CImVector& color)
{
    if (!line.texture || !line.width || !line.height || color.a == 0) return;
    const float top = baseline + line.ascent;
    const float bottom = top - static_cast<float>(line.height) * kDdcPerPixel;
    const float right = x + line.extent;
    const std::array<NTempest::C3Vector, 4> positions{{
        {x, bottom, 0.0F}, {x, top, 0.0F},
        {right, bottom, 0.0F}, {right, top, 0.0F}}};
    static constexpr std::array<NTempest::C2Vector, 4> textureCoordinates{{
        {0.0F, 1.0F}, {0.0F, 0.0F}, {1.0F, 1.0F}, {1.0F, 0.0F}}};
    static constexpr std::uint16_t indices[] = {0, 1, 2, 3};
    static const NTempest::C3Vector normal{0.0F, 0.0F, 1.0F};

    GxRsSet(GxRs_Texture0, line.texture);
    GxPrimLockVertexPtrs(4, positions.data(), sizeof(NTempest::C3Vector),
                         &normal, 0, &color, 0, nullptr, 0,
                         textureCoordinates.data(), sizeof(NTempest::C2Vector), nullptr, 0);
    GxPrimDrawElements(GxPrim_TriangleStrip, 4, indices);
    GxPrimUnlockVertexPtrs();
}

void DrawStyledTexturedLine(const CGxString& string,
                            const CGxStringRenderData::Line& line, float x, float baseline)
{
    const unsigned outline = string.font ? string.font->flags & 5U : 0U;
    if (outline) {
        NTempest::CImVector outlineColor{0xff000000U};
        outlineColor.a = string.color.a;
        const int radius = (outline & 4U) ? 2 : 1;
        for (int oy = -radius; oy <= radius; ++oy) {
            for (int ox = -radius; ox <= radius; ++ox) {
                if (ox == 0 && oy == 0) continue;
                DrawTexturedLine(line, x + ox * kDdcPerPixel,
                                 baseline + oy * kDdcPerPixel, outlineColor);
            }
        }
    }
    if (string.shadow)
        DrawTexturedLine(line, x + string.shadowOffset.x,
                         baseline + string.shadowOffset.y, string.shadowColor);
    DrawTexturedLine(line, x, baseline, string.color);
}

unsigned AvailableBytes(const char* text, unsigned bytes)
{
    if (!text) return 0;
    const unsigned length = static_cast<unsigned>(std::strlen(text));
    return bytes ? std::min(bytes, length) : length;
}

float GlyphWidth(float height, float spacing)
{
    return std::max(0.0F, height * 0.52F + spacing);
}

void RequireFont(HTEXTFONT font)
{
    if (!font || !font->font) throw std::invalid_argument("font handle");
}

unsigned Fit(const char* text, unsigned bytes, float height, float width, float spacing,
             float* extent)
{
    const unsigned available = AvailableBytes(text, bytes);
    const float glyph = GlyphWidth(height, spacing);
    const unsigned count = glyph > 0.0F
        ? std::min(available, static_cast<unsigned>(std::max(0.0F, std::floor(width / glyph))))
        : available;
    if (extent) *extent = count * glyph;
    return count;
}
} // namespace

CGxStringRenderData::Line::~Line()
{
    if (!texture || !device || g_theGxDevicePtr != device) return;
    const auto found = std::find(device->m_textures.begin(), device->m_textures.end(), texture);
    if (found != device->m_textures.end()) GxTexDestroy(texture);
}

TEXTBLOCK::~TEXTBLOCK() { delete string; }

HTEXTFONT TextBlockGenerateFont(const char* name, int flags, float height)
{
    if (!name || !*name || height <= 0.0F) return nullptr;
    std::lock_guard lock(s_fontLock);
    EnsureFontResource(name);
    const std::string key = Lower(name) + '-' + std::to_string(flags) + '-' + std::to_string(height);
    auto& item = s_fontHash[key];
    if (!item) {
        item = std::make_unique<FONTHASHOBJ>();
        item->key = key;
        item->font = new CGxFont{name, static_cast<unsigned>(flags), height};
    }
    ++item->m_refcount;
    return item.get();
}

void TextBlockReleaseFont(HTEXTFONT font)
{
    if (!font) return;
    std::lock_guard lock(s_fontLock);
    if (font->m_refcount) --font->m_refcount;
    if (font->m_refcount) return;
    const std::string key = font->key;
    delete font->font;
    font->font = nullptr;
    s_fontHash.erase(key);
}

const char* TextBlockGetFontName(HTEXTFONT font)
{
    RequireFont(font);
    return font->font->name.c_str();
}

int TextBlockGetFontFlags(HTEXTFONT font)
{
    RequireFont(font);
    return static_cast<int>(font->font->flags);
}

CGxFont* TextBlockGetFontPtr(HTEXTFONT font)
{
    RequireFont(font);
    return font->font;
}

CGxString* TextBlockGetStringPtr(HTEXTBLOCK text) { return text ? text->string : nullptr; }

double TextBlockGetOneToOneHeight(HTEXTFONT font)
{
    RequireFont(font);
    return font->font->height;
}

void TextBlockAddShadow(HTEXTBLOCK text, const NTempest::C2Vector* offset,
                        NTempest::CImVector color)
{
    if (!text || !text->string || !offset) return;
    text->string->shadow = true;
    text->string->shadowOffset = *offset;
    text->string->shadowColor = color;
}

HTEXTBLOCK TextBlockCreate(HTEXTFONT font, const char* text,
                           const NTempest::CImVector* color,
                           const NTempest::C3Vector* position, float fontHeight,
                           float blockWidth, float blockHeight, unsigned flags,
                           float charSpacing, float lineSpacing)
{
    if (!font || !font->font || !text || !color || !position) return nullptr;
    auto* result = new TEXTBLOCK;
    result->string = new CGxString{font->font, text, *color, *position, fontHeight,
                                   blockWidth, blockHeight, charSpacing, lineSpacing, flags};
    return result;
}

void TextBlockDestroy(HTEXTBLOCK text) { delete text; }

void TextBlockAnimate(HTEXTBLOCK text, const NTempest::C3Vector* position)
{
    if (text && text->string && position) text->string->position = *position;
}

void TextBlockRender(HTEXTBLOCK text)
{
    if (!text || !text->string || !text->string->font || text->string->text.empty()) return;

    CGxString& string = *text->string;
    if (GxDevApi() != GxApi_OpenGl || !wglGetCurrentContext()) {
        if (!string.renderData || string.renderData->device != g_theGxDevicePtr)
            string.renderData = BuildStringRenderData(string);
        if (!string.renderData) return;

        const float lineHeight = string.fontHeight;
        const float totalHeight = string.renderData->lines.size() * lineHeight
            + (string.renderData->lines.size() > 1
                   ? (string.renderData->lines.size() - 1) * string.lineSpacing : 0.0F);
        float bottom = string.position.y;
        if ((string.flags & 0x38U) == 8U)
            bottom += string.blockHeight - totalHeight;
        else if ((string.flags & 0x38U) == 16U)
            bottom += (string.blockHeight - totalHeight) * 0.5F;

        GxVertexShaderSelect(GxVS_PassThru);
        GxRsPush();
        GxRsSet(GxRs_TexBlend0, static_cast<int>(GxTexBlend_Mod));
        GxRsSet(GxRs_Blend, GxBlend_Alpha);
        GxRsSet(GxRs_MatDiffuse, NTempest::CImVector{0xffffffffU});
        GxRsSet(GxRs_Lighting, 0);
        GxRsSet(GxRs_Fog, 0);
        GxRsSet(GxRs_DepthTest, 0);
        GxRsSet(GxRs_DepthWrite, 0);
        GxRsSet(GxRs_Culling, 0);
        GxRsSet(GxRs_AlphaRef, 0);
        for (std::size_t index = 0; index < string.renderData->lines.size(); ++index) {
            const auto& line = *string.renderData->lines[index];
            float x = string.position.x;
            if ((string.flags & 7U) == 2U) x += (string.blockWidth - line.extent) * 0.5F;
            else if ((string.flags & 7U) == 4U) x += string.blockWidth - line.extent;
            const float baseline = bottom + totalHeight - (index + 1) * lineHeight
                - index * string.lineSpacing + lineHeight * 0.20F;
            DrawStyledTexturedLine(string, line, x, baseline);
        }
        GxRsSet(GxRs_Texture0, static_cast<CGxTex*>(nullptr));
        GxRsPop();
        return;
    }

    const GLuint base = GetGlFontLists(*string.font, string.fontHeight);
    if (!base) return;

    std::vector<std::string_view> lines;
    std::string_view remaining = string.text;
    for (;;) {
        const std::size_t newline = remaining.find('\n');
        lines.push_back(remaining.substr(0, newline));
        if (newline == std::string_view::npos) break;
        remaining.remove_prefix(newline + 1);
    }

    const float lineHeight = string.fontHeight;
    const float totalHeight = lines.size() * lineHeight +
                              (lines.size() > 1 ? (lines.size() - 1) * string.lineSpacing : 0.0F);
    float bottom = string.position.y;
    if ((string.flags & 0x38U) == 8U)
        bottom += string.blockHeight - totalHeight;
    else if ((string.flags & 0x38U) == 16U)
        bottom += (string.blockHeight - totalHeight) * 0.5F;

    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_CURRENT_BIT |
                 GL_DEPTH_BUFFER_BIT | GL_LIST_BIT | GL_TEXTURE_BIT);
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glDisable(GL_ALPHA_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    for (std::size_t index = 0; index < lines.size(); ++index) {
        const std::string_view line = lines[index];
        const float width = MeasureTextPixels(*string.font, line.data(),
                                              static_cast<unsigned>(line.size()),
                                              string.fontHeight) * kDdcPerPixel +
                            (line.empty() ? 0.0F : (line.size() - 1) * string.charSpacing);
        float x = string.position.x;
        if ((string.flags & 7U) == 2U) x += (string.blockWidth - width) * 0.5F;
        else if ((string.flags & 7U) == 4U) x += string.blockWidth - width;
        const float y = bottom + totalHeight - (index + 1) * lineHeight -
                        index * string.lineSpacing + lineHeight * 0.20F;
        DrawStyledLine(string, base, line.data(), static_cast<unsigned>(line.size()), x, y);
    }
    glPopAttrib();
}

void TextBlockUpdateColor(HTEXTBLOCK text, const NTempest::CImVector* color)
{
    if (text && text->string && color) text->string->color = *color;
}

double TextBlockGetHeight(HTEXTBLOCK text)
{
    if (!text || !text->string) return 0.0;
    return TextBlockGetWrappedTextHeight(nullptr, text->string->text.c_str(),
                                         text->string->fontHeight, text->string->blockWidth,
                                         text->string->lineSpacing, text->string->flags);
}

void TextBlockGetTextExtent(HTEXTFONT font, const char* text, unsigned count,
                            float height, float* extent, float spacing, unsigned)
{
    RequireFont(font);
    if (!extent) return;
    const unsigned bytes = AvailableBytes(text, count);
    *extent = MeasureTextPixels(*font->font, text, bytes, height) * kDdcPerPixel +
              (bytes > 1 ? (bytes - 1) * spacing : 0.0F);
}

void TextBlockGetWrapPoint(HTEXTFONT font, const char* text, float height, float width,
                           unsigned* bytes, float* extent, const char** next,
                           float spacing, unsigned)
{
    RequireFont(font);
    unsigned count = Fit(text, 0, height, width, spacing, extent);
    while (count && text[count] && !std::isspace(static_cast<unsigned char>(text[count]))) --count;
    if (!count) count = Fit(text, 0, height, width, spacing, extent);
    if (bytes) *bytes = count;
    if (next) *next = text ? text + count : nullptr;
}

double TextBlockGetWrappedTextHeight(HTEXTFONT font, const char* text, float height,
                                     float width, float spacing, unsigned)
{
    if (font) RequireFont(font);
    if (!text || !*text) return 0.0;
    const float glyph = GlyphWidth(height, 0.0F);
    const unsigned perLine = width > 0.0F && glyph > 0.0F
        ? std::max(1U, static_cast<unsigned>(width / glyph))
        : static_cast<unsigned>(std::strlen(text));
    const unsigned lines = std::max(1U,
        (static_cast<unsigned>(std::strlen(text)) + perLine - 1) / perLine);
    return lines * height + (lines - 1) * spacing;
}

unsigned TextBlockGetMaxCharsWithinWidth(HTEXTFONT font, const char* text, float height,
                                         float width, unsigned bytes, float* extent,
                                         float spacing, unsigned)
{
    RequireFont(font);
    if (!text) return 0;
    const unsigned available = AvailableBytes(text, bytes);
    unsigned count = 0;
    float measured = 0.0F;
    for (; count < available; ++count) {
        float candidate = 0.0F;
        TextBlockGetTextExtent(font, text, count + 1, height, &candidate, spacing, 0);
        if (candidate > width) break;
        measured = candidate;
    }
    if (extent) *extent = measured;
    return count;
}

unsigned TextBlockGetMaxCharsWithinWidthFromEnd(HTEXTFONT font, const char* text,
                                                float height, float width, unsigned bytes,
                                                float* extent, float spacing, unsigned flags)
{
    RequireFont(font);
    const unsigned available = AvailableBytes(text, bytes);
    if (!available) return 0;
    unsigned count = 0;
    float measured = 0.0F;
    while (count < available) {
        const char* begin = text + available - count - 1;
        float candidate = 0.0F;
        TextBlockGetTextExtent(font, begin, count + 1, height, &candidate, spacing, flags);
        if (candidate > width) break;
        measured = candidate;
        ++count;
    }
    if (extent) *extent = measured;
    return count;
}

unsigned TextBlockWrapText(HTEXTFONT font, const char* text, float height, float width,
                           unsigned* output, unsigned capacity, float spacing, unsigned flags)
{
    RequireFont(font);
    if (!text || !output || !capacity) return 0;
    unsigned offset = 0;
    unsigned lines = 0;
    const unsigned total = static_cast<unsigned>(std::strlen(text));
    while (offset < total && lines < capacity) {
        const unsigned count = TextBlockGetMaxCharsWithinWidth(
            font, text + offset, height, width, 0, nullptr, spacing, flags);
        if (!count) break;
        output[lines++] = count;
        offset += count;
        while (offset < total && std::isspace(static_cast<unsigned char>(text[offset]))) ++offset;
    }
    return lines;
}

int TextBlockSetGradient(HTEXTBLOCK text, int start, int length)
{
    if (!text || !text->string) return 0;
    text->string->gradientStart = start;
    text->string->gradientLength = length;
    return 1;
}
