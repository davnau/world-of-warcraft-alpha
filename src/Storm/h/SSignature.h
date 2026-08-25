#pragma once

#include "Storm/h/storm.h"

class BigNum {
public:
    BigData* m_data;
    BigNum();
    ~BigNum();
    BigNum& PowMod(const BigNum& base, const BigNum& exponent, const BigNum& modulus);
    unsigned char* ToBinaryBuffer(unsigned char* data, unsigned int bytes);
    void FromBinary(const void* data, unsigned int bytes);
};

namespace Crypt {
class RSA {
public:
    BigNum m_modulus;
    BigNum m_exponent;
    RSA();
    ~RSA();
    void Prepare(const void* modulus, unsigned int modulusLength, const void* exponent, unsigned int exponentLength);
    void Process(unsigned char* data, unsigned int length);
};
}

struct SSignatureData {
    unsigned int modulusSize;
    unsigned int pubExponentSize;
    unsigned int magicBufferUsed;
    unsigned int magicBufferSize;
    unsigned char* magicBuffer;
    Sha1 sha;
};

namespace Signature {
BOOL __fastcall HasMagic(const unsigned char* data, unsigned int size, unsigned int modulusSize, unsigned int* dataSize);
void __fastcall Hash(const void* data, unsigned int size, unsigned char* digest);
}

void __fastcall SSignatureVerifyStream_Begin(SSignatureData** token, unsigned int modulusSize, unsigned int pubExponentSize);
unsigned int SSignatureVerifyStream_GetSignatureLength(const SSignatureData* token);
void __fastcall SSignatureVerifyStream_ProvideData(SSignatureData* token, const void* data, unsigned int size);
BOOL __fastcall SSignatureVerifyStream_Finish(SSignatureData* token, const unsigned char* modulus,
    const unsigned char* pubExponent);
BOOL __fastcall SSignatureVerify(const void* data, unsigned int size, const unsigned char* modulus,
    unsigned int modulusSize, const unsigned char* pubExponent, unsigned int pubExponentSize);
int __fastcall SSignatureGenerate(unsigned char* data, unsigned int* size, const unsigned char* modulus,
    unsigned int modulusSize, const unsigned char* privateExponent, unsigned int privateExponentSize,
    const unsigned char* publicExponent, unsigned int publicExponentSize);

#if defined(_M_IX86)
static_assert(sizeof(BigNum) == 4);
static_assert(sizeof(Crypt::RSA) == 8);
static_assert(sizeof(SSignatureData) == 120);
#endif
