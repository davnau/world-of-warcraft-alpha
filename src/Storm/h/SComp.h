#pragma once

#include "Storm/h/sapibase.h"

enum SCOMP_TYPE : unsigned int {
    SCOMP_HUFFMAN = 0x01,
    SCOMP_ZLIB = 0x02,
    SCOMP_PKWARE = 0x08,
    SCOMP_BZIP2 = 0x10,
    SCOMP_SPARSE = 0x20,
    SCOMP_ADPCM_MONO = 0x40,
    SCOMP_ADPCM_STEREO = 0x80,
};

int __stdcall SCompCompress(void* destination, unsigned int* destinationBytes, const void* source,
    unsigned int sourceBytes, unsigned int compressionTypes, unsigned int hint, unsigned int optimization);
int __stdcall SCompDecompress(void* destination, unsigned int* destinationBytes, const void* source,
    unsigned int sourceBytes);
int __stdcall SCompDecompress2(void* destination, unsigned int* destinationBytes, const void* source,
    unsigned int sourceBytes, const char* filename);
int __stdcall SCompDestroy();
