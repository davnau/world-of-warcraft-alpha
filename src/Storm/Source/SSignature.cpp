#include "Storm/h/SSignature.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <new>
#include <vector>

namespace {
constexpr std::uint32_t SIGNATURE_MAGIC = 0x5349474EU;
}

void __fastcall SSignatureVerifyStream_Begin(SSignatureData** token, unsigned int modulusSize,
    unsigned int pubExponentSize) {
    if (!token) return;
    *token = nullptr;
    auto value = new (std::nothrow) SSignatureData{};
    if (!value) return;
    value->modulusSize = modulusSize;
    value->pubExponentSize = pubExponentSize;
    value->magicBufferSize = modulusSize + sizeof(SIGNATURE_MAGIC);
    value->magicBuffer = new (std::nothrow) unsigned char[value->magicBufferSize];
    if (!value->magicBuffer) { delete value; return; }
    value->sha.Initialize();
    *token = value;
}

unsigned int SSignatureVerifyStream_GetSignatureLength(const SSignatureData* token) {
    return token ? token->magicBufferSize : 0;
}

void __fastcall SSignatureVerifyStream_ProvideData(SSignatureData* token, const void* data, unsigned int size) {
    if (!token || (!data && size == 0) || !token->magicBuffer) return;
    const auto* input = static_cast<const unsigned char*>(data);
    if (size >= token->magicBufferSize) {
        if (token->magicBufferUsed) token->sha.Append(token->magicBuffer, token->magicBufferUsed);
        const unsigned int hashBytes = size - token->magicBufferSize;
        if (hashBytes) token->sha.Append(input, hashBytes);
        std::memcpy(token->magicBuffer, input + hashBytes, token->magicBufferSize);
        token->magicBufferUsed = token->magicBufferSize;
        return;
    }
    const unsigned int overflow = token->magicBufferUsed + size > token->magicBufferSize
        ? token->magicBufferUsed + size - token->magicBufferSize : 0;
    if (overflow) {
        token->sha.Append(token->magicBuffer, overflow);
        std::memmove(token->magicBuffer, token->magicBuffer + overflow, token->magicBufferUsed - overflow);
        token->magicBufferUsed -= overflow;
    }
    if (size) std::memcpy(token->magicBuffer + token->magicBufferUsed, input, size);
    token->magicBufferUsed += size;
}

BOOL __fastcall SSignatureVerifyStream_Finish(SSignatureData* token, const unsigned char* modulus,
    const unsigned char* pubExponent) {
    std::unique_ptr<SSignatureData> owner(token);
    if (!token) return FALSE;
    std::unique_ptr<unsigned char[]> bufferOwner(token->magicBuffer);
    if (!modulus || !pubExponent || token->magicBufferUsed != token->magicBufferSize) return FALSE;
    std::uint32_t magic = 0;
    std::memcpy(&magic, token->magicBuffer, sizeof(magic));
    if (magic != SIGNATURE_MAGIC || token->modulusSize < Sha1::DIGEST + 1U) return FALSE;
    std::vector<unsigned char> expected(token->modulusSize, 0xBBU);
    expected.back() = 0x0BU;
    token->sha.Finalize(expected.data());
    std::vector<unsigned char> decoded(token->magicBuffer + sizeof(magic),
        token->magicBuffer + token->magicBufferSize);
    Crypt::RSA rsa;
    rsa.Prepare(modulus, token->modulusSize, pubExponent, token->pubExponentSize);
    rsa.Process(decoded.data(), token->modulusSize);
    return decoded == expected;
}

BOOL __fastcall SSignatureVerify(const void* data, unsigned int size, const unsigned char* modulus,
    unsigned int modulusSize, const unsigned char* pubExponent, unsigned int pubExponentSize) {
    SSignatureData* token = nullptr;
    SSignatureVerifyStream_Begin(&token, modulusSize, pubExponentSize);
    if (!token) return FALSE;
    SSignatureVerifyStream_ProvideData(token, data, size);
    return SSignatureVerifyStream_Finish(token, modulus, pubExponent);
}

BOOL __fastcall Signature::HasMagic(const unsigned char* data, unsigned int size,
    unsigned int modulusSize, unsigned int* dataSize) {
    if (dataSize) *dataSize = size;
    if (!data || size < modulusSize + sizeof(SIGNATURE_MAGIC)) return FALSE;
    const unsigned int payloadSize = size - modulusSize - sizeof(SIGNATURE_MAGIC);
    std::uint32_t magic = 0;
    std::memcpy(&magic, data + payloadSize, sizeof(magic));
    if (magic != SIGNATURE_MAGIC) return FALSE;
    if (dataSize) *dataSize = payloadSize;
    return TRUE;
}

void __fastcall Signature::Hash(const void* data, unsigned int size, unsigned char* digest) {
    Sha1 hash;
    hash.Initialize();
    hash.Append(data, size);
    hash.Finalize(digest);
}

int __fastcall SSignatureGenerate(unsigned char* data, unsigned int* size, const unsigned char* modulus,
    unsigned int modulusSize, const unsigned char* privateExponent, unsigned int privateExponentSize,
    const unsigned char* publicExponent, unsigned int publicExponentSize) {
    if (!data || !size || !modulus || !privateExponent || !publicExponent || modulusSize < Sha1::DIGEST + 1U) return 0;
    unsigned int payloadSize = *size;
    Signature::HasMagic(data, *size, modulusSize, &payloadSize);
    std::vector<unsigned char> original(modulusSize, 0xBBU);
    original.back() = 0x0BU;
    Signature::Hash(data, payloadSize, original.data());
    std::vector<unsigned char> generated = original;
    Crypt::RSA encoder;
    encoder.Prepare(modulus, modulusSize, privateExponent, privateExponentSize);
    encoder.Process(generated.data(), modulusSize);
    std::vector<unsigned char> verified = generated;
    Crypt::RSA decoder;
    decoder.Prepare(modulus, modulusSize, publicExponent, publicExponentSize);
    decoder.Process(verified.data(), modulusSize);
    if (verified != original) return 0;
    std::memcpy(data + payloadSize, &SIGNATURE_MAGIC, sizeof(SIGNATURE_MAGIC));
    std::memcpy(data + payloadSize + sizeof(SIGNATURE_MAGIC), generated.data(), modulusSize);
    *size = payloadSize + sizeof(SIGNATURE_MAGIC) + modulusSize;
    return 1;
}
