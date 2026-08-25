#pragma once

#include "ENGINE/Source/Tempest/c2ivector.h"
#include "ENGINE/Source/Tempest/cimvector.h"
#include "ENGINE/Source/Tempest/c3vector.h"
#include "Storm/h/storm.h"

#include <cstddef>
#include <cstdint>

struct C4Pixel {
    std::uint8_t b;
    std::uint8_t g;
    std::uint8_t r;
    std::uint8_t a;
};
static_assert(sizeof(C4Pixel) == 4);

using BlpPalPixel = C4Pixel;

struct MipBits {
    C4Pixel* mip[1];
};

#ifndef SCRN_USE_GX_BLIT_FORMAT
enum BlitFormat : std::uint32_t {
    BlitFormat_Unknown,
    BlitFormat_Argb8888,
    BlitFormat_Argb4444,
    BlitFormat_Argb1555,
    BlitFormat_Rgb565,
    BlitFormat_Dxt1,
    BlitFormat_Dxt3,
    BlitFormat_Dxt5,
    BlitFormats_Last
};
#endif

enum PIXEL_FORMAT : std::uint32_t {
    PIXEL_DXT1,
    PIXEL_DXT3,
    PIXEL_ARGB8888,
    PIXEL_ARGB1555,
    PIXEL_ARGB4444,
    PIXEL_RGB565,
    PIXEL_A8,
    PIXEL_DXT5,
    PIXEL_UNSPECIFIED,
    NUM_PIXEL_FORMATS
};

enum MIPS_TYPE : std::uint32_t { MIPS_NONE, MIPS_GENERATED, MIPS_HANDMADE };
enum MipMapAlgorithm : std::uint32_t { MMA_BOX, MMA_CUBIC, MMA_FULLDFT, MMA_KAISER, MMA_LINEARLIGHTKAISER };

#pragma pack(push, 1)
struct TGAHeader {
    std::uint8_t bIDLength;
    std::uint8_t bColorMapType;
    std::uint8_t bImageType;
    std::uint16_t wColorMapStartIndex;
    std::uint16_t wColorMapEntries;
    std::uint8_t bColorMapEntrySize;
    std::uint16_t wXOrigin;
    std::uint16_t wYOrigin;
    std::uint16_t wWidth;
    std::uint16_t wHeight;
    std::uint8_t bPixelDepth;
    union {
        std::uint8_t bImageDescriptor;
        struct {
            std::uint8_t bAlphaChannelBits : 4;
            std::uint8_t bLeftRightOrder : 1;
            std::uint8_t bTopBottomOrder : 1;
            std::uint8_t bReserved : 2;
        } bits;
    };
};

struct TGAFooter {
    std::uint32_t dwExtensionOffset;
    std::uint32_t dwDeveloperOffset;
    char szSigniture[18];
};
#pragma pack(pop)

struct TGA32Pixel { std::uint8_t b, g, r, a; };

struct DxtRect { std::uint32_t l, t, r, b, w, h; };
struct DxtBlock { std::uint8_t data[8]; };
struct Dxt1Block : DxtBlock {};
struct Dxt3AlphaBlock { std::uint64_t alpha; };
struct Dxt3Block : Dxt3AlphaBlock { std::uint16_t color0, color1; std::uint8_t row[4]; };

namespace NTempest {
struct CRgb565 {
    std::uint16_t value = 0;
    CRgb565() = default;
    explicit CRgb565(std::uint16_t v) : value(v) {}
    operator std::uint16_t() const { return value; }
};
struct CArgb1555 {
    std::uint16_t value = 0;
    CArgb1555() = default;
    explicit CArgb1555(std::uint16_t v) : value(v) {}
    operator std::uint16_t() const { return value; }
    CArgb1555& operator=(std::uint16_t v) { value = v; return *this; }
};
struct CArgb4444 {
    std::uint16_t value = 0;
    CArgb4444() = default;
    explicit CArgb4444(std::uint16_t v) : value(v) {}
    operator std::uint16_t() const { return value; }
};
}

struct DxtColorBlock : DxtBlock {
    NTempest::CRgb565 color0;
    NTempest::CRgb565 color1;
    std::uint8_t row[4]{};
    struct Tables {
        std::uint16_t dt135[32]{};
        std::uint16_t dt235[32]{};
        std::uint16_t dt136[64]{};
        std::uint16_t dt236[64]{};
        Tables();
    };
    static Tables tables;
};

struct CTgaFile {
    SFile* m_file = nullptr;
    std::uint8_t* m_image = nullptr;
    TGAHeader m_header{};
    std::uint8_t* m_addlHeaderData = nullptr;
    TGAFooter m_footer{};
    std::uint32_t m_imageBytes = 0;
    std::uint8_t* m_colorMap = nullptr;

    void Close();
    unsigned Open(const char* filename);
    int ValidateColorDepth();
    void ConvertColorMapped(char flags);
    int ReadColorMappedImage(char flags);
    int LoadImageData(unsigned flags);
    int PreImageBytes();
    std::uint8_t* AddAlphaChannel(std::uint8_t* image);
    void AddAlphaChannel(std::uint8_t* alphaData, std::uint8_t* noAlphaData, std::uint8_t* alpha);
    unsigned ReadRawImage(char flags);
    unsigned ReadRleImage(char flags);
    int RLEDecompressImage(std::uint8_t* rleData, std::uint8_t* data);
    int SetTopDown(int set);
    std::uint8_t* Image();
    const std::uint8_t* Image() const;
    TGA32Pixel* ImageTGA32Pixel();
    const TGA32Pixel* ImageTGA32Pixel() const;
    int RemoveAlphaChannels();
    void RemoveHeaderTrailer();
    std::uint8_t* SetImage(const CTgaFile* source);
    int SetImage(const void* image, std::uint16_t width, std::uint16_t height,
        std::uint8_t pixelDepth, std::uint8_t alphaBits, int topDown, int rightToLeft);
    int SetImage(const void* image, std::uint16_t width, std::uint16_t height, std::uint8_t pixelDepth)
    {
        return SetImage(image, width, height, pixelDepth, 0, 0, 0);
    }
    int CountRun(std::uint8_t* image, int max);
    int RleCompressLine(std::uint8_t** uncompressed, std::uint8_t** compressed);
    int Compress();
    int Write(const char* path);
};

unsigned GetBitDepth(unsigned format);
unsigned CalcLevelSize(unsigned level, unsigned width, unsigned height, unsigned fourCC);
int CalcLevelOffset(unsigned levels, unsigned width, unsigned height, unsigned fourCC);
unsigned CalcLevelCount(unsigned height, unsigned width);
MipBits* MippedImgAllocA(unsigned format, unsigned width, unsigned height, const char* fileName, void* lineNumber);
int MippedImgCalcSize(unsigned format, unsigned width, unsigned height);
void MippedImgSet(unsigned format, unsigned width, unsigned height, MipBits* bits);
void FullShrink(C4Pixel* dest, unsigned destWidth, unsigned destHeight, const C4Pixel* source,
    unsigned sourceWidth, unsigned sourceHeight);

void Blit(const NTempest::C2iVector* alpha, int alphaIndex, const void* source, unsigned sourceStride,
    BlitFormat sourceFormat, void* dest, unsigned destStride, BlitFormat destFormat);
void InitBlit();
unsigned CalcRowStride(int width, unsigned bytesPerPixel);
