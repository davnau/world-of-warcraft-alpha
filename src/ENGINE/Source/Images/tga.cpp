#include "ENGINE/Source/Images/images.h"

#include "Storm/h/SMem.h"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <vector>

namespace {

unsigned BytesPerPixel(const TGAHeader& header)
{
    return (header.bPixelDepth + 7u) / 8u;
}

bool SamePixel(const std::uint8_t* left, const std::uint8_t* right, unsigned bytes)
{
    return std::memcmp(left, right, bytes) == 0;
}

void CopyPixel(std::uint8_t* destination, const std::uint8_t* source, unsigned bytes)
{
    std::memcpy(destination, source, bytes);
}

}

void CTgaFile::Close()
{
    if (m_image) {
        SMemFree(m_image, __FILE__, __LINE__, 0);
        m_image = nullptr;
    }
    if (m_file) {
        SFile::Close(m_file);
        m_file = nullptr;
    }
    RemoveHeaderTrailer();
    if (m_colorMap) {
        SMemFree(m_colorMap, __FILE__, __LINE__, 0);
        m_colorMap = nullptr;
    }
    m_imageBytes = 0;
}

unsigned CTgaFile::Open(const char* filename)
{
    if (!filename || !*filename)
        return 0;
    Close();
    if (SFile::Open(filename, &m_file) == 0)
        return 0;
    unsigned read = 0;
    if (!SFile::Read(m_file, &m_header, sizeof(m_header), &read, nullptr, nullptr) || read != sizeof(m_header)) {
        Close();
        return 0;
    }
    if (m_header.bIDLength) {
        m_addlHeaderData = static_cast<std::uint8_t*>(SMemAlloc(m_header.bIDLength, __FILE__, __LINE__, 0));
        if (!m_addlHeaderData || !SFile::Read(m_file, m_addlHeaderData, m_header.bIDLength, &read, nullptr, nullptr) ||
            read != m_header.bIDLength) {
            Close();
            return 0;
        }
    }
    if (m_header.bColorMapType && m_header.wColorMapEntries) {
        const unsigned mapBytes = m_header.wColorMapEntries * ((m_header.bColorMapEntrySize + 7u) / 8u);
        m_colorMap = static_cast<std::uint8_t*>(SMemAlloc(mapBytes, __FILE__, __LINE__, 0));
        if (!m_colorMap || !SFile::Read(m_file, m_colorMap, mapBytes, &read, nullptr, nullptr) || read != mapBytes) {
            Close();
            return 0;
        }
    }
    return LoadImageData(0) ? 1u : 0u;
}

int CTgaFile::ValidateColorDepth()
{
    if (m_header.bColorMapType && m_header.bColorMapEntrySize != 15 && m_header.bColorMapEntrySize != 16 &&
        m_header.bColorMapEntrySize != 24 && m_header.bColorMapEntrySize != 32)
        return 0;
    return m_header.bPixelDepth == 8 || m_header.bPixelDepth == 15 || m_header.bPixelDepth == 16 ||
        m_header.bPixelDepth == 24 || m_header.bPixelDepth == 32;
}

void CTgaFile::ConvertColorMapped(char flags)
{
    if (!m_image || !m_colorMap || !m_header.wColorMapEntries)
        return;
    const unsigned sourceBytes = BytesPerPixel(m_header);
    const unsigned mapBytes = (m_header.bColorMapEntrySize + 7u) / 8u;
    const unsigned count = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight;
    const unsigned outputBytes = (m_header.bColorMapEntrySize >= 24 ? 4u : 2u);
    auto* output = static_cast<std::uint8_t*>(SMemAlloc(count * outputBytes, __FILE__, __LINE__, 0));
    if (!output)
        return;
    for (unsigned i = 0; i < count; ++i) {
        const unsigned index = m_image[i * sourceBytes] - m_header.wColorMapStartIndex;
        const auto* source = index < m_header.wColorMapEntries ? m_colorMap + index * mapBytes : nullptr;
        if (!source) {
            std::memset(output + i * outputBytes, 0, outputBytes);
            continue;
        }
        if (outputBytes == 4) {
            output[i * 4 + 0] = source[0];
            output[i * 4 + 1] = source[1];
            output[i * 4 + 2] = source[2];
            output[i * 4 + 3] = mapBytes == 4 ? source[3] : 255;
        } else {
            std::memcpy(output + i * outputBytes, source, outputBytes);
        }
    }
    SMemFree(m_image, __FILE__, __LINE__, 0);
    m_image = output;
    m_header.bPixelDepth = static_cast<std::uint8_t>(outputBytes * 8);
    m_header.bColorMapType = 0;
    m_header.wColorMapEntries = 0;
    m_imageBytes = count * outputBytes;
    (void)flags;
}

int CTgaFile::ReadColorMappedImage(char flags)
{
    if (!ReadRawImage(flags))
        return 0;
    ConvertColorMapped(flags);
    return m_image != nullptr;
}

int CTgaFile::LoadImageData(unsigned flags)
{
    if (!ValidateColorDepth())
        return 0;
    switch (m_header.bImageType) {
    case 1:
    case 2:
    case 3:
        return m_header.bColorMapType ? ReadColorMappedImage(static_cast<char>(flags)) : ReadRawImage(static_cast<char>(flags));
    case 9:
    case 10:
    case 11:
        return m_header.bColorMapType ? (ReadRleImage(static_cast<char>(flags)) && (ConvertColorMapped(static_cast<char>(flags)), 1))
                                      : ReadRleImage(static_cast<char>(flags));
    default:
        return 0;
    }
}

int CTgaFile::PreImageBytes()
{
    unsigned bytes = m_header.bIDLength + sizeof(TGAHeader);
    if (m_header.bColorMapType) {
        const unsigned mapBytes = std::min(8u, (static_cast<unsigned>(m_header.bColorMapEntrySize) + 7u) / 8u);
        bytes += m_header.wColorMapEntries * mapBytes;
    }
    return static_cast<int>(bytes);
}

void CTgaFile::AddAlphaChannel(std::uint8_t* alphaData, std::uint8_t* noAlphaData, std::uint8_t* alpha)
{
    const unsigned oldBytes = (m_header.bPixelDepth + 7u) / 8u;
    const unsigned pixels = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight;
    for (unsigned i = 0; i < pixels; ++i) {
        CopyPixel(alphaData + i * (oldBytes + 1), noAlphaData + i * oldBytes, oldBytes);
        alphaData[i * (oldBytes + 1) + oldBytes] = alpha ? alpha[i] : 255;
    }
    m_header.bPixelDepth = static_cast<std::uint8_t>(m_header.bPixelDepth + 8);
    m_header.bImageDescriptor = static_cast<std::uint8_t>((m_header.bImageDescriptor & 0xf0) | 8);
    m_imageBytes = pixels * (oldBytes + 1);
}

std::uint8_t* CTgaFile::AddAlphaChannel(std::uint8_t* image)
{
    if (!image || m_header.bPixelDepth == 32)
        return image;
    const unsigned oldBytes = BytesPerPixel(m_header);
    const unsigned pixels = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight;
    auto* output = static_cast<std::uint8_t*>(SMemAlloc(pixels * (oldBytes + 1), __FILE__, __LINE__, 0));
    if (!output)
        return nullptr;
    AddAlphaChannel(output, image, nullptr);
    return output;
}

unsigned CTgaFile::ReadRawImage(char flags)
{
    const unsigned bytes = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight * BytesPerPixel(m_header);
    if (SFile::SetFilePointer(m_file, PreImageBytes(), nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return 0;
    m_image = static_cast<std::uint8_t*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));
    if (!m_image)
        return 0;
    unsigned read = 0;
    if (!SFile::Read(m_file, m_image, bytes, &read, nullptr, nullptr) || read != bytes) {
        SMemFree(m_image, __FILE__, __LINE__, 0);
        m_image = nullptr;
        return 0;
    }
    m_imageBytes = bytes;
    if (flags & 1)
        SetTopDown(1);
    return bytes;
}

unsigned CTgaFile::ReadRleImage(char flags)
{
    if (SFile::SetFilePointer(m_file, PreImageBytes(), nullptr, FILE_BEGIN) == INVALID_SET_FILE_POINTER)
        return 0;
    const unsigned bytes = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight * BytesPerPixel(m_header);
    std::vector<std::uint8_t> compressed;
    unsigned fileSizeHigh = 0;
    const unsigned fileSize = SFile::GetFileSize(m_file, &fileSizeHigh);
    if (fileSize < static_cast<unsigned>(PreImageBytes()))
        return 0;
    compressed.resize(fileSize - PreImageBytes());
    unsigned read = 0;
    if (!SFile::Read(m_file, compressed.data(), static_cast<unsigned>(compressed.size()), &read, nullptr, nullptr))
        return 0;
    m_image = static_cast<std::uint8_t*>(SMemAlloc(bytes, __FILE__, __LINE__, 0));
    if (!m_image)
        return 0;
    if (!RLEDecompressImage(compressed.data(), m_image)) {
        SMemFree(m_image, __FILE__, __LINE__, 0);
        m_image = nullptr;
        return 0;
    }
    m_imageBytes = bytes;
    if (flags & 1)
        SetTopDown(1);
    return bytes;
}

int CTgaFile::RLEDecompressImage(std::uint8_t* compressed, std::uint8_t* output)
{
    const unsigned bytesPerPixel = BytesPerPixel(m_header);
    const unsigned pixels = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight;
    unsigned produced = 0;
    const std::uint8_t* cursor = compressed;
    while (produced < pixels) {
        const std::uint8_t packet = *cursor++;
        const unsigned count = (packet & 0x7f) + 1;
        if (produced + count > pixels)
            return 0;
        if (packet & 0x80) {
            for (unsigned i = 0; i < count; ++i)
                CopyPixel(output + (produced + i) * bytesPerPixel, cursor, bytesPerPixel);
            cursor += bytesPerPixel;
        } else {
            const unsigned bytes = count * bytesPerPixel;
            std::memcpy(output + produced * bytesPerPixel, cursor, bytes);
            cursor += bytes;
        }
        produced += count;
    }
    return 1;
}

int CTgaFile::SetTopDown(int set)
{
    const bool topDown = (m_header.bImageDescriptor & 0x20) != 0;
    const bool rightToLeft = (m_header.bImageDescriptor & 0x10) != 0;
    if ((set != 0) == topDown && !rightToLeft)
        return 1;
    const unsigned width = m_header.wWidth;
    const unsigned height = m_header.wHeight;
    const unsigned bytes = BytesPerPixel(m_header);
    std::vector<std::uint8_t> copy(m_image, m_image + m_imageBytes);
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            const unsigned sourceY = topDown ? y : (height - 1 - y);
            const unsigned sourceX = rightToLeft ? (width - 1 - x) : x;
            CopyPixel(m_image + (y * width + x) * bytes, copy.data() + (sourceY * width + sourceX) * bytes, bytes);
        }
    }
    m_header.bImageDescriptor = static_cast<std::uint8_t>(m_header.bImageDescriptor ^ 0x30);
    return 1;
}

std::uint8_t* CTgaFile::Image() { return m_image; }
const std::uint8_t* CTgaFile::Image() const { return m_image; }
TGA32Pixel* CTgaFile::ImageTGA32Pixel() { return reinterpret_cast<TGA32Pixel*>(m_image); }
const TGA32Pixel* CTgaFile::ImageTGA32Pixel() const { return reinterpret_cast<const TGA32Pixel*>(m_image); }

int CTgaFile::RemoveAlphaChannels()
{
    if (!m_image || m_header.bPixelDepth != 32)
        return 1;
    const unsigned pixels = static_cast<unsigned>(m_header.wWidth) * m_header.wHeight;
    auto* output = static_cast<std::uint8_t*>(SMemAlloc(pixels * 3, __FILE__, __LINE__, 0));
    if (!output)
        return 0;
    for (unsigned i = 0; i < pixels; ++i)
        std::memcpy(output + i * 3, m_image + i * 4, 3);
    SMemFree(m_image, __FILE__, __LINE__, 0);
    m_image = output;
    m_header.bPixelDepth = 24;
    m_header.bImageDescriptor &= 0xf0;
    m_imageBytes = pixels * 3;
    return 1;
}

void CTgaFile::RemoveHeaderTrailer()
{
    if (m_addlHeaderData) {
        SMemFree(m_addlHeaderData, __FILE__, __LINE__, 0);
        m_addlHeaderData = nullptr;
    }
    std::memset(&m_header, 0, sizeof(m_header));
    std::memset(&m_footer, 0, sizeof(m_footer));
}

std::uint8_t* CTgaFile::SetImage(const CTgaFile* source)
{
    if (!source)
        return nullptr;
    return SetImage(source->Image(), source->m_header.wWidth, source->m_header.wHeight,
        source->m_header.bPixelDepth, source->m_header.bImageDescriptor & 0xf,
        (source->m_header.bImageDescriptor & 0x20) != 0, (source->m_header.bImageDescriptor & 0x10) != 0)
        ? m_image : nullptr;
}

int CTgaFile::SetImage(const void* image, std::uint16_t width, std::uint16_t height,
    std::uint8_t pixelDepth, std::uint8_t alphaBits, int topDown, int rightToLeft)
{
    if (!image || (pixelDepth != 24 && pixelDepth != 32) || (alphaBits != 0 && alphaBits != 8))
        return 0;
    Close();
    m_header.bPixelDepth = pixelDepth;
    m_header.bImageDescriptor = static_cast<std::uint8_t>((alphaBits & 0xf) | (rightToLeft ? 0x10 : 0) | (topDown ? 0x20 : 0));
    m_header.bImageType = 2;
    m_header.wWidth = width;
    m_header.wHeight = height;
    m_imageBytes = static_cast<unsigned>(width) * height * BytesPerPixel(m_header);
    m_image = static_cast<std::uint8_t*>(SMemAlloc(m_imageBytes, __FILE__, __LINE__, 0));
    if (!m_image)
        return 0;
    std::memcpy(m_image, image, m_imageBytes);
    std::memcpy(m_footer.szSigniture, "TRUEVISION-XFILE.", 18);
    return 1;
}

int CTgaFile::CountRun(std::uint8_t* image, int max)
{
    if (!image || max <= 0)
        return 0;
    const unsigned bytes = BytesPerPixel(m_header);
    int count = 1;
    while (count < max && count < 128 && SamePixel(image, image + count * bytes, bytes))
        ++count;
    return count;
}

int CTgaFile::RleCompressLine(std::uint8_t** uncompressed, std::uint8_t** compressed)
{
    if (!uncompressed || !compressed || !*uncompressed || !*compressed)
        return 0;
    const unsigned bytes = BytesPerPixel(m_header);
    unsigned remaining = m_header.wWidth;
    auto* input = *uncompressed;
    auto* output = *compressed;
    while (remaining) {
        const int run = CountRun(input, static_cast<int>(remaining));
        if (run >= 2) {
            *output++ = static_cast<std::uint8_t>(0x80 | (run - 1));
            CopyPixel(output, input, bytes);
            output += bytes;
            input += run * bytes;
            remaining -= run;
        } else {
            const auto* start = input;
            unsigned count = 1;
            input += bytes;
            while (count < remaining && count < 128) {
                const int next = CountRun(input, static_cast<int>(remaining - count));
                if (next >= 2)
                    break;
                ++count;
                input += bytes;
            }
            *output++ = static_cast<std::uint8_t>(count - 1);
            std::memcpy(output, start, count * bytes);
            output += count * bytes;
            remaining -= count;
        }
    }
    *uncompressed = input;
    *compressed = output;
    return 1;
}

int CTgaFile::Compress()
{
    if (!m_image || m_header.bImageType != 2)
        return 0;
    const unsigned maxSize = m_imageBytes + static_cast<unsigned>(m_header.wWidth) * m_header.wHeight + 1;
    auto* compressed = static_cast<std::uint8_t*>(SMemAlloc(maxSize, __FILE__, __LINE__, 0));
    if (!compressed)
        return 0;
    auto* output = compressed;
    const unsigned lineBytes = static_cast<unsigned>(m_header.wWidth) * BytesPerPixel(m_header);
    for (unsigned y = 0; y < m_header.wHeight; ++y) {
        auto* input = m_image + y * lineBytes;
        RleCompressLine(&input, &output);
    }
    const unsigned compressedBytes = static_cast<unsigned>(output - compressed);
    SMemFree(m_image, __FILE__, __LINE__, 0);
    m_image = compressed;
    m_imageBytes = compressedBytes;
    m_header.bImageType = static_cast<std::uint8_t>(m_header.bImageType + 8);
    return 1;
}

int CTgaFile::Write(const char* path)
{
    if (!path || !m_image)
        return 0;
    std::ofstream output(path, std::ios::binary | std::ios::trunc);
    if (!output)
        return 0;
    output.write(reinterpret_cast<const char*>(&m_header), sizeof(m_header));
    if (m_addlHeaderData)
        output.write(reinterpret_cast<const char*>(m_addlHeaderData), m_header.bIDLength);
    if (m_colorMap && m_header.wColorMapEntries)
        output.write(reinterpret_cast<const char*>(m_colorMap), m_header.wColorMapEntries * ((m_header.bColorMapEntrySize + 7u) / 8u));
    output.write(reinterpret_cast<const char*>(m_image), m_imageBytes);
    output.write(reinterpret_cast<const char*>(&m_footer), sizeof(m_footer));
    return output.good() ? 1 : 0;
}

