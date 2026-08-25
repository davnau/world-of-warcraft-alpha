#include "ENGINE/Source/Images/images.h"

#include <algorithm>
#include <array>
#include <cstring>

namespace {

struct Color { unsigned r, g, b, a; };

Color Expand565(std::uint16_t value)
{
    return {
        ((value >> 11) & 0x1f) * 255u / 31u,
        ((value >> 5) & 0x3f) * 255u / 63u,
        (value & 0x1f) * 255u / 31u,
        255u
    };
}

std::uint16_t Pack565(const Color& color)
{
    return static_cast<std::uint16_t>(((color.r * 31u / 255u) << 11) |
        ((color.g * 63u / 255u) << 5) | (color.b * 31u / 255u));
}

std::uint16_t Pack1555(const Color& color)
{
    return static_cast<std::uint16_t>(((color.a >= 128u ? 1u : 0u) << 15) |
        ((color.r * 31u / 255u) << 10) | ((color.g * 31u / 255u) << 5) | (color.b * 31u / 255u));
}

std::uint16_t Pack4444(const Color& color)
{
    return static_cast<std::uint16_t>(((color.a * 15u / 255u) << 12) |
        ((color.r * 15u / 255u) << 8) | ((color.g * 15u / 255u) << 4) | (color.b * 15u / 255u));
}

Color ReadPixel(const std::uint8_t* pixel, BlitFormat format)
{
    switch (format) {
    case BlitFormat_Argb8888: return {pixel[2], pixel[1], pixel[0], pixel[3]};
    case BlitFormat_Argb4444: {
        const auto value = *reinterpret_cast<const std::uint16_t*>(pixel);
        return {((value >> 8) & 0xf) * 17u, ((value >> 4) & 0xf) * 17u, (value & 0xf) * 17u, (value >> 12) * 17u};
    }
    case BlitFormat_Argb1555: {
        const auto value = *reinterpret_cast<const std::uint16_t*>(pixel);
        return {((value >> 10) & 0x1f) * 255u / 31u, ((value >> 5) & 0x1f) * 255u / 31u,
            (value & 0x1f) * 255u / 31u, value & 0x8000 ? 255u : 0u};
    }
    case BlitFormat_Rgb565: return Expand565(*reinterpret_cast<const std::uint16_t*>(pixel));
    default: return {};
    }
}

void WritePixel(std::uint8_t* pixel, BlitFormat format, Color color)
{
    switch (format) {
    case BlitFormat_Argb8888:
        pixel[0] = static_cast<std::uint8_t>(color.b); pixel[1] = static_cast<std::uint8_t>(color.g);
        pixel[2] = static_cast<std::uint8_t>(color.r); pixel[3] = static_cast<std::uint8_t>(color.a); break;
    case BlitFormat_Argb4444: *reinterpret_cast<std::uint16_t*>(pixel) = Pack4444(color); break;
    case BlitFormat_Argb1555: *reinterpret_cast<std::uint16_t*>(pixel) = Pack1555(color); break;
    case BlitFormat_Rgb565: *reinterpret_cast<std::uint16_t*>(pixel) = Pack565(color); break;
    default: break;
    }
}

unsigned BytesPerPixel(BlitFormat format)
{
    return format == BlitFormat_Argb8888 ? 4u : (format <= BlitFormat_Rgb565 ? 2u : 0u);
}

void DecodeDxt1(const std::uint8_t* block, C4Pixel* output, unsigned outputStride, bool dxt3)
{
    const auto color0 = static_cast<std::uint16_t>(block[0] | (block[1] << 8));
    const auto color1 = static_cast<std::uint16_t>(block[2] | (block[3] << 8));
    std::array<Color, 4> colors{Expand565(color0), Expand565(color1), {}, {}};
    if (color0 > color1 || dxt3) {
        colors[2] = {(2 * colors[0].r + colors[1].r) / 3, (2 * colors[0].g + colors[1].g) / 3,
            (2 * colors[0].b + colors[1].b) / 3, 255};
        colors[3] = {(colors[0].r + 2 * colors[1].r) / 3, (colors[0].g + 2 * colors[1].g) / 3,
            (colors[0].b + 2 * colors[1].b) / 3, 255};
    } else {
        colors[2] = {(colors[0].r + colors[1].r) / 2, (colors[0].g + colors[1].g) / 2,
            (colors[0].b + colors[1].b) / 2, 255};
        colors[3] = {0, 0, 0, 0};
    }
    const auto indices = static_cast<std::uint32_t>(block[4] | (block[5] << 8) | (block[6] << 16) | (block[7] << 24));
    for (unsigned y = 0; y < 4; ++y) {
        for (unsigned x = 0; x < 4; ++x) {
            const auto& color = colors[(indices >> (2 * (4 * y + x))) & 3];
            auto& pixel = output[y * outputStride + x];
            pixel = {static_cast<std::uint8_t>(color.b), static_cast<std::uint8_t>(color.g),
                static_cast<std::uint8_t>(color.r), static_cast<std::uint8_t>(color.a)};
        }
    }
}

void BlitUncompressed(const NTempest::C2iVector* size, const void* source, unsigned sourceStride,
    BlitFormat sourceFormat, void* destination, unsigned destinationStride, BlitFormat destinationFormat, int alphaMode)
{
    const auto width = static_cast<unsigned>(size->x);
    const auto height = static_cast<unsigned>(size->y);
    const auto sourceBytes = BytesPerPixel(sourceFormat);
    const auto destinationBytes = BytesPerPixel(destinationFormat);
    for (unsigned y = 0; y < height; ++y) {
        const auto* sourceRow = static_cast<const std::uint8_t*>(source) + y * sourceStride;
        auto* destinationRow = static_cast<std::uint8_t*>(destination) + y * destinationStride;
        for (unsigned x = 0; x < width; ++x) {
            Color color = ReadPixel(sourceRow + x * sourceBytes, sourceFormat);
            if (alphaMode == 1) color.a = color.a >= 128 ? 255 : 0;
            if (alphaMode == 2) color.a = color.a == 0 ? 0 : 255;
            WritePixel(destinationRow + x * destinationBytes, destinationFormat, color);
        }
    }
}

void BlitCompressedTo8888(const NTempest::C2iVector* size, const void* source, unsigned sourceStride,
    BlitFormat sourceFormat, void* destination, unsigned destinationStride, int alphaMode)
{
    const auto width = static_cast<unsigned>(size->x);
    const auto height = static_cast<unsigned>(size->y);
    const unsigned blockBytes = sourceFormat == BlitFormat_Dxt1 ? 8u : 16u;
    const unsigned blocksWide = (width + 3) / 4;
    const unsigned blocksHigh = (height + 3) / 4;
    std::array<C4Pixel, 16> pixels{};
    for (unsigned by = 0; by < blocksHigh; ++by) {
        for (unsigned bx = 0; bx < blocksWide; ++bx) {
            const auto* block = static_cast<const std::uint8_t*>(source) + by * sourceStride + bx * blockBytes;
            if (sourceFormat == BlitFormat_Dxt1)
                DecodeDxt1(block, pixels.data(), 4, false);
            else {
                DecodeDxt1(block + (sourceFormat == BlitFormat_Dxt3 ? 8 : 8), pixels.data(), 4, true);
                if (sourceFormat == BlitFormat_Dxt3) {
                    for (unsigned i = 0; i < 16; ++i)
                        pixels[i].a = static_cast<std::uint8_t>(((block[i / 2] >> ((i & 1) * 4)) & 0xf) * 17);
                }
            }
            for (unsigned y = 0; y < 4 && by * 4 + y < height; ++y) {
                auto* row = static_cast<std::uint8_t*>(destination) + (by * 4 + y) * destinationStride;
                for (unsigned x = 0; x < 4 && bx * 4 + x < width; ++x) {
                    Color color{pixels[y * 4 + x].r, pixels[y * 4 + x].g, pixels[y * 4 + x].b, pixels[y * 4 + x].a};
                    if (alphaMode == 1) color.a = color.a >= 128 ? 255 : 0;
                    WritePixel(row + (bx * 4 + x) * 4, BlitFormat_Argb8888, color);
                }
            }
        }
    }
}

}

void InitBlit() {}

void Blit(const NTempest::C2iVector* alpha, int alphaIndex, const void* source, unsigned sourceStride,
    BlitFormat sourceFormat, void* destination, unsigned destinationStride, BlitFormat destinationFormat)
{
    if (!alpha || !source || !destination || alpha->x <= 0 || alpha->y <= 0)
        return;
    if (sourceFormat >= BlitFormat_Dxt1 && destinationFormat == BlitFormat_Argb8888) {
        BlitCompressedTo8888(alpha, source, sourceStride, sourceFormat, destination, destinationStride, alphaIndex);
        return;
    }
    if (sourceFormat >= BlitFormat_Dxt1 || destinationFormat >= BlitFormat_Dxt1) {
        if (sourceFormat == destinationFormat) {
            const unsigned rows = sourceFormat == BlitFormat_Dxt1 ? (alpha->y + 3) / 4 : (alpha->y + 3) / 4;
            std::memcpy(destination, source, destinationStride * rows);
        }
        return;
    }
    BlitUncompressed(alpha, source, sourceStride, sourceFormat, destination, destinationStride, destinationFormat, alphaIndex);
}

#define DEFINE_BLIT(name, source, destination, mode) \
    void name(const NTempest::C2iVector* size, const void* input, unsigned inputStride, void* output, unsigned outputStride) \
    { BlitUncompressed(size, input, inputStride, source, output, outputStride, destination, mode); }

DEFINE_BLIT(Blit_Argb8888_Argb4444, BlitFormat_Argb8888, BlitFormat_Argb4444, 0)
DEFINE_BLIT(Blit_Argb8888_Argb1555, BlitFormat_Argb8888, BlitFormat_Argb1555, 0)
DEFINE_BLIT(Blit_Argb8888_Rgb565, BlitFormat_Argb8888, BlitFormat_Rgb565, 0)
DEFINE_BLIT(Blit_Argb8888_Argb8888, BlitFormat_Argb8888, BlitFormat_Argb8888, 0)
DEFINE_BLIT(Blit_Argb8888_Argb8888_A1, BlitFormat_Argb8888, BlitFormat_Argb8888, 1)
DEFINE_BLIT(Blit_Argb8888_Argb8888_A8, BlitFormat_Argb8888, BlitFormat_Argb8888, 2)
DEFINE_BLIT(Blit_uint16_uint16, BlitFormat_Rgb565, BlitFormat_Rgb565, 0)
DEFINE_BLIT(Blit_Dxt1_Rgb565, BlitFormat_Dxt1, BlitFormat_Rgb565, 0)
DEFINE_BLIT(Blit_Dxt1_Argb1555, BlitFormat_Dxt1, BlitFormat_Argb1555, 0)
DEFINE_BLIT(Blit_Dxt1_Argb8888, BlitFormat_Dxt1, BlitFormat_Argb8888, 0)
DEFINE_BLIT(Blit_Dxt3_Argb4444, BlitFormat_Dxt3, BlitFormat_Argb4444, 0)
DEFINE_BLIT(Blit_Dxt3_Argb8888, BlitFormat_Dxt3, BlitFormat_Argb8888, 0)

void Blit_Dxt1_Dxt1(const NTempest::C2iVector* size, const void* input, unsigned inputStride, void* output, unsigned outputStride)
{
    for (unsigned y = 0; y < static_cast<unsigned>(size->y + 3) / 4; ++y)
        std::memcpy(static_cast<std::uint8_t*>(output) + y * outputStride,
            static_cast<const std::uint8_t*>(input) + y * inputStride, static_cast<unsigned>(size->x + 3) / 4 * 8);
}

void Blit_Dxt35_Dxt35(const NTempest::C2iVector* size, const void* input, unsigned inputStride, void* output, unsigned outputStride)
{
    for (unsigned y = 0; y < static_cast<unsigned>(size->y + 3) / 4; ++y)
        std::memcpy(static_cast<std::uint8_t*>(output) + y * outputStride,
            static_cast<const std::uint8_t*>(input) + y * inputStride, static_cast<unsigned>(size->x + 3) / 4 * 16);
}

int Dxt3A4(unsigned value) { return static_cast<int>((value & 0xf) * 17); }
int Dxt3A8(unsigned value) { return static_cast<int>(value & 0xff); }
unsigned CalcRowStride(int width, unsigned bytesPerPixel) { return static_cast<unsigned>(width) * bytesPerPixel; }

