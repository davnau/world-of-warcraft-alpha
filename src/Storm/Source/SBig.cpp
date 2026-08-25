// Reconstructed from D:\build\buildWoW\Storm\Source\SBig.cpp.
// BigData/BigBuffer/BigStack retain their PDB layouts from Storm/h/storm.h;
// arithmetic uses the original little-endian 32-bit limb representation.

#include "Storm/h/storm.h"

#include <algorithm>
#include <cstring>
#include <string>
#include <vector>

namespace {
using Limbs = std::vector<std::uint32_t>;

void Trim(Limbs& value) { while (value.size() > 1 && value.back() == 0) value.pop_back(); if (value.empty()) value.push_back(0); }

Limbs Read(const BigData* value)
{
    Limbs result;
    if (value && value->m_primary.m_data.m_data && value->m_primary.m_data.m_count)
        result.assign(value->m_primary.m_data.m_data, value->m_primary.m_data.m_data + value->m_primary.m_data.m_count);
    Trim(result);
    return result;
}

void Write(BigData* destination, Limbs value)
{
    if (!destination) return;
    Trim(value);
    destination->m_primary.m_offset = 0;
    destination->m_primary.m_data.SetCount(static_cast<unsigned int>(value.size()));
    std::copy(value.begin(), value.end(), destination->m_primary.m_data.m_data);
}

int Compare(const Limbs& leftIn, const Limbs& rightIn)
{
    Limbs left = leftIn, right = rightIn; Trim(left); Trim(right);
    if (left.size() != right.size()) return left.size() < right.size() ? -1 : 1;
    for (size_t index = left.size(); index-- > 0;) if (left[index] != right[index]) return left[index] < right[index] ? -1 : 1;
    return 0;
}

Limbs Add(const Limbs& left, const Limbs& right)
{
    Limbs result(std::max(left.size(), right.size()) + 1, 0); std::uint64_t carry = 0;
    for (size_t index = 0; index + 1 < result.size(); ++index) { const std::uint64_t sum = carry + (index < left.size() ? left[index] : 0) + (index < right.size() ? right[index] : 0); result[index] = static_cast<std::uint32_t>(sum); carry = sum >> 32; }
    result.back() = static_cast<std::uint32_t>(carry); Trim(result); return result;
}

Limbs Sub(const Limbs& left, const Limbs& right)
{
    if (Compare(left, right) < 0) return Limbs{0};
    Limbs result(left); std::uint64_t borrow = 0;
    for (size_t index = 0; index < result.size(); ++index) { const std::uint64_t sub = (index < right.size() ? right[index] : 0) + borrow; const std::uint64_t current = result[index]; result[index] = static_cast<std::uint32_t>(current - sub); borrow = current < sub; }
    Trim(result); return result;
}

unsigned int BitLength(const Limbs& input)
{
    Limbs value = input; Trim(value); if (value.size() == 1 && value[0] == 0) return 0;
    std::uint32_t high = value.back(); unsigned int bits = static_cast<unsigned int>((value.size() - 1) * 32);
    while (high) { ++bits; high >>= 1; } return bits;
}

bool Bit(const Limbs& value, unsigned int bit) { const size_t limb = bit / 32; return limb < value.size() && ((value[limb] >> (bit % 32)) & 1U); }

Limbs Shl(const Limbs& value, unsigned int bits)
{
    const unsigned int words = bits / 32, shift = bits % 32; Limbs result(value.size() + words + 1, 0); std::uint64_t carry = 0;
    for (size_t i = 0; i < value.size(); ++i) { const std::uint64_t current = (static_cast<std::uint64_t>(value[i]) << shift) | carry; result[i + words] = static_cast<std::uint32_t>(current); carry = current >> 32; }
    result[value.size() + words] = static_cast<std::uint32_t>(carry); Trim(result); return result;
}

Limbs Shr(const Limbs& value, unsigned int bits)
{
    const unsigned int words = bits / 32, shift = bits % 32; if (words >= value.size()) return Limbs{0};
    Limbs result(value.size() - words, 0); std::uint32_t carry = 0;
    for (size_t i = value.size(); i-- > words;) { const std::uint32_t current = value[i]; result[i - words] = shift ? (current >> shift) | (carry << (32 - shift)) : current; carry = shift ? current & ((1U << shift) - 1U) : 0; }
    Trim(result); return result;
}

Limbs Mul(const Limbs& left, const Limbs& right)
{
    Limbs result(left.size() + right.size(), 0);
    for (size_t i = 0; i < left.size(); ++i) { std::uint64_t carry = 0; for (size_t j = 0; j < right.size(); ++j) { const std::uint64_t current = static_cast<std::uint64_t>(left[i]) * right[j] + result[i + j] + carry; result[i + j] = static_cast<std::uint32_t>(current); carry = current >> 32; } result[i + right.size()] = static_cast<std::uint32_t>(carry); }
    Trim(result); return result;
}

void DivMod(const Limbs& numerator, const Limbs& denominator, Limbs& quotient, Limbs& remainder)
{
    Limbs divisor = denominator; Trim(divisor); if (divisor.size() == 1 && divisor[0] == 0) { quotient = {0}; remainder = numerator; return; }
    quotient = {0}; remainder = {0}; const unsigned int bits = BitLength(numerator); quotient.resize((bits + 31) / 32, 0);
    for (unsigned int bit = bits; bit-- > 0;) { remainder = Shl(remainder, 1); if (Bit(numerator, bit)) remainder[0] |= 1; if (Compare(remainder, divisor) >= 0) { remainder = Sub(remainder, divisor); quotient[bit / 32] |= 1U << (bit % 32); } }
    Trim(quotient); Trim(remainder);
}

Limbs Mod(const Limbs& value, const Limbs& modulus)
{
    if (Compare(value, modulus) < 0) return value;
    Limbs quotient, remainder; DivMod(value, modulus, quotient, remainder); return remainder;
}

Limbs AddMod(const Limbs& left, const Limbs& right, const Limbs& modulus)
{
    Limbs result = Add(left, right);
    if (Compare(result, modulus) >= 0) result = Sub(result, modulus);
    return result;
}

Limbs MulModBinary(Limbs left, Limbs right, const Limbs& modulus)
{
    left = Mod(left, modulus);
    right = Mod(right, modulus);
    Limbs result{0};
    const unsigned int bits = BitLength(right);
    for (unsigned int bit = 0; bit < bits; ++bit) {
        if (Bit(right, bit)) result = AddMod(result, left, modulus);
        left = AddMod(left, left, modulus);
    }
    return result;
}

struct MontgomeryContext {
    Limbs modulus;
    std::size_t words;
    std::uint32_t factor;

    explicit MontgomeryContext(Limbs value) : modulus(std::move(value)), words(modulus.size()), factor(0)
    {
        Trim(modulus);
        words = modulus.size();
        std::uint32_t inverse = 1;
        for (unsigned int index = 0; index < 5; ++index)
            inverse *= 2U - modulus[0] * inverse;
        factor = 0U - inverse;
    }

    Limbs Multiply(const Limbs& leftValue, const Limbs& rightValue) const
    {
        Limbs left = leftValue, right = rightValue;
        left.resize(words, 0);
        right.resize(words, 0);
        std::vector<std::uint32_t> product(words * 2 + 2, 0);
        for (std::size_t leftIndex = 0; leftIndex < words; ++leftIndex) {
            std::uint64_t carry = 0;
            for (std::size_t rightIndex = 0; rightIndex < words; ++rightIndex) {
                const std::size_t output = leftIndex + rightIndex;
                const std::uint64_t value = static_cast<std::uint64_t>(left[leftIndex]) * right[rightIndex] +
                                            product[output] + carry;
                product[output] = static_cast<std::uint32_t>(value);
                carry = value >> 32;
            }
            std::size_t output = leftIndex + words;
            while (carry != 0) {
                const std::uint64_t value = static_cast<std::uint64_t>(product[output]) + carry;
                product[output++] = static_cast<std::uint32_t>(value);
                carry = value >> 32;
            }
        }

        for (std::size_t index = 0; index < words; ++index) {
            const std::uint32_t quotient = product[index] * factor;
            std::uint64_t carry = 0;
            for (std::size_t modulusIndex = 0; modulusIndex < words; ++modulusIndex) {
                const std::size_t output = index + modulusIndex;
                const std::uint64_t value = static_cast<std::uint64_t>(quotient) * modulus[modulusIndex] +
                                            product[output] + carry;
                product[output] = static_cast<std::uint32_t>(value);
                carry = value >> 32;
            }
            std::size_t output = index + words;
            while (carry != 0) {
                const std::uint64_t value = static_cast<std::uint64_t>(product[output]) + carry;
                product[output++] = static_cast<std::uint32_t>(value);
                carry = value >> 32;
            }
        }

        Limbs result(product.begin() + static_cast<std::ptrdiff_t>(words), product.end());
        Trim(result);
        if (Compare(result, modulus) >= 0) result = Sub(result, modulus);
        if (Compare(result, modulus) >= 0) result = Sub(result, modulus);
        if (Compare(result, modulus) >= 0) result = Mod(result, modulus);
        return result;
    }

    Limbs ToMontgomery(Limbs value) const
    {
        value = Mod(value, modulus);
        for (std::size_t bit = 0; bit < words * 32; ++bit)
            value = AddMod(value, value, modulus);
        return value;
    }

    Limbs FromMontgomery(const Limbs& value) const
    {
        return Multiply(value, Limbs{1});
    }
};

Limbs MulModFast(Limbs left, Limbs right, const Limbs& modulus)
{
    if (modulus.empty() || (modulus[0] & 1U) == 0)
        return MulModBinary(std::move(left), std::move(right), modulus);
    const MontgomeryContext context(modulus);
    return context.FromMontgomery(context.Multiply(context.ToMontgomery(std::move(left)),
                                                   context.ToMontgomery(std::move(right))));
}

Limbs PowMod(Limbs base, Limbs exponent, const Limbs& modulus)
{
    const unsigned int bits = BitLength(exponent);
    if (!modulus.empty() && (modulus[0] & 1U) != 0) {
        const MontgomeryContext context(modulus);
        Limbs result = context.ToMontgomery(Limbs{1});
        base = context.ToMontgomery(std::move(base));
        for (unsigned int bit = 0; bit < bits; ++bit) {
            if (Bit(exponent, bit)) result = context.Multiply(result, base);
            base = context.Multiply(base, base);
        }
        return context.FromMontgomery(result);
    }
    Limbs result{1}; base = Mod(base, modulus);
    for (unsigned int bit = 0; bit < bits; ++bit) { if (Bit(exponent, bit)) result = MulModBinary(result, base, modulus); base = MulModBinary(base, base, modulus); }
    return result;
}

std::uint32_t DivSmall(Limbs& value, std::uint32_t divisor)
{
    std::uint64_t remainder = 0; for (size_t i = value.size(); i-- > 0;) { const std::uint64_t current = (remainder << 32) | value[i]; value[i] = static_cast<std::uint32_t>(current / divisor); remainder = current % divisor; } Trim(value); return static_cast<std::uint32_t>(remainder);
}

Limbs MulSmall(const Limbs& value, std::uint32_t multiplier) { Limbs result(value.size() + 1, 0); std::uint64_t carry = 0; for (size_t i = 0; i < value.size(); ++i) { const std::uint64_t current = static_cast<std::uint64_t>(value[i]) * multiplier + carry; result[i] = static_cast<std::uint32_t>(current); carry = current >> 32; } result.back() = static_cast<std::uint32_t>(carry); Trim(result); return result; }

bool IsPrimeValue(const Limbs& value)
{
    if (Compare(value, Limbs{2}) < 0) return false; if (value.size() == 1) { const std::uint32_t n = value[0]; if (!(n & 1U)) return n == 2; for (std::uint32_t d = 3; static_cast<std::uint64_t>(d) * d <= n; d += 2) if (n % d == 0) return false; return true; }
    if (!(value[0] & 1U)) return false; Limbs nMinusOne = Sub(value, Limbs{1}); Limbs d = nMinusOne; unsigned int s = 0; while (!(d[0] & 1U)) { d = Shr(d, 1); ++s; }
    for (std::uint32_t base : {2U, 3U, 5U, 7U, 11U}) { Limbs x = PowMod(Limbs{base}, d, value); if (Compare(x, Limbs{1}) == 0 || Compare(x, nMinusOne) == 0) continue; bool witness = true; for (unsigned int r = 1; r < s; ++r) { x = Mod(Mul(x, x), value); if (Compare(x, nMinusOne) == 0) { witness = false; break; } } if (witness) return false; }
    return true;
}

void PutOutput(const BigData* number, const std::vector<unsigned char>& bytes)
{
    auto* output = number->Output(); output->SetCount(static_cast<unsigned int>(bytes.size())); if (!bytes.empty()) std::copy(bytes.begin(), bytes.end(), output->m_data);
}

std::vector<unsigned char> Binary(const BigData* number)
{
    Limbs value = Read(number); std::vector<unsigned char> bytes; bytes.reserve(value.size() * 4);
    for (size_t i = 0; i < value.size() * 4; ++i) { const unsigned char byte = static_cast<unsigned char>(value[i / 4] >> (8 * (i % 4))); if (byte || i / 4 + 1 < value.size()) bytes.push_back(byte); }
    if (bytes.empty()) bytes.push_back(0); return bytes;
}

std::string Decimal(const BigData* number)
{
    Limbs value = Read(number); if (BitLength(value) == 0) return "0"; std::string text; while (BitLength(value)) text.push_back(static_cast<char>('0' + DivSmall(value, 10))); std::reverse(text.begin(), text.end()); return text;
}

template <typename T> void AssignArray(TSGrowableArray<T>* array, const T* data, unsigned int count, bool append) { if (!append) array->SetCount(0); for (unsigned int i = 0; i < count; ++i) array->Add(data[i]); }
}

BigBuffer::BigBuffer() : m_data(4), m_offset(0) { m_data.SetCount(1); m_data[0] = 0; }
BigBuffer::~BigBuffer() = default;
BigBuffer& BigBuffer::operator=(const BigBuffer& other) { m_data = other.m_data; m_offset = other.m_offset; return *this; }
unsigned int& BigBuffer::operator[](unsigned int index) { GrowToFit(index); return m_data[index + m_offset]; }
const unsigned int& BigBuffer::operator[](unsigned int index) const { return m_data[index + m_offset]; }
void BigBuffer::GrowToFit(unsigned int index) { if (index + m_offset >= m_data.Count()) m_data.SetCount(index + m_offset + 1); }
bool BigBuffer::IsUsed(unsigned int index) const { return index + m_offset < m_data.Count(); }
void BigBuffer::SetCount(unsigned int count) { m_data.SetCount(count + m_offset); }
unsigned int BigBuffer::Count() const { return m_data.Count() > m_offset ? m_data.Count() - m_offset : 0; }
void BigBuffer::Trim() { while (m_data.Count() > m_offset + 1 && m_data[m_data.Count() - 1] == 0) --m_data.m_count; }
void BigBuffer::SetOffset(unsigned int offset) { m_offset = offset; GrowToFit(0); }
void BigBuffer::Clear() { m_offset = 0; m_data.SetCount(1); m_data[0] = 0; }

BigStack::BigStack() : m_used(0) {}
BigStack::~BigStack() = default;
BigBuffer* BigStack::Alloc(unsigned int* count) { const unsigned int available = SIZE - m_used; const unsigned int requested = count && *count ? std::min(*count, available) : 1U; if (count) *count = requested; BigBuffer* result = requested ? &m_buffer[m_used] : nullptr; m_used += requested; return result; }
void BigStack::Free(unsigned int count) { m_used = count > m_used ? 0 : m_used - count; }
BigBuffer* BigStack::MakeDistinct(BigBuffer* original, int required) { if (!required) return original; unsigned int count = 1; BigBuffer* result = Alloc(&count); if (result) *result = *original; return result; }
void BigStack::UnmakeDistinct(BigBuffer* original, BigBuffer* distinct) { if (original != distinct && distinct) { *original = *distinct; Free(1); } }

BigData::BigData() = default;
BigData::~BigData() = default;
BigData* BigData::Primary() { return this; }
const BigData* BigData::Primary() const { return this; }
BigStack* BigData::Stack() { return &m_stack; }
TSGrowableArray_<unsigned char, 1396853063U, 102U>* BigData::Output() const { return const_cast<TSGrowableArray_<unsigned char, 1396853063U, 102U>*>(&m_output); }

void __stdcall SBigNew(BigData** number) { if (number) *number = new BigData(); }
void __stdcall SBigDel(BigData* number) { delete number; }
void __stdcall SBigCopy(BigData* a, const BigData* b) { if (a && b) Write(a, Read(b)); }
void __stdcall SBigSetZero(BigData* a) { Write(a, {0}); }
void __stdcall SBigSetOne(BigData* a) { Write(a, {1}); }
void __stdcall SBigSet2Exp(BigData* a, unsigned int exponent) { Limbs value(exponent / 32 + 1, 0); value[exponent / 32] = 1U << (exponent % 32); Write(a, value); }
void __stdcall SBigFromUnsigned(BigData* a, unsigned int value) { Write(a, {value}); }
void __stdcall SBigToUnsigned(const BigData* a, unsigned int* value) { if (value) *value = Read(a)[0]; }
void __stdcall SBigAdd(BigData* a, const BigData* b, const BigData* c) { Write(a, Add(Read(b), Read(c))); }
void __stdcall SBigInc(BigData* a, const BigData* b) { Write(a, Add(Read(b), {1})); }
void __stdcall SBigSub(BigData* a, const BigData* b, const BigData* c) { Write(a, Sub(Read(b), Read(c))); }
void __stdcall SBigDec(BigData* a, const BigData* b) { Write(a, Sub(Read(b), {1})); }
void __stdcall SBigMul(BigData* a, const BigData* b, const BigData* c) { Write(a, Mul(Read(b), Read(c))); }
void __stdcall SBigSquare(BigData* a, const BigData* b) { const Limbs value = Read(b); Write(a, Mul(value, value)); }
void __stdcall SBigDiv(BigData* a, const BigData* b, const BigData* c) { Limbs q, r; DivMod(Read(b), Read(c), q, r); Write(a, q); }
void __stdcall SBigMod(BigData* a, const BigData* b, const BigData* c) { Write(a, Mod(Read(b), Read(c))); }
void __stdcall SBigMulMod(BigData* a, const BigData* b, const BigData* c, const BigData* d) { Write(a, MulModFast(Read(b), Read(c), Read(d))); }
void __stdcall SBigPow(BigData* a, const BigData* b, unsigned int exponent) { Limbs result{1}, base = Read(b); while (exponent) { if (exponent & 1U) result = Mul(result, base); exponent >>= 1; if (exponent) base = Mul(base, base); } Write(a, result); }
void __stdcall SBigPowMod(BigData* a, const BigData* b, const BigData* c, const BigData* d) { Write(a, PowMod(Read(b), Read(c), Read(d))); }
void __stdcall SBigShl(BigData* a, const BigData* b, unsigned int bits) { Write(a, Shl(Read(b), bits)); }
void __stdcall SBigShr(BigData* a, const BigData* b, unsigned int bits) { Write(a, Shr(Read(b), bits)); }
int __stdcall SBigCompare(const BigData* b, const BigData* c) { return Compare(Read(b), Read(c)); }
void __stdcall SBigBitLen(BigData* a, unsigned int* bits) { if (bits) *bits = BitLength(Read(a)); }
BOOL __stdcall SBigIsZero(const BigData* a) { return BitLength(Read(a)) == 0; }
unsigned int __stdcall SBigIsOne(const BigData* a) { return Compare(Read(a), {1}) == 0; }
BOOL __stdcall SBigIsEven(const BigData* a) { return !(Read(a)[0] & 1U); }
int __stdcall SBigIsOdd(const BigData* a) { return Read(a)[0] & 1U; }
int __stdcall SBigIsPrime(const BigData* a) { return IsPrimeValue(Read(a)); }
void __stdcall SBigAnd(BigData* a, const BigData* b, const BigData* c) { Limbs x = Read(b), y = Read(c), r(std::min(x.size(), y.size())); for (size_t i=0;i<r.size();++i) r[i]=x[i]&y[i]; Write(a,r); }
void __stdcall SBigOr(BigData* a, const BigData* b, const BigData* c) { Limbs x=Read(b),y=Read(c),r(std::max(x.size(),y.size()),0); for(size_t i=0;i<r.size();++i) r[i]=(i<x.size()?x[i]:0)|(i<y.size()?y[i]:0); Write(a,r); }
void __stdcall SBigXor(BigData* a, const BigData* b, const BigData* c) { Limbs x=Read(b),y=Read(c),r(std::max(x.size(),y.size()),0); for(size_t i=0;i<r.size();++i) r[i]=(i<x.size()?x[i]:0)^(i<y.size()?y[i]:0); Write(a,r); }
void __stdcall SBigNot(BigData* a, const BigData* b) { Limbs r=Read(b); for(auto& limb:r) limb=~limb; Write(a,r); }
void __stdcall SBigGcd(BigData* a, const BigData* b, const BigData* c) { Limbs x=Read(b),y=Read(c); while(BitLength(y)){ Limbs r=Mod(x,y); x=y; y=r; } Write(a,x); }
void __stdcall SBigInvMod(BigData* a, const BigData* b, const BigData* modulus) { Limbs m=Read(modulus),u=Mod(Read(b),m),v=m,x1{1},x2{0}; if(!(m[0]&1U)){Write(a,{0});return;} while(Compare(u,{1})&&Compare(v,{1})){ while(!(u[0]&1U)){u=Shr(u,1);x1=!(x1[0]&1U)?Shr(x1,1):Shr(Add(x1,m),1);} while(!(v[0]&1U)){v=Shr(v,1);x2=!(x2[0]&1U)?Shr(x2,1):Shr(Add(x2,m),1);} if(Compare(u,v)>=0){u=Sub(u,v);x1=Compare(x1,x2)>=0?Sub(x1,x2):Sub(Add(x1,m),x2);}else{v=Sub(v,u);x2=Compare(x2,x1)>=0?Sub(x2,x1):Sub(Add(x2,m),x1);} } Write(a,Compare(u,{1})==0?x1:x2); }
void __stdcall SBigFindPrime(BigData* a, unsigned int bits, const BigData* lower, const BigData* upper) { Limbs candidate=lower?Read(lower):Shl({1},bits?bits-1:0),limit=upper?Read(upper):Shl({1},bits); if(!(candidate[0]&1U)) candidate=Add(candidate,{1}); while(Compare(candidate,limit)<0&&!IsPrimeValue(candidate)) candidate=Add(candidate,{2}); Write(a,candidate); }
void __stdcall SBigRand(BigData* a, const BigData* limit, BigData* seed) { Limbs state=Read(seed),max=Read(limit); std::uint32_t x=state[0]*1664525U+1013904223U; Write(seed,{x}); Write(a,Mod({x},max)); }
void __stdcall SBigFromBinary(BigData* number, const void* data, unsigned int bytes) { const auto* source=static_cast<const unsigned char*>(data); Limbs value((bytes+3)/4,0); for(unsigned int i=0;i<bytes;++i)value[i/4]|=static_cast<std::uint32_t>(source[i])<<(8*(i%4)); Write(number,value); }
void __stdcall SBigFromStr(BigData* number, const char* string) { Limbs value{0}; if(string) for(;*string>='0'&&*string<='9';++string)value=Add(MulSmall(value,10),{static_cast<std::uint32_t>(*string-'0')}); Write(number,value); }
void __stdcall SBigFromStream(BigData* number, const void* data, unsigned int maxBytes, unsigned int* bytes) { const auto* source=static_cast<const unsigned char*>(data); unsigned int index=0,count=0,multiplier=1; while(index<maxBytes&&source[index]!=0xFF){count+=source[index++]*multiplier;multiplier*=0xFF;} if(index<maxBytes)++index; count=std::min(count,maxBytes-index); SBigFromBinary(number,source+index,count); if(bytes)*bytes=index+count; }

void __stdcall SBigToBinaryArray(const BigData* n, TSGrowableArray<unsigned char>* a, int append){auto b=Binary(n);AssignArray(a,b.data(),static_cast<unsigned int>(b.size()),append!=0);}
void __stdcall SBigToBinaryBuffer(const BigData* n,unsigned char* d,unsigned int max,unsigned int* bytes){auto b=Binary(n);unsigned int count=std::min(max,static_cast<unsigned int>(b.size()));if(d&&count)std::memcpy(d,b.data(),count);if(bytes)*bytes=count;}
void __stdcall SBigToBinaryPtr(const BigData* n,const void** data,unsigned int* bytes){auto b=Binary(n);PutOutput(n,b);if(data)*data=n->Output()->Ptr();if(bytes)*bytes=n->Output()->Count();}
void __stdcall SBigToStrArray(const BigData* n,TSGrowableArray<char>* a,int append){std::string s=Decimal(n);s.push_back('\0');AssignArray(a,s.data(),static_cast<unsigned int>(s.size()),append!=0);}
void __stdcall SBigToStrBuffer(const BigData* n,char* str,unsigned int chars){if(!str||!chars)return;std::string s=Decimal(n);strncpy_s(str,chars,s.c_str(),_TRUNCATE);}
void __stdcall SBigToStrPtr(const BigData* n,const char** str){std::string s=Decimal(n);std::vector<unsigned char>b(s.begin(),s.end());b.push_back(0);PutOutput(n,b);if(str)*str=reinterpret_cast<const char*>(n->Output()->Ptr());}
void __stdcall SBigToStreamArray(const BigData* n,TSGrowableArray<unsigned char>* a,int append){auto b=Binary(n);std::vector<unsigned char>s;unsigned int count=static_cast<unsigned int>(b.size());do{s.push_back(static_cast<unsigned char>(count%0xFF));count/=0xFF;}while(count);s.push_back(0xFF);s.insert(s.end(),b.begin(),b.end());AssignArray(a,s.data(),static_cast<unsigned int>(s.size()),append!=0);}
void __stdcall SBigToStreamBuffer(const BigData* n,unsigned char* d,unsigned int max,unsigned int* bytes){TSGrowableArray<unsigned char>a;SBigToStreamArray(n,&a,0);unsigned int count=std::min(max,a.Count());if(d&&count)std::memcpy(d,a.Ptr(),count);if(bytes)*bytes=count;}
void __stdcall SBigToStreamPtr(const BigData* n,const void** data,unsigned int* bytes){auto* out=n->Output();SBigToStreamArray(n,out,0);if(data)*data=out->Ptr();if(bytes)*bytes=out->Count();}
