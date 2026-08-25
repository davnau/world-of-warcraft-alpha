#pragma once

#include "ENGINE/Source/Tempest/c2vector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "ENGINE/Source/Tempest/cimvector.h"

#include <memory>
#include <string>

struct CGxStringRenderData;

struct CGxFont { std::string name; unsigned flags = 0; float height = 0.0F; };
struct CGxString {
    CGxFont* font = nullptr; std::string text; NTempest::CImVector color{}; NTempest::C3Vector position{};
    float fontHeight=0,blockWidth=0,blockHeight=0,charSpacing=0,lineSpacing=0; unsigned flags=0;
    bool shadow=false; NTempest::C2Vector shadowOffset{}; NTempest::CImVector shadowColor{}; int gradientStart=0,gradientLength=0;
    std::shared_ptr<CGxStringRenderData> renderData;
};
struct FONTHASHOBJ { unsigned m_refcount=0; std::string key; CGxFont* font=nullptr; };
struct TEXTBLOCK { unsigned m_refcount=0; CGxString* string=nullptr; ~TEXTBLOCK(); };
using HTEXTFONT = FONTHASHOBJ*;
using HTEXTBLOCK = TEXTBLOCK*;

HTEXTFONT TextBlockGenerateFont(const char* fontName, int fontFlags, float fontHeight);
void TextBlockReleaseFont(HTEXTFONT font);
const char* TextBlockGetFontName(HTEXTFONT fontHandle);
int TextBlockGetFontFlags(HTEXTFONT fontHandle);
CGxFont* TextBlockGetFontPtr(HTEXTFONT fontHandle);
CGxString* TextBlockGetStringPtr(HTEXTBLOCK text);
double TextBlockGetOneToOneHeight(HTEXTFONT fontHandle);
void TextBlockAddShadow(HTEXTBLOCK text,const NTempest::C2Vector* shadowOffset,NTempest::CImVector color);
HTEXTBLOCK TextBlockCreate(HTEXTFONT font,const char* text,const NTempest::CImVector* color,const NTempest::C3Vector* pos,float fontHeight,float blockWidth,float blockHeight,unsigned flags,float charSpacing,float lineSpacing);
void TextBlockDestroy(HTEXTBLOCK text);
void TextBlockAnimate(HTEXTBLOCK text,const NTempest::C3Vector* pos);
void TextBlockRender(HTEXTBLOCK text);
void TextBlockUpdateColor(HTEXTBLOCK text,const NTempest::CImVector* textColor);
double TextBlockGetHeight(HTEXTBLOCK text);
void TextBlockGetTextExtent(HTEXTFONT font,const char* text,unsigned numChars,float fontHeight,float* extent,float charSpacing,unsigned flags);
void TextBlockGetWrapPoint(HTEXTFONT font,const char* text,float fontHeight,float blockWidth,unsigned* numBytes,float* extent,const char** nextText,float spacing,unsigned flags);
double TextBlockGetWrappedTextHeight(HTEXTFONT font,const char* text,float fontHeight,float blockWidth,float spacing,unsigned flags);
unsigned TextBlockGetMaxCharsWithinWidth(HTEXTFONT font,const char* text,float height,float maxWidth,unsigned lineBytes,float* extent,float charSpacing,unsigned flags);
unsigned TextBlockGetMaxCharsWithinWidthFromEnd(HTEXTFONT font,const char* text,float height,float maxWidth,unsigned lineBytes,float* extent,float charSpacing,unsigned flags);
unsigned TextBlockWrapText(HTEXTFONT font,const char* text,float height,float maxWidth,unsigned* outputList,unsigned outputListElements,float charSpacing,unsigned flags);
int TextBlockSetGradient(HTEXTBLOCK text,int startChar,int length);
