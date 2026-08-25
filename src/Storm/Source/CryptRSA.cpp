#include "Storm/h/SSignature.h"

#include <cstring>

BigNum::BigNum() : m_data(nullptr) { SBigNew(&m_data); }
BigNum::~BigNum() { SBigDel(m_data); }
BigNum& BigNum::PowMod(const BigNum& base, const BigNum& exponent, const BigNum& modulus) {
    SBigPowMod(m_data, base.m_data, exponent.m_data, modulus.m_data);
    return *this;
}
unsigned char* BigNum::ToBinaryBuffer(unsigned char* data, unsigned int bytes) {
    unsigned int written = 0;
    SBigToBinaryBuffer(m_data, data, bytes, &written);
    if (data && written < bytes) std::memset(data + written, 0, bytes - written);
    return data;
}
void BigNum::FromBinary(const void* data, unsigned int bytes) { SBigFromBinary(m_data, data, bytes); }

Crypt::RSA::RSA() = default;
Crypt::RSA::~RSA() = default;
void Crypt::RSA::Prepare(const void* modulus, unsigned int modulusLength,
    const void* exponent, unsigned int exponentLength) {
    m_modulus.FromBinary(modulus, modulusLength);
    m_exponent.FromBinary(exponent, exponentLength);
}
void Crypt::RSA::Process(unsigned char* data, unsigned int length) {
    if (!data || !length) return;
    BigNum source;
    BigNum result;
    source.FromBinary(data, length);
    result.PowMod(source, m_exponent, m_modulus);
    result.ToBinaryBuffer(data, length);
}
