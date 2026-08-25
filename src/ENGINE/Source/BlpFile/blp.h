#pragma once

#include <cstdint>
#include <vector>

class CStatus;
struct SFile;
struct HCOLORMAP__;

struct C4Pixel { std::uint8_t b,g,r,a; };
using BlpPalPixel = C4Pixel;
struct MipBits { C4Pixel* mip[1]; };

enum BlitFormat { BlitFormat_Unknown,BlitFormat_Argb8888,BlitFormat_Argb4444,BlitFormat_Argb1555,BlitFormat_Rgb565,BlitFormat_Dxt1,BlitFormat_Dxt3,BlitFormat_Dxt5,BlitFormats_Last };
enum PIXEL_FORMAT { PIXEL_DXT1,PIXEL_DXT3,PIXEL_ARGB8888,PIXEL_ARGB1555,PIXEL_ARGB4444,PIXEL_RGB565,PIXEL_A8,PIXEL_DXT5,PIXEL_UNSPECIFIED,NUM_PIXEL_FORMATS };
enum MIPS_TYPE { MIPS_NONE,MIPS_GENERATED,MIPS_HANDMADE };
enum MipMapAlgorithm { MMA_BOX,MMA_CUBIC,MMA_FULLDFT,MMA_KAISER,MMA_LINEARLIGHTKAISER };

#pragma pack(push,1)
struct BLPHeader {
    std::uint32_t magic;
    std::uint32_t formatVersion;
    std::uint8_t colorEncoding,alphaSize,preferredFormat,hasMips;
    std::uint32_t width,height;
    std::uint32_t mipOffsets[16];
    std::uint32_t mipSizes[16];
    union Extended { BlpPalPixel palette[256]; struct { std::uint32_t headerSize; std::uint8_t header[1020]; } jpeg; } extended;
};
#pragma pack(pop)

class CBLPFile {
public:
    CBLPFile(); ~CBLPFile();
    void Close(); int CreateMipLevels(unsigned width,unsigned height);
    int Open(const char* filename); int Open(SFile* filename); int Source(void* fileBits); int Source(const void* fileBits,unsigned bytes);
    C4Pixel* Image(unsigned level=0);
    int SetImage(CBLPFile* source,unsigned mipLevel,CStatus* status=nullptr);
    int SetImage(const void* image,unsigned width,unsigned height,std::uint8_t alphaBits,unsigned mipLevel=0,CStatus* status=nullptr);
    int SetAlphaBits(unsigned alpha); int IsValidMip(unsigned level) const;
    PIXEL_FORMAT Lock(BlitFormat format,unsigned mipLevel,std::uint8_t** data,unsigned* stride);
    int Unlock(unsigned mipLevel); int GetFormatSize(PIXEL_FORMAT format,unsigned mipLevel,unsigned* size,unsigned* stride) const;
    MIPS_TYPE HasMips() const; void SetHasMips(MIPS_TYPE);
    int LockChain(PIXEL_FORMAT,MipBits** images,unsigned mipLevel=0); int LockChain2(PIXEL_FORMAT,MipBits** images,unsigned mipLevel=0);
    void DecompPalFastPath(std::uint8_t* data,const std::uint8_t* source,unsigned colors) const;
    void DecompPalARGB8888(std::uint8_t* data,const std::uint8_t* source,unsigned colors) const;
    void DecompPalARGB4444(std::uint8_t* data,const std::uint8_t* source,unsigned colors) const;
    void DecompPalARGB1555(std::uint8_t* data,const std::uint8_t* source,unsigned colors) const;
    void DecompPalARGB565(std::uint8_t* data,const std::uint8_t* source,unsigned colors) const;
    int DecompPal(PIXEL_FORMAT,unsigned mipLevel,std::uint8_t* data,const std::uint8_t* source) const;
    int Lock2(PIXEL_FORMAT,unsigned mipLevel,std::uint8_t* data,unsigned* stride); int Unlock2(unsigned mipLevel);
    unsigned Bytes() const; unsigned Bytes(unsigned mipLevel) const;

    MipBits* m_images; BLPHeader m_header; void* m_inMemoryImage; int m_inMemoryNeedsFree;
    unsigned m_numLevels,m_quality; HCOLORMAP__* m_colorMapping; MipMapAlgorithm m_mipMapAlgorithm; std::uint8_t* m_lockDecompMem;
private:
    std::vector<MipBits> m_mipEntries; std::vector<std::vector<C4Pixel>> m_ownedMips; std::vector<std::uint8_t> m_fileStorage;
};

int IsLegalDimension(unsigned value);
BlitFormat GetBlitFormat(PIXEL_FORMAT format);
