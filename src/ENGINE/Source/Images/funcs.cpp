#include "ENGINE/Source/Images/images.h"

#include "Storm/h/SMem.h"

#include <algorithm>
#include <cstring>

unsigned GetBitDepth(unsigned format)
{
    switch (format) {
    case 0: return 4;
    case 1:
    case 6:
    case 7: return 8;
    case 2: return 32;
    case 3:
    case 4:
    case 5: return 16;
    default: return 0;
    }
}

unsigned CalcLevelSize(unsigned level, unsigned width, unsigned height, unsigned fourCC)
{
    const unsigned levelWidth = std::max(1u, width >> level);
    const unsigned levelHeight = std::max(1u, height >> level);
    unsigned adjustedWidth = levelWidth;
    unsigned adjustedHeight = levelHeight;
    if (fourCC < 2 || fourCC == 7) {
        adjustedWidth = std::max(4u, adjustedWidth);
        adjustedHeight = std::max(4u, adjustedHeight);
    }
    return (adjustedWidth * adjustedHeight * GetBitDepth(fourCC)) >> 3;
}

int CalcLevelOffset(unsigned levels, unsigned width, unsigned height, unsigned fourCC)
{
    int offset = 0;
    for (unsigned level = 0; level < levels; ++level)
        offset += static_cast<int>(CalcLevelSize(level, width, height, fourCC));
    return offset;
}

unsigned CalcLevelCount(unsigned height, unsigned width)
{
    unsigned result = 1;
    while (height > 1 || width > 1) {
        height = std::max(1u, height >> 1);
        width = std::max(1u, width >> 1);
        ++result;
    }
    return result;
}

MipBits* MippedImgAllocA(unsigned format, unsigned width, unsigned height, const char* fileName, void* lineNumber)
{
    const unsigned levels = CalcLevelCount(width, height);
    const unsigned dataSize = static_cast<unsigned>(CalcLevelOffset(levels, width, height, format));
    auto* result = static_cast<MipBits*>(SMemAlloc(dataSize + sizeof(MipBits) * levels, fileName,
        static_cast<int>(reinterpret_cast<std::uintptr_t>(lineNumber)), 0));
    if (!result)
        return nullptr;

    auto* data = reinterpret_cast<std::uint8_t*>(result + levels);
    unsigned offset = 0;
    for (unsigned level = 0; level < levels; ++level) {
        result[level].mip[0] = reinterpret_cast<C4Pixel*>(data + offset);
        offset += CalcLevelSize(level, width, height, format);
    }
    return result;
}

int MippedImgCalcSize(unsigned format, unsigned width, unsigned height)
{
    const unsigned levels = CalcLevelCount(width, height);
    return CalcLevelOffset(levels, width, height, format) + static_cast<int>(sizeof(MipBits) * levels);
}

void MippedImgSet(unsigned format, unsigned width, unsigned height, MipBits* bits)
{
    if (!bits)
        return;
    const unsigned levels = CalcLevelCount(width, height);
    auto* data = reinterpret_cast<std::uint8_t*>(bits + levels);
    unsigned offset = 0;
    for (unsigned level = 0; level < levels; ++level) {
        bits[level].mip[0] = reinterpret_cast<C4Pixel*>(data + offset);
        offset += CalcLevelSize(level, width, height, format);
    }
}

void FullShrink(C4Pixel* dest, unsigned destWidth, unsigned destHeight, const C4Pixel* source,
    unsigned sourceWidth, unsigned sourceHeight)
{
    if (!dest || !source || destWidth == 0 || destHeight == 0)
        return;

    const unsigned xScale = std::max(1u, sourceWidth / destWidth);
    const unsigned yScale = std::max(1u, sourceHeight / destHeight);
    for (unsigned y = 0; y < destHeight; ++y) {
        for (unsigned x = 0; x < destWidth; ++x) {
            unsigned weightedAlpha = 0;
            unsigned weightedBlue = 0;
            unsigned weightedGreen = 0;
            unsigned weightedRed = 0;
            unsigned blue = 0;
            unsigned green = 0;
            unsigned red = 0;
            const unsigned samples = xScale * yScale;
            for (unsigned sy = 0; sy < yScale; ++sy) {
                const auto* row = source + (y * yScale + sy) * sourceWidth + x * xScale;
                for (unsigned sx = 0; sx < xScale; ++sx) {
                    const C4Pixel& pixel = row[sx];
                    weightedAlpha += pixel.a;
                    weightedBlue += pixel.a * pixel.b;
                    weightedGreen += pixel.a * pixel.g;
                    weightedRed += pixel.a * pixel.r;
                    blue += pixel.b;
                    green += pixel.g;
                    red += pixel.r;
                }
            }
            C4Pixel& output = dest[y * destWidth + x];
            if (weightedAlpha != 0) {
                output.b = static_cast<std::uint8_t>(weightedBlue / weightedAlpha);
                output.g = static_cast<std::uint8_t>(weightedGreen / weightedAlpha);
                output.r = static_cast<std::uint8_t>(weightedRed / weightedAlpha);
                output.a = static_cast<std::uint8_t>(weightedAlpha / samples);
            } else {
                output.b = static_cast<std::uint8_t>(blue / samples);
                output.g = static_cast<std::uint8_t>(green / samples);
                output.r = static_cast<std::uint8_t>(red / samples);
                output.a = 0;
            }
        }
    }
}
